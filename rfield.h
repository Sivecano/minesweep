#pragma once
#include "field.h"
#include "SDL2/SDL_net.h"
// remote minesweeper field connected to over TCP:
struct rfield : field
{
    TCPsocket sock; 
    rfield(const char* hostname);
    virtual ~rfield();

    virtual bool guess(uint16_t x, uint16_t y);
    virtual void flag(uint16_t x, uint16_t y);
    virtual void sync();
};
