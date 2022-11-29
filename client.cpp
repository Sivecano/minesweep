#include "gui.h"
#include "rfield.h"
#include "field.h"

int main()
{
    rfield a("localhost");

    gui(&a);
    return 0;
}
