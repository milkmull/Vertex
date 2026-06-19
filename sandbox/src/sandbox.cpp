
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include "vertex/std/string_convert.hpp"
#include "vertex/util/random/rng.hpp"

template <typename F>
static void std_print_scientific_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std e");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.6e", f);
        //std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_scientific_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::scientific;
    fmt.precision = 6;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx e");
        const size_t n = vx::str::write_float_scientific(f, buf, sizeof(buf), fmt);
        //std::cout << "vx:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_fixed_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std f");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.6f", f);
        //std::cout << "std: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float(const F f)
{
    vx::str::numeric_format_options fmt;
    fmt.format = vx::str::numeric_format::fixed;
    fmt.precision = 6;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx3 f");
        const size_t n = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        //std::cout << "vx3: " << std::string_view(buf, n) << std::endl;
    }
}

int main()
{
    VX_PROFILE_START("scientific_float.csv");

    vx::random::gen rng;

    //constexpr float f = FLT_MAX;
    //constexpr char buf[100] = {};
    //constexpr size_t n = make_float_string(f, const_cast<char*>(buf), sizeof(buf));



    for (int i = 0; i < 10000; ++i)
    {
        const uint32_t bits = rng.randi<uint32_t>();
        const auto f = vx::bit::bit_cast<float>(bits);
        //std::cout << f << std::endl;
        //const float f = 1468.18f;

        std_print_fixed_float(f);
        vx_print_fixed_float(f);

        std_print_scientific_float(f);
        vx_print_scientific_float(f);
    }

    VX_PROFILE_STOP();
    return 0;
}
