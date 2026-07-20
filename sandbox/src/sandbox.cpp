
#include <charconv>

#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

#include "vertex/std/io.hpp"
#include "vertex/std/string_convert.hpp"
#include "vertex/util/random/rng.hpp"

#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

namespace vx {
namespace random {
namespace bench {

// ------------------------------------------------------------------------------------
// IEEE-754 layout helper (float -> 1/8/23, double -> 1/11/52).
// Mirrors what float_bits<F>::traits presumably already encodes; re-derived here so
// this header is self-contained and only depends on <limits>.
// ------------------------------------------------------------------------------------

template <typename F>
struct ieee754_layout
{
    static_assert(
        std::is_same<F, float>::value || std::is_same<F, double>::value,
        "F must be float or double");

    using bits_type = typename std::conditional<sizeof(F) == 4, uint32_t, uint64_t>::type;

    static constexpr int total_bits = sizeof(F) * 8;
    static constexpr int mantissa_bits = std::numeric_limits<F>::digits - 1; // 23 / 52
    static constexpr int exponent_bits = total_bits - mantissa_bits - 1;     // 8 / 11
    static constexpr int exponent_bias = (1 << (exponent_bits - 1)) - 1;     // 127 / 1023

    static constexpr bits_type mantissa_field_mask = (bits_type(1) << mantissa_bits) - 1;
    static constexpr bits_type exponent_max = (bits_type(1) << exponent_bits) - 1; // 255 / 2047 (all-ones, reserved for inf/nan)
};

template <typename F>
static F bits_to_float(typename ieee754_layout<F>::bits_type bits) noexcept
{
    F out;
    std::memcpy(&out, &bits, sizeof(F));
    return out;
}

// Assembles sign|exponent|mantissa into the raw bit pattern for F.
template <typename F>
static F make_float(typename ieee754_layout<F>::bits_type sign,
    typename ieee754_layout<F>::bits_type e_bits,
    typename ieee754_layout<F>::bits_type m_bits) noexcept
{
    using layout = ieee754_layout<F>;
    using bits_type = typename layout::bits_type;

    const bits_type bits =
        (sign << (layout::total_bits - 1)) |
        (e_bits << layout::mantissa_bits) |
        (m_bits & layout::mantissa_field_mask);

    return bits_to_float<F>(bits);
}

// ------------------------------------------------------------------------------------
// The 4 case generators
// ------------------------------------------------------------------------------------

// Case 1: e_bits == 0  =>  zero or subnormal branch
// (write_float_fixed_impl returns immediately on this branch either way, so we don't
// need to special-case mantissa == 0 vs != 0 unless you want to exercise one or the other.)
template <typename F, typename RNG>
static F random_zero_or_subnormal(RNG& rng)
{
    using layout = ieee754_layout<F>;
    using bits_type = typename layout::bits_type;

    const bits_type sign = rng.template randi_range<bits_type>(0, 1);
    const bits_type m = rng.template randi_range<bits_type>(0, layout::mantissa_field_mask);

    return make_float<F>(sign, bits_type(0), m);
}

// Case 2: e2 < 0  =>  "normal" branch (magnitude in (0, 1), excluding subnormals)
// e_bits in [1, bias - 1]
template <typename F, typename RNG>
static F random_normal_small(RNG& rng)
{
    using layout = ieee754_layout<F>;
    using bits_type = typename layout::bits_type;

    const bits_type sign = rng.template randi_range<bits_type>(0, 1);
    const bits_type e_bits = rng.template randi_range<bits_type>(1, bits_type(layout::exponent_bias - 1));
    const bits_type m = rng.template randi_range<bits_type>(0, layout::mantissa_field_mask);

    return make_float<F>(sign, e_bits, m);
}

// Case 3: e2 >= 0 && shift < 0  =>  "mixed" branch (has both an integer and a fractional part)
// e_bits in [bias, bias + mantissa_bits - 1]
template <typename F, typename RNG>
static F random_mixed_int_and_frac(RNG& rng)
{
    using layout = ieee754_layout<F>;
    using bits_type = typename layout::bits_type;

    const bits_type sign = rng.template randi_range<bits_type>(0, 1);
    const bits_type e_lo = bits_type(layout::exponent_bias);
    const bits_type e_hi = bits_type(layout::exponent_bias + layout::mantissa_bits - 1);
    const bits_type e_bits = rng.template randi_range<bits_type>(e_lo, e_hi);
    const bits_type m = rng.template randi_range<bits_type>(0, layout::mantissa_field_mask);

    return make_float<F>(sign, e_bits, m);
}

// Case 4: shift >= 0  =>  "large integer" branch (no fractional part, possibly huge)
// e_bits in [bias + mantissa_bits, exponent_max - 1]  (exponent_max - 1 to stay finite, avoid inf/nan)
template <typename F, typename RNG>
static F random_large_integer(RNG& rng)
{
    using layout = ieee754_layout<F>;
    using bits_type = typename layout::bits_type;

    const bits_type sign = rng.template randi_range<bits_type>(0, 1);
    const bits_type e_lo = bits_type(layout::exponent_bias + layout::mantissa_bits);
    const bits_type e_hi = bits_type(layout::exponent_max - 1);
    const bits_type e_bits = rng.template randi_range<bits_type>(e_lo, e_hi);
    const bits_type m = rng.template randi_range<bits_type>(0, layout::mantissa_field_mask);

    return make_float<F>(sign, e_bits, m);
}

} // namespace bench
} // namespace random
} // namespace vx

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
        //std::cout << "std e: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_scientific_float(const F f)
{
    vx::str::float_to_string_format_options<wchar_t> fmt;
    fmt.format = vx::str::float_format::scientific;
    fmt.precision = 3;

    wchar_t buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx1 e");
        const auto res = vx::str::write_float_scientific(f, buf, sizeof(buf), fmt);
        //std::cout << "vx e :  " << std::string_view(buf, res.count) << std::endl;
    }
}

template <typename F>
static void std_print_scientific_float_2(const F f)
{
    char buf[5000];

    {
        std::to_chars_result result;

        VX_PROFILE_SCOPE("std e2");
        result = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::scientific, 104);
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
        const size_t n = std::snprintf(const_cast<char*>(buf), sizeof(buf), "%.6f", f);
        //std::cout << "std f: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static VX_NO_INLINE void std_print_fixed_float_2(const F f)
{
    char buf[5000];

    {
        std::to_chars_result result;

        VX_PROFILE_SCOPE("std f2");
        result = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::fixed, 3);
        const size_t n = static_cast<size_t>(result.ptr - buf);
        std::cout << "std f: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static VX_NO_INLINE void vx_print_fixed_float(const F f)
{
    vx::str::float_to_string_format_options fmt;
    fmt.format = vx::str::float_format::fixed;
    fmt.precision = 3;

    char buf[5000] = {};
    {
        VX_PROFILE_SCOPE("vx f");
        const auto res = vx::str::write_float_fixed(f, buf, sizeof(buf), fmt);
        std::cout << "vx f : " << std::string_view(buf, res.count) << std::endl;
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
        //std::cout << "std a: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void std_print_hex_float_2(const F f)
{
    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("std a2");
        std::to_chars_result r = std::to_chars(buf, buf + sizeof(buf), f, std::chars_format::hex, 0);
        const size_t n = static_cast<size_t>(r.ptr - buf);
        std::cout << "std a: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_hex_float(const F f)
{
    vx::str::float_to_string_format_options fmt;
    fmt.format = vx::str::float_format::hex;
    fmt.precision = 0;

    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("vx a");
        const auto res = vx::str::write_float_hex(f, buf, sizeof(buf), fmt);
        std::cout << "vx a : " << std::string_view(buf, res.count) << std::endl;
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
        std::cout << "std g: " << std::string_view(buf, n) << std::endl;
    }
}

template <typename F>
static void vx_print_general_float(const F f)
{
    vx::str::float_to_string_format_options fmt;
    fmt.format = vx::str::float_format::general;
    fmt.uppercase = false;

    char buf[5000] = {};

    {
        VX_PROFILE_SCOPE("vx g");
        const auto res = vx::str::write_float(f, buf, sizeof(buf), fmt);
        std::cout << "vx g : " << std::string_view(buf, res.count) << std::endl;
    }
}

//==============================================================================

template <typename I>
static void vx_print_integer_string(const I i)
{
    vx::str::integer_format_options fmt;
    fmt.base = 5;

    const vx::string s = vx::str::to_string(i, fmt);
    vx::println(s);
}

template <typename F>
static void vx_print_float_string(const F f)
{
    vx::str::float_to_string_format_options fmt;
    fmt.format = vx::str::float_format::general;
    fmt.uppercase = false;

    const vx::string s = vx::str::to_string(f, fmt);
    vx::println(s);
}

//==============================================================================

template <typename F>
static void vx_parse_fixed_float(const F in_value)
{
    char buf[5000] = {};

    vx::str::float_parse_format_options fmt;
    fmt.format = vx::str::float_format::fixed;

    const size_t n = vx::str::write_float_fixed(in_value, buf, 5000, fmt);

    F v;
    const auto res = vx::str::parse_float(buf, n, v, fmt);
    std::cout << "vx : " << v << std::endl;
}

template <typename F>
static void std_parse_fixed_float_2(const F in_value)
{
    char buf[5000] = {};

    const auto r1 = std::to_chars(buf, buf + sizeof(buf), in_value, std::chars_format::fixed, 6);
    const size_t n = static_cast<size_t>(r1.ptr - buf);

    F v;
    const auto r2 = std::from_chars(buf, buf + n, v, std::chars_format::fixed);
    std::cout << "std: " << v << std::endl;
}

//==============================================================================

int main()
{
    VX_PROFILE_START("scientific_float.csv");

    vx::random::gen rng;

    //constexpr float f = FLT_MAX;
    //constexpr char buf[100] = {};
    //constexpr size_t n = make_float_string(f, const_cast<char*>(buf), sizeof(buf));


    for (int i = 0; i < 1; ++i)
    {
        //const auto f = vx::random::bench::random_normal_small<double>(rng);

        //const auto bits = rng.randi<uint32_t>();
        //const auto f = vx::bit::bit_cast<float>(bits);
        //std::cout << f << ' ' << std::hexfloat << f << std::endl;
        const float f = 9.9999f;
        //
        //vx_parse_fixed_float(f);
        //std_parse_fixed_float_2(f);

        //std_print_integer(bits);
        //std_print_integer_2(bits);
        //vx_print_integer(bits);

        //std_print_fixed_float(f);
        std_print_fixed_float_2(f);
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

    //const vx::string s = "9007199254740984";
    //
    //double v;
    //
    //v = 0;
    //const auto r1 = std::from_chars(s.data(), s.data() + s.size(), v, std::chars_format::general);
    //
    //v = 0;
    //const auto r2 = vx::str::from_string(s, v, { vx::str::float_format::general });

    VX_PROFILE_STOP();
    return 0;
}
