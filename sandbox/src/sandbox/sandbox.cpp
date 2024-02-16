#include "sandbox/sandbox.h"

#include "vertex/math/color/type/color_type.h"
#include "vertex/math/color/srgb.h"

int main()
{
    using namespace vx;

    constexpr math::color8 c1 = math::color8::RED();
    constexpr math::color8 c2 = math::color8::GREY();
    constexpr math::color c3 = c2;
    constexpr auto y = math::lighten(c2, 0.5);
    constexpr auto z = math::invert(c3);

    return 0;
}