#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

namespace _range_priv {

template <typename IT1, typename IT2, typename Pred>
constexpr bool is_permutation_impl(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    // the common prefix needs no checking
    const auto mm = range::mismatch(first1, last1, first2, pred);
    first1 = mm.first;
    first2 = mm.second;

    IT2 last2 = first2;
    vx::advance(last2, vx::distance(first1, last1));

    // O(n^2): for each distinct value, compare its count in both ranges
    for (IT1 scan = first1; scan != last1; ++scan)
    {
        const auto same = [&](const auto& x)
        { return static_cast<bool>(pred(*scan, x)); };

        if (range::find_if(first1, scan, same) != scan) // counted already
        {
            continue;
        }

        const size_t matches = range::count_if(first2, last2, same);

        if (matches == 0 || matches != range::count_if(scan, last1, same))
        {
            return false;
        }
    }

    return true;
}

} // namespace _range_priv

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && !traits::is_iterator<Pred>::value))>
constexpr bool is_permutation(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    return _range_priv::is_permutation_impl(first1, last1, first2, pred);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool is_permutation(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    using T1 = traits::remove_cv_t<typename traits::value_type<IT1>::type>;
    using T2 = traits::remove_cv_t<typename traits::value_type<IT2>::type>;

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        // byte-sized integers: O(n) with a 256-entry histogram
        VX_IF_CONSTEXPR ((sizeof(T1) == 1 && sizeof(T2) == 1 && traits::is_integral<T1>::value && traits::is_integral<T2>::value))
        {
            size_t counts[256] = {};

            for (; first1 != last1; ++first1, ++first2)
            {
                ++counts[static_cast<unsigned char>(*first1)];
                --counts[static_cast<unsigned char>(*first2)]; // unsigned wrap-around cancels out
            }

            for (size_t i = 0; i < 256; ++i)
            {
                if (counts[i] != 0)
                {
                    return false;
                }
            }

            return true;
        }
    }

    return _range_priv::is_permutation_impl(first1, last1, first2, vx::equal_to<>{});
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool is_permutation(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    return vx::distance(first1, last1) == vx::distance(first2, last2) && _range_priv::is_permutation_impl(first1, last1, first2, pred);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool is_permutation(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    return vx::distance(first1, last1) == vx::distance(first2, last2) && range::is_permutation(first1, last1, first2);
}

} // namespace range
} // namespace vx
