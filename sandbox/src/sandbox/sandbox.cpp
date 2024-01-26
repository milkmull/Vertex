#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    color c(0.1, 0.5, 0.9);

    color8 c8(c);

    color16 c16(c);
    color16 c162(c8);

    color8 c82(c16);

    color c2(c82);

    std::cout << c.to_string() << std::endl;
    std::cout << c2.to_string() << std::endl;
    std::cout << c8.to_string() << std::endl;
    std::cout << c82.to_string() << std::endl;
    std::cout << c16.to_string() << std::endl;
    std::cout << c162.to_string() << std::endl;


    return 0;
}