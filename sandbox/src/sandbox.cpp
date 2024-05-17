#include "sandbox/sandbox.h"

#include "vertex/system/error.h"

#include "vertex/image/image.h"
#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/sampler.h"
#include "vertex/pixel/filter/filter_bicubic.h"

#include "vertex/image/fn_transform.h"
#include "vertex/image/fn_color.h"
#include "vertex/image/fn_blit.h"

int main()
{
    using namespace vx;

    bool status;
    img::image i = img::load("../../assets/michael.png", status);

    if (!status)
    {
        VX_LOG_ERROR << error::get_error().message;
        return 1;
    }

    img::image i2(i.width() * 2, i.height() * 2, i.format());

    img::image_sampler_rgba_8 s = img::create_sampler<img::image_pixel_format::PIXEL_FORMAT_RGBA_8>(i);
    s.resolution = math::vec2(0.2);
    
    for (size_t y = 0; y < i2.height(); ++y)
    {
        for (size_t x = 0; x < i2.width(); ++x)
        {
            math::color c = s.sample((float)x / (float)i2.width(), (float)y / (float)i2.height());
            i2.set_pixel(x, y, math::clamp(c));
        }
    }

    status = img::write_png("../../assets/pixel_sampler_test.png", i2);
    if (!status)
    {
        VX_LOG_ERROR << error::get_error().message;
        return 1;
    }

    VX_LOG_INFO << "SUCCESS";

    return 0;
}
