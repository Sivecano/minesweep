#include "gui.h"
#include "hfield.h"

int main()
{

    hfield a(15, 15);
    a.fill(60);
    a.fair_start();

    gui(&a);
    return 0;
}
