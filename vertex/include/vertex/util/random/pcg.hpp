#pragma once

#include "vertex/util/bit.hpp"

namespace vx {
namespace random {

// https://en.wikipedia.org/wiki/Permuted_congruential_generator
// https://github.com/imneme/pcg-cpp

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

public:

    VX_FORCE_INLINE uint32_t operator()()
    {
        uint64_t old_state = m_state;
        m_state = m_state * multiplier + increment;

        uint32_t count = static_cast<uint32_t>(old_state >> 59) & 31;
        old_state ^= old_state >> 18;
        return bit::rotr(static_cast<uint32_t>(old_state >> 27), count);
    }

private:

    uint64_t m_state = 0;
};

using pcg32 = pcg32_gen<>;

} // namespace random
} // namespace vx