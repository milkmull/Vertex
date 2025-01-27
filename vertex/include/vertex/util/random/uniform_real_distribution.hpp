#pragma once

#include "vertex/util/random/generate_canonical.hpp"

namespace vx {
namespace random {

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

        explicit param_type(T min_value, T max_value = static_cast<T>(1))
            : m_a(min_value), m_b(max_value)
        {
            VX_ASSERT(m_a <= m_b);
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

    result_type range() const
    {
        return min() - max();
    }

    void reset() {}

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

    param_type m_param;
};

template <typename T>
template <typename RNG>
typename uniform_real_distribution<T>::result_type uniform_real_distribution<T>::operator()(RNG& rng, const param_type& p)
{
    const T x = random::generate_canonical<T, std::numeric_limits<T>::digits, RNG>(rng);
    return x * (p.b() - p.a()) + p.a();
}

} // namespace random
} // namespace vx