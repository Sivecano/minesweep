#pragma once 
#include "field.h"
#include "SDL2/SDL_net.h"
#include <vector>

struct hfield : field
{
    TCPsocket server;
    SDLNet_SocketSet set;
    std::vector<TCPsocket> sockets;

    hfield();
    virtual ~hfield();
    void sync();

};
