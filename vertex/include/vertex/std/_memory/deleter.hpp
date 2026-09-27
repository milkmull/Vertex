#pragma once

#include "vertex/std/_memory/memory_core.hpp"

namespace vx {
namespace mem {

//=========================================================================
// default deleters
//=========================================================================

template <typename T>
struct default_delete
{
    constexpr default_delete() noexcept = default;

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    default_delete(const default_delete<U>&) noexcept
    {}

    void operator()(T* ptr) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(T) > 0, "can't delete pointer to incomplete type");
        if (ptr)
        {
            mem::destroy(ptr);
        }
    }
};

template <typename T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U (*)[], T (*)[]>::value)>
    default_delete(const default_delete<U[]>&) noexcept
    {}

    void operator()(T* ptr, size_t count) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(T) > 0, "can't delete pointer to incomplete type");
        if (ptr && count)
        {
            mem::destroy_array(ptr, count);
        }
    }
};

} // namespace mem
} // namespace vx
