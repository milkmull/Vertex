#pragma once

#include "vertex/std/_memory/memory_core.hpp"
#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/util.hpp"
#include "vertex/std/iterator.hpp"

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
    const T& result = min(a, b);
    return min(result, args...);
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
    const T& result = max(a, b);
    return max(result, args...);
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

//=========================================================================
// range
//=========================================================================

namespace range {

namespace _range_priv {

template <typename T>
bool is_all_bits_zero(const T& x) noexcept
{
    // checks if scalar type has all bits set to zero
    VX_STATIC_ASSERT(std::is_scalar<T>::value && !std::is_member_pointer<T>::value);

    VX_IF_CONSTEXPR ((std::is_same<T, nullptr_t>::value))
    {
        return true;
    }
    else
    {
        constexpr T zero{};
        return mem::compare(&x, &zero, sizeof(T)) == 0;
    }
}

} // namespace _range_priv

//=========================================================================
// construct/destroy range
//=========================================================================

template <typename T>
T* construct(T* ptr, size_t count)
{
    const T* last = ptr + count;
    while (ptr != last)
    {
        mem::construct_in_place(*ptr);
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
        mem::construct_in_place_maybe_trivial(*ptr);
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
            mem::construct_in_place(*ptr, value);
            ++ptr;
        }

        return ptr;
    }
}

template <typename IT, typename U, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
IT fill(IT first, IT last, const U& value)
{
    using T = typename type_traits::value_type<IT>::type;

    const size_t count = static_cast<size_t>(std::distance(first, last));
    const size_t bytes = count * sizeof(T);

    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<IT, U>::value))
    {
        // can optimize with memset
        mem::set(first, value, bytes);
        return last;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                mem::set(first, 0, bytes);
                return last;
            }
        }

        for (; first != last; ++first)
        {
            *first = value;
        }

        return last;
    }
}

//=========================================================================
// copy
//=========================================================================

template <typename T>
constexpr T* copy(T* dst, const T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED() && type_traits::memmove_is_safe<T*>::value)
    {
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, src, bytes);
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
            mem::construct_in_place(*dst, *src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            *dst = *first;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(*dst, *first);
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
            mem::construct_in_place(*dst, std::move(*src));
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 move(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        const size_t bytes = count * sizeof(T);
        mem::move(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            *dst = std::move(*first);
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(last - first);
        const size_t bytes = count * sizeof(T);
        move(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(*dst, std::move(*first));
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
        mem::copy(dst, src, bytes);
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
            mem::construct_in_place(*dst, std::move(*src));
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_or_move(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            *dst = std::move(*first);
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_or_move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        const size_t bytes = count * sizeof(T);
        mem::copy(dst, first, bytes);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(*dst, std::move(*first));
            ++dst;
        }

        return dst;
    }
}

//=========================================================================
// shift_forward
//=========================================================================

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

template <typename T>
T* swap(T* a, T* b, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        // For trivially copyable/movable types, swap using a small local block stack buffer_type
        // to minimize loop overhead and leverage vectorization if possible.
        constexpr size_t buffer_bytes = 64;
        constexpr size_t elements_per_block = buffer_bytes / sizeof(T) > 0 ? buffer_bytes / sizeof(T) : 1;

        T temp_buffer[elements_per_block];

        while (count > 0)
        {
            const size_t current_chunk = min(count, elements_per_block);
            const size_t current_bytes = current_chunk * sizeof(T);

            mem::move(temp_buffer, a, current_bytes);
            mem::move(a, b, current_bytes);
            mem::move(b, temp_buffer, current_bytes);

            a += current_chunk;
            b += current_chunk;
            count -= current_chunk;
        }
        return a;
    }
    else
    {
        for (; 0 < count; --count)
        {
            ::vx::swap(*a, *b);
            ++a;
            ++b;
        }
        return a;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 swap(IT1 first1, IT1 last1, IT2 first2)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first1, last1));
        ::vx::swap(std::addressof(*first1), std::addressof(*first2), count);
        return first1 + count;
    }
    else
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            ::vx::swap(*first1, *first2);
        }
        return first1;
    }
}

//=========================================================================
// compare
//=========================================================================

template <typename T>
int compare(const T* a, const T* b, size_t count)
{
    return mem::compare(a, b, count * sizeof(T));
}

template <typename T>
int compare(const T* a, size_t count_a, const T* b, size_t count_b)
{
    size_t min_count = min(count_a, count_b);

    VX_IF_CONSTEXPR ((type_traits::is_bitwise_comparable<T, T>::value))
    {
        const int res = compare(a, b, min_count);
        if (res != 0)
        {
            return res;
        }
    }
    else
    {
        for (; 0 < min_count; --min_count)
        {
            if (*a != *b)
            {
                return (*a < *b) ? -1 : 1;
            }

            ++a;
            ++b;
        }
    }

    if (count_a < count_b)
    {
        return -1;
    }

    if (count_a > count_b)
    {
        return 1;
    }

    return 0;
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    const size_t count1 = static_cast<size_t>(std::distance(first1, last1));
    const size_t count2 = static_cast<size_t>(std::distance(first2, last2));
    size_t min_count = min(count1, count2);

    for (; 0 < min_count; --min_count)
    {
        if (*first1 != *first2)
        {
            return (*first1 < *first2) ? -1 : 1;
        }

        ++first1;
        ++first2;
    }

    if (count1 < count2)
    {
        return -1;
    }

    if (count1 > count2)
    {
        return 1;
    }

    return 0;
}

//=========================================================================
// reverse
//=========================================================================

template <typename IT>
void reverse(IT first, IT last)
{
    if (first == last)
    {
        return;
    }

#if VX_STD_USE_SIMD_ALGORITHMS

    using value_type = typename type_traits::value_type<IT>::type;

    constexpr size_t N = sizeof(value_type);
    constexpr bool simd_size_ok = N < 8 && (N & (N - 1)) == 0;

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

    --last;

    while (first < last)
    {
        ::vx::swap(*first, *last);
        ++first;
        --last;
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
        reverse(first, middle);
        reverse(middle, last);
        reverse(first, last);
        return first + (last - middle);
    }
    else
    {
        IT first2 = middle;

        do
        {
            ::vx::swap(*first, *first2);

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
                ::vx::swap(*first, *first2);

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
