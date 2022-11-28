#include "hfield.h"
#include "port.h"
#include "input.h"
#include <SDL2/SDL_net.h>

hfield::hfield(uint16_t w, uint16_t h) : field(w, h)
{
    IPaddress ip;

    if (SDLNet_ResolveHost(&ip, NULL, PORT) < 0)
    {
        std::cerr << "failed to resolve host NULL" << std::endl;
        return;
    }

    if (!(server = SDLNet_TCP_Open(&ip)))
    {
        std::cerr << "failed to open socket" << std::endl;
        return;
    }

    sync();

    set = SDLNet_AllocSocketSet(setsize);
}

hfield::~hfield()
{
    SDLNet_FreeSocketSet(set);

    for (TCPsocket sock : sockets)
        SDLNet_TCP_Close(sock);

    SDLNet_TCP_Close(server);
}

void hfield::sync()
{
    TCPsocket youngling;
    if ((youngling = SDLNet_TCP_Accept(server)))
    {
        IPaddress* name = SDLNet_TCP_GetPeerAddress(youngling);
        std::cout << "adopted youngling" << name->host << ":" << name->port <<std::endl;
        
        if (SDLNet_TCP_Send(youngling, &width, sizeof(width)) < sizeof(width))
        {
            std::cerr << "couldn't send width to youngling" << std::endl;
        }

        if (SDLNet_TCP_Send(youngling, &height, sizeof(height)) < sizeof(height))
        {
            std::cerr << "couldn't send width to youngling" << std::endl;
        }

        sockets.push_back(youngling);
        if (sockets.size() > setsize)
        {
            setsize *= 2;
            SDLNet_FreeSocketSet(set);
            set = SDLNet_AllocSocketSet(setsize);
            for (TCPsocket sock : sockets)
                SDLNet_TCP_AddSocket(set, sock);
        }
        else
            SDLNet_TCP_AddSocket(set, youngling);
    }

    int out = SDLNet_CheckSockets(set, 0);
    if (out == 0) return;
    if (out < 0)
    {
        std::cerr << "error: " << SDLNet_GetError() << std::endl;
        return;
    }

    input message;

    for (TCPsocket sock : sockets)
    {
        if (!SDLNet_SocketReady(sock)) continue;
        
        if (SDLNet_TCP_Recv(sock, &message, sizeof(message)) < sizeof(message))
        {
            IPaddress* id = SDLNet_TCP_GetPeerAddress(sock);
            std::cerr << "error recieving message from socket " << id->host << ":" << id->port << std::endl;
            continue;
        }

        switch(message.mode)
        {
            case SYNC:
                if (SDLNet_TCP_Send(sock, grid, sizeof(bool) * width * height) < sizeof(bool) * width * height)
                {
                    std::cerr << "error sending grid" << std::endl;
                    break;
                }
                
                if (SDLNet_TCP_Send(sock, status, sizeof(int8_t) * width * height) 
                        < sizeof(int8_t) * width * height)
                {
                    std::cerr << "error sending status" << std::endl;
                    break;
                }
                break;
            
            case FLAG:
                flag(message.x, message.y);
                break;

            case GUESS:
                bool res = guess(message.x, message.y);
                if (SDLNet_TCP_Send(sock, &res, sizeof(res)) < sizeof(res))
                {
                    std::cerr << "couldn't send result of " << res << " back" << std::endl;
                }

                break;
        }
    }
}
