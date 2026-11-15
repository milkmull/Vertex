#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

//=========================================================================
// 6c. merge / inplace_merge / stable_sort
//=========================================================================

// Stable: on ties the element from the first range goes first.
template <typename OUT, typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT merge(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    while (first1 != last1 && first2 != last2)
    {
        if (comp(*first2, *first1))
        {
            *dst = *first2;
            ++first2;
        }
        else
        {
            *dst = *first1;
            ++first1;
        }

        ++dst;
    }

    // whichever tail is left goes through copy (memmove for trivial types)
    dst = range::copy(dst, first1, last1);
    return range::copy(dst, first2, last2);
}

template <typename OUT, typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr OUT merge(OUT dst, IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::merge(dst, first1, last1, first2, last2, vx::less<>{});
}

namespace _range_priv {

// Merges the sorted halves [first, mid) and [mid, last) in place.
// len1/len2 are their lengths; buf has room for `cap` objects.
//   - the smaller half fits the buffer: move it out, merge back (O(n))
//   - otherwise: split the larger half at its middle, binary search the cut in
//     the other half, rotate, recurse (allocation-free, O(n log n))
template <typename IT, typename T, typename Compare>
void merge_adaptive(IT first, IT mid, IT last, size_t len1, size_t len2, T* buf, size_t cap, Compare comp)
{
    if (len1 == 0 || len2 == 0)
    {
        return;
    }

    if (len1 <= len2 && len1 <= cap)
    {
        T* const buf_end = range::move_uninitialized(buf, first, mid);
        T* b = buf;

        while (b != buf_end && mid != last)
        {
            if (comp(*mid, *b))
            {
                *first = _range_priv::as_rvalue(*mid);
                ++mid;
            }
            else
            {
                *first = _range_priv::as_rvalue(*b);
                ++b;
            }

            ++first;
        }

        range::move(first, b, buf_end); // leftover buffer; leftover right half is already in place
        range::destroy(buf, buf_end);
        return;
    }

    if (len2 <= cap)
    {
        T* const buf_end = range::move_uninitialized(buf, mid, last);
        T* b = buf_end;
        IT out = last;

        while (b != buf && mid != first)
        {
            --out;

            if (comp(*(b - 1), *(mid - 1)))
            {
                --mid;
                *out = _range_priv::as_rvalue(*mid);
            }
            else
            {
                --b;
                *out = _range_priv::as_rvalue(*b);
            }
        }

        range::move_backward(out, buf, b); // leftover buffer; leftover left half is already in place
        range::destroy(buf, buf_end);
        return;
    }

    if (len1 + len2 == 2) // one element each, no buffer: swap if out of order
    {
        if (comp(*mid, *first))
        {
            vx::iter_swap(first, mid);
        }
        return;
    }

    IT cut1;
    IT cut2;
    size_t d1;
    size_t d2;

    if (len1 > len2)
    {
        d1 = len1 / 2;
        cut1 = first + d1;
        cut2 = range::lower_bound(mid, last, *cut1, comp);
        d2 = static_cast<size_t>(cut2 - mid);
    }
    else
    {
        d2 = len2 / 2;
        cut2 = mid + d2;
        cut1 = range::upper_bound(first, mid, *cut2, comp);
        d1 = static_cast<size_t>(cut1 - first);
    }

    const IT new_mid = range::rotate(cut1, mid, cut2);
    _range_priv::merge_adaptive(first, cut1, new_mid, d1, d2, buf, cap, comp);
    _range_priv::merge_adaptive(new_mid, cut2, last, len1 - d1, len2 - d2, buf, cap, comp);
}

template <typename IT, typename T, typename Compare>
void stable_sort_rec(IT first, IT last, T* buf, size_t cap, Compare comp)
{
    const size_t len = static_cast<size_t>(last - first);

    if (len <= sort_threshold)
    {
        _range_priv::insertion_sort(first, last, comp);
        return;
    }

    const size_t half = len / 2;
    const IT mid = first + half;

    _range_priv::stable_sort_rec(first, mid, buf, cap, comp);
    _range_priv::stable_sort_rec(mid, last, buf, cap, comp);

    if (comp(*mid, *(mid - 1))) // skip the merge when the halves are already in order
    {
        _range_priv::merge_adaptive(first, mid, last, half, len - half, buf, cap, comp);
    }
}

template <typename IT, typename Compare>
void stable_sort_impl(IT first, IT last, Compare comp)
{
    using T = typename traits::value_type<IT>::type;

    const size_t len = static_cast<size_t>(last - first);

    if (len <= sort_threshold) // no buffer needed
    {
        _range_priv::insertion_sort(first, last, comp);
        return;
    }

    _range_priv::temp_buffer<T> buf(len / 2); // the smaller half of any merge is <= len / 2
    _range_priv::stable_sort_rec(first, last, buf.ptr, buf.cap, comp);
}

} // namespace _range_priv

// Merges the sorted ranges [first, middle) and [middle, last) in place. Stable.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
void inplace_merge(IT first, IT middle, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, middle);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(middle, last);
    VX_PRIV_ASSERT_SORTED_RANGE(first, middle, comp);
    VX_PRIV_ASSERT_SORTED_RANGE(middle, last, comp);

    if (first == middle || middle == last) // needed: *middle / *(middle - 1) are read below
    {
        return;
    }

    // Trim what is already in place: the left prefix <= *middle and the right
    // suffix >= the left maximum. Shrinks the work and the buffer.
    first = range::upper_bound(first, middle, *middle, comp);
    last = range::lower_bound(middle, last, *(middle - 1), comp);

    if (first == middle || middle == last)
    {
        return;
    }

    using T = typename traits::value_type<IT>::type;

    const size_t len1 = static_cast<size_t>(middle - first);
    const size_t len2 = static_cast<size_t>(last - middle);

    _range_priv::temp_buffer<T> buf(len1 < len2 ? len1 : len2);
    _range_priv::merge_adaptive(first, middle, last, len1, len2, buf.ptr, buf.cap, comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
void inplace_merge(IT first, IT middle, IT last)
{
    range::inplace_merge(first, middle, last, vx::less<>{});
}

// Stable merge sort (top-down, insertion sort below 16 elements).
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
void stable_sort(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    VX_PRIV_ASSERT_STRICT_WEAK(first, last, comp);
    _range_priv::stable_sort_impl(_range_priv::unwrap(first), _range_priv::unwrap(last), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
void stable_sort(IT first, IT last)
{
    range::stable_sort(first, last, vx::less<>{});
}

template <typename T, typename Compare>
void stable_sort(T* ptr, size_t n, Compare comp)
{
    range::stable_sort(ptr, ptr + n, comp);
}

template <typename T>
void stable_sort(T* ptr, size_t n)
{
    range::stable_sort(ptr, ptr + n, vx::less<>{});
}

} // namespace range
} // namespace vx
