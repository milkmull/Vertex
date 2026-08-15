#pragma once

#include "vertex/std/memory.hpp"

namespace vx {
namespace _dynamic_array_base_priv {

template <typename T>
struct dynamic_array_data
{
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    pointer ptr;
    size_type size;
    size_type capacity;

    dynamic_array_data() noexcept
        : ptr(), size(), capacity()
    {}

    dynamic_array_data release() noexcept
    {
        dynamic_array_data old = mem::move(*this);

        ptr = nullptr;
        size = 0;
        capacity = 0;

        return old;
    }

    void acquire(dynamic_array_data& other) noexcept
    {
        *this = other.release();
    }
};

template <typename growth_rate>
inline constexpr size_t grow_capacity(
    const size_t required_capacity,
    size_t current_capacity,
    const size_t max_capacity) noexcept
{
    VX_IF_CONSTEXPR (growth_rate::num == 1 && growth_rate::den == 1)
    {
        if (required_capacity > max_capacity)
        {
            return max_capacity;
        }

        return required_capacity;
    }
    else
    {
        current_capacity = current_capacity ? current_capacity : 1;

        size_t new_capacity;

        VX_IF_CONSTEXPR (growth_rate::num == 3 && growth_rate::den == 2)
        {
            // Guard against multiplication overflow: old_capacity * num
            if (current_capacity > max_capacity - current_capacity / 2)
            {
                return max_capacity;
            }

            new_capacity = current_capacity + current_capacity / 2;
        }
        else
        {
            // Guard against multiplication overflow: old_capacity * num
            if (current_capacity > max_capacity / growth_rate::num)
            {
                return max_capacity;
            }

            new_capacity = current_capacity * growth_rate::num / growth_rate::den;
        }

        if (new_capacity < required_capacity)
        {
            new_capacity = required_capacity;
        }

        return new_capacity;
    }
}

} // namespace _dynamic_array_base_priv
} // namespace vx
