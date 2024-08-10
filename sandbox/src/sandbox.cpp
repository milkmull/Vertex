#include "sandbox/sandbox.h"
#include "vertex/system/profiler.h"

#include "vertex/image/image.h"

#include "vertex/pixel/surface.h"
#include "vertex/pixel/blend.h"
#include "vertex/pixel/blit.h"

using namespace vx;

static void double_loop(pixel::surface& surf)
{
    math::color c = math::color::MAGENTA();

    for (int i = 0; i < 50000; ++i)
    {
        VX_PROFILE_SCOPE("double_loop");

        for (size_t y = 0; y < surf.height(); ++y)
        {
            for (size_t x = 0; x < surf.width(); ++x)
            {
                surf.set_pixel(x, y, c);
            }
        }
    }
}

static void iter_loop(pixel::surface& surf)
{
    math::color c = math::color::MAGENTA();

    for (int i = 0; i < 50000; ++i)
    {
        VX_PROFILE_SCOPE("iter_loop");

        for (auto it = surf.begin(); it != surf.end(); ++it)
        {
            it.set_pixel(c);
        }
    }
}

int main()
{
    bool status;
    pixel::surface sunflower_surf;
    pixel::surface frog_surf;

    status = img::load("../../assets/sunflower.png", sunflower_surf);
    if (!status)
    {
        return -1;
    }

    status = img::load("../../assets/seth_frog_resized.png", frog_surf);
    if (!status)
    {
        return -1;
    }

    //pixel::blend_func blend;
    //pixel::blit(frog_surf, frog_surf.get_rect(), sunflower_surf, { 0, 0 }, blend);

    pixel::surface half_surf(sunflower_surf.width(), sunflower_surf.height() / 2, sunflower_surf.format());
    for (auto it = half_surf.begin(); it != half_surf.end(); ++it)
    {
        half_surf.set_pixel(it.position(), sunflower_surf.get_pixel(it.position()));
    }

    status = img::save_png("../../assets/order_test.png", half_surf);
    if (!status)
    {
        return -1;
    }

    return 0;
}
