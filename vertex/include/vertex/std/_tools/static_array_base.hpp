#pragma once

#include "vertex/std/memory.hpp"

namespace vx {
namespace _static_array_base_priv {

//=========================================================================
// data type
//=========================================================================

template <size_t N, typename T>
struct static_array_data
{
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    value_type ptr[N];
    size_type size = 0;
};

template <size_t N, typename T, bool = std::is_trivially_destructible<T>::value>
struct static_array_base
{
    static_array_data<N, T> m_data;
    ~static_array_base() = default;
};

template <size_t N, typename T>
struct static_array_base<N, T, false>
{
    static_array_data<N, T> m_data;

    ~static_array_base()
    {
        if (m_data.size)
        {
            mem::destroy_range(m_data.ptr, m_data.size);
        }
    }
};

} // namespace _static_array_base_priv
} // namespace vx
