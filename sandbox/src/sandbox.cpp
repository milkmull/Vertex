#include <iostream>

#include "vertex/os/random.hpp"

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

    pixel::surface<pixel::pixel_format::R_8> surf(500, 500);

    {
        math::noise_sampler s;
        s.frequency = 0.01f;

        os::get_entropy(reinterpret_cast<uint8_t*>(&s.seed.x), sizeof(float));
        os::get_entropy(reinterpret_cast<uint8_t*>(&s.seed.y), sizeof(float));

        for (auto it = begin(surf); it != end(surf); ++it)
        {
            const auto n = s.simplex_noise(it.local());
            it.set_pixel(math::color(n));
        }
    }

    img::image img(surf.data(), 500, 500, img::pixel_format::R_8);

    {
        ok = img::write_png("../../assets/sdf_test.png", img);
        if (!ok)
        {
            std::cout << err::get().message << std::endl;
        }
    }

}
