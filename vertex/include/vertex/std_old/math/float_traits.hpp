#pragma once

#include <cstdint>
#include <limits>

namespace vx {

//==============================================================================
// float traits
//==============================================================================

template <typename F>
struct float_traits;

template <>
struct float_traits<float> : std::numeric_limits<float>
{
    using uint_type = uint32_t;

    static constexpr size_t sign_bits = 1;
    static constexpr size_t total_bits = sizeof(float) * CHAR_BIT;
    static constexpr size_t full_mantissa_bits = digits;
    static constexpr size_t mantissa_bits = full_mantissa_bits - 1;
    static constexpr size_t exponent_bits = total_bits - sign_bits - mantissa_bits;
    static constexpr bool has_explicit_leading_bit = false;

    static constexpr int exponent_bias = max_exponent - 1;
    static constexpr int inf_nan_exponent = (1 << exponent_bits) - 1;

    // shifts
    static constexpr size_t sign_bit_shift = total_bits - sign_bits;
    static constexpr size_t exponent_field_shift = mantissa_bits;
    static constexpr size_t mantissa_field_shift = 0;

    // masks
    static constexpr uint_type sign_bit_mask = uint_type(1) << (total_bits - 1);
    static constexpr uint_type exponent_field_mask = uint_type(inf_nan_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_field_mask = (uint_type(1) << mantissa_bits) - 1;
    static constexpr uint_type mantissa_with_implicit_bit_mask = (uint_type(1) << digits) - 1;
    static constexpr uint_type quiet_nan_bit_mask = uint_type(1) << (mantissa_bits - 1);
};

template <>
struct float_traits<double> : std::numeric_limits<double>
{
    using uint_type = uint64_t;

    static constexpr size_t sign_bits = 1;
    static constexpr size_t total_bits = sizeof(double) * CHAR_BIT;
    static constexpr size_t mantissa_bits = digits - 1;
    static constexpr size_t full_mantissa_bits = digits;
    static constexpr size_t exponent_bits = total_bits - sign_bits - mantissa_bits;
    static constexpr bool has_explicit_leading_bit = false;

    static constexpr int exponent_bias = max_exponent - 1;
    static constexpr int inf_nan_exponent = (1 << exponent_bits) - 1;

    // shifts
    static constexpr size_t sign_bit_shift = total_bits - sign_bits;
    static constexpr size_t exponent_field_shift = mantissa_bits;
    static constexpr size_t mantissa_shift = 0;

    // masks
    static constexpr uint_type sign_bit_mask = uint_type(1) << (total_bits - 1);
    static constexpr uint_type exponent_field_mask = uint_type(inf_nan_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_field_mask = (uint_type(1) << mantissa_bits) - 1;
    static constexpr uint_type mantissa_with_implicit_bit_mask = (uint_type(1) << digits) - 1;
    static constexpr uint_type quiet_nan_bit_mask = uint_type(1) << (mantissa_bits - 1);
};

} // namespace vx
