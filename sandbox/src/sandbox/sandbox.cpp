#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"

#include "vertex/math/random/rng.h"
#include "vertex/math/texture/noise/perlin_noise.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image noise(200, 200, img::image_format::R8);

    math::rng rng;

    math::mix(0.1, 0.1, 0.1);

    const math::vec2 offset(rng(), rng());

    using pixel_type = img::pixel_r8;
    for (auto it = img::begin<pixel_type>(noise); it != img::end<pixel_type>(noise); ++it)
    {
        *it = math::color(math::perlin_noise(math::vec4(it.uv(), 0.0f, 0.0f) * 10));
    }

    //img::replace_color(dst, math::color::BLACK(), math::color(0.2f, 0.8f, 0.75f), 0.5f);

    err = img::write_png("../../assets/noise_test_4d.png", noise);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}