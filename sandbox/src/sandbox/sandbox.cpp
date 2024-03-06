#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"
#include "vertex/math/texture/noise/cellular_noise.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image noise(200, 200, img::image_format::R8);

    using pixel_type = img::pixel_r8;
    for (auto it = img::begin<pixel_type>(noise); it != img::end<pixel_type>(noise); ++it)
    {
        math::vec2 z = math::cellular_noise(math::vec3(it.uv(), 0.0f) * 10.0f);
        //z = (z + 1.0f) * 0.5f;
        *it = math::color((z.x + z.y) * 0.5f);
    }

    //img::replace_color(dst, math::color::BLACK(), math::color(0.2f, 0.8f, 0.75f), 0.5f);

    err = img::write_png("../../assets/noise_test.png", noise);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}