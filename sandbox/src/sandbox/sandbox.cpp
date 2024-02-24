#include "sandbox/sandbox.h"

#include "vertex/math/math.h"
#include "vertex/math/color/blend.h"

int main()
{
    using namespace vx;

    constexpr math::color8 c1 = math::color::BLACK();
    constexpr math::color8 c2 = math::color::WHITE();

    constexpr math::color8 c3 = math::blend(c1, blend_mode::ONE, c2, blend_mode::ONE);

    return 0;
}