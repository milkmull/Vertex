#pragma once

#include <cstdint>
#include <limits>

#include "vertex/std/string_convert.hpp"

using namespace vx;

inline constexpr float float_inf = std::numeric_limits<float>::infinity();
inline constexpr float float_nan = std::numeric_limits<float>::quiet_NaN();
inline constexpr float float_nan_payload = __builtin_nanf("1729");

inline constexpr double double_inf = std::numeric_limits<double>::infinity();
inline constexpr double double_nan = std::numeric_limits<double>::quiet_NaN();
inline constexpr double double_nan_payload = __builtin_nan("1729");

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
