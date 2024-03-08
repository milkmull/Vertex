#include "sandbox/sandbox.h"

#include "vertex/math/random/rng.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"
#include "vertex/image/noise_sampler.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image noise(200, 200, img::image_format::R8);

    math::rng rng;

    img::noise_sampler s;
    s.frequency = 10.0f;

    s.seed.x = rng.randf_range(0.0f, 100.0f);
    s.seed.y = rng.randf_range(0.0f, 100.0f);

    using pixel_type = img::pixel_r8;
    for (auto it = img::begin<pixel_type>(noise); it != img::end<pixel_type>(noise); ++it)
    {
        float z = s.simplex_noise(it.uv());
        *it = math::color(z);
    }

    //img::replace_color(dst, math::color::BLACK(), math::color(0.2f, 0.8f, 0.75f), 0.5f);

    err = img::write_png("../../assets/noise_test.png", noise);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}