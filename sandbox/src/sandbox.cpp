
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include "vertex/std/string_convert.hpp"
#include "vertex/util/random/rng.hpp"

constexpr size_t make_float_string(const float f, char* buf, const size_t buf_size)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 10;
    return vx::str::write_float_fixed(f, buf, buf_size, fmt);
}

template <typename F>
static void std_print_scientific_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%e", f);
        std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_fixed_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.1f", f);
        std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 500;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx");
        const size_t n = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float_2(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 1;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx2");
        const size_t n = vx::str::write_float_fixed_3(f, buf, sizeof(buf), fmt);
        std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_scientific_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::scientific;
    fmt.precision = 0;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx");
        const size_t n = vx::str::write_float_fixed_2(f, buf, sizeof(buf), fmt);
        //std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

int main()
{
    VX_PROFILE_START("fixed_float.csv");

    vx::random::gen rng;

    //constexpr float f = FLT_MAX;
    //constexpr char buf[100] = {};
    //constexpr size_t n = make_float_string(f, const_cast<char*>(buf), sizeof(buf));



    for (int i = 0; i < 1; ++i)
    {
        //const uint32_t bits = rng();
        //const float f = vx::bit::bit_cast<float>(bits);
        const float f = 9.99f;
        std::cout << f << std::endl;
        std_print_fixed_float(f);
        vx_print_fixed_float_2(f);
    }

    VX_PROFILE_STOP();
    return 0;
}
