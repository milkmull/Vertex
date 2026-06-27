
#include <charconv>

#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include "vertex/std/string_convert.hpp"
#include "vertex/util/random/rng.hpp"

template <typename I>
static void std_print_integer(const I v)
{
    char buf[5000] = {};

    {
        int n = 0;
        VX_PROFILE_SCOPE("std i");

        VX_IF_CONSTEXPR (std::is_signed<I>::value)
        {
            VX_IF_CONSTEXPR (sizeof(I) <= sizeof(int))
            {
                n = std::snprintf(buf, sizeof(buf), "%d", static_cast<int>(v));
            }
            else VX_IF_CONSTEXPR (sizeof(I) <= sizeof(long))
            {
                n = std::snprintf(buf, sizeof(buf), "%ld", static_cast<long>(v));
            }
            else VX_IF_CONSTEXPR (sizeof(I) <= sizeof(long long))
            {
                n = std::snprintf(buf, sizeof(buf), "%lld", static_cast<long long>(v));
            }
        }
        else
        {
            VX_IF_CONSTEXPR (sizeof(I) <= sizeof(unsigned int))
            {
                n = std::snprintf(buf, sizeof(buf), "%u", static_cast<unsigned int>(v));
            }
            else VX_IF_CONSTEXPR (sizeof(I) <= sizeof(unsigned long))
            {
                n = std::snprintf(buf, sizeof(buf), "%lu", static_cast<unsigned long>(v));
            }
            else VX_IF_CONSTEXPR (sizeof(I) <= sizeof(unsigned long long))
            {
                n = std::snprintf(buf, sizeof(buf), "%llu", static_cast<unsigned long long>(v));
            }
        }

        //std::cout << "std i: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename I>
static void std_print_integer_2(const I v)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std i2");
        auto r = std::to_chars(buf, buf + sizeof(buf), v);
        const size_t n = static_cast<size_t>(r.ptr - buf);
        //std::cout << "std i2: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename I>
static void vx_print_integer(const I v)
{
    vx::str::integer_format_options fmt;
    fmt.base = 10;
    fmt.uppercase = false;
    fmt.force_sign = false;

    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("vx i");
        const size_t n = vx::str::write_integer(v, buf, sizeof(buf), fmt);
        //std::cout << "vx i : " << std::string_view(buf, n) << std::endl;
    }
}

//==============================================================================

template <typename F>
static void std_print_scientific_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std e");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.6e", f);
        std::cout << "std e: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_scientific_float(const F f)
{
    vx::str::float_format_options fmt;
    fmt.format = vx::str::float_format::scientific;
    fmt.precision = 6;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx1 e");
        const size_t n = vx::str::write_float_scientific(f, buf, sizeof(buf), fmt);
        std::cout << "vx e:  " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_scientific_float_2(const F f)
{
    char buf[5000];

    {
        std::to_chars_result result;

        VX_PROFILE_SCOPE("std e2");
        result = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::scientific, 6);
        const size_t n = static_cast<size_t>(result.ptr - buf);
        std::cout << "std e2: " << std::string_view(buf, n) << std::endl;
    }
}

//==============================================================================

template <typename F>
static void std_print_fixed_float(const F f)
{
    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("std f1");
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.65f", f);
        std::cout << "std f: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_fixed_float_2(const F f)
{
    char buf[5000];

    {
        std::to_chars_result result;

        VX_PROFILE_SCOPE("std f2");
        result = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::fixed, 65);
        const size_t n = static_cast<size_t>(result.ptr - buf);
        std::cout << "std f: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_fixed_float(const F f)
{
    vx::str::float_format_options fmt;
    fmt.format = vx::str::float_format::fixed;
    fmt.precision = 65;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx f");
        const size_t n = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        //std::cout << "vx f : " << std::string_view(buf, n) << std::endl;
    }
}

//==============================================================================

template <typename F>
static void std_print_hex_float(const F f)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std a");
        const size_t n = std::snprintf(buf, sizeof(buf), "%.60a", f);
        std::cout << "std a: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_hex_float_2(const F f)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std a2");
        std::to_chars_result r = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::hex, 60);
        const size_t n = static_cast<size_t>(r.ptr - buf);
        std::cout << "std a: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_hex_float(const F f)
{
    vx::str::float_format_options fmt;
    fmt.format = vx::str::float_format::hex;
    fmt.precision = 60;
    fmt.uppercase = false;

    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("vx a");
        const size_t n = vx::str::write_float_hex(f, buf, sizeof(buf), fmt);
        std::cout << "vx a : " << std::string_view(buf, n) << std::endl;
    }
}

//==============================================================================

template <typename F>
static void std_print_general_float(const F f)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std g");
        const size_t n = std::snprintf(buf, sizeof(buf), "%g", f);
        //std::cout << "std g: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_general_float_2(const F f)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std g2");
        std::to_chars_result r = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::general);
        const size_t n = static_cast<size_t>(r.ptr - buf);
        //std::cout << "std g: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_general_float(const F f)
{
    vx::str::float_format_options fmt;
    fmt.format = vx::str::float_format::general;
    fmt.uppercase = false;

    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("vx g");
        const size_t n = vx::str::write_float(f, buf, sizeof(buf), fmt);
        //std::cout << "vx g : " << std::string_view(buf, n) << std::endl;
    }
}

//==============================================================================

template <typename I>
static void vx_print_integer_string(const I i)
{
    vx::str::integer_format_options fmt;
    fmt.base = 5;

    const vx::string s = vx::str::to_string(i, fmt);
    std::cout << s << std::endl;
}

template <typename F>
static void vx_print_float_string(const F f)
{
    vx::str::float_format_options fmt;
    fmt.format = vx::str::float_format::general;
    fmt.uppercase = false;

    const vx::string s = vx::str::to_string(f, fmt);
    std::cout << s << std::endl;
}

//==============================================================================

int main()
{
    VX_PROFILE_START("scientific_float.csv");

    vx::random::gen rng;

    //constexpr float f = FLT_MAX;
    //constexpr char buf[100] = {};
    //constexpr size_t n = make_float_string(f, const_cast<char*>(buf), sizeof(buf));


    for (int i = 0; i < 20000; ++i)
    {
        const auto bits = rng.randi<uint64_t>();
        const auto f = vx::bit::bit_cast<double>(bits);
        //std::cout << f << ' ' << std::endl;
        //const float f = FLT_TRUE_MIN;

        //std_print_integer(bits);
        //std_print_integer_2(bits);
        //vx_print_integer(bits);

        //std_print_fixed_float(f);
        //std_print_fixed_float_2(f);
        vx_print_fixed_float(f);

        //
        //std_print_scientific_float(f);
        //std_print_scientific_float_2(f);
        //vx_print_scientific_float(f);
        //
        //std_print_hex_float(f);
        //std_print_hex_float_2(f);
        //vx_print_hex_float(f);
        //
        //std_print_general_float(f);
        //std_print_general_float_2(f);
        //vx_print_general_float(f);

        //vx_print_integer_string(static_cast<int64_t>(bits));
        //vx_print_float_string(f);
    }

    VX_PROFILE_STOP();
    return 0;
}
