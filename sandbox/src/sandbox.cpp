#include <iostream>

#include "vertex/util/random.hpp"

#include "vertex/system/error.hpp"
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"

#include "vertex/math/color/functions/color.hpp"
#include "vertex/math/procedural/sdf2d.hpp"
#include "vertex/math/procedural/noise/noise_sampler.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    bool ok;

    img::image img;
    img::load("../../assets/sunflower.png", img);

    img = img.convert(img::pixel_format::RGB_8);

    {
        ok = img::write_bmp("../../assets/bmp_test.bmp", img);
        if (!ok)
        {
            std::cout << err::get().message << std::endl;
        }
    }

}
