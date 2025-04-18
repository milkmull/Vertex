#pragma once

#include <cmath>

#include "vertex/util/random/generate_canonical.hpp"

namespace vx {
namespace random {

// https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/random.h#L2118

template <typename T = double>
class normal_distribution
{
    static_assert(std::is_floating_point<T>::value, "T must be an floating point type");

public:

    using result_type = T;

    struct param_type
    {
        using distribution_type = normal_distribution<T>;

        param_type() noexcept : param_type(0) {}

        explicit param_type(T mean, T stddev = static_cast<T>(1)) noexcept
            : m_mean(mean), m_stddev(stddev) {}

        result_type mean() const noexcept
        {
            return m_mean;
        }

        result_type stddev() const noexcept
        {
            return m_stddev;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs) noexcept
        {
            return lhs.m_mean == rhs.m_mean && lhs.m_stddev == rhs.m_stddev;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs) noexcept
        {
            return !(lhs == rhs);
        }

    private:

        result_type m_mean;
        result_type m_stddev;
    };

public:

    normal_distribution() noexcept : normal_distribution(0) {}

    explicit normal_distribution(T mean, T stddev = static_cast<T>(1)) noexcept
        : m_param(mean, stddev) {}

    explicit normal_distribution(const param_type& p) noexcept : m_param(p) {}

    result_type mean() const noexcept { return m_param.mean(); }
    result_type stddev() const noexcept { return m_param.stddev(); }

    param_type param() const noexcept
    {
        return m_param;
    }

    void param(const param_type& p) noexcept
    {
        m_param = p;
    }

    result_type min() const noexcept { return std::numeric_limits<T>::lowest(); }
    result_type max() const noexcept { return std::numeric_limits<T>::max(); }

    void reset() noexcept { m_saved_available = false; }

    friend bool operator==(
        const normal_distribution& lhs,
        const normal_distribution& rhs
        ) noexcept
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const normal_distribution& lhs,
        const normal_distribution& rhs
        ) noexcept
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

    param_type m_param;
    bool m_saved_available = false;
    T m_saved = static_cast<T>(0);
};

// https://github.com/gcc-mirror/gcc/blob/440be01b07941506d1c8819448bd17c8717d55f5/libstdc%2B%2B-v3/include/bits/random.tcc#L1809

// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform

template <typename T>
template <typename RNG>
typename normal_distribution<T>::result_type normal_distribution<T>::operator()(RNG& rng, const param_type& p)
{
    using rng_type = typename RNG::result_type;

    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

#   define generate_canonical(gen) random::generate_canonical<result_type, std::numeric_limits<result_type>::digits, RNG>(gen)

    result_type ret;

    if (m_saved_available)
    {
        m_saved_available = false;
        ret = m_saved;
    }
    else
    {
        result_type x, y, r2;

        do
        {
            x = static_cast<result_type>(2) * generate_canonical(rng) - static_cast<result_type>(1);
            y = static_cast<result_type>(2) * generate_canonical(rng) - static_cast<result_type>(1);
            r2 = x * x + y * y;

        } while (r2 > static_cast<result_type>(1) || r2 == static_cast<result_type>(0));

        const result_type mult = std::sqrt(static_cast<result_type>(-2) * std::log(r2) / r2);
        ret = y * mult;

        m_saved = x * mult;
        m_saved_available = true;
    }

    ret = ret * p.stddev() + p.mean();
    return ret;

#   undef generate_canonical
}

} // namespace random
} // namespace vx