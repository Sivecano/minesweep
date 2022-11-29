#include "field.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gui.h"
#include "gui.config.h"

#ifndef SCALE
#define SCALE 2
#endif

const uint16_t swidth = 16;

void gui(uint16_t wcells, uint16_t hcells, uint16_t mines)
{
    field a(wcells, hcells);
    a.fill(mines);
    a.fair_start();
    gui(&a);
}

void gui(field* game)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* tex;

    SDL_CreateWindowAndRenderer(game->width * SCALE * swidth , (game->height * SCALE * swidth  + SCALE * 10) , 0, &win, &ren);
    SDL_SetWindowTitle(win, "MineSweep!");
    tex = IMG_LoadTexture(ren, "tiles.png");
    
    bool running = true;
    int8_t playstate = 0; // -1 is loss, 0 is playing, 1 is win;

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
                    x = e.button.x;
                    y = e.button.y;
                    state = e.button.button;
                    // std::cout << "Click " << int(state) << " at " << x << ", " << y << "\n";
                    if (x < 0 || x > SCALE * swidth * game->width  || y < SCALE * 10  || (y-SCALE * 10)  > SCALE * swidth  * game->height) continue;

                    x /= SCALE * swidth ;
                    y -= SCALE * 10 ;
                    y /= SCALE * swidth ;

                    switch(state)
                    {
                        case 1:
                            game->guess(x, y);
                            break;
                        case 3:
                            game->flag(x,y);
                            break;

                    }
                    break;

                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        
        game->sync();

        SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
        SDL_RenderClear(ren);
        
        SDL_Rect src = {0, 0, swidth, swidth};
        SDL_Rect dst = {0, SCALE * 10 , SCALE * swidth , SCALE * swidth };

        for (uint16_t n = 0; n < game->width * game->height; n++)
        {
            if (n % game->width == 0 && n != 0)
            {
                dst.x = 0;
                dst.y += SCALE * swidth ;
            }

            switch (game->status[n])
            {
                case -1:
                    src.y = 0;
                    break;
                case 0:
                    src.y = swidth;
                    break;
                case 1:
                    if (game->grid[n])
                        src.y = 2 * swidth ;
                    else 
                        src.y = (3 * swidth + swidth* game->count(n % game->width, n / game->width)) ;
            }

            SDL_RenderCopy(ren, tex, &src, &dst);
            
            dst.x += SCALE * swidth ;
        }




        SDL_RenderPresent(ren);
    }


    SDL_DestroyTexture(tex);
    SDL_Quit();

}
