#include "gui.h"
#include "hfield.h"

int main()
{

    hfield a(120, 65);
    a.fill(500);
    a.fair_start();

    gui(&a);
    return 0;
}
