#include "field.h"
#include <cmath>
#include <iomanip>

field::field(uint16_t w, uint16_t h) : width(w), height(h)
{
    grid = new bool[w*h];
    std::fill_n(grid, w*h, false);
    status = new int8_t[w*h];
    std::fill_n(status, w*h, 0);
}

field::~field()
{
    delete[] grid;
    delete[] status;
}

void field::uncover(uint16_t x, uint16_t y)
{

    if (x > width || y > height 
            || x == 0 || y == 0 || 
            status[(x-1) + width * (y-1)] != 0)
        return;

    status[x - 1 + width * (y - 1)] = 1;
   
    if (count(x - 1, y - 1) == 0)
        for (uint16_t dx = x - 1; dx <= x + 1; dx++)
            for (uint16_t dy = y - 1; dy <= y + 1; dy++)
                uncover(dx, dy);

}

bool field::guess(uint16_t x, uint16_t y)
{
    if (x >= width || y >= height) return false;
    
    // chording
    if (status[x + width * y] == 1 && count(x, y) == fcount(x,y))
    {
        bool out = false;

        for (uint16_t dx = x - (x > 0); dx <= x + (x < width - 1); dx++)
            for (uint16_t dy = y - (y > 0); dy <= y + (y < height - 1); dy++)
            {
                out = out || grid[dx + dy * width];
                uncover(dx + 1, dy + 1);
            }

        return out;
    }

    uncover(x + 1, y + 1);

    return grid[x + width * y];
}

void field::flag(uint16_t x, uint16_t y)
{
    uint16_t n = x + width * y;

    if (status[n] == 0) status[n] = -1;
    else if (status[n] == -1) status[n] = 0;
}

// this is local so no syncing
void field::sync()
{
    //std::cout << "this is a local object" << std::endl;
}

uint16_t field::count(uint16_t x, uint16_t y)
{
    uint16_t out = 0;
    
    for (uint16_t dx = x - (x > 0); dx <= x + (x < width - 1); dx++)
        for (uint16_t dy = y - (y > 0); dy <= y + (y < height - 1); dy++)
            if (grid[dx + width * dy]) out++;

    return out;
}


uint16_t field::fcount(uint16_t x, uint16_t y)
{
    uint16_t out = 0;
    
    for (uint16_t dx = x - (x > 0); dx <= x + (x < width - 1); dx++)
        for (uint16_t dy = y - (y > 0); dy <= y + (y < height - 1); dy++)
            if (status[dx + width * dy] == -1) out++;

    return out;
}

// renders field to ostream;
void field::render(std::ostream& out)
{
    uint16_t tw = log10l(height) + 1;
    uint16_t exp = 1;
    for (uint16_t i = 1; i < tw; i++)
        exp *= 10;
    
    for (; exp > 0; exp /= 10)
    {
        out << std::setw(tw + 1) << " ";
        for (uint16_t n = 0; n < width; n++) 
            if (n >= exp || (n == 0 && exp == 1))
                out << " " << (n / exp) % 10;
            else
                out << "  ";
        out << "\n";
    }

    out << std::setw(tw) << " " << " ╔";
    for (uint16_t n = 0; n < 2 * width - 1; n++) out << ((n % 2) ? "╤" : "═" );
    out << "╗\n";
    
    for (uint16_t y = 0; y < height; y++)
    {
        out << std::setw(tw) << y << " ║";
        for (uint16_t x = 0; x < width; x++)
        {
            switch(status[x + y * width])
            {
                case -1:
                    out << "";
                    break;
                case 0:
                    out << " ";
                    break;
                case 1:
                    if (grid[x + y * width])
                        out << "⦻";
                    else 
                        out << count(x, y);
                    break;

            } 
            if (x < width - 1)
                out << "│";
        }
        out << "║\n";
        if (y < height - 1)
        {
            out << std::setw(tw) << " " << " ╟";
            for (uint16_t n = 0; n < 2 * width - 1; n++) out << ((n % 2) ? "┼" : "─");
            out << "╢\n";
        }
    }

    out << std::setw(tw) << " " << " ╚";
    for (uint16_t n = 0; n < 2 * width - 1; n++) out << (n % 2 ? "╧" : "═");
    out << "╝" << std::endl;
}

void field::fill(uint16_t mines)
{
    srand(time(0));
    for (uint16_t n = 0; n < width * height; n++)
    {
        if (rand() % ((width*height - n)/mines) == 0) 
        {
            grid[n] = true;
            if (--mines == 0)
                break;
        }
    }

}

void field::fair_start()
{
    for (uint16_t m = 0; m < 9; m++)
        for (uint16_t n = 0; n < (width) * (height); n++)
            if (count(n % (width), n / (width)) == m && !grid[n])
            {
                guess(n % (width), n / (width));
                return;
            }
}
