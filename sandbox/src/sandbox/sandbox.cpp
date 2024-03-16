#include "sandbox/sandbox.h"

#include "vertex/math/math/type/vec4.h"
#include "vertex/math/math/type/vec4i.h"
#include "vertex/math/math/fn/fn_bitwise.h"

int main()
{
    using namespace vx;

    constexpr int x = 1 << 31;
    constexpr uint32_t y = math::find_msb(x);

    return 0;
}