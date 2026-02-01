#pragma once

#include "vertex/util/random/generate_canonical.hpp"

namespace vx {
namespace random {

// https://github.com/gcc-mirror/gcc/blob/440be01b07941506d1c8819448bd17c8717d55f5/libstdc%2B%2B-v3/include/bits/random.h#L3727

/**
 * @brief A distribution for generating Bernoulli random variables.
 *
 * This class represents a Bernoulli distribution, which generates random Boolean values
 * based on a probability `p`. It can be used to model events with two outcomes, such as
 * coin flips (with `p` being the probability of heads).
 */
class bernoulli_distribution
{
public:

    using result_type = bool;

    /**
     * @brief Parameter structure for the Bernoulli distribution.
     *
     * This structure holds the parameter `p`, which is the probability of generating
     * `true` in a Bernoulli trial.
     */
    struct param_type
    {
        using distribution_type = bernoulli_distribution;

        /**
         * @brief Default constructor, sets the probability to 0.5.
         */
        param_type() noexcept : param_type(0.5) {}

        /**
         * @brief Constructs the parameter type with the specified probability.
         *
         * @param p The probability of generating `true` in a Bernoulli trial.
         * It must be in the range [0.0, 1.0].
         */
        explicit param_type(double p) noexcept : m_p(p)
        {
            VX_ASSERT(0.0 <= m_p && m_p <= 1.0);
        }

        /**
         * @brief Returns the probability `p`.
         *
         * @return The probability `p`.
         */
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

    /**
     * @brief Default constructor, sets the probability to 0.5.
     */
    bernoulli_distribution() noexcept : bernoulli_distribution(0.5) {}

    /**
     * @brief Constructs the Bernoulli distribution with the specified probability.
     *
     * @param p The probability of generating `true` in a Bernoulli trial.
     */
    explicit bernoulli_distribution(double p) noexcept : m_param(p) {}

    /**
     * @brief Constructs the Bernoulli distribution with the specified parameters.
     *
     * @param p The parameter object containing the probability.
     */
    explicit bernoulli_distribution(const param_type& p) noexcept : m_param(p) {}


    /**
     * @brief Returns the probability `p` of generating `true`.
     *
     * @return The probability `p`.
     */
    double p() const noexcept { return m_param.p(); }

    /**
     * @brief Returns the parameter object for the distribution.
     *
     * @return The parameter object.
     */
    param_type param() const noexcept { return m_param; }

    /**
     * @brief Sets the parameter object for the distribution.
     *
     * @param p The parameter object to set.
     */
    void param(const param_type& p) noexcept { m_param = p; }

    /**
     * @brief Returns the minimum possible result of the distribution.
     *
     * @return The minimum possible result, which is `false`.
     */
    result_type min() const noexcept { return std::numeric_limits<result_type>::min(); }

    /**
     * @brief Returns the maximum possible result of the distribution.
     *
     * @return The maximum possible result, which is `true`.
     */
    result_type max() const noexcept { return std::numeric_limits<result_type>::max(); }

    /**
     * @brief Resets the internal state of the distribution. No-op for stateless distributions.
     */
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

    /**
     * @brief Generates a random result based on the distribution.
     *
     * This function generates a random result (`true` or `false`) based on the distribution's
     * probability `p` by using a random number generator.
     *
     * @tparam RNG The type of the random number generator.
     * @param gen The random number generator to use.
     * @return A random result, either `true` or `false`.
     */
    template <typename RNG>
    result_type operator()(RNG& gen)
    {
        return operator()(gen, m_param);
    }

    /**
     * @brief Generates a random result based on the distribution and specific parameters.
     *
     * This function generates a random result (`true` or `false`) based on the distribution's
     * parameter `p` by using a random number generator.
     *
     * @tparam RNG The type of the random number generator.
     * @param gen The random number generator to use.
     * @param p The parameter object containing the probability `p`.
     * @return A random result, either `true` or `false`.
     */
    template <typename RNG>
    result_type operator()(RNG& gen, const param_type& p);

private:

    param_type m_param;
};

template <typename RNG>
typename bernoulli_distribution::result_type bernoulli_distribution::operator()(RNG& gen, const param_type& p)
{
#   define generate_canonical(gen) random::generate_canonical<double, std::numeric_limits<double>::digits, RNG>(gen)
    return static_cast<result_type>(generate_canonical(gen) < p.p());
#   undef generate_canonical
}

} // namespace random
} // namespace vx
