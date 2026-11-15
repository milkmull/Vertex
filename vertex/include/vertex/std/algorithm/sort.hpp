#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/type_traits.hpp"

namespace vx {
namespace range {

namespace _range_priv {

// `last` has a smaller-or-equal element somewhere before it.
template <typename IT, typename Compare>
constexpr void unguarded_linear_insert(IT last, Compare comp)
{
    using T = typename traits::value_type<IT>::type;

    T value = _range_priv::as_rvalue(*last);
    IT prev = last;
    --prev;

    while (comp(value, *prev))
    {
        *last = _range_priv::as_rvalue(*prev);
        last = prev;
        --prev;
    }

    *last = _range_priv::as_rvalue(value);
}

// Stable. An element smaller than everything so far shifts the whole prefix
// with one move_backward (memmove for trivial types).
template <typename IT, typename Compare>
constexpr void insertion_sort(IT first, IT last, Compare comp)
{
    using T = typename traits::value_type<IT>::type;

    if (first == last) // needed: ++i below must not pass last
    {
        return;
    }

    for (IT i = first; ++i != last;)
    {
        if (comp(*i, *first))
        {
            T value = _range_priv::as_rvalue(*i);
            range::move_backward(i + 1, first, i);
            *first = _range_priv::as_rvalue(value);
        }
        else
        {
            _range_priv::unguarded_linear_insert(i, comp);
        }
    }
}

template <typename IT, typename Compare>
constexpr void unguarded_insertion_sort(IT first, IT last, Compare comp)
{
    for (; first != last; ++first)
    {
        _range_priv::unguarded_linear_insert(first, comp);
    }
}

template <typename IT, typename Compare>
constexpr void final_insertion_sort(IT first, IT last, Compare comp)
{
    if (static_cast<size_t>(last - first) > sort_threshold)
    {
        _range_priv::insertion_sort(first, first + sort_threshold, comp);
        _range_priv::unguarded_insertion_sort(first + sort_threshold, last, comp);
    }
    else
    {
        _range_priv::insertion_sort(first, last, comp);
    }
}

// Swaps the median of *a, *b, *c into *result.
template <typename IT, typename Compare>
constexpr void move_median_to_first(IT result, IT a, IT b, IT c, Compare comp)
{
    if (comp(*a, *b))
    {
        if (comp(*b, *c))
        {
            vx::iter_swap(result, b);
        }
        else if (comp(*a, *c))
        {
            vx::iter_swap(result, c);
        }
        else
        {
            vx::iter_swap(result, a);
        }
    }
    else if (comp(*a, *c))
    {
        vx::iter_swap(result, a);
    }
    else if (comp(*b, *c))
    {
        vx::iter_swap(result, c);
    }
    else
    {
        vx::iter_swap(result, b);
    }
}

// Hoare partition around *pivot. Needs a stopper on both sides, which the
// median-of-3 pivot provides. Returns the split point.
template <typename IT, typename Compare>
constexpr IT unguarded_partition(IT first, IT last, IT pivot, Compare comp)
{
    for (;;)
    {
        while (comp(*first, *pivot))
        {
            ++first;
        }

        --last;

        while (comp(*pivot, *last))
        {
            --last;
        }

        if (!(first < last))
        {
            return first;
        }

        vx::iter_swap(first, last);
        ++first;
    }
}

// Median-of-3 pivot moved to *first, then partition [first + 1, last).
template <typename IT, typename Compare>
constexpr IT unguarded_partition_pivot(IT first, IT last, Compare comp)
{
    const IT mid = first + (last - first) / 2;
    _range_priv::move_median_to_first(first, first + 1, mid, last - 1, comp);
    return _range_priv::unguarded_partition(first + 1, last, first, comp);
}

template <typename IT, typename Compare>
constexpr void introsort_loop(IT first, IT last, size_t depth, Compare comp)
{
    while (static_cast<size_t>(last - first) > sort_threshold)
    {
        if (depth == 0)
        {
            const size_t n = static_cast<size_t>(last - first);
            _range_priv::make_heap_impl(first, n, comp);
            _range_priv::sort_heap_impl(first, n, comp);
            return;
        }

        --depth;
        const IT cut = _range_priv::unguarded_partition_pivot(first, last, comp);
        _range_priv::introsort_loop(cut, last, depth, comp);
        last = cut;
    }
}

template <typename IT, typename Compare>
constexpr void sort_impl(IT first, IT last, Compare comp)
{
    _range_priv::introsort_loop(first, last, 2 * _range_priv::floor_log2(static_cast<size_t>(last - first)), comp);
    _range_priv::final_insertion_sort(first, last, comp);
}

template <typename IT, typename Compare>
constexpr void nth_element_impl(IT first, IT nth, IT last, Compare comp)
{
    if (nth == last) // nothing to place
    {
        return;
    }

    size_t depth = 2 * _range_priv::floor_log2(static_cast<size_t>(last - first));

    while (static_cast<size_t>(last - first) > 3)
    {
        if (depth == 0)
        {
            _range_priv::heap_select(first, nth + 1, last, comp);
            vx::iter_swap(first, nth);
            return;
        }

        --depth;
        const IT cut = _range_priv::unguarded_partition_pivot(first, last, comp);

        if (cut <= nth)
        {
            first = cut;
        }
        else
        {
            last = cut;
        }
    }

    _range_priv::insertion_sort(first, last, comp);
}

} // namespace _range_priv

template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void sort(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    VX_PRIV_ASSERT_STRICT_WEAK(first, last, comp);
    _range_priv::sort_impl(_range_priv::unwrap(first), _range_priv::unwrap(last), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void sort(IT first, IT last)
{
    range::sort(first, last, vx::less<>{});
}

template <typename T, typename Compare>
constexpr void sort(T* ptr, size_t n, Compare comp)
{
    range::sort(ptr, ptr + n, comp);
}

template <typename T>
constexpr void sort(T* ptr, size_t n)
{
    range::sort(ptr, ptr + n, vx::less<>{});
}

// Sorts [first, middle) with the smallest elements; the rest is unspecified.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void partial_sort(IT first, IT middle, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, middle);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(middle, last);
    VX_PRIV_ASSERT_STRICT_WEAK(first, last, comp);

    const auto f = _range_priv::unwrap(first);
    _range_priv::heap_select(f, f + (middle - first), f + (last - first), comp);
    _range_priv::sort_heap_impl(f, static_cast<size_t>(middle - first), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void partial_sort(IT first, IT middle, IT last)
{
    range::partial_sort(first, middle, last, vx::less<>{});
}

// Introselect. Afterwards *nth is what a full sort would put there, nothing
// after it is smaller and nothing before it is larger.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void nth_element(IT first, IT nth, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, nth);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(nth, last);
    VX_PRIV_ASSERT_STRICT_WEAK(first, last, comp);

    const auto f = _range_priv::unwrap(first);
    _range_priv::nth_element_impl(f, f + (nth - first), f + (last - first), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void nth_element(IT first, IT nth, IT last)
{
    range::nth_element(first, nth, last, vx::less<>{});
}

template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool is_sorted(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: ++next below must not pass last
    {
        return true;
    }

    for (IT next = first; ++next != last; ++first)
    {
        if (comp(*next, *first))
        {
            return false;
        }
    }

    return true;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool is_sorted(IT first, IT last)
{
    return range::is_sorted(first, last, vx::less<>{});
}

template <typename T, typename Compare>
constexpr bool is_sorted(const T* ptr, size_t n, Compare comp)
{
    for (size_t i = 1; i < n; ++i)
    {
        if (comp(ptr[i], ptr[i - 1]))
        {
            return false;
        }
    }

    return true;
}

template <typename T>
constexpr bool is_sorted(const T* ptr, size_t n)
{
    return range::is_sorted(ptr, n, vx::less<>{});
}

} // namespace vx
