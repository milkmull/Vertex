#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"
#include "vertex/std/util.hpp"

namespace vx {
namespace range {

//=========================================================================
// copy / move / copy_or_move (forward)
//=========================================================================

// ---- copy -----------------------------------------------------------------

template <typename T>
constexpr T* copy(T* dst, const T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            mem::move(dst, src, count * sizeof(T));
            return dst + count;
        }
    }

    for (; 0 < count; --count, ++src, ++dst)
    {
        *dst = *src;
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

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

    for (; first != last; ++first, ++dst)
    {
        *dst = *first;
    }

    return dst;
}

// Assigns `count` elements from first to dst. Returns dst advanced by count.
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

    for (; 0 < count; --count, ++dst, ++first)
    {
        *dst = *first;
    }

    return dst;
}

// dst is uninitialized memory and must not overlap src
template <typename T>
T* copy_uninitialized(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::copy(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count, ++src, ++dst)
        {
            mem::construct_in_place(dst, *src);
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        range::copy_uninitialized(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first, ++dst)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), *first);
        }

        return dst;
    }
}

// dst is raw memory and must not overlap the source. No rollback if a
// constructor throws.
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
        for (; 0 < count; --count, ++dst, ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), *first);
        }

        return dst;
    }
}

// ---- move -----------------------------------------------------------------

template <typename T>
constexpr T* move(T* dst, T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            mem::move(dst, src, count * sizeof(T));
            return dst + count;
        }
    }

    for (; 0 < count; --count, ++src, ++dst)
    {
        *dst = _range_priv::as_rvalue(*src);
    }

    return dst;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 move(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

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

    for (; first != last; ++first, ++dst)
    {
        *dst = _range_priv::as_rvalue(*first);
    }

    return dst;
}

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

    for (; 0 < count; --count, ++dst, ++first)
    {
        *dst = _range_priv::as_rvalue(*first);
    }

    return dst;
}

// dst is uninitialized memory
template <typename T>
T* move_uninitialized(T* dst, T* src, size_t count)
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count, ++src, ++dst)
        {
            mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        range::move_uninitialized(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first, ++dst)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), _range_priv::as_rvalue(*first));
        }

        return dst;
    }
}

// ---- copy_or_move: these were byte-for-byte identical to move, so they
// ---- forward to it (the uninitialized ones now use memmove, a superset of
// ---- the old memcpy precondition).

template <typename T>
constexpr T* copy_or_move(T* dst, T* src, size_t count)
{
    return range::move(dst, src, count);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 copy_or_move(IT1 dst, IT2 first, IT2 last)
{
    return range::move(dst, first, last);
}

template <typename T>
T* copy_or_move_uninitialized(T* dst, T* src, size_t count)
{
    return range::move_uninitialized(dst, src, count);
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 copy_or_move_uninitialized(IT1 dst, IT2 first, IT2 last)
{
    return range::move_uninitialized(dst, first, last);
}

//=========================================================================
// copy / move (backward)
//=========================================================================

// dst_last is one past the destination; src is the BEGINNING of the source.
// Returns the beginning of the written destination.
template <typename T>
constexpr T* copy_backward(T* dst_last, const T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            T* dst_first = dst_last - count;
            mem::move(dst_first, src, count * sizeof(T));
            return dst_first;
        }
    }

    for (const T* src_last = src + count; 0 < count; --count)
    {
        --src_last;
        --dst_last;
        *dst_last = *src_last;
    }

    return dst_last;
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
            range::copy_backward(_range_priv::to_pointer(dst_last), _range_priv::to_pointer(first), count);
            return dst_last - count;
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

template <typename T>
constexpr T* move_backward(T* dst_last, T* src, size_t count)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            T* dst_first = dst_last - count;
            mem::move(dst_first, src, count * sizeof(T));
            return dst_first;
        }
    }

    for (T* src_last = src + count; 0 < count; --count)
    {
        --src_last;
        --dst_last;
        *dst_last = _range_priv::as_rvalue(*src_last);
    }

    return dst_last;
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 move_backward(IT1 dst_last, IT2 first, IT2 last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first, last));
            range::move_backward(_range_priv::to_pointer(dst_last), _range_priv::to_pointer(first), count);
            return dst_last - count;
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
// relocate / shift
//=========================================================================

// Move-constructs each element into raw dst, then destroys the source.
// dst must not overlap src, or must precede it.
template <typename T>
T* relocate(T* dst, T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count, ++src, ++dst)
        {
            mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
            mem::destroy_in_place(src);
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
IT1 relocate(IT1 dst, IT2 first, IT2 last) noexcept
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(vx::distance(first, last));
        range::relocate(_range_priv::to_pointer(dst), _range_priv::to_pointer(first), count);
        return dst + count;
    }
    else
    {
        for (; first != last; ++first, ++dst)
        {
            mem::construct_in_place(_range_priv::to_pointer(dst), _range_priv::as_rvalue(*first));
            mem::destroy_in_place(_range_priv::to_pointer(first));
        }

        return dst;
    }
}

// Shifts [first, last) toward higher addresses by `count`. The destination
// slots past `last` must hold live objects (they are assigned to).
// Returns the new beginning (first + count).
template <typename T>
constexpr T* shift_forward(T* first, T* last, size_t count) noexcept
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            mem::move(first + count, first, static_cast<size_t>(last - first) * sizeof(T));
            return first + count;
        }
    }

    for (T *src = last, *dst = last + count; src != first;)
    {
        --src;
        --dst;
        *dst = _range_priv::as_rvalue(*src);
    }

    return first + count;
}

// Same, but the slots past `last` are raw memory (constructed into).
template <typename T>
T* shift_forward_uninitialized(T* first, T* last, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::move(first + count, first, static_cast<size_t>(last - first) * sizeof(T));
        return first + count;
    }
    else
    {
        for (T *src = last, *dst = last + count; src != first;)
        {
            --src;
            --dst;

            if (dst >= last)
            {
                mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
            }
            else
            {
                *dst = _range_priv::as_rvalue(*src);
            }
        }

        return first + count;
    }
}

// Shifts [first, last) toward lower addresses by `count`. The slots before
// `first` must hold live objects. Returns the new end (last - count).
template <typename T>
constexpr T* shift_backward(T* first, T* last, size_t count) noexcept
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            mem::move(first - count, first, static_cast<size_t>(last - first) * sizeof(T));
            return last - count;
        }
    }

    for (T *src = first, *dst = first - count; src != last; ++src, ++dst)
    {
        *dst = _range_priv::as_rvalue(*src);
    }

    return last - count;
}

// Same, but the slots before `first` are raw memory (constructed into).
template <typename T>
T* shift_backward_uninitialized(T* first, T* last, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
    {
        mem::move(first - count, first, static_cast<size_t>(last - first) * sizeof(T));
        return last - count;
    }
    else
    {
        for (T *src = first, *dst = first - count; src != last; ++src, ++dst)
        {
            if (dst < first)
            {
                mem::construct_in_place(dst, _range_priv::as_rvalue(*src));
            }
            else
            {
                *dst = _range_priv::as_rvalue(*src);
            }
        }

        return last - count;
    }
}

//=========================================================================
// swap
//=========================================================================

// Swaps `count` elements between two NON-overlapping ranges; returns a + count.
template <typename T>
constexpr T* swap(T* a, T* b, size_t count) noexcept
{
    VX_ASSERT(a + count <= b || b + count <= a);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR (traits::memmove_is_safe<T*>::value)
        {
            // trivially relocatable: swap raw bytes through a small stack buffer
            _range_priv::swap_bytes(a, b, count * sizeof(T));
            return a + count;
        }
    }

    for (size_t i = 0; i < count; ++i)
    {
        vx::swap(a[i], b[i]);
    }

    return a + count;
}

// Returns last1.
template <typename IT1, typename IT2, VX_REQUIRES((traits::is_iterator<IT1>::value && traits::is_iterator<IT2>::value))>
constexpr IT1 swap(IT1 first1, IT1 last1, IT2 first2)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first1, last1);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::memmove_is_safe<IT1, IT2>::value))
        {
            const size_t count = static_cast<size_t>(vx::distance(first1, last1));
            range::swap(_range_priv::to_pointer(first1), _range_priv::to_pointer(first2), count);
            return last1;
        }
    }

    for (; first1 != last1; ++first1, ++first2)
    {
        vx::iter_swap(first1, first2);
    }

    return first1;
}

} // namespace range
} // namespace vx
