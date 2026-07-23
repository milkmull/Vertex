#pragma once

#include <limits>

#include "vertex/config/language_config.hpp"
#include "vertex/std/string_convert.hpp"
#include "vertex/util/random.hpp"
#include "vertex/std/math/float_bits.hpp"

#if VX_HAS_BUILTIN(__builtin_nanf)
#define VX_HAS_NAN_PAYLOAD
#endif

using namespace vx;

constexpr str::from_string_error fse_inv_arg = str::from_string_error::invalid_argument;
constexpr str::from_string_error fse_out_ran = str::from_string_error::out_of_range;
constexpr str::from_string_error fse_none = str::from_string_error::none;
constexpr str::to_string_error tse_none = str::to_string_error::none;

inline constexpr float float_inf = std::numeric_limits<float>::infinity();
inline constexpr float float_nan = std::numeric_limits<float>::quiet_NaN();

inline constexpr double double_inf = std::numeric_limits<double>::infinity();
inline constexpr double double_nan = std::numeric_limits<double>::quiet_NaN();

#if defined(VX_HAS_NAN_PAYLOAD)
inline constexpr float float_nan_payload = __builtin_nanf("1729");
inline constexpr double double_nan_payload = __builtin_nan("1729");
#endif

//==============================================================================

template <typename F, typename C>
struct float_from_string_test_case
{
    constexpr float_from_string_test_case(const C* input_, str::float_format fmt_, size_t count, str::from_string_error err_, F value)
        : input(input_), fmt{ fmt_ }, correct_count(count), correct_err(err_), correct_value(value)
    {}

    const C* input;
    str::float_from_string_format_options<C> fmt;
    size_t correct_count;
    str::from_string_error correct_err;
    F correct_value;
};

template <typename F, typename C>
struct float_to_string_test_case
{
    constexpr float_to_string_test_case(F value_, str::float_format fmt_, const C* correct_)
        : value(value_), fmt{ fmt_ }, correct(correct_)
    {}

    constexpr float_to_string_test_case(F value_, str::float_format fmt_, int precision, const C* correct_)
        : value(value_), fmt{ fmt_, static_cast<size_t>(precision) }, correct(correct_)
    {}

    F value;
    str::float_to_string_format_options<C> fmt;
    const C* correct;
};

//==============================================================================

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
