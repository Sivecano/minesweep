#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

struct field
{
    bool* grid;     // tells you if a mine is somewhere
    int8_t* status; // 0 is neutral, -1 is marked, 1 is uncovered
    const uint16_t width, height;

    field(uint16_t w, uint16_t h) : width(w), height(h)
    {
        grid = new bool[w*h];
        std::fill_n(grid, w*h, false);
        status = new int8_t[w*h];
        std::fill_n(status, w*h, 0);
    }

    ~field()
    {
        delete[] grid;
        delete[] status;
    }
    
    void uncover(uint16_t x, uint16_t y)
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

    bool guess(uint16_t x, uint16_t y)
    {
        if (x >= width || y >= height) return false;

        uncover(x + 1, y + 1);

        return grid[x + width * y];
    }

    uint16_t count(uint16_t x, uint16_t y)
    {
        uint16_t out = 0;
        
        for (uint16_t dx = x - ((x > 0) ? 1 : 0); dx <= x + ((x < width - 1) ? 1 : 0); dx++)
            for (uint16_t dy = y - ((y > 0) ? 1 : 0); dy <= y + ((y < height - 1) ? 1 : 0); dy++)
                if (grid[dx + width * dy]) out++;

        return out;
    }

    // renders field to ostream;
    void render(std::ostream& out)
    {
        out << "  ";
        for (uint16_t n = 0; n < width; n++) out << " " << n;
        out << "\n  ╔";
        for (uint16_t n = 0; n < 2 * width - 1; n++) out << ((n % 2) ? "╤" : "═" );
        out << "╗\n";
        
        for (uint16_t y = 0; y < height; y++)
        {
            out << y << " ║";
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
                out << "  ╟";
                for (uint16_t n = 0; n < 2 * width - 1; n++) out << ((n % 2) ? "┼" : "─");
                out << "╢\n";
            }
        }
    
        out << "  ╚";
        for (uint16_t n = 0; n < 2 * width - 1; n++) out << (n % 2 ? "╧" : "═");
        out << "╝" << std::endl;
    }

    void fill(uint16_t mines)
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

    void fair_start()
    {
        for (uint16_t m = 0; m < 9; m++)
            for (uint16_t n = 0; n < (width - 2) * (height - 2); n++)
                if (count(n % (width - 2) + 1, n / (width - 2) + 1) == m && !grid[(n % (width - 2)) + 1 + (n / (width - 2) + 1)* width])
                {
                    uncover(n % (width - 2) + 2, n / (width - 2) + 2);
                    return;
                }
    }

};


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
                    x = e.button.x;
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

