#include <iostream>

#include "vertex/util/random.hpp"

#include "vertex/system/error.hpp"
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"
#include "vertex/pixel/sampler.hpp"
#include "vertex/pixel/blit.hpp"

#include "vertex/math/color/functions/color.hpp"
#include "vertex/math/color/blend.hpp"
#include "vertex/math/color/util.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    bool ok;

    img::image img1;
    ok = img::load("../../assets/treasurechest.png", img1);
    if (!ok)
    {
        std::cout << err::get().message << std::endl;
        return 1;
    }

    img::image img2;
    ok = img::load("../../assets/sunflower.png", img2);
    if (!ok)
    {
        std::cout << err::get().message << std::endl;
        return 1;
    }

    auto surf1 = img1.to_surface<pixel::pixel_format::RGBA_8>();
    auto surf2 = img2.to_surface<pixel::pixel_format::RGBA_8>();

    math::blend_func_separate blender;
    blender.src_alpha_blend = math::blend_mode::DST_ALPHA;
    pixel::blit(surf1, surf1.get_rect(), surf2, (img2.size() - img1.size()) / 2, blender);

    img::image img3(surf2.data(), surf2.width(), surf2.height(), img::pixel_format::RGBA_8);

    ok = img::write_png("../../assets/blit_test_2025.png", img3);
    if (!ok)
    {
        std::cout << err::get().message << std::endl;
        return 1;
    }
}
