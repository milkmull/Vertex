#pragma once

#include <climits>

#include "vertex/std/math/limits.hpp"
#include "vertex/std/math/uint_n.hpp"

namespace vx {

//==============================================================================
// float traits
//==============================================================================

template <typename F>
struct float_traits;

template <>
struct float_traits<float> : numeric_limits<float>
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
struct float_traits<double> : numeric_limits<double>
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

//==============================================================================
// float bits
//==============================================================================

template <typename F>
struct float_bits;

template <typename F>
struct float_bits
{
    VX_STATIC_ASSERT_MSG((std::is_floating_point<F>::value), "float_bits: F must be a floating-point type");
    using traits = float_traits<F>;
    using uint_type = typename traits::uint_type;

    uint_type bits;

    constexpr float_bits() noexcept
        : bits(0)
    {}

    constexpr explicit float_bits(F f) noexcept
        : bits(bit::bit_cast<uint_type>(f))
    {}

    constexpr explicit float_bits(uint_type bits) noexcept
        : bits(bits)
    {}

    constexpr bool sign() const noexcept
    {
        return (bits & traits::sign_bit_mask) != 0;
    }

    constexpr uint_type exponent() const noexcept
    {
        return (bits & traits::exponent_field_mask) >> traits::mantissa_bits;
    }

    constexpr int true_exponent() const noexcept
    {
        if (is_subnormal())
        {
            return 1 - traits::exponent_bias;
        }
        return unbiased_exponent();
    }

    constexpr uint_type mantissa() const noexcept
    {
        return (bits & traits::mantissa_field_mask);
    }

    constexpr int unbiased_exponent() const noexcept
    {
        return static_cast<int>(exponent()) - traits::exponent_bias;
    }

    constexpr bool is_zero() const noexcept
    {
        return exponent() == 0 && mantissa() == 0;
    }

    constexpr bool is_negative() const noexcept
    {
        return sign();
    }

    constexpr bool is_normal() const noexcept
    {
        return exponent() != 0 && exponent() != traits::inf_nan_exponent;
    }

    constexpr bool is_subnormal() const noexcept
    {
        return exponent() == 0 && mantissa() != 0;
    }

    constexpr bool is_finite() const noexcept
    {
        return exponent() != traits::inf_nan_exponent;
    }

    constexpr bool is_inf() const noexcept
    {
        return exponent() == traits::inf_nan_exponent && mantissa() == 0;
    }

    constexpr bool is_nan() const noexcept
    {
        return exponent() == traits::inf_nan_exponent && mantissa() != 0;
    }

    constexpr bool is_quiet_nan() const noexcept
    {
        return is_nan() && ((mantissa() & traits::quiet_nan_bit_mask) != 0);
    }

    constexpr bool is_signaling_nan() const noexcept
    {
        return is_nan() && ((mantissa() & traits::quiet_nan_bit_mask) == 0);
    }
};

} // namespace vx
