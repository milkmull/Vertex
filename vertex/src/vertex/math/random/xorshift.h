#pragma once

#include <cstdint>

namespace vx {
namespace math {

// https://en.wikipedia.org/wiki/Xorshift#

template <uint32_t A = 13, uint32_t B = 17, uint32_t C = 5>
class xorshift32
{
public:

    // =============== constructors ===============

    inline constexpr xorshift32(uint32_t seed) : m_state(seed) {}

    // =============== seeding ===============

    inline constexpr void seed(uint32_t seed)
    {
        m_state = seed;
    }

    // =============== next ===============

    inline constexpr uint32_t operator()()
    {
        return next();
    }

    inline constexpr uint32_t next()
    {
        m_state ^= m_state << A;
        m_state ^= m_state >> B;
        m_state ^= m_state << C;

        return m_state;
    }

private:

    uint32_t m_state = 1;

};

template <uint64_t A = 13, uint64_t B = 7, uint64_t C = 17>
class xorshift64
{
public:

    // =============== constructors ===============

    inline constexpr xorshift64(uint64_t seed) : m_state(seed) {}

    // =============== seeding ===============

    inline constexpr void seed(uint64_t seed)
    {
        m_state = seed;
    }

    // =============== next ===============

    inline constexpr uint64_t operator()()
    {
        return next();
    }

    inline constexpr uint64_t next()
    {
        m_state ^= m_state << A;
        m_state ^= m_state >> B;
        m_state ^= m_state << C;

        return m_state;
    }

private:

    uint64_t m_state = 1;

};

}
}