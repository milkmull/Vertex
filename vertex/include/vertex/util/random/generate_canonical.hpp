#pragma once

#include "vertex/system/assert.hpp"

namespace vx {
namespace random {

namespace __detail {

// https://github.com/microsoft/STL/blob/25dc2b72b6f594326253cf3a05543f7a5750f802/stl/inc/random#L297

// For a URBG type 'G' with range == '(G::max() - G::min()) + 1', 
// returns the number of calls to generate at least '_Bits' bits of entropy.
// Specifically, this function returns 'max(1, ceil(_Bits / log2(range)))'.
inline constexpr size_t generate_canonical_iterations(size_t bits, uint64_t rng_min, uint64_t rng_max) noexcept
{
    // Make sure bits is in a valid range
    constexpr uint64_t max_bits = sizeof(uint64_t) * 8;
    VX_ASSERT(bits <= max_bits);

    if (bits == 0 || (rng_max == std::numeric_limits<uint64_t>::max() && rng_min == 0))
    {
        return 1;
    }

    const uint64_t range = (rng_max - rng_min) + 1;
    const uint64_t target = ~static_cast<uint64_t>(0) >> ((max_bits - bits) % max_bits); // mod for the compiler

    uint64_t prod = static_cast<uint64_t>(1);
    size_t ceil = static_cast<size_t>(0);

    while (prod <= target)
    {
        ++ceil;

        // check to avoid overflow
        if (prod > std::numeric_limits<uint64_t>::max() / range)
        {
            break;
        }

        prod *= range;
    }

    return ceil;
}

} // namespace __detail

// https://github.com/microsoft/STL/blob/25dc2b72b6f594326253cf3a05543f7a5750f802/stl/inc/random#L297

template <typename T, size_t bits, typename RNG>
inline constexpr T generate_canonical(RNG& rng) noexcept(noexcept((RNG::min)()) && noexcept((RNG::max)()))
{
    using result_type = T;
    static_assert(std::is_floating_point<result_type>::value, "T must be an floating point type");

    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    constexpr size_t digits = static_cast<size_t>(std::numeric_limits<result_type>::digits);
    constexpr size_t min_bits = static_cast<size_t>(digits < bits ? digits : bits);

    // using double means we have a better chance of consistent cross platform behaviour
    using common_type = double;

    constexpr common_type frng_min = static_cast<common_type>((RNG::min)());
    constexpr common_type frng_max = static_cast<common_type>((RNG::max)());
    constexpr common_type r = (frng_max - frng_min) + static_cast<common_type>(1);
    constexpr size_t k = __detail::generate_canonical_iterations(min_bits, (RNG::min)(), (RNG::max)());

    common_type ret = static_cast<common_type>(0);
    common_type factor = static_cast<common_type>(1);

    for (size_t i = 0; i < k; ++i)
    {
        ret += (static_cast<common_type>(rng()) - frng_min) * factor;
        factor *= r;
    }

    ret /= factor;
    return static_cast<result_type>(ret);
}

} // namespace random
} // namespace vx