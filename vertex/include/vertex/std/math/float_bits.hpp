#pragma once

#include <climits>

#include "vertex/std/math/limits.hpp"
#include "vertex/std/math/uint_n.hpp"

namespace vx {

namespace _float_bits_priv {

// Returns floor(log_10(2^e)). Requires 0 <= e <= 1650.
// The first value this approximation fails for is 2^1651 > 10^297.
inline constexpr uint32_t log10_pow2(int e) noexcept
{
    // The first value this approximation fails for is 2^1651 which is just greater than 10^297.
    VX_ASSERT(e >= 0);
    VX_ASSERT(e <= 1650);
    return static_cast<uint32_t>((static_cast<uint64_t>(e) * 0x13441350fbdll) >> 42);
}

// Returns floor(log_10(5^e)). Requires 0 <= e <= 2620.
// The first value this approximation fails for is 5^2621 > 10^1832.
inline constexpr int log10_pow5(int e) noexcept
{
    // The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
    assert(e >= 0);
    assert(e <= 2620);
    return (static_cast<uint32_t>(e) * 732923u) >> 20;
}

} // namespace _float_bits_priv

//==============================================================================
// float traits
//==============================================================================

template <typename F>
struct float_traits;

template <>
struct float_traits<float> : numeric_limits<float>
{
    using uint_type = uint32_t;

    static constexpr int sign_bits = 1;
    static constexpr int storage_bits = sizeof(float) * CHAR_BIT;
    static constexpr int mantissa_bits = digits - 1;
    static constexpr int exponent_bits = storage_bits - sign_bits - mantissa_bits;
    static constexpr bool explicit_integer_bit = false;

    static constexpr int exponent_bias = max_exponent - 1;
    static constexpr int filled_exponent = (1 << exponent_bits) - 1;

    // shifts
    static constexpr int sign_shift = storage_bits - sign_bits;
    static constexpr int exponent_shift = mantissa_bits;
    static constexpr int mantissa_shift = 0;

    // masks
    static constexpr uint_type sign_mask = uint_type(1) << (storage_bits - 1);
    static constexpr uint_type exponent_mask = uint_type(filled_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_mask = (uint_type(1) << mantissa_bits) - 1;
    static constexpr uint_type normal_mantissa_mask = (uint_type(1) << digits) - 1;
    static constexpr uint_type quiet_nan_mask = uint_type(1) << (mantissa_bits - 1);
};

template <>
struct float_traits<double> : numeric_limits<double>
{
    using uint_type = uint64_t;

    static constexpr size_t sign_bits = 1;
    static constexpr size_t storage_bits = sizeof(double) * CHAR_BIT;
    static constexpr size_t mantissa_bits = digits - 1;
    static constexpr size_t exponent_bits = storage_bits - sign_bits - mantissa_bits;
    static constexpr bool explicit_integer_bit = false;

    static constexpr int exponent_bias = max_exponent - 1;
    static constexpr int filled_exponent = (1 << exponent_bits) - 1;

    // shifts
    static constexpr size_t sign_shift = storage_bits - sign_bits;
    static constexpr size_t exponent_shift = mantissa_bits;
    static constexpr size_t mantissa_shift = 0;

    // masks
    static constexpr uint_type sign_mask = uint_type(1) << (storage_bits - 1);
    static constexpr uint_type exponent_mask = uint_type(filled_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_mask = (uint_type(1) << mantissa_bits) - 1;
    static constexpr uint_type normal_mantissa_mask = (uint_type(1) << digits) - 1;
    static constexpr uint_type quiet_nan_mask = uint_type(1) << (mantissa_bits - 1);
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
        return (bits & traits::sign_mask) != 0;
    }

    constexpr uint_type exponent() const noexcept
    {
        return (bits & traits::exponent_mask) >> traits::mantissa_bits;
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
        return (bits & traits::mantissa_mask);
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
        return exponent() != 0 && exponent() != traits::filled_exponent;
    }

    constexpr bool is_subnormal() const noexcept
    {
        return exponent() == 0 && mantissa() != 0;
    }

    constexpr bool is_finite() const noexcept
    {
        return exponent() != traits::filled_exponent;
    }

    constexpr bool is_inf() const noexcept
    {
        return exponent() == traits::filled_exponent && mantissa() == 0;
    }

    constexpr bool is_nan() const noexcept
    {
        return exponent() == traits::filled_exponent && mantissa() != 0;
    }

    constexpr bool is_quiet_nan() const noexcept
    {
        return is_nan() && ((mantissa() & traits::quiet_nan_mask) != 0);
    }

    constexpr bool is_signaling_nan() const noexcept
    {
        return is_nan() && ((mantissa() & traits::quiet_nan_mask) == 0);
    }
};

} // namespace vx
