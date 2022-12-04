#include "hfield.h"
#include "port.h"
#include "input.h"
#include "SDL2/SDL_net.h"
#include <queue>

hfield::hfield(uint16_t w, uint16_t h) : field(w, h)
{
    SDLNet_Init();
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

    set = SDLNet_AllocSocketSet(setsize);

    sync();
}

hfield::~hfield()
{
    SDLNet_FreeSocketSet(set);

    for (TCPsocket sock : sockets)
        SDLNet_TCP_Close(sock);

    SDLNet_TCP_Close(server);
    SDLNet_Init();
}

void hfield::sync()
{
    TCPsocket youngling;
    std::queue<size_t> to_delete;

    while ((youngling = SDLNet_TCP_Accept(server)))
    {
        IPaddress* name = SDLNet_TCP_GetPeerAddress(youngling);
        std::cout << "adopted youngling " << name->host << ":" << name->port <<std::endl;
        
        if (SDLNet_TCP_Send(youngling, &width, sizeof(width)) < sizeof(width))
        {
            std::cerr << "couldn't send width to youngling" << std::endl;
            continue;
        }

        if (SDLNet_TCP_Send(youngling, &height, sizeof(height)) < sizeof(height))
        {
            std::cerr << "couldn't send width to youngling" << std::endl;
            continue;
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

    int out;
    while((out = SDLNet_CheckSockets(set, 0)) > 0)
    {
        input message;
        size_t index = 0;

        for (TCPsocket sock : sockets)
        {
            index++;
            if (!SDLNet_SocketReady(sock)) continue;
            
            uint8_t attempts = 0;

            while (SDLNet_TCP_Recv(sock, &message, sizeof(message)) < sizeof(message) && attempts < 10)
            {
                IPaddress* id = SDLNet_TCP_GetPeerAddress(sock);
                std::cerr << "error recieving message from socket " << id->host << ":" << id->port << std::endl;
                std::cerr << SDLNet_GetError() << std::endl;
                attempts++;
            }

            if (attempts == 10)
            {
                IPaddress* id = SDLNet_TCP_GetPeerAddress(sock);
                std::cerr << "couldn't accept message from " << id->host << ":" << id->port << std::endl;
                std::cerr << "removing socket " << index - 1 << std::endl;
               //TODO: remove socket 
               to_delete.push(index - 1);
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


        while(!to_delete.empty())
        {
            index = to_delete.front();
            std::cerr << "deleting: " << index << std::endl;
            to_delete.pop();
            SDLNet_TCP_DelSocket(set, sockets[index]);
            SDLNet_TCP_Close(sockets[index]);
            sockets.erase(sockets.begin() + index);
        }
    }
    


    if (out < 0)
    {
        std::cerr << "error: " << SDLNet_GetError() << std::endl;
        return;
    }
}
