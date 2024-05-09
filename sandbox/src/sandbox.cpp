#include "sandbox/sandbox.h"

#include "vertex/image/io_load.h"
#include "vertex/image/io_write.h"
#include "vertex/image/iterator.h"
#include "vertex/image/fn_edit.h"
#include "vertex/math/sample/sdf.h"
#include "vertex/math/sample/filter/filter_bicubic.h"

int main()
{
    using namespace vx;

    bool status;
    img::image i = img::load("../../assets/michael.png", status);

    using pixel_type = pixel::pixel_rgb_332;
    using channel_type = typename pixel_type::channel_type;

    std::vector<pixel_type> surf(i.width() * i.height());

    for (size_t y = 0; y < i.height(); ++y)
    {
        for (size_t x = 0; x < i.width(); ++x)
        {
            surf[i.width() * y + x] = i.get_pixel(x, y);
        }
    }

    std::vector<pixel_type> surf2(i.width() * i.height() * 4);
    
    pixel_type::pixel_type masks[4] = {
        pixel_type::info.channels.mask.r,
        pixel_type::info.channels.mask.g,
        pixel_type::info.channels.mask.b,
        pixel_type::info.channels.mask.a
    };
    
    uint8_t shifts[4] = {
        pixel_type::info.channels.shift.r,
        pixel_type::info.channels.shift.g,
        pixel_type::info.channels.shift.b,
        pixel_type::info.channels.shift.a
    };
    
    math::filter_bicubic<pixel_type::pixel_type>(
        (uint8_t*)surf.data(), i.width(), i.height(), (uint8_t*)surf2.data(), i.width() * 2, i.height() * 2,
        pixel_type::info.channel_count, masks, shifts
    );

    //math::filter_bicubic<uint8_t>(
    //    (uint8_t*)surf.data(), i.width(), i.height(),
    //    (uint8_t*)surf2.data(), i.width() * 2, i.height() * 2,
    //    pixel_type::info.channel_count
    //);

    img::image i2(i.width() * 2, i.height() * 2, i.format());

    for (size_t y = 0; y < i2.height(); ++y)
    {
        for (size_t x = 0; x < i2.width(); ++x)
        {
            i2.set_pixel(x, y, surf2[i2.width() * y + x]);
        }
    }

    //auto it = img::begin<img::PIXEL_FORMAT_RGBA_8>(i);
    //while (it != img::end<img::PIXEL_FORMAT_RGBA_8>(i))
    //{
    //    const float circle = math::sdf::sd_circle(it.local(), math::cmin(it.resolution()) / 2);
    //    if (circle <= 0)
    //    {
    //        *it = math::color::RED();
    //    }
    //    else
    //    {
    //        *it = math::color::BLUE();
    //    }
    //
    //    ++it;
    //}

    status = img::write_png("../../assets/pixel_resize_test.png", i2);

    VX_LOG_INFO << "Status: " << status;

    return 0;
}
