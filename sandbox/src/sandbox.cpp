#include <iostream>

#define VX_ENABLE_PROFILING 1
#include "vertex/system/profiler.hpp"

#include "vertex/math/math.hpp"
#include "vertex/math/procedural/noise/perlin_noise.hpp"
#include "vertex/util/time/timer.hpp"

using namespace vx;

static time::time_point test()
{
    time::timer t;
    
    {
        t.start();
        const auto x = math::perlin_noise(math::vec4{ 1, 2, 3, 4 });
        t.stop();
    }

    return t.elapsed_time();
}

int main(int argc, char* argv[])
{
    double elapsed = 0.0;

    for (double i = 0; i < 1000000000; ++i)
    {
        const auto tp = test();
        elapsed = ((i * elapsed) + tp.as_float_nanoseconds()) / (i + 1);
    }

    std::cout << elapsed;
}
