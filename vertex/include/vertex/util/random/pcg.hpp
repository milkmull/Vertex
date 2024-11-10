#pragma once

#include <cstdint>
#include <limits>

#include "vertex/core/compiler.hpp"

namespace vx {
namespace rand {

// https://en.wikipedia.org/wiki/Permuted_congruential_generator

template <uint64_t multiplier = 6364136223846793005ull, uint64_t increment = 1442695040888963407ull>
class pcg32_gen
{
public:

    using result_type = uint32_t;
    static VX_FORCE_INLINE constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    static VX_FORCE_INLINE constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    static constexpr uint64_t default_seed = 0xCAFEF00DD15EA5E5ull;

public:

    VX_FORCE_INLINE pcg32_gen() : m_state(default_seed + increment) { operator()(); }
    VX_FORCE_INLINE pcg32_gen(uint64_t seed) : m_state(seed + increment) { operator()(); }

    VX_FORCE_INLINE void seed(uint64_t seed)
    {
        m_state = seed + increment;
        operator()();
    }

private:

    static VX_FORCE_INLINE constexpr uint32_t rotr32(uint32_t x, uint32_t r)
    {
        constexpr uint32_t bits = sizeof(x) * 8;
        return (x >> r) | (x << ((bits - r) & 31));
    }

public:

    VX_FORCE_INLINE uint32_t operator()()
    {
        uint64_t old_state = m_state;
        m_state = m_state * multiplier + increment;

        uint32_t count = static_cast<uint32_t>(old_state >> 59) & 31;
        old_state ^= old_state >> 18;
        return rotr32(static_cast<uint32_t>(old_state >> 27), count);
    }

private:

    uint64_t m_state = 0;
};

using pcg32 = pcg32_gen<>;

} // namespace rand
} // namespace vx