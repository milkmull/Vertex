#include "sandbox/sandbox.h"

void func(vx::vec2 v)
{
    return;
}

int main()
{
    using namespace vx;

    vec2i vi;
    vec2f vf(1, 2);
    vec2d vd;

    std::cout << vf.swiz('y', 'x').to_string() << std::endl;

    return 0;
}