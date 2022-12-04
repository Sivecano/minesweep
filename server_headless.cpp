#include "hfield.h"
#include "field.h"
#include <iostream>
#include <cstdint>

int main(int argc, char** argv)
{
    uint16_t w,h,m;
    std::cout << "width, height, mines: ";

    switch(scanf("%hu, %hu, %hu", &w, &h, &m))
    {
        case 0:
            w = 15;
        case 1:
            h = w;
        case 2: 
            m = h*w/6;
        default:
            break;
    }

    hfield f(w,h);
    f.fill(m);
    f.fair_start();

    bool running = true;
    bool dead = false;
    int t0 = time(0);

    while(running)
    {
        f.sync();
        if (time(0) - t0 > 20)
        {
            f.render(std::cout);
            t0 = time(0);
        }


        running = false;
        for (uint16_t n = 0; n < f.width*f.height; n++)
        {
            if (f.grid[n] && f.status[n] == 1)
            {
                dead = true;
                break;
            }
            if (f.grid[n] && f.status[n] != -1)
            {
                running = true;
                break;
            }
            if (!f.grid[n] && f.status[n] != 1)
            {
                running = true;
                break;
            }
        }

    }

    for (uint16_t n = 0; n < f.width * f.height; n++)
    {
        if (f.grid[n] && f.status[n] == -1) continue;

        f.status[n] = 1;
    }

    f.render(std::cout);
    
    if (dead)
        std::cout << "YOU LOST!" << std::endl;
    else
        std::cout << "YOU WIN!" << std::endl;

    while (!f.sockets.empty())
    {
        f.sync();
    }

    return 0;
}
