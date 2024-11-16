#pragma once

#include <algorithm>

#include "vertex/system/error.hpp"
#include "vertex/system/validate.hpp"
#include "vertex/util/random/uniform_int_distribution.hpp"
#include "vertex/util/random/discrete_distribution.hpp"

namespace vx {
namespace rand {

// https://github.com/gcc-mirror/gcc/blob/d7ef7d1258d8ef715be29dea0788a3e410c1d279/libstdc%2B%2B-v3/include/bits/stl_algo.h#L3680

template <typename Iterator, typename RNG>
void shuffle(Iterator first, Iterator last, RNG& rng)
{
    __VX_ITERATOR_VALID_RANGE(first, last);

    if (first == last)
    {
        return;
    }

    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<Iterator>::difference_type;
    using udiff_type = typename std::make_unsigned<diff_type>::type;

    using dist_type = uniform_int_distribution<udiff_type>;
    using dist_param_type = typename dist_type::param_type;

    using ucommon_type = typename std::common_type<rng_type, udiff_type>::type;

    constexpr ucommon_type urng_min = static_cast<ucommon_type>((RNG::min)());
    constexpr ucommon_type urng_max = static_cast<ucommon_type>((RNG::max)());
    constexpr ucommon_type urng_range = urng_max - urng_min;

    const ucommon_type urange = static_cast<ucommon_type>(std::distance(first, last));
    dist_type dist;

    // (urng_range >= urange * urange) but without wrap issues
    if (urng_range / urange >= urange)
    {
        Iterator it = first + 1;

        // Since we know the range isn't empty, an even number of elements
        // means an uneven number of elements to swap, in which case we
        // do the first one up front:

        if ((urange % 2) == 0)
        {
            std::iter_swap(it++, first + dist(rng, dist_param_type(0, 1)));
        }

        // Now we know that (last - it) is even, so we do the rest in pairs,
        // using a single distribution invocation to produce swap positions
        // for two successive elements at a time:

        while (it != last)
        {
            // https://github.com/gcc-mirror/gcc/blob/d7ef7d1258d8ef715be29dea0788a3e410c1d279/libstdc%2B%2B-v3/include/bits/stl_algo.h#L3657
            // Requires: r0 * r1 <= rng.max() - rng.min()

            // Using uniform_int_distribution with a range that is very
            // small relative to the range of the generator ends up wasting
            // potentially expensively generated randomness, since
            // uniform_int_distribution does not store leftover randomness
            // between invocations.

            // If we know we want two integers in ranges that are sufficiently
            // small, we can compose the ranges, use a single distribution
            // invocation, and significantly reduce the waste.

            const ucommon_type r0 = static_cast<ucommon_type>(std::distance(first, it)) + 1;
            const ucommon_type r1 = r0 + 1;
            const ucommon_type x = dist(rng, dist_param_type(0, (r0 * r1) - 1));

            std::iter_swap(it++, first + (x / r1));
            std::iter_swap(it++, first + (x % r1));
        }

        return;
    }
    else
    {
        for (Iterator it = first + 1; it != last; ++it)
        {
            std::iter_swap(it, first + dist(rng, dist_param_type(0, it - first)));
        }
    }
}

template <typename PopulationIterator, typename SampleIterator, typename RNG>
SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, RNG& rng
)
{
    __VX_ITERATOR_VALID_RANGE(first, last);

    if (first == last || n == 0)
    {
        return out;
    }

    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<PopulationIterator>::difference_type;
    using udiff_type = typename std::make_unsigned<diff_type>::type;

    using dist_type = uniform_int_distribution<udiff_type>;
    using dist_param_type = typename dist_type::param_type;

    using ucommon_type = typename std::common_type<rng_type, udiff_type>::type;

    constexpr ucommon_type urng_min = static_cast<ucommon_type>((RNG::min)());
    constexpr ucommon_type urng_max = static_cast<ucommon_type>((RNG::max)());
    constexpr ucommon_type urng_range = urng_max - urng_min;

    const ucommon_type urange = static_cast<ucommon_type>(std::distance(first, last));
    dist_type dist(0, urange - 1);

    // (urng_range >= urange * urange) but without wrap issues
    if (urng_range / urange >= urange)
    {
        // If there are an uneven number of samples to take, do one upfront:

        if ((n % 2) != 0)
        {
            *out++ = *(first + dist(rng));
            --n;
        }

        // Now we know that n is even, so we do the rest in pairs,
        // using a single distribution invocation to produce indices
        // for two elements at a time:

        const dist_param_type p(0, (urange * urange) - 1);

        while (n >= 2)
        {
            const ucommon_type x = dist(rng, p);

            *out++ = *(first + (x / urange));
            *out++ = *(first + (x % urange));
            n -= 2;
        }
    }
    else
    {
        while (n != 0)
        {
            *out++ = *(first + dist(rng));
            --n;
        }
    }

    return out;
}

template <typename Index, typename PopulationIterator, typename SampleIterator, typename RNG>
SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    size_t n, discrete_distribution<Index>& weights,
    RNG& rng
)
{
    __VX_ITERATOR_VALID_RANGE(first, last);

    if (first == last || n == 0)
    {
        return out;
    }

    using rng_type = typename RNG::result_type;
    static_assert(
        std::is_integral<rng_type>::value && std::is_unsigned<rng_type>::value,
        "RNG::result_type must be an unsigned integral type"
    );

    using diff_type = typename std::iterator_traits<PopulationIterator>::difference_type;
    using udiff_type = typename std::make_unsigned<diff_type>::type;

    using dist_type = discrete_distribution<Index>;
    using dist_param_type = typename dist_type::param_type;

    using ucommon_type = typename std::common_type<rng_type, udiff_type>::type;

    constexpr ucommon_type urng_min = static_cast<ucommon_type>((RNG::min)());
    constexpr ucommon_type urng_max = static_cast<ucommon_type>((RNG::max)());
    constexpr ucommon_type urng_range = urng_max - urng_min;

    const ucommon_type urange = static_cast<ucommon_type>(std::distance(first, last));

    if (weights.probabilities().size() != urange)
    {
        VX_ERR(err::SIZE_ERROR) << "weights distribution size must match population size";
        return out;
    }

    while (n != 0)
    {
        *out++ = *(first + weights(rng));
        --n;
    }

    return out;
}

} // namespace rand
} // namespace vx