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

// First position in [first, first + n) where pred is false. pred must be
// true for a prefix of the range and false for the rest.
template <typename IT, typename Pred>
constexpr IT partition_point_n(IT first, size_t n, Pred pred)
{
    VX_IF_CONSTEXPR (traits::is_random_access_iterator<IT>::value)
    {
        while (n > 1)
        {
            const size_t half = n / 2;
            first += pred(*(first + (half - 1))) ? half : 0;
            n -= half;
        }

        return first + static_cast<size_t>(n != 0 && pred(*first));
    }
    else
    {
        while (n > 0)
        {
            const size_t half = n / 2;
            IT mid = first;
            vx::advance(mid, half);

            if (pred(*mid))
            {
                first = ++mid;
                n -= half + 1;
            }
            else
            {
                n = half;
            }
        }

        return first;
    }
}

} // namespace _range_priv

// First element for which !comp(element, value), or last.
template <typename T, typename U, typename Compare>
constexpr T* lower_bound(T* ptr, size_t n, const U& value, Compare comp)
{
    return _range_priv::partition_point_n(ptr, n, [&](const auto& e)
        { return comp(e, value); });
}

template <typename T, typename U>
constexpr T* lower_bound(T* ptr, size_t n, const U& value)
{
    return range::lower_bound(ptr, n, value, vx::less<>{});
}

template <typename IT, typename U, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT lower_bound(IT first, IT last, const U& value, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    return _range_priv::partition_point_n(first, static_cast<size_t>(vx::distance(first, last)), [&](const auto& e)
        { return comp(e, value); });
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT lower_bound(IT first, IT last, const U& value)
{
    return range::lower_bound(first, last, value, vx::less<>{});
}

// First element for which comp(value, element), or last.
template <typename T, typename U, typename Compare>
constexpr T* upper_bound(T* ptr, size_t n, const U& value, Compare comp)
{
    return _range_priv::partition_point_n(ptr, n, [&](const auto& e)
        { return !comp(value, e); });
}

template <typename T, typename U>
constexpr T* upper_bound(T* ptr, size_t n, const U& value)
{
    return range::upper_bound(ptr, n, value, vx::less<>{});
}

template <typename IT, typename U, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT upper_bound(IT first, IT last, const U& value, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    return _range_priv::partition_point_n(first, static_cast<size_t>(vx::distance(first, last)), [&](const auto& e)
        { return !comp(value, e); });
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT upper_bound(IT first, IT last, const U& value)
{
    return range::upper_bound(first, last, value, vx::less<>{});
}

// [lower_bound, upper_bound)
template <typename T, typename U, typename Compare>
constexpr vx::pair<T*, T*> equal_range(T* ptr, size_t n, const U& value, Compare comp)
{
    T* const lo = range::lower_bound(ptr, n, value, comp);
    return vx::pair<T*, T*>(lo, range::upper_bound(lo, n - static_cast<size_t>(lo - ptr), value, comp));
}

template <typename T, typename U>
constexpr vx::pair<T*, T*> equal_range(T* ptr, size_t n, const U& value)
{
    return range::equal_range(ptr, n, value, vx::less<>{});
}

template <typename IT, typename U, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr vx::pair<IT, IT> equal_range(IT first, IT last, const U& value, Compare comp)
{
    const IT lo = range::lower_bound(first, last, value, comp);
    return vx::pair<IT, IT>(lo, range::upper_bound(lo, last, value, comp));
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr vx::pair<IT, IT> equal_range(IT first, IT last, const U& value)
{
    return range::equal_range(first, last, value, vx::less<>{});
}

template <typename T, typename U, typename Compare>
constexpr bool binary_search(const T* ptr, size_t n, const U& value, Compare comp)
{
    const T* const it = range::lower_bound(ptr, n, value, comp);
    return it != ptr + n && !comp(value, *it);
}

template <typename T, typename U>
constexpr bool binary_search(const T* ptr, size_t n, const U& value)
{
    return range::binary_search(ptr, n, value, vx::less<>{});
}

template <typename IT, typename U, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool binary_search(IT first, IT last, const U& value, Compare comp)
{
    const IT it = range::lower_bound(first, last, value, comp);
    return it != last && !comp(value, *it);
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool binary_search(IT first, IT last, const U& value)
{
    return range::binary_search(first, last, value, vx::less<>{});
}

} // namespace range
} // namespace vx
