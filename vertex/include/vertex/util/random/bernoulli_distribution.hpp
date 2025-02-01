#pragma once

#include "vertex/util/random/generate_canonical.hpp"

namespace vx {
namespace random {

// https://github.com/gcc-mirror/gcc/blob/440be01b07941506d1c8819448bd17c8717d55f5/libstdc%2B%2B-v3/include/bits/random.h#L3727

class bernoulli_distribution
{
public:

    using result_type = bool;

    struct param_type
    {
        using distribution_type = bernoulli_distribution;

        param_type() noexcept : param_type(0.5) {}

        explicit param_type(double p) noexcept : m_p(p)
        {
            VX_ASSERT(0.0 <= m_p && m_p <= 1.0);
        }

        double p() const noexcept
        {
            return m_p;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs) noexcept
        {
            return lhs.m_p == rhs.m_p;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs) noexcept
        {
            return !(lhs == rhs);
        }

    private:

        double m_p;
    };

public:

    bernoulli_distribution() noexcept : bernoulli_distribution(0.5) {}
    explicit bernoulli_distribution(double p) noexcept : m_param(p) {}
    explicit bernoulli_distribution(const param_type& p) noexcept : m_param(p) {}

    double p() const noexcept { return m_param.p(); }

    param_type param() const noexcept
    {
        return m_param;
    }

    void param(const param_type& p) noexcept
    {
        m_param = p;
    }

    result_type min() const noexcept { return std::numeric_limits<result_type>::min(); }
    result_type max() const noexcept { return std::numeric_limits<result_type>::max(); }

    void reset() noexcept {}

    friend bool operator==(
        const bernoulli_distribution& lhs,
        const bernoulli_distribution& rhs
        ) noexcept
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const bernoulli_distribution& lhs,
        const bernoulli_distribution& rhs
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
};

template <typename RNG>
typename bernoulli_distribution::result_type bernoulli_distribution::operator()(RNG& rng, const param_type& p)
{
#   define generate_canonical(gen) random::generate_canonical<double, std::numeric_limits<double>::digits, RNG>(gen)

    return static_cast<result_type>(generate_canonical(rng) < p.p());

#   undef generate_canonical
}

} // namespace random
} // namespace vx