#pragma once

#include <charconv>
#include <cstdio>

#include "vertex/std/string_convert.hpp"
#include "vertex/util/random.hpp"
#define VX_ENABLE_PROFILING
#include "vertex/system/profiler.hpp"

using namespace vx;

//=========================================================================

template <typename T, typename RNG>
T random_integer(RNG& rng)
{
    using limits = std::numeric_limits<T>;
    random::uniform_int_distribution<T> dist(limits::lowest(), limits::max());
    return dist(rng);
}

template <typename F, typename RNG>
F random_normalized(RNG& rng)
{
    using traits = float_traits<F>;
    using U = typename traits::uint_type;

    constexpr U mant_mask = (U{ 1 } << traits::mantissa_bits) - 1;

    random::uniform_int_distribution<int> exp_dist(1, traits::exponent_bias - 1); // unbiased exponent < 0
    random::uniform_int_distribution<U> mant_dist(0, mant_mask);
    random::bernoulli_distribution sign_dist(0.5);

    const U sign = U(sign_dist(rng)) << (traits::mantissa_bits + traits::exponent_bits);
    const U exp = U(exp_dist(rng)) << traits::mantissa_bits;
    const U mant = mant_dist(rng);

    return bit::bit_cast<F>(sign | exp | mant);
}

template <typename F, typename RNG>
F random_subnormal(RNG& rng)
{
    using traits = float_traits<F>;
    using U = typename traits::uint_type;

    constexpr U mant_mask = (U{ 1 } << traits::mantissa_bits) - 1;

    random::uniform_int_distribution<U> mant_dist(1, mant_mask);
    random::bernoulli_distribution sign_dist(0.5);

    const U sign = U(sign_dist(rng)) << (traits::mantissa_bits + traits::exponent_bits);
    const U mant = mant_dist(rng);

    return bit::bit_cast<F>(sign | mant);
}

template <typename F, typename RNG>
F random_large_integer(RNG& rng)
{
    using traits = float_traits<F>;
    using U = typename traits::uint_type;

    constexpr int min_exp = traits::exponent_bias + traits::mantissa_bits;
    constexpr int max_exp = (1 << traits::exponent_bits) - 2; // avoid Inf/NaN
    constexpr U mant_mask = (U{ 1 } << traits::mantissa_bits) - 1;

    random::uniform_int_distribution<int> exp_dist(min_exp, max_exp);
    random::uniform_int_distribution<U> mant_dist(0, mant_mask);
    random::bernoulli_distribution sign_dist(0.5);

    const U sign = U(sign_dist(rng)) << (traits::mantissa_bits + traits::exponent_bits);
    const U exp = U(exp_dist(rng)) << traits::mantissa_bits;
    const U mant = mant_dist(rng);

    return bit::bit_cast<F>(sign | exp | mant);
}

template <typename F, typename RNG>
F random_mixed(RNG& rng)
{
    using traits = float_traits<F>;
    using U = typename traits::uint_type;

    constexpr U mant_mask = (U{ 1 } << traits::mantissa_bits) - 1;

    random::uniform_int_distribution<int> exp_dist(traits::exponent_bias, traits::exponent_bias + traits::mantissa_bits - 1);
    random::uniform_int_distribution<U> mant_dist(0, mant_mask);
    random::bernoulli_distribution sign_dist(0.5);

    const U sign = U(sign_dist(rng)) << (traits::mantissa_bits + traits::exponent_bits);
    const U exp = U(exp_dist(rng)) << traits::mantissa_bits;
    const U mant = mant_dist(rng);

    return bit::bit_cast<F>(sign | exp | mant);
}

enum class float_category
{
    normalized,
    subnormal,
    large_integer,
    mixed
};

template <typename F, typename RNG>
F random_float(
    RNG& rng,
    double normalized_weight,
    double subnormal_weight,
    double large_integer_weight,
    double mixed_weight)
{
    random::discrete_distribution<int> dist{
        normalized_weight,
        subnormal_weight,
        large_integer_weight,
        mixed_weight
    };

    switch (static_cast<float_category>(dist(rng)))
    {
        default:
        case float_category::normalized:
        {
            return random_normalized<F>(rng);
        }
        case float_category::subnormal:
        {
            return random_subnormal<F>(rng);
        }
        case float_category::large_integer:
        {
            return random_large_integer<F>(rng);
        }
        case float_category::mixed:
        {
            return random_mixed<F>(rng);
        }
    }
}

template <typename F, typename RNG>
F random_float_typical(RNG& rng)
{
    return random_float<F>(
        rng,
        85.0, // normalized (e2 < 0)
        0.1,  // subnormal
        2.0,  // large integer
        12.9  // mixed
    );
}

template <typename F, typename RNG>
F random_float_uniform(RNG& rng)
{
    return random_float<F>(
        rng,
        1.0,
        1.0,
        1.0,
        1.0);
}

//=========================================================================

enum class float_format
{
    fixed,
    scientific,
    hex
};

static constexpr size_t buf_size = 5000;

template <float_format F>
constexpr const char* float_format_name()
{
    VX_IF_CONSTEXPR (F == float_format::fixed)
    {
        return "fixed";
    }
    else VX_IF_CONSTEXPR (F == float_format::scientific)
    {
        return "scientific";
    }
    else
    {
        return "hex";
    }
}

template <typename T>
std::string function_name_int(const char* fn)
{
    VX_IF_CONSTEXPR (std::is_same<T, bool>::value)
    {
        return std::string("(bool) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, char>::value)
    {
        return std::string("(char) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, signed char>::value)
    {
        return std::string("(signed char) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, unsigned char>::value)
    {
        return std::string("(unsigned char) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, short>::value)
    {
        return std::string("(short) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, unsigned short>::value)
    {
        return std::string("(unsigned short) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, int>::value)
    {
        return std::string("(int) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, unsigned int>::value)
    {
        return std::string("(unsigned int) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, long>::value)
    {
        return std::string("(long) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, unsigned long>::value)
    {
        return std::string("(unsigned long) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, long long>::value)
    {
        return std::string("(long long) ") + fn;
    }
    else VX_IF_CONSTEXPR (std::is_same<T, unsigned long long>::value)
    {
        return std::string("(unsigned long long) ") + fn;
    }
    else
    {
        VX_STATIC_ASSERT_MSG(std::is_integral<T>::value, "Unsupported integral type");
        return std::string(fn);
    }
}

template <typename T, float_format F>
std::string function_name_float(const char* fn)
{
    const char* fmt = float_format_name<F>();

    VX_IF_CONSTEXPR (std::is_same<T, float>::value)
    {
        return std::string("(float. ") + fmt + ") " + std::string(fn);
    }
    else VX_IF_CONSTEXPR (std::is_same<T, double>::value)
    {
        return std::string("(double. ") + fmt + ") " + std::string(fn);
    }
    else VX_IF_CONSTEXPR (std::is_same<T, long double>::value)
    {
        return std::string("(long double. ") + fmt + ") " + std::string(fn);
    }
    else
    {
        VX_STATIC_ASSERT_MSG(std::is_floating_point<T>::value, "Unsupported floating point type");
        return std::string(fn);
    }
}

// number of repetitions
static constexpr size_t RR = 5000;

#define function_str_int(str) (function_name_int<T>(str))
#define function_str_float(str) (function_name_float<T, Format>(str))

#define start_timer_int(str)   ::vx::profile::_priv::profile_timer timer(function_str_int(str))
#define start_timer_float(str) ::vx::profile::_priv::profile_timer timer(function_str_float(str))

#define stop_timer()      timer.stop()

template <typename T>
using test_fn = size_t (*)(T);

//=========================================================================

template <typename T>
struct profile_std_to_chars_integer
{
    static size_t run(const T v)
    {
        char buf[buf_size] = {};

        start_timer_int("std::to_chars");
        const auto r = std::to_chars(buf, buf + sizeof(buf), v);
        stop_timer();

        return static_cast<size_t>(r.ptr - buf);
    }
};

template <typename T>
struct profile_vx_to_string_integer
{
    static size_t run(const T v)
    {
        char buf[buf_size] = {};

        start_timer_int("vx::to_string");
        const auto r = str::to_string(v, buf, buf_size);
        stop_timer();

        return r.count;
    }
};

//=========================================================================

template <typename T>
struct profile_std_from_chars_integer
{
    static T run(const T v)
    {
        char buf[buf_size] = {};
        const auto res = std::to_chars(buf, buf + sizeof(buf), v);
        T value{};

        start_timer_int("std::from_chars");
        std::from_chars(buf, res.ptr, value);
        stop_timer();

        return value;
    }
};

template <typename T>
struct profile_vx_from_string_integer
{
    static T run(const T v)
    {
        char buf[buf_size] = {};
        const auto res = str::to_string(v, buf, buf_size);
        T value{};

        start_timer_int("vx::from_string");
        str::from_string(buf, res.count, value, 10);
        stop_timer();

        return value;
    }
};

//=========================================================================

template <typename T, template <typename> class Profile>
static size_t run_integer_test_base(size_t R)
{
    size_t n = 0;
    vx::random::gen rng;

    for (size_t r = 0; r < R; ++r)
    {
        const T value = random_integer<T>(rng);
        n += static_cast<size_t>(Profile<T>::run(value));
    }

    return n;
}

template <template <typename> class Profile>
static size_t run_integer_tests(size_t R)
{
    size_t n = 0;

    n += run_integer_test_base<char, Profile>(R);
    n += run_integer_test_base<signed char, Profile>(R);
    n += run_integer_test_base<unsigned char, Profile>(R);
    
    n += run_integer_test_base<short, Profile>(R);
    n += run_integer_test_base<unsigned short, Profile>(R);
    
    n += run_integer_test_base<int, Profile>(R);
    n += run_integer_test_base<unsigned int, Profile>(R);
    
    n += run_integer_test_base<long, Profile>(R);
    n += run_integer_test_base<unsigned long, Profile>(R);

    n += run_integer_test_base<long long, Profile>(R);
    n += run_integer_test_base<unsigned long long, Profile>(R);

    return n;
}

//=========================================================================

template <typename float_format Format>
constexpr std::chars_format std_format_chooser()
{
    VX_IF_CONSTEXPR (Format == float_format::fixed)
    {
        return std::chars_format::fixed;
    }
    else VX_IF_CONSTEXPR (Format == float_format::scientific)
    {
        return std::chars_format::scientific;
    }
    else
    {
        return std::chars_format::hex;
    }
}

template <typename float_format Format>
constexpr str::float_format vx_format_chooser()
{
    VX_IF_CONSTEXPR (Format == float_format::fixed)
    {
        return str::float_format::fixed;
    }
    else VX_IF_CONSTEXPR (Format == float_format::scientific)
    {
        return str::float_format::scientific;
    }
    else
    {
        return str::float_format::hex;
    }
}

template <typename F, typename float_format Format>
constexpr uint32_t precision_chooser()
{
    VX_IF_CONSTEXPR (Format == float_format::fixed)
    {
        return str::_string_convert_priv::float_fixed_default_precision;
    }
    else VX_IF_CONSTEXPR (Format == float_format::scientific)
    {
        return str::_string_convert_priv::float_scientific_default_precision;
    }
    else
    {
        VX_IF_CONSTEXPR (std::is_same<F, float>::value)
        {
            return str::_string_convert_priv::float_hex_default_precision;
        }
        else VX_IF_CONSTEXPR (std::is_same<F, double>::value)
        {
            return str::_string_convert_priv::double_hex_default_precision;
        }
    }
}

//=========================================================================

template <typename T, float_format Format>
struct profile_std_to_chars_float
{
    static size_t run(T v)
    {
        char buf[buf_size] = {};
        std::to_chars_result res;

        start_timer_float("std::to_chars");
        res = std::to_chars(buf, buf + sizeof(buf), v, std_format_chooser<Format>(), precision_chooser<T, Format>());
        stop_timer();

        return static_cast<size_t>(res.ptr - buf);
    }
};

template <typename T, float_format Format>
struct profile_vx_to_string_float
{
    static size_t run(T v)
    {
        const str::float_to_string_format_options<char> fmt{ vx_format_chooser<Format>(), precision_chooser<T, Format>() };

        char buf[buf_size] = {};
        str::to_string_result res;

        start_timer_float("vx::to_string");

        VX_IF_CONSTEXPR (Format == float_format::fixed)
        {
            res = str::write_float_fixed(v, buf, buf_size, fmt);
        }
        else VX_IF_CONSTEXPR (Format == float_format::scientific)
        {
            res = str::write_float_scientific(v, buf, buf_size, fmt);
        }
        else
        {
            res = str::write_float_hex(v, buf, buf_size, fmt);
        }

        stop_timer();
        return res.count;
    }
};

//=========================================================================

template <typename T, float_format Format>
struct profile_std_from_chars_float
{
    static T run(T v)
    {
        char buf[buf_size] = {};
        const auto res = std::to_chars(buf, buf + sizeof(buf), v, std_format_chooser<Format>(), precision_chooser<T, Format>());
        T value{};

        start_timer_float("std::from_chars");
        std::from_chars(buf, res.ptr, value, std_format_chooser<Format>());
        stop_timer();

        return value;
    }
};

template <typename T, float_format Format>
struct profile_vx_from_string_float
{
    static T run(T v)
    {
        const str::float_to_string_format_options<char> to_string_fmt{ vx_format_chooser<Format>(), precision_chooser<T, Format>() };
        const str::float_from_string_format_options<char> from_string_fmt{ vx_format_chooser<Format>() };

        char buf[buf_size] = {};

        const auto res = str::to_string(v, buf, buf_size, to_string_fmt);
        T value{};

        start_timer_float("vx::from_string");
        str::parse_float(buf, res.count, value, from_string_fmt);
        stop_timer();

        return value;
    }
};

//=========================================================================

template <typename T, float_format Format, template <typename, float_format> class Profile>
static size_t run_float_test_base(size_t R)
{
    size_t n = 0;
    random::gen rng;

    for (size_t r = 0; r < R; ++r)
    {
        const T value = random_float_typical<T>(rng);
        n += static_cast<size_t>(Profile<T, Format>::run(value));
    }

    return n;
}

template <float_format Format, template <typename, float_format> class Profile>
static size_t run_float_tests(size_t R)
{
    size_t n = 0;

    n += run_float_test_base<float, Format, Profile>(R);
    n += run_float_test_base<double, Format, Profile>(R);

    return n;
}

//=========================================================================

inline size_t test_std_to_chars(size_t R)
{
    size_t n = 0;
    n += run_integer_tests<profile_std_to_chars_integer>(R);
    n += run_float_tests<float_format::fixed, profile_std_to_chars_float>(R);
    n += run_float_tests<float_format::scientific, profile_std_to_chars_float>(R);
    n += run_float_tests<float_format::hex, profile_std_to_chars_float>(R);
    return n;
}

inline size_t test_vx_to_string(size_t R)
{
    size_t n = 0;
    n += run_integer_tests<profile_vx_to_string_integer>(R);
    n += run_float_tests<float_format::fixed, profile_vx_to_string_float>(R);
    n += run_float_tests<float_format::scientific, profile_vx_to_string_float>(R);
    n += run_float_tests<float_format::hex, profile_vx_to_string_float>(R);
    return n;
}

//=========================================================================

inline size_t test_std_from_chars(size_t R)
{
    size_t n = 0;
    //n += run_integer_tests<profile_std_from_chars_integer>(R);
    n += run_float_tests<float_format::fixed, profile_std_from_chars_float>(R);
    n += run_float_tests<float_format::scientific, profile_std_from_chars_float>(R);
    n += run_float_tests<float_format::hex, profile_std_from_chars_float>(R);
    return n;
}

inline size_t test_vx_from_string(size_t R)
{
    size_t n = 0;
    //n += run_integer_tests<profile_vx_from_string_integer>(R);
    n += run_float_tests<float_format::fixed, profile_vx_from_string_float>(R);
    n += run_float_tests<float_format::scientific, profile_vx_from_string_float>(R);
    n += run_float_tests<float_format::hex, profile_vx_from_string_float>(R);
    return n;
}
