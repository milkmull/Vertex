#pragma once

#include "vertex/std/memory.hpp"

namespace vx {
namespace _dynamic_array_base_priv {

//=========================================================================
// data type
//=========================================================================

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

} // namespace _dynamic_array_base_priv
} // namespace vx
