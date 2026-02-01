#pragma once

#include <algorithm>

#include "vertex/system/error.hpp"
#include "vertex/system/validate.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"
#include "vertex/util/random/discrete_distribution.hpp"

namespace vx {
namespace random {

// https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle

/**
 * @brief Randomly shuffles the elements in the range [first, last) using the provided RNG.
 *
 * Implements the Fisher–Yates shuffle algorithm. Ensures unbiased shuffling using
 * a uniform distribution in the range [0, i] for each i in reverse order.
 *
 * @tparam Iterator A random-access iterator type.
 * @tparam RNG A random number generator type with unsigned integral result_type.
 * @param first Iterator to the beginning of the range.
 * @param last Iterator to the end of the range.
 * @param gen A random number generator instance.
 */
template <typename Iterator, typename RNG>
inline void shuffle(Iterator first, Iterator last, RNG& gen)
{
    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<Iterator>::difference_type;
    const diff_type n = std::distance(first, last);

    if (n <= 1)
    {
        return;
    }

    random::uniform_int_distribution<diff_type> dist;
    using param_type = typename decltype(dist)::param_type;

    for (diff_type i = n - 1; i > 0; --i)
    {
        const diff_type j = dist(gen, param_type(0, i));
        std::iter_swap(first + i, first + j);
    }
}

/**
 * @brief Randomly selects `n` elements from the input range [first, last) with uniform probability.
 *
 * Selected elements are written to the output iterator. Elements may be duplicated
 * if `n` is greater than the population size.
 *
 * @tparam PopulationIterator Input iterator type for the population range.
 * @tparam SampleIterator Output iterator type.
 * @tparam RNG Random number generator type with unsigned integral result_type.
 * @param first Iterator to the beginning of the population.
 * @param last Iterator to the end of the population.
 * @param out Output iterator to store the sampled elements.
 * @param n Number of elements to sample.
 * @param gen A random number generator instance.
 * @return An iterator to the element past the last written output.
 */
template <typename PopulationIterator, typename SampleIterator, typename RNG>
inline SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, RNG& gen
)
{
    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<PopulationIterator>::difference_type;

    if (first == last || n == 0)
    {
        return out;
    }

    const diff_type range = std::distance(first, last);
    if (range < 1)
    {
        return out;
    }
    
    // Use a uniform distribution to randomly select indices from the population
    random::uniform_int_distribution<diff_type> dist(0, range - 1);

    while (n != 0)
    {
        *out++ = *(first + dist(gen));
        --n;
    }

    return out;
}

/**
 * @brief Randomly selects `n` elements from the input range [first, last) according to the provided discrete distribution.
 *
 * The `weights` distribution determines the likelihood of each element being selected.
 * Output may contain repeated elements. The size of `weights` must match the population size.
 *
 * @tparam Index Index type used in the discrete distribution.
 * @tparam PopulationIterator Input iterator type for the population.
 * @tparam SampleIterator Output iterator type.
 * @tparam RNG Random number generator type with unsigned integral result_type.
 * @param first Iterator to the beginning of the population.
 * @param last Iterator to the end of the population.
 * @param out Output iterator to store the sampled elements.
 * @param n Number of elements to sample.
 * @param weights A discrete distribution defining the sampling weights.
 * @param gen A random number generator instance.
 * @return An iterator to the element past the last written output.
 */
template <typename Index, typename PopulationIterator, typename SampleIterator, typename RNG>
inline SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, discrete_distribution<Index>& weights,
    RNG& gen
)
{
    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<PopulationIterator>::difference_type;

    if (first == last || n == 0)
    {
        return out;
    }

    const diff_type range = std::distance(first, last);
    if (range < 1)
    {
        return out;
    }

    if (weights.probabilities().size() != static_cast<size_t>(range))
    {
        err::set(err::size_error, "weights distribution size must match population size");
        return out;
    }

    while (n != 0)
    {
        *out++ = *(first + weights(gen));
        --n;
    }

    return out;
}

} // namespace random
} // namespace vx
