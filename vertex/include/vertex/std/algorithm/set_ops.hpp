#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

template <typename OUT, typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_union(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            *dst = *first1;
            ++first1;
        }
        else if (comp(*first2, *first1))
        {
            *dst = *first2;
            ++first2;
        }
        else // equivalent: emit one, drop the other
        {
            *dst = *first1;
            ++first1;
            ++first2;
        }

        ++dst;
    }

    dst = range::copy(dst, first1, last1);
    return range::copy(dst, first2, last2);
}

template <typename OUT, typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_union(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::set_union(dst, first1, last1, first2, last2, vx::less<>{});
}

template <typename OUT, typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_intersection(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            ++first1;
        }
        else if (comp(*first2, *first1))
        {
            ++first2;
        }
        else
        {
            *dst = *first1;
            ++dst;
            ++first1;
            ++first2;
        }
    }

    return dst;
}

template <typename OUT, typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_intersection(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::set_intersection(dst, first1, last1, first2, last2, vx::less<>{});
}

// Elements of range 1 that are not in range 2.
template <typename OUT, typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_difference(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first1, *first2))
        {
            *dst = *first1;
            ++dst;
            ++first1;
        }
        else if (comp(*first2, *first1))
        {
            ++first2;
        }
        else
        {
            ++first1;
            ++first2;
        }
    }

    return range::copy(dst, first1, last1);
}

template <typename OUT, typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT set_difference(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::set_difference(dst, first1, last1, first2, last2, vx::less<>{});
}

} // namespace range
} // namespace vx
