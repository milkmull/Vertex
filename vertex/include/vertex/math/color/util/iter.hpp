#pragma once

#include "../types/base.hpp"
#include "vertex/config/language_config.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// vec iterators
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T* begin(color_t<T>& c) noexcept
{
    return &c.r;
}

template <typename T>
VX_FORCE_INLINE constexpr T* end(color_t<T>& c) noexcept
{
    return &c.r + 4;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* begin(const color_t<T>& c) noexcept
{
    return &c.r;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* end(const color_t<T>& c) noexcept
{
    return &c.r + 4;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* cbegin(const color_t<T>& c) noexcept
{
    return &c.r;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* cend(const color_t<T>& c) noexcept
{
    return &c.r + 4;
}

} // namespace math
} // namespace vx