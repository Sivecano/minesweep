#pragma once 
#include "field.h"
#define WITHOUT_SDL
#include "SDL2/SDL_net.h"
#include <vector>

struct hfield : field
{
    TCPsocket server;
    SDLNet_SocketSet set;
    uint8_t setsize = 4;
    std::vector<TCPsocket> sockets;

    hfield(uint16_t w, uint16_t h);
    virtual ~hfield();
    void sync();

};
