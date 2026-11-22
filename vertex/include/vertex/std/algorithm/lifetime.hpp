#pragma once

#include "vertex/std/_simd/simd_algorithms.hpp"
#include "vertex/std/algorithm/_priv/range_helpers.hpp"
#include "vertex/std/iterator/iterator.hpp"
#include "vertex/std/memory/_priv/memory_core.hpp"
#include "vertex/std/type_traits.hpp"

namespace vx {
namespace range {

//=========================================================================
// object lifetime: construct / destroy
//=========================================================================

template <typename T>
T* construct(T* ptr, size_t count)
{
    for (const T* const last = ptr + count; ptr != last; ++ptr)
    {
        mem::construct_in_place(ptr);
    }

    return ptr;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT construct(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        mem::construct_in_place(_range_priv::to_pointer(first));
    }

    return first;
}

template <typename T>
T* construct_maybe_trivial(T* ptr, size_t count)
{
    for (const T* const last = ptr + count; ptr != last; ++ptr)
    {
        mem::construct_in_place_maybe_trivial(ptr);
    }

    return ptr;
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT construct_maybe_trivial(IT first, IT last)
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    for (; first != last; ++first)
    {
        mem::construct_in_place_maybe_trivial(_range_priv::to_pointer(first));
    }

    return first;
}

template <typename T>
T* destroy(T* ptr, size_t count) noexcept
{
    VX_IF_CONSTEXPR (traits::is_trivially_destructible<T>::value)
    {
        return ptr + count;
    }
    else
    {
        for (const T* const last = ptr + count; ptr != last; ++ptr)
        {
            mem::destroy_in_place(ptr);
        }

        return ptr;
    }
}

template <typename IT, VX_REQUIRES(traits::is_iterator<IT>::value)>
IT destroy(IT first, IT last) noexcept
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    using T = typename traits::value_type<IT>::type;

    VX_IF_CONSTEXPR (traits::is_trivially_destructible<T>::value)
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

// Destroys `count` live objects starting at first. Returns first advanced by count.
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
            VX_IF_CONSTEXPR (!traits::is_trivially_destructible<T>::value)
            {
                mem::destroy_in_place(_range_priv::to_pointer(first));
            }
            ++first;
        }

        return first;
    }
}

} // namespace range
} // namespace vx
