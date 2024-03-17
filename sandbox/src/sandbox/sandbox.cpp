#include "sandbox/sandbox.h"

#include "vertex/math/color/blend.h"
#include "vertex/math/color/fn/color_fn_interpolation.h"

#include "vertex/math/color/util/hash.h"
#include "vertex/math/color/util/to_string.h"

int main()
{
    using namespace vx;

    constexpr auto blend = math::blend_func();
    constexpr math::color c1 = math::color::BLACK();
    constexpr math::color c2 = math::color::WHITE();

    constexpr auto y = blend(c1, c2);

    std::cout << c1;

    return 0;
}