#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"

namespace vx {
namespace range {

//=========================================================================
// fill / fill_n
//=========================================================================

template <typename T, typename U>
constexpr T* fill(T* ptr, size_t count, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<T*, U>::value))
        {
            mem::set(ptr, static_cast<int>(value), count * sizeof(T));
            return ptr + count;
        }
        else VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                mem::set(ptr, 0, count * sizeof(T));
                return ptr + count;
            }
        }
    }

    for (; 0 < count; --count, ++ptr)
    {
        *ptr = value;
    }

    return ptr;
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT fill(IT first, IT last, const U& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value || traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            // contiguous: the pointer form picks memset (or the zero check)
            range::fill(_range_priv::to_pointer(first), static_cast<size_t>(vx::distance(first, last)), value);
            return last;
        }
    }

    for (; first != last; ++first)
    {
        *first = value;
    }

    return last;
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
constexpr IT fill_n(IT dst, size_t count, const U& value)
{
    if (!VX_IS_CONSTANT_EVALUATED())
    {
        VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value || traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            range::fill(_range_priv::to_pointer(dst), count, value);
            return dst + count;
        }
    }

    for (; 0 < count; --count, ++dst)
    {
        *dst = value;
    }

    return dst;
}

// dst is raw memory
template <typename T, typename U>
T* fill_uninitialized(T* ptr, size_t count, const U& value)
{
    VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<T*, U>::value))
    {
        mem::set(ptr, static_cast<int>(value), count * sizeof(T));
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_range_priv::is_all_bits_zero(value))
            {
                mem::set(ptr, 0, count * sizeof(T));
                return ptr + count;
            }
        }

        for (; 0 < count; --count, ++ptr)
        {
            mem::construct_in_place(ptr, value);
        }

        return ptr;
    }
}

template <typename IT, typename U, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT fill_uninitialized(IT first, IT last, const U& value)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    VX_IF_CONSTEXPR ((traits::is_fill_memset_safe<IT, U>::value || traits::is_fill_zero_memset_safe<IT, U>::value))
    {
        range::fill_uninitialized(_range_priv::to_pointer(first), static_cast<size_t>(vx::distance(first, last)), value);
        return last;
    }
    else
    {
        for (; first != last; ++first)
        {
            mem::construct_in_place(_range_priv::to_pointer(first), value);
        }

        return last;
    }
}

} // namespace range
} // namespace vx
