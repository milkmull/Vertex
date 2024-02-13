#include "sandbox/sandbox.h"

#include "vertex/math/color/type/colorf_type.h"
#include "vertex/math/color/fn/color_fn_common.h"

int main()
{
    using namespace vx;

    constexpr math::color c = math::color::MAGENTA();
    constexpr auto x = math::color::luminance(c);

    return 0;
}