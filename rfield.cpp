#include "rfield.h"
#include "port.h"
#include <SDL2/SDL_net.h>

rfield::rfield(const char* hostname) : field(0, 0)
{ 
    IPaddress server;
    
    if (!SDLNet_ResolveHost(&server, hostname, PORT))
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
    
}

