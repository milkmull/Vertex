#pragma once

#include <stddef.h>
#include <type_traits>

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/util.hpp"

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

namespace vx {

//=========================================================================
// min
//=========================================================================

template <typename T>
constexpr const T& min(const T& a, const T& b) noexcept(noexcept(b < a))
{
    return (b < a) ? b : a;
}

template <typename T, typename... Args>
constexpr const T& min(const T& a, const T& b, const Args&... args)
{
    return vx::min(vx::min(a, b), args...);
}

//=========================================================================
// max
//=========================================================================

template <typename T>
constexpr const T& max(const T& a, const T& b) noexcept(noexcept(a < b))
{
    return (a < b) ? b : a;
}

template <typename T, typename... Args>
constexpr const T& max(const T& a, const T& b, const Args&... args)
{
    return vx::max(vx::max(a, b), args...);
}

//=========================================================================
// clamp
//=========================================================================

template <typename T>
constexpr const T& clamp(
    const T& value,
    const T& low,
    const T& high) noexcept(noexcept(value < low) &&
    noexcept(high < value))
{
    if (value < low)
    {
        return low;
    }

    if (high < value)
    {
        return high;
    }

    return value;
}

template <typename T, typename Compare>
constexpr const T& clamp(
    const T& value,
    const T& low,
    const T& high,
    Compare comp)
{
    if (comp(value, low))
    {
        return low;
    }

    if (comp(high, value))
    {
        return high;
    }

    return value;
}

//=========================================================================
// range
//=========================================================================

namespace range {

namespace _range_priv {

// converts a pointer or a contiguous pointer-like iterator to a raw pointer.
// The iterator must be dereferenceable (callers guard against empty ranges).
template <typename T>
constexpr T* to_pointer(T* p) noexcept
{
    return p;
}

template <typename IT, VX_REQUIRES(!std::is_pointer<IT>::value)>
constexpr auto to_pointer(IT it) noexcept
{
    return &(*it);
}

// checks if a scalar has all bits set to zero
template <typename T>
bool is_all_bits_zero_impl(const T& x, std::true_type) noexcept
{
    VX_IF_CONSTEXPR ((std::is_same<T, decltype(nullptr)>::value))
    {
        return true;
    }
    else
    {
        // long double on some targets has indeterminate padding bytes
        VX_IF_CONSTEXPR ((std::is_floating_point<T>::value && (sizeof(T) > 8)))
        {
            return false;
        }
        else
        {
            constexpr T zero{};
            return mem::compare(&x, &zero, sizeof(T)) == 0;
        }
    }
}

// non-scalars (and member pointers, whose null may not be all-zero) are never "zero"
template <typename T>
bool is_all_bits_zero_impl(const T&, std::false_type) noexcept
{
    return false;
}

template <typename T>
bool is_all_bits_zero(const T& x) noexcept
{
    using tag = std::integral_constant<bool, std::is_scalar<T>::value && !std::is_member_pointer<T>::value>;
    return is_all_bits_zero_impl(x, tag{});
}

// swaps two non-overlapping byte ranges in fixed-size chunks.
// Does not require T to be default constructible.
inline void swap_bytes(void* a, void* b, size_t bytes) noexcept
{
    constexpr size_t chunk_size = 64;
    unsigned char temp[chunk_size];

    unsigned char* pa = static_cast<unsigned char*>(a);
    unsigned char* pb = static_cast<unsigned char*>(b);

    while (bytes > 0)
    {
        const size_t n = bytes < chunk_size ? bytes : chunk_size;

        mem::copy(temp, pa, n);
        mem::copy(pa, pb, n);
        mem::copy(pb, temp, n);

        pa += n;
        pb += n;
        bytes -= n;
    }
}

} // namespace _range_priv

//=========================================================================
// iter_swap
//=========================================================================

template <typename IT1, typename IT2>
constexpr void iter_swap(IT1 a, IT2 b)
{
    vx::swap(*a, *b);
}

//=========================================================================
// construct/destroy range
//=========================================================================

template <typename T>
T* construct(T* ptr, size_t count)
{
    const T* last = ptr + count;
    while (ptr != last)
    {
        mem::construct_in_place(ptr);
        ++ptr;
    }

    return ptr;
}

template <typename T>
T* construct_maybe_trivial(T* ptr, size_t count)
{
    const T* last = ptr + count;
    while (ptr != last)
    {
        mem::construct_in_place_maybe_trivial(ptr);
        ++ptr;
    }

    return ptr;
}

template <typename T>
T* destroy(T* ptr, size_t count) noexcept
{
    VX_IF_CONSTEXPR (std::is_trivially_destructible<T>::value)
    {
        return ptr + count;
    }
    else
    {
        const T* last = ptr + count;
        while (ptr != last)
        {
            mem::destroy_in_place(ptr);
            ++ptr;
        }

        return ptr;
    }
}

//=========================================================================
// fill
//=========================================================================

template <typename T, typename U>
constexpr T* fill(T* ptr, size_t count, const U& value)
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        // in a constant expression, just do the simple loop
        for (; 0 < count; --count)
        {
            *ptr = static_cast<T>(value);
            ++ptr;
        }
        return ptr;
    }

    const size_t bytes = count * sizeof(T);

    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        mem::set(ptr, static_cast<int>(value), bytes);
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                mem::set(ptr, 0, bytes);
                return ptr + count;
            }
        }

        for (; 0 < count; --count)
        {
            *ptr = static_cast<T>(value);
            ++ptr;
        }

        return ptr;
    }
}

template <typename T, typename U>
T* fill_uninitialized(T* ptr, size_t count, const U& value)
{
    const size_t bytes = count * sizeof(T);

    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        mem::set(ptr, static_cast<int>(value), bytes);
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                mem::set(ptr, 0, bytes);
                return ptr + count;
            }
        }

        for (; 0 < count; --count)
        {
            mem::construct_in_place(ptr, value);
            ++ptr;
        }

        return ptr;
    }
}

template <typename IT, typename U, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
constexpr IT fill(IT first, IT last, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename type_traits::value_type<IT>::type;

        VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<IT, U>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            mem::set(_range_priv::to_pointer(first), static_cast<int>(value), count * sizeof(T));
            return last;
        }
        else VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                const size_t count = static_cast<size_t>(vx::distance(first, last));
                mem::set(_range_priv::to_pointer(first), 0, count * sizeof(T));
                return last;
            }
        }
    }

    for (; first != last; ++first)
    {
        *first = value;
    }

    return last;
}

//=========================================================================
// copy
//=========================================================================

// Semantics match std::copy: dst may overlap src if dst precedes src,
// so the trivial path uses memmove, never memcpy.

template <typename T>
constexpr T* copy(T* dst, const T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED() && type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::move(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = *src;
            ++src;
            ++dst;
        }

        return dst;
    }
}

// dst is uninitialized memory and must not overlap src
template <typename T>
T* copy_uninitialized(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            mem::construct_in_place(dst, *src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
constexpr IT1 copy(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename type_traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            mem::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
            return dst + count;
        }
    }

    for (; first != last; ++first)
    {
        *dst = *first;
        ++dst;
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        mem::copy(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), *first);
            ++dst;
        }

        return dst;
    }
}

//=========================================================================
// move
//=========================================================================

template <typename T>
constexpr T* move(T* dst, T* src, size_t count) noexcept
{
    if (!VX_IS_CONSTANT_EVALUATED() && type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::move(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = std::move(*src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename T>
T* move_uninitialized(T* dst, T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::move(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            mem::construct_in_place(dst, std::move(*src));
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
constexpr IT1 move(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename type_traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            mem::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
            return dst + count;
        }
    }

    for (; first != last; ++first)
    {
        *dst = std::move(*first);
        ++dst;
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        mem::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), std::move(*first));
            ++dst;
        }

        return dst;
    }
}

//=========================================================================
// copy_or_move
//=========================================================================

template <typename T>
constexpr T* copy_or_move(T* dst, T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED() && type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::move(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = std::move(*src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

// dst is uninitialized memory and must not overlap src
template <typename T>
T* copy_or_move_uninitialized(T* dst, T* src, size_t count)
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, src, bytes);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            mem::construct_in_place(dst, std::move(*src));
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
constexpr IT1 copy_or_move(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename type_traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            mem::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
            return dst + count;
        }
    }

    for (; first != last; ++first)
    {
        *dst = std::move(*first);
        ++dst;
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_or_move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        mem::copy(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), std::move(*first));
            ++dst;
        }

        return dst;
    }
}

//=========================================================================
// shift_forward / shift_backward
//=========================================================================

// Moves [first, last) forward by `count` slots; returns first + count.
// Every destination slot must already hold a live object (it is move-ASSIGNED).
// If the tail [last, last + count) is raw memory, use shift_forward_uninitialized.
// Source slots are left in a moved-from state.
template <typename T>
T* shift_forward(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = num_elements * sizeof(T);
        mem::move(first + count, first, bytes);
        return first + count;
    }
    else
    {
        T* src = last;
        T* dst = last + count;
        for (size_t i = 0; i < num_elements; ++i)
        {
            --src;
            --dst;
            *dst = std::move(*src);
        }
        return first + count;
    }
}

// Like shift_forward, but the slots at or beyond `last` are raw memory:
// they are move-CONSTRUCTED, while slots inside [first, last) are move-assigned.
// This is the correct primitive for vector insert.
template <typename T>
T* shift_forward_uninitialized(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = num_elements * sizeof(T);
        mem::move(first + count, first, bytes);
        return first + count;
    }
    else
    {
        T* src = last;
        T* dst = last + count;
        for (size_t i = 0; i < num_elements; ++i)
        {
            --src;
            --dst;

            if (dst >= last)
            {
                mem::construct_in_place(dst, std::move(*src));
            }
            else
            {
                *dst = std::move(*src);
            }
        }
        return first + count;
    }
}

// Moves [first, last) backward by `count` slots; returns last - count.
// Destination slots must hold live objects (move-assigned), as in vector erase.
template <typename T>
T* shift_backward(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = num_elements * sizeof(T);
        mem::move(first - count, first, bytes);
        return last - count;
    }
    else
    {
        T* src = first;
        T* dst = first - count;
        for (size_t i = 0; i < num_elements; ++i)
        {
            *dst = std::move(*src);
            ++src;
            ++dst;
        }
        return last - count;
    }
}

//=========================================================================
// swap
//=========================================================================

// swaps `count` elements between two non-overlapping ranges; returns a + count
template <typename T>
T* swap(T* a, T* b, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        // trivially relocatable: swap raw bytes through a small stack buffer.
        // Works for any T (no default-construction requirement).
        _range_priv::swap_bytes(a, b, count * sizeof(T));
        return a + count;
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            ::vx::swap(a[i], b[i]);
        }
        return a + count;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 swap(IT1 first1, IT1 last1, IT2 first2)
{
    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first1, last1));
        range::swap(_range_priv::to_pointer(first1), _range_priv::to_pointer(first2), count);
        return last1;
    }
    else
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            range::iter_swap(first1, first2);
        }
        return first1;
    }
}

//=========================================================================
// compare (three-way, returns -1 / 0 / 1)
//=========================================================================

// Only operator< is required of the element type.

template <typename T>
int compare(const T* a, size_t count_a, const T* b, size_t count_b)
{
    size_t min_count = vx::min(count_a, count_b);

    VX_IF_CONSTEXPR ((type_traits::memcmp_lexicographical_is_safe<const T*, const T*>::value))
    {
        const int res = mem::compare(a, b, min_count * sizeof(T));
        if (res != 0)
        {
            return (res < 0) ? -1 : 1;
        }
    }
    else
    {
        for (; 0 < min_count; --min_count)
        {
            if (*a < *b)
            {
                return -1;
            }

            if (*b < *a)
            {
                return 1;
            }

            ++a;
            ++b;
        }
    }

    if (count_a < count_b)
    {
        return -1;
    }

    if (count_b < count_a)
    {
        return 1;
    }

    return 0;
}

// equal-length convenience; routes through the safe overload above
template <typename T>
int compare(const T* a, const T* b, size_t count)
{
    return range::compare(a, count, b, count);
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    if (first1 == last1)
    {
        return (first2 == last2) ? 0 : -1;
    }

    VX_IF_CONSTEXPR ((type_traits::memcmp_lexicographical_is_safe<IT1, IT2>::value))
    {
        // contiguous and byte-comparable: use the pointer overload
        const size_t count1 = static_cast<size_t>(vx::distance(first1, last1));
        const size_t count2 = static_cast<size_t>(vx::distance(first2, last2));
        return range::compare(_range_priv::to_pointer(first1), count1, _range_priv::to_pointer(first2), count2);
    }
    else
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (*first1 < *first2)
            {
                return -1;
            }

            if (*first2 < *first1)
            {
                return 1;
            }
        }

        if (first1 != last1)
        {
            return 1;
        }

        if (first2 != last2)
        {
            return -1;
        }

        return 0;
    }
}

//=========================================================================
// lexicographical_compare (bool, matches std semantics)
//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
bool lexicographical_compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::compare(first1, last1, first2, last2) < 0;
}

template <typename IT1, typename IT2, typename Compare, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
bool lexicographical_compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
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

//=========================================================================
// reverse
//=========================================================================

// Works with bidirectional iterators. Uses a SIMD kernel for contiguous
// trivially-relocatable ranges at runtime.
template <typename IT>
constexpr void reverse(IT first, IT last)
{
    if (first == last)
    {
        return;
    }

#if VX_STD_USE_SIMD_ALGORITHMS

    using value_type = typename type_traits::value_type<IT>::type;

    constexpr size_t N = sizeof(value_type);

    // TODO: widen to N <= 8 once reverse_simd<8> is confirmed to exist.
    constexpr bool simd_size_ok =
        N < 8 && (N & (N - 1)) == 0 && type_traits::memmove_is_safe<IT>::value;

    VX_IF_CONSTEXPR (simd_size_ok)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            VX_IF_CONSTEXPR (std::is_pointer<IT>::value)
            {
                _simd::reverse_simd<N>(first, last);
                return;
            }
            else VX_IF_CONSTEXPR (_priv::is_forward_pointer_iterator<IT>::value)
            {
                _simd::reverse_simd<N>(first.ptr(), last.ptr());
                return;
            }
        }
    }

#endif // VX_STD_USE_SIMD_ALGORITHMS

    // bidirectional: only !=, ++ and -- are required
    while (first != last)
    {
        --last;

        if (first == last)
        {
            break;
        }

        range::iter_swap(first, last);
        ++first;
    }
}

//=========================================================================
// rotate
//=========================================================================

template <typename IT>
constexpr IT rotate(IT first, IT middle, IT last)
{
    if (first == middle)
    {
        return last;
    }

    if (middle == last)
    {
        return first;
    }

    VX_IF_CONSTEXPR (type_traits::is_random_access_iterator<IT>::value)
    {
        // fast path: random access iterators support O(1) +/- and distance
        range::reverse(first, middle);
        range::reverse(middle, last);
        range::reverse(first, last);
        return first + (last - middle);
    }
    else
    {
        IT first2 = middle;

        do
        {
            range::iter_swap(first, first2);

            ++first;
            ++first2;

            if (first == middle)
            {
                middle = first2;
            }

        } while (first2 != last);

        IT result = first;

        while (middle != last)
        {
            first2 = middle;

            do
            {
                range::iter_swap(first, first2);

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

} // namespace range
} // namespace vx
