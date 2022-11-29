#include "hfield.h"
#include <iostream>
#include <cstdint>

int main()
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
    int t0 = time(0);

    while(running)
    {
        f.sync();
        if (time(0) - t0 > 20)
        {
            f.render(std::cout);
            t0 = time(0);
        }
    }

}
