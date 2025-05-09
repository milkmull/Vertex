#pragma once

#include <vector>
#include <numeric>

#include "vertex/util/random/generate_canonical.hpp"

namespace vx {
namespace random {

// https://github.com/gcc-mirror/gcc/blob/440be01b07941506d1c8819448bd17c8717d55f5/libstdc%2B%2B-v3/include/bits/random.h#L5546

/**
 * @brief A distribution for generating random integers with discrete probabilities.
 *
 * The `discrete_distribution` class generates random integers with a probability distribution
 * specified by a set of weights. Each weight corresponds to the probability of selecting
 * a particular integer value. The class works with integral result types (e.g., `int`, `long`).
 *
 * @tparam T The integral type (e.g., `int`, `long`, etc.) to be used for the distribution.
 */
template <typename T = int>
class discrete_distribution
{
    static_assert(std::is_integral<T>::value, "T must be an integral type type");

public:

    using result_type = T;

    /**
     * @brief Parameter structure for the `discrete_distribution`.
     *
     * This structure holds the probabilities (weights) used to generate the discrete random values.
     */
    struct param_type
    {
        using distribution_type = discrete_distribution<T>;
        friend class discrete_distribution<T>;

        /**
         * @brief Default constructor, initializes with an empty probability list.
         */
        param_type() {}

        /**
         * @brief Constructs the parameter object with the specified probability weights.
         *
         * @param first The start iterator of the probability list.
         * @param last The end iterator of the probability list.
         */
        template <typename IT>
        param_type(IT first, IT last)
            : m_prob(first, last)
        {
            init();
        }

        /**
         * @brief Constructs the parameter object with the specified initializer list of weights.
         *
         * @param weights The initializer list of probability weights.
         */
        param_type(std::initializer_list<double> weights)
            : m_prob(weights)
        {
            init();
        }

        /**
         * @brief Constructs the parameter object with a set number of values and an optional function
         * to modify each value.
         *
         * @param count The number of elements.
         * @param xmin The minimum value of the range.
         * @param xmax The maximum value of the range.
         * @param unary_op The unary operation to apply to each value in the range.
         */
        template<typename Func>
        param_type(size_t count, double xmin, double xmax, Func unary_op)
        {
            const size_t n = (count == 0) ? 1 : count;
            const double delta = (xmin - xmax) / n;

            m_prob.reserve(n);
            for (size_t k = 0; k < count; ++k)
            {
                m_prob.push_back(unary_op(xmin + k * delta + 0.5 * delta));
            }

            init();
        }

        // See: http://cpp-next.com/archive/2010/10/implicit-move-must-go/
        param_type(const param_type&) = default;
        param_type& operator=(const param_type&) = default;

    private:

        void init()
        {
            if (m_prob.size() < 2)
            {
                m_prob = std::vector<double>{ 1.0, 1.0 };
            }

            const double sum = std::accumulate(m_prob.begin(), m_prob.end(), 0.0);
            VX_ASSERT(sum > 0.0);

            // Normalize weights
            for (double& w : m_prob)
            {
                w /= sum;
            }

            // Accumulate partial sum
            m_cp.reserve(m_prob.size());
            std::partial_sum(m_prob.begin(), m_prob.end(), std::back_inserter(m_cp));
            // Make sure the last cumulative probability is one
            m_cp[m_cp.size() - 1] = 1.0;
        }

    public:

        /**
         * @brief Returns the list of probabilities (weights) for the discrete distribution.
         *
         * @return The list of probabilities.
         */
        const std::vector<double>& probabilities() const noexcept
        {
            return m_prob;
        }

        friend bool operator==(const param_type& lhs, const param_type& rhs)
        {
            return lhs.m_prob == rhs.m_prob;
        }

        friend bool operator!=(const param_type& lhs, const param_type& rhs)
        {
            return !(lhs == rhs);
        }

    private:

        std::vector<double> m_prob;
        std::vector<double> m_cp;
    };

public:

    /**
     * @brief Default constructor, initializes the distribution with default probabilities.
     */
    discrete_distribution() {}

    /**
     * @brief Constructs the discrete distribution with the specified probability weights.
     *
     * @param first The start iterator of the probability list.
     * @param last The end iterator of the probability list.
     */
    template <typename IT>
    discrete_distribution(IT first, IT last)
        : m_param(first, last) {}

    /**
     * @brief Constructs the discrete distribution with the specified initializer list of weights.
     *
     * @param weights The initializer list of probability weights.
     */
    discrete_distribution(std::initializer_list<double> weights)
        : m_param(weights) {}

    /**
     * @brief Constructs the discrete distribution with a set number of values and an optional function
     * to modify each value.
     *
     * @param count The number of elements.
     * @param xmin The minimum value of the range.
     * @param xmax The maximum value of the range.
     * @param unary_op The unary operation to apply to each value in the range.
     */
    template<typename Func>
    discrete_distribution(size_t count, double xmin, double xmax, Func unary_op)
        : m_param(count, xmin, xmax, unary_op) {}

    /**
     * @brief Constructs the discrete distribution using a parameter object.
     *
     * @param p The parameter object containing the probabilities (weights).
     */
    explicit discrete_distribution(const param_type& p) : m_param(p) {}

    /**
     * @brief Returns the list of probabilities (weights) used by the distribution.
     *
     * @return The list of probabilities.
     */
    const std::vector<double>& probabilities() const noexcept
    {
        return m_param.probabilities();
    }

    /**
     * @brief Returns the parameter object for the distribution.
     *
     * @return The parameter object containing the probabilities (weights).
     */
    param_type param() const { return m_param; }

    /**
     * @brief Sets the parameter object for the distribution.
     *
     * @param p The parameter object to set.
     */
    void param(const param_type& p) { m_param = p; }

    /**
     * @brief Returns the minimum possible value generated by the distribution.
     *
     * @return The minimum value (0).
     */
    result_type min() const noexcept { return 0.0; }

    /**
     * @brief Returns the maximum possible value generated by the distribution.
     *
     * @return The maximum value (the last index of the probability list).
     */
    result_type max() const noexcept { return m_param.m_prob.back(); }

    /**
     * @brief Resets the internal state of the distribution. No-op for stateless distributions.
     */
    void reset() noexcept {}

    friend bool operator==(
        const discrete_distribution& lhs,
        const discrete_distribution& rhs
        )
    {
        return lhs.m_param == rhs.m_param;
    }

    friend bool operator!=(
        const discrete_distribution& lhs,
        const discrete_distribution& rhs
        )
    {
        return !(lhs == rhs);
    }

    /**
     * @brief Generates a random integer based on the distribution.
     *
     * This function generates a random integer based on the probabilities (weights) in the range [0, N-1),
     * where N is the size of the probabilities vector.
     *
     * @tparam RNG The type of the random number generator.
     * @param rng The random number generator to use.
     * @return A random integer in the range [0, N-1).
     */
    template <typename RNG>
    result_type operator()(RNG& rng)
    {
        return operator()(rng, m_param);
    }

    /**
     * @brief Generates a random integer based on the distribution and specific parameters.
     *
     * This function generates a random integer based on the probabilities (weights) in the range [0, N-1),
     * where N is the size of the probabilities vector.
     *
     * @tparam RNG The type of the random number generator.
     * @param rng The random number generator to use.
     * @param p The parameter object containing the probabilities (weights).
     * @return A random integer in the range [0, N-1).
     */
    template <typename RNG>
    result_type operator()(RNG& rng, const param_type& p);

private:

    param_type m_param;
};

// https://github.com/gcc-mirror/gcc/blob/440be01b07941506d1c8819448bd17c8717d55f5/libstdc%2B%2B-v3/include/bits/random.tcc#L2700

template <typename T>
template <typename RNG>
typename discrete_distribution<T>::result_type discrete_distribution<T>::operator()(RNG& rng, const param_type& p)
{
    if (p.m_cp.empty())
    {
        return static_cast<result_type>(0.0);
    }

#   define generate_canonical(gen) random::generate_canonical<double, std::numeric_limits<double>::digits, RNG>(gen)

    const double x = generate_canonical(rng);
    // Find the position in cumulative probabilities where x would fit
    const auto it = std::lower_bound(p.m_cp.begin(), p.m_cp.end(), x);
    return static_cast<result_type>(std::distance(p.m_cp.begin(), it));

#   undef generate_canonical
}

} // namespace random
} // namespace vx