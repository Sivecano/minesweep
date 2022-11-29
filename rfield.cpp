#include "rfield.h"
#include "port.h"
#include "input.h"
#include <SDL2/SDL_net.h>

rfield::rfield(const char* hostname) : field(0, 0)
{ 
    IPaddress server;
    
    if (SDLNet_ResolveHost(&server, hostname, PORT) == -1)
    {
        std::cerr << "couldn't resolve host " << hostname << std::endl;
        return;
    }

    if (!(sock = SDLNet_TCP_Open(&server)))
    {
        std::cerr << "couldn't connect to host " << hostname 
            << " at " << server.host << ":" << server.port << std::endl;
        return;
    }

    if (SDLNet_TCP_Recv(sock, &width, sizeof(width)) < sizeof(width))
    {
        std::cerr << "couldn't get width from " << hostname << " at " 
            << server.host << ":" << server.port << std::endl;
        return;
    }

    
    if (SDLNet_TCP_Recv(sock, &height, sizeof(height)) < sizeof(height))
    {
        std::cerr << "couldn't get height from " << hostname << " at " 
            << server.host << ":" << server.port << std::endl;
        return;
    }

    grid = new bool[width*height];
    status = new int8_t[width*height];
    
    sync();
} 

rfield::~rfield() 
{
    SDLNet_TCP_Close(sock);
}

void rfield::sync()
{
    input message = {SYNC, 0, 0};
    
    if (SDLNet_TCP_Send(sock, &message, sizeof(input)) < sizeof(input))
    {
        std::cerr << "couldn't request sync" << std::endl;
        return;
    }

    if (SDLNet_TCP_Recv(sock, grid, sizeof(bool) * width * height) < sizeof(bool) * width * height)
    {
        std::cerr << "couldn't get grid :(" << std::endl;
        return;
    }

    if (SDLNet_TCP_Recv(sock, status, sizeof(int8_t) * width * height) < sizeof(bool) * width * height)
    {
        std::cerr << "couldn't get status :(" << std::endl;
        return;
    }


}

void rfield::flag(uint16_t x, uint16_t y)
{ 
    sync();
    input in = {FLAG, x, y};
    if (SDLNet_TCP_Send(sock, &in, sizeof(in)) < sizeof(in))
        std::cerr << "couldn't send flag" << std::endl;
}

bool rfield::guess(uint16_t x, uint16_t y)
{
    sync();
    input in = {GUESS, x, y};
    if (SDLNet_TCP_Send(sock, &in, sizeof(in)) < sizeof(in))
        std::cerr << "couldn't send guess" << std::endl;
    
    bool out = false;
    if (SDLNet_TCP_Recv(sock, &out, sizeof(bool)) < sizeof(bool))
        std::cerr << "couldn't recieve answer" << std::endl;
    return out;
}

