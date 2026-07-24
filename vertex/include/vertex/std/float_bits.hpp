#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/float_traits.hpp"

namespace vx {

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
