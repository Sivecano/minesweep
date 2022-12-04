#include "field.h"
#include <iostream>

int main(int argc, char** argv)
{
    uint16_t w = 10, h = 10, mines = 8;
    std::cout << "width, height : ";
    std::cin >> w;
    std::cin >> h;
    std::cout << "mines: ";
    std::cin >> mines;
    field a(w, h);
    a.fill(mines);
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

    return 0;
}
