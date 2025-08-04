#include <iostream>

#include "vertex/util/random.hpp"

#include "vertex/system/error.hpp"
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"
#include "vertex/pixel/mipmaps.hpp"

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

    auto surf1 = img1.to_surface<pixel::pixel_format::RGBA_8>();
    auto mipmaps = pixel::generate_mipmaps(surf1);

    for (size_t i = 0; i < mipmaps.size(); ++i)
    {
        const auto& m = mipmaps[i];
        img::image img(m.data(), m.width(), m.height(), img::pixel_format::RGBA_8);

        const std::string filename = "../../assets/mipmap_" + std::to_string(i) + ".png";
        ok = img::write_png(filename, img);
        if (!ok)
        {
            std::cout << err::get().message << std::endl;
            return 1;
        }
    }
}
