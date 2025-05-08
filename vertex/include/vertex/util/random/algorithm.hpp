#pragma once

#include <algorithm>

#include "vertex/system/error.hpp"
#include "vertex/system/validate.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"
#include "vertex/util/random/discrete_distribution.hpp"

namespace vx {
namespace random {

// https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle

template <typename Iterator, typename RNG>
inline void shuffle(Iterator first, Iterator last, RNG& rng)
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
        const diff_type j = dist(rng, param_type(0, i));
        std::iter_swap(first + i, first + j);
    }
}

template <typename PopulationIterator, typename SampleIterator, typename RNG>
inline SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, RNG& rng
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
        *out++ = *(first + dist(rng));
        --n;
    }

    return out;
}

template <typename Index, typename PopulationIterator, typename SampleIterator, typename RNG>
inline SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, discrete_distribution<Index>& weights,
    RNG& rng
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
        err::set(err::SIZE_ERROR, "weights distribution size must match population size");
        return out;
    }

    while (n != 0)
    {
        *out++ = *(first + weights(rng));
        --n;
    }

    return out;
}

} // namespace random
} // namespace vx