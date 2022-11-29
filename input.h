#pragma once 
#include <cstdint>

enum inputmode
{
    SYNC,
    GUESS,
    FLAG
};

struct input
{
    inputmode mode;
    uint16_t x, y;
};
