#include <iostream>

#include "vertex/util/random.hpp"

#include "vertex/system/error.hpp"
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"
#include "vertex/pixel/surface_transform.hpp"
#include "vertex/pixel/draw.hpp"

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
    pixel::draw::draw_rect(surf1, { 100, 100, 50, 50 }, math::color::red());

    img::image img2 = img::image::from_surface(surf1);
    ok = img::write_png("../../assets/tester_out.png", img2);
    if (!ok)
    {
        std::cout << err::get().message << std::endl;
        return 1;
    }
}
