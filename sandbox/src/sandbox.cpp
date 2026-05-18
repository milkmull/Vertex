
#define VX_ENABLE_PROFILING
#include "vertex/std/string_convert.hpp"
#include "vertex/system/profiler.hpp"

static void std_print_fixed_float(const float f)
{
    char buf[200] = {};
    {
        VX_PROFILE_SCOPE("std");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.100f", f);
        //std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

static void vx_print_fixed_float(const float f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 100;

    char buf[200] = {};
    {
        VX_PROFILE_SCOPE("vx");
        const size_t n = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        //std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

int main()
{
    VX_PROFILE_START("fixed_float.csv");

    for (int i = 0; i < 1000; ++i)
    {
        const float f = static_cast<float>(i) / 1000.0;
        std_print_fixed_float(f);
        vx_print_fixed_float(f);
    }

    VX_PROFILE_STOP();
    return 0;
}
