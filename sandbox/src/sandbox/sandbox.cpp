#include "sandbox/sandbox.h"

#include "vertex/math/math/fn/fn_common.h"

int main()
{
    using namespace vx;

    constexpr auto x = math::min({ 1, 2, 3, 4 });

    return 0;
}