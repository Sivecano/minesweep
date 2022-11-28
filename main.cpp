#include <algorithm>
#include <iostream>
#include <cstdint>
#include "field.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"



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


void gui(uint16_t wcells = 15, uint16_t hcells = 15, uint16_t mines = 40)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* tex;

    SDL_CreateWindowAndRenderer(wcells * 32, hcells * 32 + 20, 0, &win, &ren);
    SDL_SetWindowTitle(win, "MineSweep!");
    tex = IMG_LoadTexture(ren, "tiles.png");
    
    bool running = true;
    int8_t playstate = 0; // -1 is loss, 0 is playing, 1 is win;

    field a(wcells, hcells);
    a.fill(mines);
    a.fair_start();
 
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            int x,y;
            uint8_t state;
            switch(e.type)
            {
                case SDL_MOUSEBUTTONUP:
                    x = e.button.x;-
                    y = e.button.y;
                    state = e.button.button;
                    // std::cout << "Click " << int(state) << " at " << x << ", " << y << "\n";
                    if (x < 0 || x > 32 * a.width || y < 20 || (y-20) > 32*a.height) continue;

                    x /= 32;
                    y -= 20;
                    y /= 32;

                    switch(state)
                    {
                        case 1:
                            a.guess(x, y);
                            break;
                        case 3:
                            if (a.status[x + a.width * y] == 0) a.status[x + a.width * y] = -1;
                            else if (a.status[x + a.width * y] == -1) a.status[x + a.width * y] = 0;

                            break;

                    }
                    break;

                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
        SDL_RenderClear(ren);
        
        SDL_Rect src = {0, 0, 16, 16};
        SDL_Rect dst = {0, 20, 32, 32};

        for (uint16_t n = 0; n < a.width * a.height; n++)
        {
            if (n % a.width == 0 && n != 0)
            {
                dst.x = 0;
                dst.y += 32;
            }

            switch (a.status[n])
            {
                case -1:
                    src.y = 0;
                    break;
                case 0:
                    src.y = 16;
                    break;
                case 1:
                    if (a.grid[n])
                        src.y = 32;
                    else 
                        src.y = 48 + 16* a.count(n % a.width, n / a.width);
            }

            SDL_RenderCopy(ren, tex, &src, &dst);
            
            dst.x += 32;
        }




        SDL_RenderPresent(ren);
    }


    SDL_DestroyTexture(tex);
    SDL_Quit();
}

int main()
{
    gui(10, 10, 40);
    return 0;
}

