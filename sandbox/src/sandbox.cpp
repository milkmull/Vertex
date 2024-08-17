#include "sandbox/sandbox.h"

#define VX_ENABLE_PROFILING 1

#include "vertex/system/profiler.h"
#include "vertex/pixel/surface.h"

using namespace vx;

static void timer_test()
{
    pixel::surface s(300, 300, pixel::pixel_format::RGBA_8);

    for (int i = 0; i < 2000; ++i)
    {
        if (i % 2)
        {
            VX_PROFILE_SCOPE1("timer1");

            for (auto it = s.begin(); it != s.end(); ++it)
            {
                it.set_pixel(math::color::RED());
            }
        }
        else
        {
            VX_PROFILE_SCOPE2("timer2");

            for (auto it = s.begin(); it != s.end(); ++it)
            {
                it.set_pixel(math::color::RED());
            }
        }
    }
}

int main()
{
    profile::profiler::begin_session("sesh", "../../assets/results.txt");

    timer_test();

    return 0;
}
