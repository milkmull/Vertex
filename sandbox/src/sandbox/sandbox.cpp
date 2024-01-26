#include "sandbox/sandbox.h"

void func(vx::vec2 v)
{
    return;
}

int main()
{
    using namespace vx;

    vec2i vi;
    vec2f vf;
    vec2d vd;

    vf = vi << vi;

    return 0;
}