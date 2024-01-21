#include <iostream>
#include <map>

#include "vertex/math/math.h"
#include "vertex/math/random/rng.h"

#include "vertex/math/geometry/rect.h"
#include "vertex/math/geometry/spline.h"
#include "vertex/math/geometry/triangulate.h"

#include "vertex/math/math/euler.h"

#include "vertex/math/texture/filter_bicubic.h"
#include "vertex/math/texture/filter_bilinear.h"
#include "vertex/math/texture/filter_box.h"
#include "vertex/math/texture/filter_nearest.h"

int main()
{
    using namespace vx;

    math::rng rng;

    vec2 v(rng.randf(), rng.randf());
    vec2i vi;

    math::frexp(v, vi);

    std::cout << vi.to_string();

    return 0;
}