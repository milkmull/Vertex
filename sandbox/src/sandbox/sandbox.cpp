#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    int y = 1;
    auto x = math::frexp(2, y);

    return 0;
}