#pragma once

#include "vertex/system/error.hpp"
#include "vertex/os/random.hpp"
#include "vertex/util/random/pcg.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"
#include "vertex/util/random/uniform_real_distribution.hpp"
#include "vertex/util/random/bernoulli_distribution.hpp"

namespace vx {
namespace random {

template <typename RNG>
class basic_rng
{
public:

    using generator_type = RNG;
    using result_type = typename generator_type::result_type;

    static constexpr result_type min() noexcept { return generator_type::min(); }
    static constexpr result_type max() noexcept { return generator_type::max(); }

public:

    basic_rng() { seed(); }
    basic_rng(uint32_t seed) { m_rng.seed(seed); }

    ////////////////////////////////////////////////////////////////////////////////
    // seeding
    ////////////////////////////////////////////////////////////////////////////////

    inline void seed(uint64_t seed)
    {
        m_rng.seed(seed);
    }

    inline uint64_t seed()
    {
        uint64_t s = decltype(m_rng)::default_seed;

        if (!os::get_entropy(reinterpret_cast<uint8_t*>(&s), sizeof(s)))
        {
            err::clear();
        }

        seed(s);
        return s;
    }

    generator_type& use_generator() noexcept
    {
        return m_rng;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // generators
    ////////////////////////////////////////////////////////////////////////////////

    inline uint32_t operator()()
    {
        return m_rng();
    }

    inline uint32_t randi()
    {
        return operator()();
    }

    inline double randf()
    {
        return random::uniform_real_distribution<double>()(m_rng);
    }

    inline bool randb()
    {
        return random::bernoulli_distribution()(m_rng);
    }

    template <typename T>
    inline T randi_range(T min, T max)
    {
        return random::uniform_int_distribution<T>(min, max)(m_rng);
    }

    template <typename T>
    inline T randf_range(T min, T max)
    {
        return random::uniform_real_distribution<T>(min, max)(m_rng);
    }

private:

    generator_type m_rng;
};

using rng = basic_rng<pcg32>;

} // namespace random
} // namespace vx