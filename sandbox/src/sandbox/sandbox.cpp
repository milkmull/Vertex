#include "sandbox/sandbox.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/image/pixel_iterator.h"
#include "vertex/image/fn_color_adjust.h"

#include "vertex/math/texture/sdf.h"
#include "vertex/math/color/blend.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image fg = img::load("../../assets/michael.png", err);
    assert(err == img::error_code::NONE);

    img::image bg = img::load("../../assets/treasurechest.png", err);
    assert(err == img::error_code::NONE);

    img::image dst(fg.get_info());

    img::sampler fg_sampler;
    fg_sampler.resolution /= 4;
    fg_sampler.xwrap = img::image_wrap::MIRRORED_REPEAT;

    img::sampler bg_sampler;
    bg_sampler.resolution /= 2;
    bg_sampler.xwrap = img::image_wrap::MIRRORED_REPEAT;
    bg_sampler.ywrap = img::image_wrap::MIRRORED_REPEAT;

    math::blend_func blend;
    blend.src_blend = blend_mode::ONE;
    blend.dst_blend = blend_mode::ONE;

    using pixel_type = img::pixel_rgba8;
    for (auto it = img::begin<pixel_type>(dst); it != img::end<pixel_type>(dst); ++it)
    {
        math::vec2 p = it.local();

        float d = math::sdf::sd_circle(p, 50.0f);
        d = math::sdf::op_annularize(d, 30.0f);

        if (d <= 0.0f)
        {
            math::color c1 = fg_sampler.sample(fg, it.uv());
            math::color c2 = bg_sampler.sample(bg, it.uv());

            it.set_color(blend(c1, c2));
            
        }
        else
        {
            it.set_color(bg_sampler.sample(bg, it.uv()));
        }
    }

    //img::replace_color(dst, math::color::BLACK(), math::color(0.2f, 0.8f, 0.75f), 0.5f);

    err = img::write_png("../../assets/sampler_test.png", dst);
    std::cout << "image saved with error: " << (int)err << std::endl;

    return 0;
}