#include "gui.h"
#include "rfield.h"
#include "field.h"

int main()
{
    std::string hostname;
    std::cout << "give hostname: ";
    std::cin >> hostname;

    rfield a(hostname.c_str());

    gui(&a);
    return 0;
}
