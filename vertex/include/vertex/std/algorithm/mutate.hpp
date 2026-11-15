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
// in-place mutation: replace
//=========================================================================

template <typename T, typename U, typename V>
constexpr void replace(T* ptr, size_t num, const U& old_value, const V& new_value)
{
    for (size_t i = 0; i < num; ++i)
    {
        if (ptr[i] == old_value)
        {
            ptr[i] = new_value;
        }
    }
}

template <typename IT, typename U, typename V, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr void replace(IT first, IT last, const U& old_value, const V& new_value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (*first == old_value)
        {
            *first = new_value;
        }
    }
}

template <typename T, typename Pred, typename V>
constexpr void replace_if(T* ptr, size_t num, Pred pred, const V& new_value)
{
    for (size_t i = 0; i < num; ++i)
    {
        if (pred(ptr[i]))
        {
            ptr[i] = new_value;
        }
    }
}

template <typename IT, typename Pred, typename V, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr void replace_if(IT first, IT last, Pred pred, const V& new_value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (pred(*first))
        {
            *first = new_value;
        }
    }
}

//=========================================================================
// in-place mutation: reverse / rotate
//=========================================================================

template <typename T>
constexpr void reverse(T* ptr, size_t num)
{
#if VX_STD_USE_SIMD_ALGORITHMS
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (_range_priv::simd_reverse_is_safe<T*>::value)
        {
            if (num >= 2)
            {
                _simd::reverse_simd<sizeof(T)>(ptr, ptr + num);
                return;
            }
        }
    }
#endif

    // num / 2 swaps; the loop is empty for num < 2
    for (size_t i = 0, half = num / 2; i < half; ++i)
    {
        vx::swap(ptr[i], ptr[num - 1 - i]);
    }
}

// Bidirectional iterators are enough. Contiguous ranges forward to the
// pointer form, so the SIMD dispatch lives in one place.
template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr void reverse(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (_range_priv::simd_reverse_is_safe<IT>::value)
        {
            range::reverse(_range_priv::to_pointer(first), static_cast<size_t>(vx::distance(first, last)));
            return;
        }
    }

    // only !=, ++ and -- are required
    while (first != last)
    {
        --last;

        if (first == last)
        {
            break;
        }

        vx::iter_swap(first, last);
        ++first;
    }
}

// Rotates so that ptr[mid] becomes the first element (mid <= num).
// Returns the new position of the old first element: ptr + (num - mid).
template <typename T>
constexpr T* rotate(T* ptr, size_t num, size_t mid)
{
    VX_ASSERT(mid <= num);

    // These two exits are about work, not loop emptiness: without them the
    // fallbacks below would memmove the whole range onto itself / reverse twice.
    if (mid == 0)
    {
        return ptr + num;
    }

    if (mid == num)
    {
        return ptr;
    }

    const size_t left = mid;
    const size_t right = num - mid;

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            // Park the SHORTER side in a stack buffer, memmove the longer
            // side over, then drop the buffer in: about n element moves
            // instead of the ~1.5n swaps of three reverses.
            if (_range_priv::rotate_bytes(ptr, left * sizeof(T), right * sizeof(T)))
            {
                return ptr + right;
            }
        }
    }

    // three reverses: [0,left) [left,num) then the whole range
    range::reverse(ptr, left);
    range::reverse(ptr + left, right);
    range::reverse(ptr, num);
    return ptr + right;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT rotate(IT first, IT middle, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, middle);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(middle, last);

    // needed: the generic paths below don't handle empty halves
    if (first == middle)
    {
        return last;
    }

    if (middle == last)
    {
        return first;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<IT>::value)
        {
            const size_t num = static_cast<size_t>(vx::distance(first, last));
            const size_t mid = static_cast<size_t>(vx::distance(first, middle));
            range::rotate(_range_priv::to_pointer(first), num, mid);
            return last - mid; // first + (num - mid)
        }
    }

    VX_IF_CONSTEXPR (traits::is_random_access_iterator<IT>::value)
    {
        range::reverse(first, middle);
        range::reverse(middle, last);
        range::reverse(first, last);
        return first + (last - middle);
    }
    else
    {
        // forward iterators
        IT first2 = middle;

        do
        {
            vx::iter_swap(first, first2);

            ++first;
            ++first2;

            if (first == middle)
            {
                middle = first2;
            }

        } while (first2 != last);

        const IT result = first;

        while (middle != last)
        {
            first2 = middle;

            do
            {
                vx::iter_swap(first, first2);

                ++first;
                ++first2;

                if (first == middle)
                {
                    middle = first2;
                }

            } while (first2 != last);
        }

        return result;
    }
}

//=========================================================================
// in-place mutation: remove / unique
//=========================================================================

// Moves every element that is NOT equal to `value` toward the front,
// preserving their order. Returns the new logical end.
// Elements in [returned end, ptr + num) are valid but unspecified
// (moved-from); nothing is destroyed.
template <typename T, typename U>
constexpr T* remove(T* ptr, size_t num, const U& value)
{
    T* const last = ptr + num;

    // skip the untouched prefix (memchr path for byte types)
    T* dst = range::find(ptr, num, value);

    // Starting at dst (which matches `value`) re-tests one element but makes
    // the "nothing found" case an empty loop.
    for (T* src = dst; src != last; ++src)
    {
        if (!(*src == value))
        {
            *dst = _range_priv::as_rvalue(*src);
            ++dst;
        }
    }

    return dst;
}

// Drops elements for which pred returns true.
template <typename T, typename Pred>
constexpr T* remove_if(T* ptr, size_t num, Pred pred)
{
    T* const last = ptr + num;
    T* dst = range::find_if(ptr, num, pred);

    // needed: dst + 1 would pass last. (Not folded into the loop like
    // remove() so pred still runs exactly once per element.)
    if (dst == last)
    {
        return last;
    }

    for (T* src = dst + 1; src != last; ++src)
    {
        if (!pred(*src))
        {
            *dst = _range_priv::as_rvalue(*src);
            ++dst;
        }
    }

    return dst;
}

// Forward iterators are enough.
template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT remove(IT first, IT last, const U& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    first = range::find(first, last, value);

    for (IT src = first; src != last; ++src)
    {
        if (!(*src == value))
        {
            *first = _range_priv::as_rvalue(*src);
            ++first;
        }
    }

    return first;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT remove_if(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    first = range::find_if(first, last, pred);

    if (first == last) // needed: ++src must not pass last
    {
        return last;
    }

    for (IT src = first; ++src != last;)
    {
        if (!pred(*src))
        {
            *first = _range_priv::as_rvalue(*src);
            ++first;
        }
    }

    return first;
}

// Collapses each run of consecutive equal elements to its first element.
// Returns the new logical end. Tail is unspecified, as with remove.
template <typename T>
constexpr T* unique(T* ptr, size_t num)
{
    T* const last = ptr + num;
    T* result = range::adjacent_find(ptr, num);

    if (result == last) // needed: result + 1 would pass last
    {
        return last;
    }

    for (T* it = result + 1; ++it != last;)
    {
        if (!(*result == *it))
        {
            ++result;
            *result = _range_priv::as_rvalue(*it);
        }
    }

    return result + 1;
}

// pred(a, b) returns true when a and b are equivalent
template <typename T, typename Pred>
constexpr T* unique(T* ptr, size_t num, Pred pred)
{
    T* const last = ptr + num;
    T* result = range::adjacent_find(ptr, num, pred);

    if (result == last) // needed: result + 1 would pass last
    {
        return last;
    }

    for (T* it = result + 1; ++it != last;)
    {
        if (!pred(*result, *it))
        {
            ++result;
            *result = _range_priv::as_rvalue(*it);
        }
    }

    return result + 1;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT unique(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    IT result = range::adjacent_find(first, last);

    if (result == last) // needed: ++first below must not pass last
    {
        return last;
    }

    for (first = result, ++first; ++first != last;)
    {
        if (!(*result == *first))
        {
            ++result;
            *result = _range_priv::as_rvalue(*first);
        }
    }

    ++result;
    return result;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT unique(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    IT result = range::adjacent_find(first, last, pred);

    if (result == last) // needed: ++first below must not pass last
    {
        return last;
    }

    for (first = result, ++first; ++first != last;)
    {
        if (!pred(*result, *first))
        {
            ++result;
            *result = _range_priv::as_rvalue(*first);
        }
    }

    ++result;
    return result;
}

//=========================================================================
// copying variants: copy_if / remove_copy / unique_copy / reverse_copy /
// rotate_copy
//
// All write to dst and return dst advanced by the number of elements
// written. dst must not overlap the source.
//=========================================================================

template <typename T, typename U, typename Pred>
constexpr T* copy_if(T* dst, const U* src, size_t num, Pred pred)
{
    for (size_t i = 0; i < num; ++i)
    {
        if (pred(src[i]))
        {
            *dst = src[i];
            ++dst;
        }
    }

    return dst;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_if(IT1 dst, IT2 first, IT2 last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (pred(*first))
        {
            *dst = *first;
            ++dst;
        }
    }

    return dst;
}

template <typename T, typename U, typename V>
constexpr T* remove_copy(T* dst, const U* src, size_t num, const V& value)
{
    for (size_t i = 0; i < num; ++i)
    {
        if (!(src[i] == value))
        {
            *dst = src[i];
            ++dst;
        }
    }

    return dst;
}

template <typename IT1, typename IT2, typename V, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 remove_copy(IT1 dst, IT2 first, IT2 last, const V& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (!(*first == value))
        {
            *dst = *first;
            ++dst;
        }
    }

    return dst;
}

template <typename T, typename U, typename Pred>
constexpr T* remove_copy_if(T* dst, const U* src, size_t num, Pred pred)
{
    for (size_t i = 0; i < num; ++i)
    {
        if (!pred(src[i]))
        {
            *dst = src[i];
            ++dst;
        }
    }

    return dst;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 remove_copy_if(IT1 dst, IT2 first, IT2 last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (!pred(*first))
        {
            *dst = *first;
            ++dst;
        }
    }

    return dst;
}

// Copies the first element of each run of equivalent elements.
// Compares against the previous SOURCE element, so forward iterators work.
template <typename T, typename U>
constexpr T* unique_copy(T* dst, const U* src, size_t num)
{
    if (num == 0) // needed: src[0] is read
    {
        return dst;
    }

    *dst = src[0];
    ++dst;

    for (size_t i = 1; i < num; ++i)
    {
        if (!(src[i - 1] == src[i]))
        {
            *dst = src[i];
            ++dst;
        }
    }

    return dst;
}

template <typename T, typename U, typename Pred>
constexpr T* unique_copy(T* dst, const U* src, size_t num, Pred pred)
{
    if (num == 0) // needed: src[0] is read
    {
        return dst;
    }

    *dst = src[0];
    ++dst;

    for (size_t i = 1; i < num; ++i)
    {
        if (!pred(src[i - 1], src[i]))
        {
            *dst = src[i];
            ++dst;
        }
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 unique_copy(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: *first is read
    {
        return dst;
    }

    IT2 prev = first;
    *dst = *first;
    ++dst;

    for (++first; first != last; ++first, ++prev)
    {
        if (!(*prev == *first))
        {
            *dst = *first;
            ++dst;
        }
    }

    return dst;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 unique_copy(IT1 dst, IT2 first, IT2 last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: *first is read
    {
        return dst;
    }

    IT2 prev = first;
    *dst = *first;
    ++dst;

    for (++first; first != last; ++first, ++prev)
    {
        if (!pred(*prev, *first))
        {
            *dst = *first;
            ++dst;
        }
    }

    return dst;
}

// dst[i] = src[num - 1 - i] (use range::reverse for in-place). Returns dst + num.
template <typename T, typename U>
constexpr T* reverse_copy(T* dst, const U* src, size_t num)
{
    for (const U* s = src + num; s != src; ++dst)
    {
        --s;
        *dst = *s;
    }

    return dst;
}

// Source needs bidirectional iterators.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 reverse_copy(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    while (first != last)
    {
        --last;
        *dst = *last;
        ++dst;
    }

    return dst;
}

// Writes the rotation of [src, src + num) to dst, so that src[mid] lands at
// dst[0]: first [mid, num), then [0, mid). mid is an index, matching
// range::rotate. Returns dst + num. (Goes through the iterator copy so T and
// U may differ, and still hits memmove when safe.)
template <typename T, typename U>
constexpr T* rotate_copy(T* dst, const U* src, size_t num, size_t mid)
{
    VX_ASSERT(mid <= num);

    T* out = range::copy(dst, src + mid, src + num);
    return range::copy(out, src, src + mid);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 rotate_copy(IT1 dst, IT2 first, IT2 middle, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, middle);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(middle, last);

    dst = range::copy(dst, middle, last);
    return range::copy(dst, first, middle);
}

} // namespace range
} // namespace vx
