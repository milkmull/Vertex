#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"

namespace vx {
namespace range {

//=========================================================================
// comparison: compare / lexicographical_compare / equal / mismatch
//=========================================================================

// ---- compare (three-way: -1, 0, 1) ----------------------------------------

// Custom comparators never take the memcmp path (memcmp only matches the
// default operator<).
template <typename T, typename Compare>
constexpr int compare(const T* a, size_t count_a, const T* b, size_t count_b, Compare comp)
{
    size_t n = vx::min(count_a, count_b);

    for (; 0 < n; --n, ++a, ++b)
    {
        if (comp(*a, *b))
        {
            return -1;
        }

        if (comp(*b, *a))
        {
            return 1;
        }
    }

    return _range_priv::length_order(count_a, count_b);
}

template <typename T>
constexpr int compare(const T* a, size_t count_a, const T* b, size_t count_b)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_lexicographical_is_safe<const T*, const T*>::value))
        {
            const int res = mem::compare(a, b, vx::min(count_a, count_b) * sizeof(T));
            return (res != 0) ? ((res < 0) ? -1 : 1) : _range_priv::length_order(count_a, count_b);
        }
    }

    return range::compare(a, count_a, b, count_b, vx::less<>{});
}

template <typename T>
constexpr int compare(const T* a, const T* b, size_t count)
{
    return range::compare(a, count, b, count);
}

template <typename T, typename Compare>
constexpr int compare(const T* a, const T* b, size_t count, Compare comp)
{
    return range::compare(a, count, b, count, comp);
}

template <typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (comp(*first1, *first2))
        {
            return -1;
        }

        if (comp(*first2, *first1))
        {
            return 1;
        }
    }

    // whichever range still has elements is the greater one
    return static_cast<int>(first1 != last1) - static_cast<int>(first2 != last2);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_lexicographical_is_safe<IT1, IT2>::value))
        {
            return range::compare(
                _range_priv::to_pointer(first1), static_cast<size_t>(vx::distance(first1, last1)),
                _range_priv::to_pointer(first2), static_cast<size_t>(vx::distance(first2, last2)));
        }
    }

    return range::compare(first1, last1, first2, last2, vx::less<>{});
}

// ---- lexicographical_compare ----------------------------------------------

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool lexicographical_compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::compare(first1, last1, first2, last2) < 0;
}

template <typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool lexicographical_compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    for (; first1 != last1; ++first1, ++first2)
    {
        if (first2 == last2)
        {
            return false;
        }

        if (comp(*first1, *first2))
        {
            return true;
        }

        if (comp(*first2, *first1))
        {
            return false;
        }
    }

    return first2 != last2;
}

// ---- equal ------------------------------------------------------------------

// Both ranges have `count` elements.
template <typename T, typename U>
constexpr bool equal(const T* a, const U* b, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<const T*, const U*>::value))
        {
            return a == b || mem::compare(a, b, count * sizeof(T)) == 0;
        }
    }

    for (; 0 < count; --count, ++a, ++b)
    {
        if (!(*a == *b))
        {
            return false;
        }
    }

    return true;
}

template <typename T, typename U, typename Pred>
constexpr bool equal(const T* a, const U* b, size_t count, Pred pred)
{
    for (; 0 < count; --count, ++a, ++b)
    {
        if (!pred(*a, *b))
        {
            return false;
        }
    }

    return true;
}

// Ranges of different size are never equal.
template <typename T, typename U>
constexpr bool equal(const T* a, size_t count_a, const U* b, size_t count_b)
{
    return count_a == count_b && range::equal(a, b, count_a);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            return range::equal(
                _range_priv::to_pointer(first1), _range_priv::to_pointer(first2),
                static_cast<size_t>(vx::distance(first1, last1)));
        }
    }

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            return false;
        }
    }

    return true;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && !traits::is_iterator<Pred>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!pred(*first1, *first2))
        {
            return false;
        }
    }

    return true;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    VX_IF_CONSTEXPR ((traits::is_random_access_iterator<IT1>::value && traits::is_random_access_iterator<IT2>::value))
    {
        // O(1) length check, then the 3-iterator form gets the memcmp path
        return vx::distance(first1, last1) == vx::distance(first2, last2) && range::equal(first1, last1, first2);
    }
    else
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
            {
                return false;
            }
        }

        return first1 == last1 && first2 == last2;
    }
}

// ---- mismatch ---------------------------------------------------------------

template <typename IT1, typename IT2>
struct mismatch_result
{
    IT1 first;
    IT2 second;
};

// Returns the first position where the ranges differ. If they are equal,
// returns {a + count, b + count}.
template <typename T, typename U>
constexpr mismatch_result<const T*, const U*> mismatch(const T* a, const U* b, size_t count)
{
    size_t i = 0;

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<const T*, const U*>::value))
        {
            // memcmp can't report a position, so skip whole equal blocks with
            // memcmp and let the scalar loop below pinpoint the element inside
            // the first block that differs.
            constexpr size_t block = (sizeof(T) >= 256) ? 1 : (256 / sizeof(T));

            while (count - i >= block && mem::compare(a + i, b + i, block * sizeof(T)) == 0)
            {
                i += block;
            }
        }
    }

    for (; i < count; ++i)
    {
        if (!(a[i] == b[i]))
        {
            break;
        }
    }

    return { a + i, b + i };
}

template <typename T, typename U, typename Pred>
constexpr mismatch_result<const T*, const U*> mismatch(const T* a, const U* b, size_t count, Pred pred)
{
    size_t i = 0;

    for (; i < count; ++i)
    {
        if (!pred(a[i], b[i]))
        {
            break;
        }
    }

    return { a + i, b + i };
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            const auto* p = _range_priv::to_pointer(first1);
            const auto* q = _range_priv::to_pointer(first2);

            const auto r = range::mismatch(p, q, static_cast<size_t>(vx::distance(first1, last1)));
            return { first1 + (r.first - p), first2 + (r.second - q) };
        }
    }

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            break;
        }
    }

    return { first1, first2 };
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && !traits::is_iterator<Pred>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!pred(*first1, *first2))
        {
            break;
        }
    }

    return { first1, first2 };
}

// Stops at the end of the shorter range.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    VX_IF_CONSTEXPR ((traits::is_random_access_iterator<IT1>::value && traits::is_random_access_iterator<IT2>::value))
    {
        // clamp to the shorter length, then use the memcmp-capable 3-iterator form
        const auto n1 = vx::distance(first1, last1);
        const auto n2 = vx::distance(first2, last2);
        return range::mismatch(first1, n1 < n2 ? last1 : first1 + n2, first2);
    }
    else
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
            {
                break;
            }
        }

        return { first1, first2 };
    }
}

} // namespace range
} // namespace vx
