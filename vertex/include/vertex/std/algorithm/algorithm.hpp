#pragma once

#include <algorithm>

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/functional.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

namespace vx {

//=========================================================================
// helpers
//=========================================================================

namespace range {
namespace _range_priv {

template <typename T>
constexpr T* to_pointer(T* p) noexcept
{
    return p;
}

template <typename IT, VX_REQUIRES(!traits::is_pointer<IT>::value)>
constexpr auto to_pointer(IT it) noexcept
{
    return &(*it);
}

// checks if a scalar has all bits set to zero
template <typename T>
bool is_all_bits_zero_impl(const T& x, traits::true_type) noexcept
{
    VX_IF_CONSTEXPR ((traits::is_same<T, decltype(nullptr)>::value))
    {
        return true;
    }
    else
    {
        // long double on some targets has indeterminate padding bytes
        VX_IF_CONSTEXPR ((traits::is_floating_point<T>::value && (sizeof(T) > 8)))
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
bool is_all_bits_zero_impl(const T&, traits::false_type) noexcept
{
    return false;
}

template <typename T>
bool is_all_bits_zero(const T& x) noexcept
{
    using tag = traits::integral_constant<bool, traits::is_scalar<T>::value && !traits::is_member_pointer<T>::value>;
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

// memchr is usable when the elements are single bytes compared by value
// and the searched value is an integer.
template <typename T, typename U>
struct can_find_memchr
{
    static constexpr bool value =
        sizeof(T) == 1 &&
        traits::is_integral<traits::remove_cv_t<T>>::value &&
        traits::is_integral<U>::value;
};

// True when range::reverse can hand the range to a SIMD kernel:
// contiguous, trivially copyable, power-of-two element size, and an iterator
// that can give up a raw pointer (a raw pointer, or one with .ptr()).
template <typename IT>
struct simd_reverse_is_safe
{
#if VX_STD_USE_SIMD_ALGORITHMS

private:

    static constexpr size_t N = sizeof(typename value_type<IT>::type);

public:

    // TODO: widen to N <= 8 once reverse_simd<8> is confirmed to exist.
    static constexpr bool value =
        N < 8 && (N & (N - 1)) == 0 &&
        memmove_is_safe<IT>::value &&
        (is_pointer<IT>::value || _priv::is_forward_pointer_iterator<IT>::value);
#else
    static constexpr bool value = false;
#endif
};

} // namespace _range_priv
} // namespace range

//=========================================================================
// min
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

namespace range {

// Returns the first smallest element, or last if the range is empty.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT min_element(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    IT result = last;

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

//=========================================================================

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

//=========================================================================

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

} // namespace range

//=========================================================================
// max
//=========================================================================

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

namespace range {

// Returns the first largest element, or last if the range is empty.
template <typename IT, typename Compare, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT max_element(IT first, IT last, Compare comp)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);
    IT result = last;

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

//=========================================================================

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

//=========================================================================

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

} // namespace range

//=========================================================================
// minmax
//=========================================================================

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

namespace range {

template <typename IT>
struct minmax_element_result
{
    IT min;
    IT max;
};

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

//=========================================================================

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

//=========================================================================

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

} // namespace range

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

namespace range {

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

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT construct(IT first, IT last)
{
    for (; first != last; ++first)
    {
        mem::construct_in_place(_range_priv::to_pointer(first));
    }

    return first;
}

//=========================================================================

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

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT construct_maybe_trivial(IT first, IT last)
{
    for (; first != last; ++first)
    {
        mem::construct_in_place_maybe_trivial(_range_priv::to_pointer(first));
    }

    return first;
}

//=========================================================================

template <typename T>
T* destroy(T* ptr, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::is_trivially_destructible<T>::value)
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

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT destroy(IT first, IT last) noexcept
{
    using T = typename traits::value_type<IT>::type;

    VX_IF_CONSTEXPR (std::is_trivially_destructible<T>::value)
    {
        return last;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::destroy_in_place(_range_priv::to_pointer(first));
        }

        return last;
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

    VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        mem::set(ptr, static_cast<int>(value), bytes);
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<T*, U>::value))
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

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT fill(IT first, IT last, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT>::type;

        VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            mem::set(_range_priv::to_pointer(first), static_cast<int>(value), count * sizeof(T));
            return last;
        }
        else VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<IT, U>::value))
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

template <typename T, typename U>
T* fill_uninitialized(T* ptr, size_t count, const U& value)
{
    const size_t bytes = count * sizeof(T);

    VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        mem::set(ptr, static_cast<int>(value), bytes);
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<T*, U>::value))
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

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT fill_uninitialized(IT first, IT last, const U& value)
{
    using T = typename traits::value_type<IT>::type;

    VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        mem::set(_range_priv::to_pointer(first), static_cast<int>(value), count * sizeof(T));
        return last;
    }
    else
    {
        VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                const size_t count = static_cast<size_t>(vx::distance(first, last));
                mem::set(_range_priv::to_pointer(first), 0, count * sizeof(T));
                return last;
            }
        }

        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(first), value);
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
    if (!VX_IS_CONSTANT_EVALUATED() && traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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

//=========================================================================

// dst is uninitialized memory and must not overlap src
template <typename T>
T* copy_uninitialized(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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
    if (!VX_IS_CONSTANT_EVALUATED() && traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 move(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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

//=========================================================================

template <typename T>
T* move_uninitialized(T* dst, T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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
    if (!VX_IS_CONSTANT_EVALUATED() && traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_or_move(IT1 dst, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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

//=========================================================================

// dst is uninitialized memory and must not overlap src
template <typename T>
T* copy_or_move_uninitialized(T* dst, T* src, size_t count)
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 copy_or_move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    using T = typename traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
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
// copy_backward
//=========================================================================

template <typename T>
constexpr T* copy_backward(T* dst_last, const T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED() && traits::memmove_is_safe<T*>::value)
    {
        T* dst_first = dst_last - count;
        mem::move(dst_first, src, count * sizeof(T));
        return dst_first;
    }
    else
    {
        const T* src_last = src + count;

        for (; 0 < count; --count)
        {
            --src_last;
            --dst_last;
            *dst_last = *src_last;
        }

        return dst_last;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_backward(IT1 dst_last, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            IT1 dst_first = dst_last - count;
            range::copy_backward(_range_priv::to_pointer(dst_last), _range_priv::to_pointer(first), count);
            return dst_first;
        }
    }

    while (first != last)
    {
        --last;
        --dst_last;
        *dst_last = *last;
    }

    return dst_last;
}

//=========================================================================
// move_backward
//=========================================================================

template <typename T>
constexpr T* move_backward(T* dst_last, T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED() && traits::memmove_is_safe<T*>::value)
    {
        T* dst_first = dst_last - count;
        mem::move(dst_first, src, count * sizeof(T));
        return dst_first;
    }
    else
    {
        T* src_last = src + count;

        for (; 0 < count; --count)
        {
            --src_last;
            --dst_last;
            *dst_last = _range_priv::as_rvalue(*src_last);
        }

        return dst_last;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 move_backward(IT1 dst_last, IT2 first, IT2 last)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        using T = typename traits::value_type<IT1>::type;

        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            IT1 dst_first = dst_last - count;
            mem::move(_range_priv::to_pointer(dst_first), _range_priv::to_pointer(first), count * sizeof(T));
            return dst_first;
        }
    }

    while (first != last)
    {
        --last;
        --dst_last;
        *dst_last = _range_priv::as_rvalue(*last);
    }

    return dst_last;
}

//=========================================================================
// copy_or_move_backward
//=========================================================================

template <typename T>
constexpr T* copy_or_move_backward(T* dst_last, T* src, size_t count)
{
    return range::move_backward(dst_last, src, count);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_or_move_backward(IT1 dst_last, IT2 first, IT2 last)
{
    return range::move_backward(dst_last, first, last);
}

//=========================================================================
// relocate
//=========================================================================

template <typename T>
T* relocate(T* dst, T* src, size_t count) noexcept
{
    if (count == 0)
    {
        return dst;
    }

    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
            mem::destroy_in_place(src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 relocate(IT1 dst, IT2 first, IT2 last) noexcept
{
    using T = typename traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        mem::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), _range_priv::as_rvalue(*first));
            mem::destroy_in_place(_range_priv::to_pointer(first));
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

    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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
T* shift_forward_uninitialized(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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

//=========================================================================
// shift_backward
//=========================================================================

template <typename T>
T* shift_backward(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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

template <typename T>
T* shift_backward_uninitialized(T* first, T* last, const size_t count) noexcept
{
    const size_t num_elements = static_cast<size_t>(last - first);

    if (num_elements == 0 || count == 0)
    {
        return last - count;
    }

    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
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
            if (dst < first)
            {
                mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
            }
            else
            {
                *dst = _range_priv::as_rvalue(*src);
            }

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
constexpr T* swap(T* a, T* b, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        // no partial overlap
        VX_ASSERT(a == b || a + count <= b || b + count <= a);

        // trivially relocatable: swap raw bytes through a small stack buffer.
        // Works for any T (no default-construction requirement).
        _range_priv::swap_bytes(a, b, count * sizeof(T));
        return a + count;
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            vx::swap(a[i], b[i]);
        }
        return a + count;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 swap(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first1, last1));
        range::swap(_range_priv::to_pointer(first1), _range_priv::to_pointer(first2), count);
        return last1;
    }
    else
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            vx::iter_swap(first1, first2);
        }
        return first1;
    }
}

//=========================================================================
// fill_n
//=========================================================================

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT fill_n(IT dst, size_t count, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value || traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            // contiguous: the pointer form picks memset (or the zero check)
            range::fill(_range_priv::to_pointer(dst), count, value);
            return dst + count;
        }
    }

    for (; 0 < count; --count)
    {
        *dst = value;
        ++dst;
    }

    return dst;
}

//=========================================================================
// copy_n
//=========================================================================

// Assigns `count` elements from first to dst. Returns dst advanced by count.
// Same overlap rule as copy: dst may overlap the source only if it precedes it.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_n(IT1 dst, IT2 first, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            range::copy(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
            return dst + count;
        }
    }

    for (; 0 < count; --count)
    {
        *dst = *first;
        ++dst;
        ++first;
    }

    return dst;
}

//=========================================================================
// move_n
//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 move_n(IT1 dst, IT2 first, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            range::move(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
            return dst + count;
        }
    }

    for (; 0 < count; --count)
    {
        *dst = _range_priv::as_rvalue(*first);
        ++dst;
        ++first;
    }

    return dst;
}

//=========================================================================
// destroy_n
//=========================================================================

// Destroys `count` live objects starting at first. Returns first advanced by
// count.
template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT destroy_n(IT first, size_t count) noexcept
{
    using T = typename traits::value_type<IT>::type;

    VX_IF_CONSTEXPR (traits::is_trivially_destructible<T>::value && traits::is_random_access_iterator<IT>::value)
    {
        return first + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            // trivially destructible: nothing to run, just advance
            VX_IF_CONSTEXPR (!traits::is_trivially_destructible<T>::value)
            {
                mem::destroy_in_place(_range_priv::to_pointer(first));
            }
            ++first;
        }

        return first;
    }
}

//=========================================================================
// copy_uninitialized_n
//=========================================================================

// dst is raw memory and must not overlap the source. Returns dst advanced by
// count. No rollback if a constructor throws (see note).
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized_n(IT1 dst, IT2 first, size_t count)
{
    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        range::copy_uninitialized(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), *first);
            ++dst;
            ++first;
        }

        return dst;
    }
}

//=========================================================================
// compare
//=========================================================================

template <typename T, typename Compare>
constexpr int compare(const T* a, size_t count_a, const T* b, size_t count_b, Compare comp)
{
    size_t min_count = vx::min(count_a, count_b);

    VX_IF_CONSTEXPR ((traits::memcmp_lexicographical_is_safe<const T*, const T*>::value))
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
            if (comp(*a, *b))
            {
                return -1;
            }

            if (comp(*b, *a))
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

template <typename T>
constexpr int compare(const T* a, size_t count_a, const T* b, size_t count_b)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_lexicographical_is_safe<const T*, const T*>::value))
        {
            const size_t min_count = vx::min(count_a, count_b);

            if (min_count != 0)
            {
                const int res = mem::compare(a, b, min_count * sizeof(T));
                if (res != 0)
                {
                    return (res < 0) ? -1 : 1;
                }
            }

            if (count_a < count_b)
            {
                return -1;
            }

            return (count_b < count_a) ? 1 : 0;
        }
    }

    return range::compare(a, count_a, b, count_b, vx::less<>{});
}

template <typename T>
constexpr int compare(const T* a, const T* b, size_t count)
{
    return range::compare(a, count, b, count);
}

template <typename T, typename Compare>
constexpr int compare(const T* a, const T* b, size_t count, Compare comp)
{
    return range::compare(a, count, b, count, comp);
}

template <typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2, Compare comp)
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
    {
        if (comp(*first1, *first2))
        {
            return -1;
        }

        if (comp(*first2, *first1))
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr int compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_lexicographical_is_safe<IT1, IT2>::value))
        {
            // contiguous and byte-comparable: use the pointer overload
            if (first1 == last1)
            {
                return (first2 == last2) ? 0 : -1;
            }

            if (first2 == last2)
            {
                return 1;
            }

            const size_t count1 = static_cast<size_t>(vx::distance(first1, last1));
            const size_t count2 = static_cast<size_t>(vx::distance(first2, last2));
            return compare(_range_priv::to_pointer(first1), count1, _range_priv::to_pointer(first2), count2);
        }
    }

    return range::compare(first1, last1, first2, last2, vx::less<>{});
}

//=========================================================================
// lexicographical_compare
//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
bool lexicographical_compare(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    return range::compare(first1, last1, first2, last2) < 0;
}

template <typename IT1, typename IT2, typename Compare, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
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
// find
//=========================================================================

// Returns a pointer to the first element equal to `value`, or ptr + count
// if there is none.
template <typename T, typename U>
constexpr T* find(T* ptr, size_t count, const U& value)
{
    if (count == 0)
    {
        return ptr;
    }

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

            void* hit = mem::find(ptr, static_cast<unsigned char>(value), count);
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
            if (first == last)
            {
                return last;
            }

            const size_t count = static_cast<size_t>(vx::distance(first, last));
            auto* ptr = _range_priv::to_pointer(first);
            return first + (range::find(ptr, count, value) - ptr);
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

//=========================================================================
// find_if
//=========================================================================

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

//=========================================================================
// find_if_not
//=========================================================================

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
// count
//=========================================================================

// Number of elements equal to `value`.
template <typename T, typename U>
constexpr size_t count(const T* ptr, size_t num, const U& value)
{
    size_t result = 0;
    const T* const last = ptr + num;

    for (; ptr != last; ++ptr)
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
            const size_t num = static_cast<size_t>(vx::distance(first, last));
            return range::count(_range_priv::to_pointer(first), num, value);
        }
    }

    size_t result = 0;

    for (; first != last; ++first)
    {
        result += static_cast<size_t>(*first == value);
    }

    return result;
}

//=========================================================================
// count_if
//=========================================================================

// Number of elements for which pred returns true.
template <typename T, typename Pred>
constexpr size_t count_if(const T* ptr, size_t num, Pred pred)
{
    size_t result = 0;
    const T* const last = ptr + num;

    for (; ptr != last; ++ptr)
    {
        if (pred(*ptr))
        {
            ++result;
        }
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
        if (pred(*first))
        {
            ++result;
        }
    }

    return result;
}

//=========================================================================
// any_of
//=========================================================================

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

//=========================================================================
// all_of
//=========================================================================

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

//=========================================================================
// none_of
//=========================================================================

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

template <typename IT1, typename IT2>
struct mismatch_result
{
    IT1 first;
    IT2 second;
};

//=========================================================================
// equal
//=========================================================================

// Same element count in both ranges.
template <typename T, typename U>
constexpr bool equal(const T* a, const U* b, size_t count)
{
    if (count == 0)
    {
        return true;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<const T*, const U*>::value))
        {
            return a == b || mem::compare(a, b, count * sizeof(T)) == 0;
        }
    }

    for (; 0 < count; --count, ++a, ++b)
    {
        if (!(*a == *b))
        {
            return false;
        }
    }

    return true;
}

template <typename T, typename U, typename Pred>
constexpr bool equal(const T* a, const U* b, size_t count, Pred pred)
{
    for (; 0 < count; --count, ++a, ++b)
    {
        if (!pred(*a, *b))
        {
            return false;
        }
    }

    return true;
}

// Different lengths: ranges of different size are never equal.
template <typename T, typename U>
constexpr bool equal(const T* a, size_t count_a, const U* b, size_t count_b)
{
    return count_a == count_b && range::equal(a, b, count_a);
}

//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            if (first1 == last1)
            {
                return true;
            }

            const size_t count = static_cast<size_t>(vx::distance(first1, last1));
            return range::equal(_range_priv::to_pointer(first1), _range_priv::to_pointer(first2), count);
        }
    }

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            return false;
        }
    }

    return true;
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && !traits::is_iterator<Pred>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!pred(*first1, *first2))
        {
            return false;
        }
    }

    return true;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr bool equal(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    VX_IF_CONSTEXPR ((traits::is_random_access_iterator<IT1>::value && traits::is_random_access_iterator<IT2>::value))
    {
        // O(1) length check, then the 3-iterator form gets the memcmp path
        if (vx::distance(first1, last1) != vx::distance(first2, last2))
        {
            return false;
        }

        return range::equal(first1, last1, first2);
    }
    else
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
            {
                return false;
            }
        }

        return first1 == last1 && first2 == last2;
    }
}

//=========================================================================
// mismatch
//=========================================================================

// Returns the first position where the ranges differ. If they are equal,
// returns {a + count, b + count}.
template <typename T, typename U>
constexpr mismatch_result<const T*, const U*> mismatch(const T* a, const U* b, size_t count)
{
    size_t i = 0;

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<const T*, const U*>::value))
        {
            // memcmp can't report a position, so skip whole equal blocks with
            // memcmp and let the scalar loop below pinpoint the element inside
            // the first block that differs.
            constexpr size_t block = (sizeof(T) >= 256) ? 1 : (256 / sizeof(T));

            while (count - i >= block &&
                mem::compare(a + i, b + i, block * sizeof(T)) == 0)
            {
                i += block;
            }
        }
    }

    for (; i < count; ++i)
    {
        if (!(a[i] == b[i]))
        {
            break;
        }
    }

    return { a + i, b + i };
}

template <typename T, typename U, typename Pred>
constexpr mismatch_result<const T*, const U*> mismatch(const T* a, const U* b, size_t count, Pred pred)
{
    size_t i = 0;

    for (; i < count; ++i)
    {
        if (!pred(a[i], b[i]))
        {
            break;
        }
    }

    return { a + i, b + i };
}

//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            if (first1 == last1)
            {
                return { last1, first2 };
            }

            const size_t count = static_cast<size_t>(vx::distance(first1, last1));
            const auto* p = _range_priv::to_pointer(first1);
            const auto* q = _range_priv::to_pointer(first2);

            const auto r = range::mismatch(p, q, count);
            return { first1 + (r.first - p), first2 + (r.second - q) };
        }
    }

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!(*first1 == *first2))
        {
            break;
        }
    }

    return { first1, first2 };
}

template <typename IT1, typename IT2, typename Pred, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && !traits::is_iterator<Pred>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2, Pred pred)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2)
    {
        if (!pred(*first1, *first2))
        {
            break;
        }
    }

    return { first1, first2 };
}

// Stops at the end of the shorter range.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr mismatch_result<IT1, IT2> mismatch(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    VX_IF_CONSTEXPR ((traits::is_random_access_iterator<IT1>::value && traits::is_random_access_iterator<IT2>::value))
    {
        // clamp to the shorter length, then use the memcmp-capable 3-iterator form
        const auto n1 = vx::distance(first1, last1);
        const auto n2 = vx::distance(first2, last2);
        return range::mismatch(first1, n1 < n2 ? last1 : first1 + n2, first2);
    }
    else
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        {
            if (!(*first1 == *first2))
            {
                break;
            }
        }

        return { first1, first2 };
    }
}

//=========================================================================
// for_each / for_each_n
//=========================================================================

// Calls fn on every element. Returns fn so stateful functors can be read back.
template <typename T, typename Fn>
constexpr Fn for_each(T* ptr, size_t num, Fn fn)
{
    T* const last = ptr + num;

    for (; ptr != last; ++ptr)
    {
        fn(*ptr);
    }

    return fn;
}

template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr Fn for_each(IT first, IT last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        fn(*first);
    }

    return fn;
}

// Iterator + count. Returns first advanced by count (as std::for_each_n does).
template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT for_each_n(IT first, size_t num, Fn fn)
{
    for (; 0 < num; --num, ++first)
    {
        fn(*first);
    }

    return first;
}

//=========================================================================
// search (first occurrence of a subsequence)
//=========================================================================

// Returns a pointer to the first position in [a, a + na) where [b, b + nb)
// occurs, or a + na if there is none. An empty needle matches at a.
template <typename T, typename U>
constexpr T* search(T* a, size_t na, const U* b, size_t nb)
{
    if (nb == 0)
    {
        return a;
    }

    if (nb > na)
    {
        return a + na;
    }

    if (nb == 1)
    {
        // single element: reuse find (gets the memchr path)
        return range::find(a, na, b[0]);
    }

    // TODO: mem::find_bytes (memmem) when memcmp_equal_is_safe, or a SIMD
    // first-element + last-element filter, slots in here.

    const size_t positions = na - nb + 1;

    for (size_t i = 0; i < positions; ++i)
    {
        if (range::equal(a + i, b, nb))
        {
            return a + i;
        }
    }

    return a + na;
}

template <typename T, typename U, typename Pred>
constexpr T* search(T* a, size_t na, const U* b, size_t nb, Pred pred)
{
    if (nb == 0)
    {
        return a;
    }

    if (nb > na)
    {
        return a + na;
    }

    const size_t positions = na - nb + 1;

    for (size_t i = 0; i < positions; ++i)
    {
        if (range::equal(a + i, b, nb, pred))
        {
            return a + i;
        }
    }

    return a + na;
}

//=========================================================================

// Forward iterators are enough.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 search(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (first2 == last2)
    {
        return first1;
    }

    if (first1 == last1)
    {
        return last1;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            const size_t n1 = static_cast<size_t>(vx::distance(first1, last1));
            const size_t n2 = static_cast<size_t>(vx::distance(first2, last2));
            auto* p = _range_priv::to_pointer(first1);

            return first1 + (range::search(p, n1, _range_priv::to_pointer(first2), n2) - p);
        }
    }

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

    if (first2 == last2)
    {
        return first1;
    }

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
// find_end (last occurrence of a subsequence)
//=========================================================================

// Returns a + na if there is no match or the needle is empty (as std does).
template <typename T, typename U>
constexpr T* find_end(T* a, size_t na, const U* b, size_t nb)
{
    if (nb == 0 || nb > na)
    {
        return a + na;
    }

    // scan backward so the first hit is the last occurrence
    for (size_t i = na - nb + 1; i-- > 0;)
    {
        if (range::equal(a + i, b, nb))
        {
            return a + i;
        }
    }

    return a + na;
}

template <typename T, typename U, typename Pred>
constexpr T* find_end(T* a, size_t na, const U* b, size_t nb, Pred pred)
{
    if (nb == 0 || nb > na)
    {
        return a + na;
    }

    for (size_t i = na - nb + 1; i-- > 0;)
    {
        if (range::equal(a + i, b, nb, pred))
        {
            return a + i;
        }
    }

    return a + na;
}

//=========================================================================

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 find_end(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first2, last2);

    if (first2 == last2 || first1 == last1)
    {
        return last1;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memcmp_equal_is_safe<IT1, IT2>::value))
        {
            const size_t n1 = static_cast<size_t>(vx::distance(first1, last1));
            const size_t n2 = static_cast<size_t>(vx::distance(first2, last2));
            auto* p = _range_priv::to_pointer(first1);

            return first1 + (range::find_end(p, n1, _range_priv::to_pointer(first2), n2) - p);
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

    if (first2 == last2 || first1 == last1)
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
// find_first_of (first element of range 1 equal to ANY element of range 2)
//=========================================================================

template <typename T, typename U>
constexpr T* find_first_of(T* a, size_t na, const U* b, size_t nb)
{
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

//=========================================================================
// adjacent_find
//=========================================================================

// Returns the first element equal to its successor, or ptr + num if none.
template <typename T>
constexpr T* adjacent_find(T* ptr, size_t num)
{
    if (num < 2)
    {
        return ptr + num;
    }

    T* const last = ptr + num - 1;

    for (; ptr != last; ++ptr)
    {
        if (*ptr == ptr[1])
        {
            return ptr;
        }
    }

    return ptr + 1;
}

template <typename T, typename Pred>
constexpr T* adjacent_find(T* ptr, size_t num, Pred pred)
{
    if (num < 2)
    {
        return ptr + num;
    }

    T* const last = ptr + num - 1;

    for (; ptr != last; ++ptr)
    {
        if (pred(*ptr, ptr[1]))
        {
            return ptr;
        }
    }

    return ptr + 1;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT adjacent_find(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last)
    {
        return last;
    }

    IT next = first;

    for (++next; next != last; ++first, ++next)
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

    if (first == last)
    {
        return last;
    }

    IT next = first;

    for (++next; next != last; ++first, ++next)
    {
        if (pred(*first, *next))
        {
            return first;
        }
    }

    return last;
}

//=========================================================================
// reverse
//=========================================================================

namespace _range_priv {

// Raw pointer from a contiguous iterator WITHOUT dereferencing, so it is
// safe on end iterators (unlike to_pointer).
template <typename T>
constexpr T* raw_ptr(T* p) noexcept
{
    return p;
}

template <typename IT, VX_REQUIRES(!traits::is_pointer<IT>::value)>
constexpr auto raw_ptr(IT it) noexcept
{
    return it.ptr();
}

} // namespace _range_priv

//=========================================================================
// reverse
//=========================================================================

template <typename T>
constexpr void reverse(T* ptr, size_t num)
{
    if (num < 2)
    {
        return;
    }

#if VX_STD_USE_SIMD_ALGORITHMS
    VX_IF_CONSTEXPR (traits::simd_reverse_is_safe<T*>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            _simd::reverse_simd<sizeof(T)>(ptr, ptr + num);
            return;
        }
    }
#endif

    for (T* last = ptr + num - 1; ptr < last; ++ptr, --last)
    {
        vx::swap(*ptr, *last);
    }
}

// Bidirectional iterators are enough. Contiguous ranges forward to the
// pointer form, so the SIMD dispatch lives in one place.
template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr void reverse(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last)
    {
        return;
    }

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::simd_reverse_is_safe<IT>::value)
        {
            const size_t num = static_cast<size_t>(vx::distance(first, last));
            range::reverse(_range_priv::raw_ptr(first), num);
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

//=========================================================================
// rotate
//=========================================================================

// Rotates so that ptr[mid] becomes the first element (mid <= num).
// Returns the new position of the old first element: ptr + (num - mid).
template <typename T>
constexpr T* rotate(T* ptr, size_t num, size_t mid)
{
    VX_ASSERT(mid <= num);

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
            // side over, then drop the buffer in. About n element moves
            // instead of the ~1.5n swaps of three reverses.
            constexpr size_t buffer_bytes = 512;
            unsigned char buffer[buffer_bytes];

            if (left <= right && left * sizeof(T) <= buffer_bytes)
            {
                mem::copy(buffer, ptr, left * sizeof(T));
                mem::move(ptr, ptr + left, right * sizeof(T));
                mem::copy(ptr + right, buffer, left * sizeof(T));
                return ptr + right;
            }

            if (right < left && right * sizeof(T) <= buffer_bytes)
            {
                mem::copy(buffer, ptr + left, right * sizeof(T));
                mem::move(ptr + right, ptr, left * sizeof(T));
                mem::copy(ptr, buffer, right * sizeof(T));
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
            // first != last here, so dereferencing in to_pointer is safe
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
// transform
//=========================================================================

// dst[i] = fn(src[i]). dst may equal src (in-place), but must not partially
// overlap it. Returns dst + num.
template <typename T, typename U, typename Fn>
constexpr T* transform(T* dst, const U* src, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        dst[i] = fn(src[i]);
    }

    return dst + num;
}

// dst[i] = fn(a[i], b[i])
template <typename T, typename U, typename V, typename Fn>
constexpr T* transform(T* dst, const U* a, const V* b, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        dst[i] = fn(a[i], b[i]);
    }

    return dst + num;
}

//=========================================================================

// Returns dst advanced by the number of elements written.
template <typename IT1, typename IT2, typename Fn, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 transform(IT1 dst, IT2 first, IT2 last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first, ++dst)
    {
        *dst = fn(*first);
    }

    return dst;
}

template <typename IT1, typename IT2, typename IT3, typename Fn, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value && traits::is_iterator<IT3>::value))>
constexpr IT1 transform(IT1 dst, IT2 first1, IT2 last1, IT3 first2, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    for (; first1 != last1; ++first1, ++first2, ++dst)
    {
        *dst = fn(*first1, *first2);
    }

    return dst;
}

//=========================================================================
// generate / generate_n
//=========================================================================

// ptr[i] = fn(), called in order. Returns ptr + num.
template <typename T, typename Fn>
constexpr T* generate(T* ptr, size_t num, Fn fn)
{
    for (size_t i = 0; i < num; ++i)
    {
        ptr[i] = fn();
    }

    return ptr + num;
}

template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT generate(IT first, IT last, Fn fn)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        *first = fn();
    }

    return last;
}

// Iterator + count. Returns first advanced by num.
template <typename IT, typename Fn, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT generate_n(IT first, size_t num, Fn fn)
{
    for (; 0 < num; --num, ++first)
    {
        *first = fn();
    }

    return first;
}

//=========================================================================
// replace / replace_if
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
// remove / remove_if
//=========================================================================

// Moves every element that is NOT equal to `value` toward the front,
// preserving their order. Returns the new logical end.
// Elements in [returned end, ptr + num) are valid but unspecified
// (moved-from); nothing is destroyed.
template <typename T, typename U>
constexpr T* remove(T* ptr, size_t num, const U& value)
{
    T* const last = ptr + num;

    // skip the untouched prefix (gets the memchr path for byte types)
    T* dst = range::find(ptr, num, value);

    if (dst == last)
    {
        return last;
    }

    for (T* src = dst + 1; src != last; ++src)
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

    if (first == last)
    {
        return last;
    }

    for (IT src = first; ++src != last;)
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

    if (first == last)
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

//=========================================================================
// unique
//=========================================================================

// Collapses each run of consecutive equal elements to its first element.
// Returns the new logical end. Tail is unspecified, as with remove.
template <typename T>
constexpr T* unique(T* ptr, size_t num)
{
    T* const last = ptr + num;
    T* result = range::adjacent_find(ptr, num);

    if (result == last)
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

    if (result == last)
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

    if (result == last)
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

    if (result == last)
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
// copy_if / remove_copy / remove_copy_if
//=========================================================================

// All write to dst and return dst advanced by the number of elements written.
// dst must not overlap the source.

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

//=========================================================================
// unique_copy
//=========================================================================

// Copies the first element of each run of equivalent elements.
// Compares against the previous SOURCE element, so forward iterators work.
template <typename T, typename U>
constexpr T* unique_copy(T* dst, const U* src, size_t num)
{
    if (num == 0)
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

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 unique_copy(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (first == last)
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

    if (first == last)
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

//=========================================================================
// reverse_copy
//=========================================================================

// dst[i] = src[num - 1 - i]. dst must not overlap the source
// (use range::reverse for in-place). Returns dst + num.
template <typename T, typename U>
constexpr T* reverse_copy(T* dst, const U* src, size_t num)
{
    const U* s = src + num;

    for (size_t i = 0; i < num; ++i)
    {
        --s;
        *dst = *s;
        ++dst;
    }

    return dst;
}

// Source needs bidirectional iterators. Returns dst advanced by the number
// of elements written.
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

//=========================================================================
// rotate_copy
//=========================================================================

// Writes the rotation of [src, src + num) to dst, so that src[mid] lands at
// dst[0]: first [mid, num), then [0, mid). dst must not overlap the source.
// mid is an index, matching range::rotate. Returns dst + num.
template <typename T, typename U>
constexpr T* rotate_copy(T* dst, const U* src, size_t num, size_t mid)
{
    VX_ASSERT(mid <= num);

    T* out = range::copy(dst, src + mid, num - mid);
    return range::copy(out, src, mid);
}

// Returns dst advanced by the number of elements written.
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
