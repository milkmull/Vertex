#pragma once

#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/type_traits.hpp"

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

namespace vx {

//=========================================================================
// Result type
//=========================================================================

// Shared by the value and range APIs:
//   minmax_result<const T&>  -> references to values   (vx::minmax)
//   minmax_result<IT>        -> iterators / pointers   (vx::range::minmax)
template <typename T>
struct minmax_result
{
    T min;
    T max;
};

//=========================================================================
// Scalar: min / max / minmax / clamp (two values)
//=========================================================================

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
constexpr minmax_result<const T&> minmax(const T& a, const T& b) noexcept(noexcept(b < a))
{
    return (b < a) ? minmax_result<const T&>{ b, a } : minmax_result<const T&>{ a, b };
}

template <typename T, typename Compare>
constexpr minmax_result<const T&> minmax(const T& a, const T& b, Compare comp) noexcept(noexcept(comp(b, a)))
{
    return comp(b, a) ? minmax_result<const T&>{ b, a } : minmax_result<const T&>{ a, b };
}

template <typename T>
constexpr const T& clamp(const T& value, const T& low, const T& high) noexcept(noexcept(value < low) && noexcept(high < value))
{
    return (value < low) ? low : ((high < value) ? high : value);
}

template <typename T, typename Compare>
constexpr const T& clamp(const T& value, const T& low, const T& high, Compare comp) noexcept(noexcept(comp(value, low)) && noexcept(comp(high, value)))
{
    return comp(value, low) ? low : (comp(high, value) ? high : value);
}

namespace range {

//=========================================================================
// Iterator-based: min / max / minmax
// (return iterators; `last` if the range is empty)
//=========================================================================

// ---- min ----------------------------------------------------------------

// Returns the first smallest element, or last if the range is empty.
// (Starts at `first`, so the first iteration compares an element with itself:
// one extra comparison, but no empty-range branch and no deref of `last`.)
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT min(IT first, IT last, Compare comp)
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
constexpr IT min(IT first, IT last)
{
    return range::min(first, last, vx::less<>{});
}

// ---- max ----------------------------------------------------------------

// Returns the first largest element, or last if the range is empty.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT max(IT first, IT last, Compare comp)
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
constexpr IT max(IT first, IT last)
{
    return range::max(first, last, vx::less<>{});
}

// ---- minmax -------------------------------------------------------------

// First smallest, LAST largest (same as std::minmax_element).
// Returns {last, last} if the range is empty.
// Processes elements in pairs: ~1.5 comparisons per element.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr minmax_result<IT> minmax(IT first, IT last, Compare comp)
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
constexpr minmax_result<IT> minmax(IT first, IT last)
{
    return range::minmax(first, last, vx::less<>{});
}

//=========================================================================
// Pointer + count
// (return pointers; ptr + count if the range is empty)
//=========================================================================

template <typename T, typename Compare>
constexpr const T* min(const T* ptr, size_t count, Compare comp)
{
    return range::min(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T* min(const T* ptr, size_t count)
{
    return range::min(ptr, ptr + count, vx::less<>{});
}

template <typename T, typename Compare>
constexpr const T* max(const T* ptr, size_t count, Compare comp)
{
    return range::max(ptr, ptr + count, comp);
}

template <typename T>
constexpr const T* max(const T* ptr, size_t count)
{
    return range::max(ptr, ptr + count, vx::less<>{});
}

template <typename T, typename Compare>
constexpr minmax_result<const T*> minmax(const T* ptr, size_t count, Compare comp)
{
    return range::minmax(ptr, ptr + count, comp);
}

template <typename T>
constexpr minmax_result<const T*> minmax(const T* ptr, size_t count)
{
    return range::minmax(ptr, ptr + count, vx::less<>{});
}

} // namespace range
} // namespace vx
