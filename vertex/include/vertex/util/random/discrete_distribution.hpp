#pragma once

#include <random>

#include "vertex/core/assert.hpp"

namespace vx {
namespace rand {

// https://github.com/gcc-mirror/gcc/blob/7e1d9f58858153bee4bcbab45aa862442859d958/libstdc%2B%2B-v3/include/bits/random.h#L1880

template <typename T = double>
class uniform_real_distribution
{
    static_assert(std::is_floating_point<T>::value, "T must be an floating point type");

public:

    using result_type = T;

    struct param_type
    {
        using distribution_type = uniform_real_distribution<T>;

        param_type() : param_type(0) {}

        explicit param_type(T min_value, T max_value = std::numeric_limits<T>::max())
            : m_a(min_value), m_b(max_value)
        {
            assert(m_a <= m_b);
        }

        result_type a() const
        {
            return m_a;
        }

        result_type b() const
        {
            return m_b;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs)
        {
            return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs)
        {
            return !(lhs == rhs);
        }

    private:

        result_type m_a;
        result_type m_b;
    };

public:

    uniform_real_distribution() : uniform_real_distribution(0) {}

    explicit uniform_real_distribution(T min_value, T max_value = static_cast<T>(1))
        : m_param(min_value, max_value) {}

    explicit uniform_real_distribution(const param_type& p) : m_param(p) {}

    result_type a() const { return m_param.a(); }
    result_type b() const { return m_param.b(); }

    param_type param() const
    {
        return m_param;
    }

    void param(const param_type& p)
    {
        m_param = p;
    }

    result_type min() const { return a(); }
    result_type max() const { return b(); }

    friend bool operator==(
        const uniform_real_distribution& lhs,
        const uniform_real_distribution& rhs
        )
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const uniform_real_distribution& lhs,
        const uniform_real_distribution& rhs
        )
    {
        return !(lhs == rhs);
    }

    template <typename RNG>
    result_type operator()(RNG& rng)
    {
        return operator()(rng, m_param);
    }

    template <typename RNG>
    result_type operator()(RNG& rng, const param_type& p);

private:

    // For a URBG type 'G' with range == '(G::max() - G::min()) + 1', 
    // returns the number of calls to generate at least '_Bits' bits of entropy.
    // Specifically, this function returns 'max(1, ceil(_Bits / log2(range)))'.
    static constexpr size_t iteration_count(size_t bits, uintmax_t rng_min, uintmax_t rng_max)
    {
        // Make sure bits is in a valid range
        constexpr uintmax_t max_bits = sizeof(uintmax_t) * 8;
        assert(0 <= bits && bits <= max_bits);

        if (bits == 0 || (rng_max == std::numeric_limits<uintmax_t>::max() && rng_min == 0))
        {
            return 1;
        }

        const uintmax_t range = (rng_max - rng_min) + 1;
        const uintmax_t target = ~static_cast<uintmax_t>(0) >> (max_bits - bits);

        uintmax_t prod = static_cast<uintmax_t>(1);
        size_t ceil = static_cast<size_t>(0);

        while (prod <= target)
        {
            ++ceil;

            // check to avoid overflow
            if (prod > std::numeric_limits<uintmax_t>::max() / range)
            {
                break;
            }

            prod *= range;
        }

        return ceil;
    }

private:

    param_type m_param;
};

template <typename T>
template <typename RNG>
typename uniform_real_distribution<T>::result_type uniform_real_distribution<T>::operator()(RNG& rng, const param_type& p)
{
    using range_type = T;
    using rng_type = typename RNG::result_type;
    using common_type = long double;

    // Determine the number of bits we need to work with for precision.
    // We use the minimum between the number of digits that T can hold
    // and the number of bits requested by the user.
    constexpr size_t bits = std::numeric_limits<T>::digits;

    // Calculate the range size from the random number generator.
    // This is the difference between the maximum and minimum possible values of the RNG.
    constexpr common_type frng_min = static_cast<common_type>((RNG::min)());
    constexpr common_type frng_max = static_cast<common_type>((RNG::max)());
    constexpr common_type frng_range = frng_max - frng_min + static_cast<common_type>(1);

    constexpr size_t k = iteration_count(bits, (RNG::min)(), (RNG::max)());

    common_type ret = static_cast<common_type>(0);
    common_type factor = static_cast<common_type>(1);

    for (size_t i = 0; i < k; ++i)
    {
        ret += (static_cast<common_type>(rng()) - frng_min) * factor;
        factor *= frng_range;
    }

    ret /= factor;

    return ret * (p.b() - p.a()) + p.a();
}

} // namespace rand
} // namespace vx