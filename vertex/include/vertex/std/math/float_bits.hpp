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
    return (static_cast<uint32_t>(e) * 78913u) >> 18;
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

    // masks
    static constexpr uint_type sign_mask = uint_type(1) << (storage_bits - 1);
    static constexpr uint_type exponent_mask = uint_type(filled_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_mask = (uint_type(1) << mantissa_bits) - 1;
};

template <>
struct float_traits<double>
{
    using uint_type = uint64_t;
    using lim = numeric_limits<double>;

    static constexpr size_t sign_bits = 1;
    static constexpr size_t storage_bits = sizeof(double) * CHAR_BIT;

    static constexpr size_t mantissa_bits = lim::digits - 1;
    static constexpr size_t exponent_bits = storage_bits - sign_bits - mantissa_bits;
    static constexpr bool explicit_integer_bit = false;

    static constexpr int exponent_bias = lim::max_exponent - 1;
    static constexpr int true_min_exponent10 = lim::min_exponent10 - (static_cast<int>(mantissa_bits) - 1);
    static constexpr int true_max_exponent10 = lim::max_exponent10;
    static constexpr int filled_exponent = (1 << exponent_bits) - 1;

    // masks
    static constexpr uint_type sign_mask = uint_type(1) << (storage_bits - 1);
    static constexpr uint_type exponent_mask = uint_type(filled_exponent) << mantissa_bits;
    static constexpr uint_type mantissa_mask = (uint_type(1) << mantissa_bits) - 1;
};

//==============================================================================
// float bits
//==============================================================================

template <typename F>
struct float_bits;

template <typename F, VX_REQUIRES(std::is_floating_point<F>::value)>
inline constexpr float_bits<F> make_float_bits(F value) noexcept
{
    using uint_type = typename float_traits<F>::uint_type;

#if __cpp_lib_bit_cast >= 201806L

    return float_bits<F>(std::bit_cast<uint_type>(value));

#else

    uint_type bits;
    mem::copy(&bits, &value, sizeof(F));
    return float_bits<F>(bits);

#endif
}

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
        : bits(make_float_bits(f).bits)
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

    constexpr int exponent10() const noexcept
    {
        if (is_subnormal())
        {
            const int leading = bit::countl_zero(mantissa()) - (sizeof(uint_type) * CHAR_BIT - traits::mantissa_bits);
            const int e2 = 1 - traits::exponent_bias - leading;
            return _float_bits_priv::log10_pow5(-e2) + e2;
        }

        const int e2 = true_exponent();
        if (e2 >= 0)
        {
            return _float_bits_priv::log10_pow2(e2) - (e2 > 3);
        }
        else
        {
            return _float_bits_priv::log10_pow5(-e2) + e2;
        }
    }

    constexpr uint_type mantissa() const noexcept
    {
        return (bits & traits::mantissa_mask);
    }

    constexpr uint_type mantissa_with_integer_bit() const noexcept
    {
        if (is_normal())
        {
            VX_IF_CONSTEXPR (traits::explicit_integer_bit)
            {
                return mantissa();
            }
            else
            {
                return mantissa() | (uint_type(1) << traits::mantissa_bits);
            }
        }
        return mantissa();
    }

    constexpr int unbiased_exponent() const noexcept
    {
        return static_cast<int>(exponent()) - traits::exponent_bias;
    }

    constexpr int shift() const noexcept
    {
        return true_exponent() - traits::mantissa_bits;
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
};

} // namespace vx
