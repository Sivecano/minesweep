#include <SDL2/SDL_events.h>
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
        if (x >= width || y >= height) return true;

        uncover(x + 1, y + 1);

        return grid[x + width * y];
    }

    uint16_t count(uint16_t x, uint16_t y)
    {
        uint16_t out = 0;
        
        for (uint16_t dx = x - (x > 0); dx <= x + (x < width); dx++)
            for (uint16_t dy = y - (y > 0); dy <= y + (y < height); dy++)
                out += grid[dx + width * dy] ? 1 : 0;
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

};


void tui()
{
    field a(10,10);
    a.fill(8);
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

void gui()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Texture* tex;

    SDL_CreateWindowAndRenderer(800, 800, 0, &win, &ren);
    tex = IMG_LoadTexture(ren, "tiles.png");
    
    bool running = true;

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

    }


    SDL_DestroyTexture(tex);
    SDL_Quit();
}

int main()
{
    return 0;
}

