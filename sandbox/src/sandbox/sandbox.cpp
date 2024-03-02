#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"

#include "vertex/math/texture/sdf.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image src = img::load("../../assets/michael.png", err);
    assert(err == img::error_code::NONE);

    img::image dst(src.get_info());

    img::sampler s(src);

    using pixel_type = img::pixel_rgba8;
    for (auto it = img::begin<pixel_type>(dst); it != img::end<pixel_type>(dst); ++it)
    {
        float d = math::sdf::sd_circle(it.local(), 75.0f);
        d = math::sdf::op_annularize(d, 30.0f);

        if (d < 0.0f)
        {
            *it = s(it.frag_coord());
        }
        else
        {
            *it = math::color::WHITE();
        }
    }

    err = img::write_png("../../assets/sampler_test.png", dst);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}