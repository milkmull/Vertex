#pragma once

#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_base.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {
namespace _range_priv {

//=========================================================================

// Raw pointer from an iterator WITHOUT dereferencing when the iterator can
// give one up via .ptr(), so it is safe on end iterators and empty ranges.
// Falls back to &*it for iterators without .ptr().
template <typename IT>
constexpr auto to_pointer_impl(IT it, int) noexcept -> decltype(it.ptr())
{
    return it.ptr();
}

template <typename IT>
constexpr auto to_pointer_impl(IT it, long) noexcept -> decltype(&*it)
{
    return &*it;
}

template <typename T>
constexpr T* to_pointer(T* p) noexcept
{
    return p;
}

template <typename IT, VX_REQUIRES(!traits::is_pointer<IT>::value)>
constexpr auto to_pointer(IT it) noexcept
{
    return _range_priv::to_pointer_impl(it, 0);
}

//=========================================================================

// contiguous iterators (raw pointer or .ptr()) -> raw pointer, others unchanged
template <typename IT>
constexpr auto unwrap_impl(IT it, int) noexcept -> decltype(it.ptr())
{
    return it.ptr();
}

template <typename IT>
constexpr IT unwrap_impl(IT it, long) noexcept
{
    return it;
}

template <typename IT>
constexpr auto unwrap(IT it) noexcept
{
    return _range_priv::unwrap_impl(it, 0);
}

//=========================================================================

// -1 / 0 / 1 ordering of two lengths
constexpr int length_order(size_t a, size_t b) noexcept
{
    return static_cast<int>(b < a) - static_cast<int>(a < b);
}

//=========================================================================

// checks if a scalar has all bits set to zero
template <typename T>
constexpr bool is_all_bits_zero_impl(const T& x, traits::true_type) noexcept
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

//=========================================================================

// Swaps two NON-overlapping byte ranges in fixed-size chunks.
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

//=========================================================================

// Rotates [p, p + left + right) bytes so the right part comes first, parking
// the shorter side in a small stack buffer. Returns false (touching nothing)
// when the shorter side doesn't fit. Kept out of rotate() because constexpr
// functions can't declare uninitialized locals before C++20.
inline bool rotate_bytes(void* p, size_t left, size_t right) noexcept
{
    constexpr size_t buffer_bytes = 512;
    unsigned char buffer[buffer_bytes];
    unsigned char* b = static_cast<unsigned char*>(p);

    if (left <= right)
    {
        if (left > buffer_bytes)
        {
            return false;
        }

        mem::copy(buffer, b, left);
        mem::move(b, b + left, right);
        mem::copy(b + right, buffer, left);
    }
    else
    {
        if (right > buffer_bytes)
        {
            return false;
        }

        mem::copy(buffer, b + left, right);
        mem::move(b + right, b, left);
        mem::copy(b, buffer, right);
    }

    return true;
}

//=========================================================================

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

} // namespace _range_priv
} // namespace range
} // namespace vx
