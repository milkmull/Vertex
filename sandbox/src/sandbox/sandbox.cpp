#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"

#include "vertex/math/random/rng.h"
#include "vertex/math/texture/noise/perlin_noise.h"
#include "vertex/math/texture/noise/simplex_noise.h"

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
        math::vec2 xy = it.xy();
        xy -= 1;
        math::vec2 uv = xy / it.resolution();

        if (math::is_zero_approx(it.position() % 50))
        {
            std::cout << "stop" << std::endl;
        }

        auto z = (math::simplex_noise(math::vec4(it.uv(), it.uv()) * 10.0f) + 1.0f) / 2.0f;
        //std::cout << z << std::endl;
        *it = math::color(z);

        math::color8 c = math::color(*it);
        c *= 1;
    }

    //img::replace_color(dst, math::color::BLACK(), math::color(0.2f, 0.8f, 0.75f), 0.5f);

    err = img::write_png("../../assets/noise_test_simplex_4d.png", noise);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}