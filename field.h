#pragma once

#include <iostream>
#include <cstdint>

struct field
{
    bool* grid;     // tells you if a mine is somewhere
    int8_t* status; // 0 is neutral, -1 is marked, 1 is uncovered
    uint16_t width, height;

    field(uint16_t w, uint16_t h); 
    virtual ~field();
    void uncover(uint16_t x, uint16_t y);
    virtual bool guess(uint16_t x, uint16_t y);
    virtual void flag(uint16_t x, uint16_t y);
    virtual void sync();
    void render(std::ostream& out);
    void fill(uint16_t mines);
    void fair_start();
    uint16_t count(uint16_t x, uint16_t y);
    uint16_t fcount(uint16_t x, uint16_t y);

};
