#include "sandbox/sandbox.h"

#include "vertex/image/img/image.h"
#include "vertex/image/img/image_sampler.h"
#include "vertex/image/img/image_write.h"

#include "vertex/math/texture/filter_bilinear.h"

int main()
{
    using namespace vx;

    img::error_code err{};
    img::image src("../../assets/treasurechest.png", err);

    //img::image dst(src.width() * 2, src.height() * 2, src.format());
    //
    //math::filter_bilinear(src.raw_data(), src.width(), src.height(), dst.raw_data(), dst.width(), dst.height(), dst.channels());
    //
    //err = img::write_png("../../assets/treasurechest2.png", dst.get_info(), dst.raw_data());
    //std::cout << (int)err;


    if (err == img::error_code::NONE)
    {
        math::recti area = src.get_rect();

        img::image_sampler sampler(src);
    
        img::image dst(src.width() * 4, src.height() * 4, src.format());
    
        for (int y = 0; y < (int)dst.height(); ++y)
        {
            for (int x = 0; x < (int)dst.width(); ++x)
            {
                dst.set_pixel(x, y, sampler.sample_pixel(x - src.width() * 2, y - src.height() * 2));
            }
        }
    
        err = img::write_png("../../assets/sampler_text.png", dst.get_info(), dst.data().data());
    
        std::cout << "image saved with error code: " << (int)err << std::endl;
    }
    else
    {
        std::cout << (int)err << std::endl;
    }

    return 0;
}