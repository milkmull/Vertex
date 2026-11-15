#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

namespace vx {
namespace range {

template <typename T>
constexpr const T& min(const T& a, const T& b) noexcept(noexcept(b < a))
{
    return (b < a) ? b : a;
}

template <typename T, typename Compare>
constexpr const T& min(const T& a, const T& b, Compare comp) noexcept(noexcept(comp(b, a)))
{
    return comp(b, a) ? b : a;
}

template <typename T>
constexpr const T& max(const T& a, const T& b) noexcept(noexcept(a < b))
{
    return (a < b) ? b : a;
}

template <typename T, typename Compare>
constexpr const T& max(const T& a, const T& b, Compare comp) noexcept(noexcept(comp(a, b)))
{
    return comp(a, b) ? b : a;
}

template <typename T>
struct minmax_result
{
    const T& min;
    const T& max;
};

template <typename T>
constexpr minmax_result<T> minmax(const T& a, const T& b) noexcept(noexcept(b < a))
{
    return (b < a) ? minmax_result<T>{ b, a } : minmax_result<T>{ a, b };
}

template <typename T, typename Compare>
constexpr minmax_result<T> minmax(const T& a, const T& b, Compare comp) noexcept(noexcept(comp(b, a)))
{
    return comp(b, a) ? minmax_result<T>{ b, a } : minmax_result<T>{ a, b };
}

template <typename T>
constexpr const T& clamp(const T& value, const T& low, const T& high) noexcept(noexcept(value < low) && noexcept(high < value))
{
    return (value < low) ? low : ((high < value) ? high : value);
}

template <typename T, typename Compare>
constexpr const T& clamp(const T& value, const T& low, const T& high, Compare comp)
{
    return comp(value, low) ? low : (comp(high, value) ? high : value);
}

namespace range {

//=========================================================================
// extrema over ranges
//=========================================================================

// Returns the first smallest element, or last if the range is empty.
// (Starts at `first`, so the first iteration compares an element with itself:
// one extra comparison, but no empty-range branch and no deref of `last`.)
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT min_element(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    IT result = first;

    for (; first != last; ++first)
    {
        if (comp(*first, *result))
        {
            result = first;
        }
    }

    return result;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT min_element(IT first, IT last)
{
    return range::min_element(first, last, vx::less<>{});
}

// pointer + count: returns ptr + count if the range is empty
template <typename T, typename Compare>
constexpr const T* min_element(const T* ptr, size_t count, Compare comp)
{
    return range::min_element(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T* min_element(const T* ptr, size_t count)
{
    return range::min_element(ptr, ptr + count, vx::less<>{});
}

// value of the smallest element. The range must not be empty.
template <typename T, typename Compare>
constexpr const T& min(const T* ptr, size_t count, Compare comp)
{
    VX_ASSERT(count != 0);
    return *range::min_element(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T& min(const T* ptr, size_t count)
{
    VX_ASSERT(count != 0);
    return *range::min_element(ptr, ptr + count, vx::less<>{});
}

//=========================================================================

// Returns the first largest element, or last if the range is empty.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT max_element(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    IT result = first;

    for (; first != last; ++first)
    {
        if (comp(*result, *first))
        {
            result = first;
        }
    }

    return result;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT max_element(IT first, IT last)
{
    return range::max_element(first, last, vx::less<>{});
}

template <typename T, typename Compare>
constexpr const T* max_element(const T* ptr, size_t count, Compare comp)
{
    return range::max_element(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T* max_element(const T* ptr, size_t count)
{
    return range::max_element(ptr, ptr + count, vx::less<>{});
}

template <typename T, typename Compare>
constexpr const T& max(const T* ptr, size_t count, Compare comp)
{
    VX_ASSERT(count != 0);
    return *range::max_element(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T& max(const T* ptr, size_t count)
{
    VX_ASSERT(count != 0);
    return *range::max_element(ptr, ptr + count, vx::less<>{});
}

//=========================================================================

template <typename IT>
struct minmax_element_result
{
    IT min;
    IT max;
};

// First smallest, LAST largest (same as std::minmax_element).
// Processes elements in pairs: ~1.5 comparisons per element.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr minmax_element_result<IT> minmax_element(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last)
    {
        return { last, last };
    }

    IT min_it = first;
    IT max_it = first;

    ++first;
    if (first == last)
    {
        return { min_it, max_it };
    }

    // Seed with the second element so the loop below only sees full pairs.
    // On a tie it becomes the max (later element wins for max).
    if (comp(*first, *min_it))
    {
        min_it = first;
    }
    else
    {
        max_it = first;
    }

    ++first;

    while (first != last)
    {
        IT a = first;
        ++first;

        if (first == last)
        {
            // odd element out: compare against both
            if (comp(*a, *min_it))
            {
                min_it = a;
            }
            else if (!comp(*a, *max_it))
            {
                max_it = a;
            }
            break;
        }

        // `a` is the earlier element of the pair, `first` the later one.
        // On a tie the later element counts as the larger.
        if (comp(*first, *a))
        {
            if (comp(*first, *min_it))
            {
                min_it = first;
            }
            if (!comp(*a, *max_it))
            {
                max_it = a;
            }
        }
        else
        {
            if (comp(*a, *min_it))
            {
                min_it = a;
            }
            if (!comp(*first, *max_it))
            {
                max_it = first;
            }
        }

        ++first;
    }

    return { min_it, max_it };
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr minmax_element_result<IT> minmax_element(IT first, IT last)
{
    return range::minmax_element(first, last, vx::less<>{});
}

// pointer + count: returns {ptr + count, ptr + count} if the range is empty
template <typename T, typename Compare>
constexpr minmax_element_result<const T*> minmax_element(const T* ptr, size_t count, Compare comp)
{
    return range::minmax_element(ptr, ptr + count, comp);
}

template <typename T>
constexpr minmax_element_result<const T*> minmax_element(const T* ptr, size_t count)
{
    return range::minmax_element(ptr, ptr + count, vx::less<>{});
}

// References to the smallest and largest elements. The range must not be empty.
template <typename T, typename Compare>
constexpr minmax_result<T> minmax(const T* ptr, size_t count, Compare comp)
{
    VX_ASSERT(count != 0);
    const minmax_element_result<const T*> r = range::minmax_element(ptr, ptr + count, comp);
    return { *r.min, *r.max };
}

template <typename T>
constexpr minmax_result<T> minmax(const T* ptr, size_t count)
{
    return range::minmax(ptr, count, vx::less<>{});
}

//=========================================================================
// min / max / minmax for initializer_list (value forms, non-empty)
//=========================================================================

template <typename T, typename Compare>
constexpr T min(std::initializer_list<T> il, Compare comp)
{
    VX_ASSERT(il.size() != 0);
    return *range::min_element(il.begin(), il.end(), comp);
}

template <typename T>
constexpr T min(std::initializer_list<T> il)
{
    return vx::min(il, vx::less<>{});
}

template <typename T, typename Compare>
constexpr T max(std::initializer_list<T> il, Compare comp)
{
    VX_ASSERT(il.size() != 0);
    return *range::max_element(il.begin(), il.end(), comp);
}

template <typename T>
constexpr T max(std::initializer_list<T> il)
{
    return vx::max(il, vx::less<>{});
}

template <typename T, typename Compare>
constexpr vx::pair<T, T> minmax(std::initializer_list<T> il, Compare comp)
{
    VX_ASSERT(il.size() != 0);
    const auto r = range::minmax_element(il.begin(), il.end(), comp);
    return vx::pair<T, T>(*r.min, *r.max);
}

template <typename T>
constexpr vx::pair<T, T> minmax(std::initializer_list<T> il)
{
    return vx::minmax(il, vx::less<>{});
}

} // namespace vx
