
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"
#include "vertex/pixel/surface.hpp"
#include "vertex/pixel/surface_manip.hpp"
#include "vertex/std/error.hpp"

int main()
{
    vx::err::set_hook(vx::err::print_error_hook);

    const char* in_path = R"(C:\Users\Owner\Desktop\Milk_Stuff\2026\mmj\cards\me.png)";
    const char* out_path = R"(C:\Users\Owner\Desktop\Milk_Stuff\2026\mmj\cards\me_hollow.png)";

    vx::img::image img;
    if (!vx::img::load(in_path, img))
    {
        return 1;
    }

    vx::pixel::surface_rgba8 surf = img.to_surface<vx::pixel::pixel_format::rgba_8>();

    constexpr float threshold = 0.2f;
    constexpr float boosted_threshold = 0;
    constexpr vx::math::color color1 = vx::math::color8::red();
    constexpr vx::math::color black = vx::math::color::black();
    constexpr vx::math::color clear = vx::math::color::clear();

    constexpr int drange = 1;

    for (auto it = surf.begin(); it != surf.end(); ++it)
    {
        const size_t x = it.x();
        const size_t y = it.y();

        if (vx::math::equal_approx(it.color(), color1, threshold))
        {
            it.set_pixel(clear);
            continue;
        }
    }

    img = vx::img::image::from_surface(surf);
    if (!vx::img::write_png(out_path, img))
    {
        return 1;
    }

    return 0;
}
