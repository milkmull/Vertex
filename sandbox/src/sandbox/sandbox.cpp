#include "sandbox/sandbox.h"

vx::color func(vx::color v)
{
    return v;
}

int main()
{
    using namespace vx;

    color8 c;
    color8 c2 = func(c);

    std::cout << func(c).to_string() << std::endl;

    return 0;
}