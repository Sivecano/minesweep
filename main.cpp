#include <algorithm>
#include <iostream>
#include <cstdint>
#include "field.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gui.h"


void tui()
{
    field a(10,10);
    a.fill(8);
    a.fair_start();
    uint16_t x,y;

    do
    {
        a.render(std::cout);
        std::cout << "guess [x, y] : ";
        std::cin >> x >> y;
        
    } while(!a.guess(x, y));
    
    std::cout << "GAME OVER!\n";

    std::fill_n(a.status, a.width*a.height, 1);

    a.render(std::cout);
}


int main()
{
    gui(100, 100, 2300);
    return 0;
}

