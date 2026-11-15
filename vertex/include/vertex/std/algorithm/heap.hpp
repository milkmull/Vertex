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

// Moves `value` up from `hole` while its parent is smaller (never above `top`).
template <typename IT, typename T, typename Compare>
constexpr void heap_sift_up(IT first, size_t hole, size_t top, T& value, Compare comp)
{
    while (hole > top)
    {
        const size_t parent = (hole - 1) / 2;

        if (!comp(first[parent], value))
        {
            break;
        }

        first[hole] = _range_priv::as_rvalue(first[parent]);
        hole = parent;
    }

    first[hole] = _range_priv::as_rvalue(value);
}

// Sifts the hole at `hole` down to a leaf, then places `value` by sifting up.
// len >= 1.
template <typename IT, typename T, typename Compare>
constexpr void heap_sift_down(IT first, size_t hole, size_t len, T& value, Compare comp)
{
    const size_t top = hole;
    size_t child = hole;

    while (child < (len - 1) / 2)
    {
        child = 2 * (child + 1);

        if (comp(first[child], first[child - 1]))
        {
            --child;
        }

        first[hole] = _range_priv::as_rvalue(first[child]);
        hole = child;
    }

    if ((len & 1) == 0 && child == (len - 2) / 2)
    {
        child = 2 * (child + 1);
        first[hole] = _range_priv::as_rvalue(first[child - 1]);
        hole = child - 1;
    }

    _range_priv::heap_sift_up(first, hole, top, value, comp);
}

// Moves the heap top into *result and re-inserts the old *result into the
// remaining heap [first, first + len).
template <typename IT, typename Compare>
constexpr void heap_pop_to(IT first, size_t len, IT result, Compare comp)
{
    using T = typename traits::value_type<IT>::type;

    T value = _range_priv::as_rvalue(*result);
    *result = _range_priv::as_rvalue(*first);
    _range_priv::heap_sift_down(first, 0, len, value, comp);
}

template <typename IT, typename Compare>
constexpr void make_heap_impl(IT first, size_t n, Compare comp)
{
    using T = typename traits::value_type<IT>::type;

    for (size_t i = n / 2; i-- > 0;)
    {
        T value = _range_priv::as_rvalue(first[i]);
        _range_priv::heap_sift_down(first, i, n, value, comp);
    }
}

template <typename IT, typename Compare>
constexpr void sort_heap_impl(IT first, size_t n, Compare comp)
{
    for (; n > 1; --n)
    {
        _range_priv::heap_pop_to(first, n - 1, first + (n - 1), comp);
    }
}

// Puts the (middle - first) smallest elements of [first, last) in a heap at
// the front (the largest of them at first).
template <typename IT, typename Compare>
constexpr void heap_select(IT first, IT middle, IT last, Compare comp)
{
    const size_t len = static_cast<size_t>(middle - first);

    if (len == 0) // needed: *first is read below
    {
        return;
    }

    _range_priv::make_heap_impl(first, len, comp);

    for (IT it = middle; it < last; ++it)
    {
        if (comp(*it, *first))
        {
            _range_priv::heap_pop_to(first, len, it, comp);
        }
    }
}

} // namespace _range_priv

template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void make_heap(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    _range_priv::make_heap_impl(_range_priv::unwrap(first), static_cast<size_t>(last - first), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void make_heap(IT first, IT last)
{
    range::make_heap(first, last, vx::less<>{});
}

// The new element is at last - 1.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void push_heap(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    using T = typename traits::value_type<IT>::type;

    const auto f = _range_priv::unwrap(first);
    const size_t n = static_cast<size_t>(last - first);

    T value = _range_priv::as_rvalue(f[n - 1]);
    _range_priv::heap_sift_up(f, n - 1, 0, value, comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void push_heap(IT first, IT last)
{
    range::push_heap(first, last, vx::less<>{});
}

// Moves the top to last - 1 and restores the heap in [first, last - 1).
template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void pop_heap(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    const auto f = _range_priv::unwrap(first);
    const size_t n = static_cast<size_t>(last - first);

    if (n >= 2) // needed: n == 1 would self-move
    {
        _range_priv::heap_pop_to(f, n - 1, f + (n - 1), comp);
    }
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void pop_heap(IT first, IT last)
{
    range::pop_heap(first, last, vx::less<>{});
}

template <typename IT, typename Compare, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void sort_heap(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    _range_priv::sort_heap_impl(_range_priv::unwrap(first), static_cast<size_t>(last - first), comp);
}

template <typename IT, VX_REQUIRES(traits::is_random_access_iterator<IT>::value)>
constexpr void sort_heap(IT first, IT last)
{
    range::sort_heap(first, last, vx::less<>{});
}

} // namespace range
} // namespace vx
