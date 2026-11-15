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
// searching: find / find_if / find_if_not
//=========================================================================

// Returns a pointer to the first element equal to `value`, or ptr + count.
template <typename T, typename U>
constexpr T* find(T* ptr, size_t count, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((_range_priv::can_find_memchr<T, U>::value))
        {
            using V = traits::remove_cv_t<T>;

            // a value that doesn't survive the cast to T can never match
            if (static_cast<U>(static_cast<V>(value)) != value)
            {
                return ptr + count;
            }

            auto* hit = mem::find(ptr, static_cast<unsigned char>(value), count);
            return hit ? static_cast<T*>(hit) : ptr + count;
        }
    }

    T* const last = ptr + count;
    for (; ptr != last; ++ptr)
    {
        if (*ptr == value)
        {
            return ptr;
        }
    }

    return last;
}

// Returns the first position whose element equals `value`, or last.
template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT find(IT first, IT last, const U& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT>::type;

        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT>::value && _range_priv::can_find_memchr<T, U>::value))
        {
            auto* ptr = _range_priv::to_pointer(first);
            return first + (range::find(ptr, static_cast<size_t>(vx::distance(first, last)), value) - ptr);
        }
    }

    for (; first != last; ++first)
    {
        if (*first == value)
        {
            return first;
        }
    }

    return last;
}

template <typename T, typename Pred>
constexpr T* find_if(T* ptr, size_t count, Pred pred)
{
    T* const last = ptr + count;

    for (; ptr != last; ++ptr)
    {
        if (pred(*ptr))
        {
            return ptr;
        }
    }

    return last;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT find_if(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (pred(*first))
        {
            return first;
        }
    }

    return last;
}

template <typename T, typename Pred>
constexpr T* find_if_not(T* ptr, size_t count, Pred pred)
{
    T* const last = ptr + count;

    for (; ptr != last; ++ptr)
    {
        if (!pred(*ptr))
        {
            return ptr;
        }
    }

    return last;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT find_if_not(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        if (!pred(*first))
        {
            return first;
        }
    }

    return last;
}

//=========================================================================
// searching: count / any_of / all_of / none_of
//=========================================================================

// Number of elements equal to `value`.
template <typename T, typename U>
constexpr size_t count(const T* ptr, size_t num, const U& value)
{
    size_t result = 0;

    for (const T* const last = ptr + num; ptr != last; ++ptr)
    {
        // branchless form keeps the loop easy to vectorize
        result += static_cast<size_t>(*ptr == value);
    }

    return result;
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr size_t count(IT first, IT last, const U& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT>::value))
        {
            return range::count(_range_priv::to_pointer(first), static_cast<size_t>(vx::distance(first, last)), value);
        }
    }

    size_t result = 0;

    for (; first != last; ++first)
    {
        result += static_cast<size_t>(*first == value);
    }

    return result;
}

// Number of elements for which pred returns true.
template <typename T, typename Pred>
constexpr size_t count_if(const T* ptr, size_t num, Pred pred)
{
    size_t result = 0;

    for (const T* const last = ptr + num; ptr != last; ++ptr)
    {
        result += static_cast<size_t>(static_cast<bool>(pred(*ptr)));
    }

    return result;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr size_t count_if(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    size_t result = 0;

    for (; first != last; ++first)
    {
        result += static_cast<size_t>(static_cast<bool>(pred(*first)));
    }

    return result;
}

// True if pred returns true for at least one element. False for an empty range.
template <typename T, typename Pred>
constexpr bool any_of(const T* ptr, size_t num, Pred pred)
{
    return range::find_if(ptr, num, pred) != ptr + num;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool any_of(IT first, IT last, Pred pred)
{
    return range::find_if(first, last, pred) != last;
}

// True if pred returns true for every element. True for an empty range.
template <typename T, typename Pred>
constexpr bool all_of(const T* ptr, size_t num, Pred pred)
{
    return range::find_if_not(ptr, num, pred) == ptr + num;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool all_of(IT first, IT last, Pred pred)
{
    return range::find_if_not(first, last, pred) == last;
}

// True if pred returns false for every element. True for an empty range.
template <typename T, typename Pred>
constexpr bool none_of(const T* ptr, size_t num, Pred pred)
{
    return range::find_if(ptr, num, pred) == ptr + num;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr bool none_of(IT first, IT last, Pred pred)
{
    return range::find_if(first, last, pred) == last;
}

//=========================================================================
// searching: adjacent_find
//=========================================================================

// Returns the first element equal to its successor, or ptr + num if none.
template <typename T>
constexpr T* adjacent_find(T* ptr, size_t num)
{
    for (size_t i = 1; i < num; ++i)
    {
        if (ptr[i - 1] == ptr[i])
        {
            return ptr + (i - 1);
        }
    }

    return ptr + num;
}

template <typename T, typename Pred>
constexpr T* adjacent_find(T* ptr, size_t num, Pred pred)
{
    for (size_t i = 1; i < num; ++i)
    {
        if (pred(ptr[i - 1], ptr[i]))
        {
            return ptr + (i - 1);
        }
    }

    return ptr + num;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT adjacent_find(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: ++next below must not pass last
    {
        return last;
    }

    for (IT next = first; ++next != last; ++first)
    {
        if (*first == *next)
        {
            return first;
        }
    }

    return last;
}

template <typename IT, typename Pred, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT adjacent_find(IT first, IT last, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last) // needed: ++next below must not pass last
    {
        return last;
    }

    for (IT next = first; ++next != last; ++first)
    {
        if (pred(*first, *next))
        {
            return first;
        }
    }

    return last;
}

//=========================================================================
// searching: search / find_end / find_first_of (subsequences)
//=========================================================================

// Returns a pointer to the first position in [a, a + na) where [b, b + nb)
// occurs, or a + na if there is none. An empty needle matches at a.
//
// Skips ahead with find() on the needle's first element (memchr for byte
// types), then verifies the rest with equal() (memcmp when safe).
// TODO: mem::find_bytes (memmem) could replace this for byte types.
template <typename T, typename U>
constexpr T* search(T* a, size_t na, const U* b, size_t nb)
{
    if (nb == 0) // needed: b[0] is read below
    {
        return a;
    }

    if (nb > na) // needed: na - nb + 1 would underflow
    {
        return a + na;
    }

    T* const end = a + (na - nb + 1); // one past the last valid start position

    for (T* p = a; p != end; ++p)
    {
        p = range::find(p, static_cast<size_t>(end - p), b[0]);

        if (p == end)
        {
            break;
        }

        if (range::equal(p + 1, b + 1, nb - 1))
        {
            return p;
        }
    }

    return a + na;
}

template <typename T, typename U, typename Pred>
constexpr T* search(T* a, size_t na, const U* b, size_t nb, Pred pred)
{
    if (nb > na) // needed: na - nb + 1 would underflow
    {
        return a + na;
    }

    const size_t positions = na - nb + 1; // an empty needle matches at i == 0

    for (size_t i = 0; i < positions; ++i)
    {
        if (range::equal(a + i, b, nb, pred))
        {
            return a + i;
        }
    }

    return a + na;
}

// Forward iterators are enough.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 search(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            auto* p = _range_priv::to_pointer(first1);

            return first1 + (range::search(p, static_cast<size_t>(vx::distance(first1, last1)), _range_priv::to_pointer(first2), static_cast<size_t>(vx::distance(first2, last2))) - p);
        }
    }

    // an empty needle returns first1 on the first pass; an empty haystack returns last1
    for (;; ++first1)
    {
        IT1 it1 = first1;
        IT2 it2 = first2;

        for (;;)
        {
            if (it2 == last2)
            {
                return first1;
            }

            if (it1 == last1)
            {
                return last1;
            }

            if (!(*it1 == *it2))
            {
                break;
            }

            ++it1;
            ++it2;
        }
    }
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 search(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    for (;; ++first1)
    {
        IT1 it1 = first1;
        IT2 it2 = first2;

        for (;;)
        {
            if (it2 == last2)
            {
                return first1;
            }

            if (it1 == last1)
            {
                return last1;
            }

            if (!pred(*it1, *it2))
            {
                break;
            }

            ++it1;
            ++it2;
        }
    }
}

//=========================================================================

// Returns a + na if there is no match or the needle is empty (as std does).
// Scans backward so the first hit is the last occurrence; the cheap
// first-element test avoids a memcmp call at every position.
template <typename T, typename U>
constexpr T* find_end(T* a, size_t na, const U* b, size_t nb)
{
    if (nb == 0 || nb > na) // needed: b[0] is read / na - nb + 1 would underflow
    {
        return a + na;
    }

    for (size_t i = na - nb + 1; i-- > 0;)
    {
        if (a[i] == b[0] && range::equal(a + i + 1, b + 1, nb - 1))
        {
            return a + i;
        }
    }

    return a + na;
}

template <typename T, typename U, typename Pred>
constexpr T* find_end(T* a, size_t na, const U* b, size_t nb, Pred pred)
{
    if (nb > na) // needed: na - nb + 1 would underflow
    {
        return a + na;
    }

    // an empty needle "matches" at na, which is the documented result
    for (size_t i = na - nb + 1; i-- > 0;)
    {
        if (range::equal(a + i, b, nb, pred))
        {
            return a + i;
        }
    }

    return a + na;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 find_end(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (first2 == last2) // needed: the forward loop below would return the last element
    {
        return last1;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            auto* p = _range_priv::to_pointer(first1);

            return first1 + (range::find_end(p, static_cast<size_t>(vx::distance(first1, last1)), _range_priv::to_pointer(first2), static_cast<size_t>(vx::distance(first2, last2))) - p);
        }
    }

    // forward iterators: keep searching past each hit
    IT1 result = last1;

    for (;;)
    {
        const IT1 hit = range::search(first1, last1, first2, last2);

        if (hit == last1)
        {
            return result;
        }

        result = hit;
        first1 = hit;
        ++first1;
    }
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 find_end(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (first2 == last2) // needed: the forward loop below would return the last element
    {
        return last1;
    }

    IT1 result = last1;

    for (;;)
    {
        const IT1 hit = range::search(first1, last1, first2, last2, pred);

        if (hit == last1)
        {
            return result;
        }

        result = hit;
        first1 = hit;
        ++first1;
    }
}

//=========================================================================

// First element of range 1 equal to ANY element of range 2.
template <typename T, typename U>
constexpr T* find_first_of(T* a, size_t na, const U* b, size_t nb)
{
    if (nb == 1) // single candidate: plain find (gets the memchr path)
    {
        return range::find(a, na, b[0]);
    }

    // TODO: for 1-byte types, build a 256-entry lookup table from b when
    // nb is large; a SIMD any-of-set compare is the next step after that.
    for (size_t i = 0; i < na; ++i)
    {
        for (size_t j = 0; j < nb; ++j)
        {
            if (a[i] == b[j])
            {
                return a + i;
            }
        }
    }

    return a + na;
}

template <typename T, typename U, typename Pred>
constexpr T* find_first_of(T* a, size_t na, const U* b, size_t nb, Pred pred)
{
    for (size_t i = 0; i < na; ++i)
    {
        for (size_t j = 0; j < nb; ++j)
        {
            if (pred(a[i], b[j]))
            {
                return a + i;
            }
        }
    }

    return a + na;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 find_first_of(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    for (; first1 != last1; ++first1)
    {
        for (IT2 it = first2; it != last2; ++it)
        {
            if (*first1 == *it)
            {
                return first1;
            }
        }
    }

    return last1;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 find_first_of(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    for (; first1 != last1; ++first1)
    {
        for (IT2 it = first2; it != last2; ++it)
        {
            if (pred(*first1, *it))
            {
                return first1;
            }
        }
    }

    return last1;
}

} // namespace range
} // namespace vx
