#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

// First element for which pred is false. pred must be true for a prefix.
template <typename T, typename Pred>
constexpr T* partition_point(T* ptr, size_t n, Pred pred)
{
    return _range_priv::partition_point_n(ptr, n, pred);
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT partition_point(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    return _range_priv::partition_point_n(first, static_cast<size_t>(vx::distance(first, last)), pred);
}

// True if every element satisfying pred comes before every one that doesn't.
template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool is_partitioned(IT first, IT last, Pred pred)
{
    first = range::find_if_not(first, last, pred);
    return range::find_if(first, last, pred) == last;
}

// Not stable. Returns the first element of the second group.
template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT partition(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    VX_IF_CONSTEXPR (traits::is_random_access_iterator<IT>::value)
    {
        // two pointers walking toward each other: ~n/2 swaps at most
        for (;;)
        {
            for (;;)
            {
                if (first == last)
                {
                    return first;
                }

                if (!pred(*first))
                {
                    break;
                }

                ++first;
            }

            --last;

            for (;;)
            {
                if (first == last)
                {
                    return first;
                }

                if (pred(*last))
                {
                    break;
                }

                --last;
            }

            vx::iter_swap(first, last);
            ++first;
        }
    }
    else
    {
        first = range::find_if_not(first, last, pred);

        if (first == last) // needed: ++it below must not pass last
        {
            return first;
        }

        for (IT it = first; ++it != last;)
        {
            if (pred(*it))
            {
                vx::iter_swap(first, it);
                ++first;
            }
        }

        return first;
    }
}

template <typename T, typename Pred>
constexpr T* partition(T* ptr, size_t n, Pred pred)
{
    return range::partition(ptr, ptr + n, pred);
}

namespace _range_priv {

// Allocation-free stable partition: split in half, partition both, rotate
// the middle. len >= 1. Returns the first element of the second group.
template <typename IT, typename Pred>
IT stable_partition_inplace(IT first, IT last, size_t len, Pred pred)
{
    if (len == 1)
    {
        return first + static_cast<size_t>(static_cast<bool>(pred(*first)));
    }

    const size_t half = len / 2;
    const IT mid = first + half;
    const IT left = _range_priv::stable_partition_inplace(first, mid, half, pred);
    const IT right = _range_priv::stable_partition_inplace(mid, last, len - half, pred);
    return range::rotate(left, mid, right);
}

} // namespace _range_priv

// Stable. Buffered O(n) when a temp buffer is available, otherwise in-place
// O(n log n). Requires random access iterators.
template <typename IT, typename Pred, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
IT stable_partition(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    using T = typename traits::value_type<IT>::type;

    first = range::find_if_not(first, last, pred); // already-partitioned prefix

    if (first == last) // needed: *first is read below
    {
        return last;
    }

    const size_t len = static_cast<size_t>(last - first);
    _range_priv::temp_buffer<T> buf(len);

    if (buf.cap < len)
    {
        return _range_priv::stable_partition_inplace(first, last, len, pred);
    }

    // *first fails pred. Passing elements are compacted forward, failing ones
    // are parked in the buffer and moved back behind them afterwards.
    T* out = buf.ptr;
    IT dst = first;

    mem::construct_in_place(out, _range_priv::as_rvalue(*first));
    ++out;

    for (IT it = first + 1; it != last; ++it)
    {
        if (pred(*it))
        {
            *dst = _range_priv::as_rvalue(*it);
            ++dst;
        }
        else
        {
            mem::construct_in_place(out, _range_priv::as_rvalue(*it));
            ++out;
        }
    }

    range::move(dst, buf.ptr, out);
    range::destroy(buf.ptr, out);
    return dst;
}

template <typename T, typename Pred>
T* stable_partition(T* ptr, size_t n, Pred pred)
{
    return range::stable_partition(ptr, ptr + n, pred);
}

} // namespace range
} // namespace vx
