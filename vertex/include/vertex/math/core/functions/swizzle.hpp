#pragma once

#include "../types.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// run time
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T>
VX_FORCE_INLINE constexpr T swizzle(
    const vec<L, T>& v,
    size_t xi
) noexcept
{
    return v[xi];
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr vec<2, T> swizzle(
    const vec<L, T>& v,
    size_t xi,
    size_t yi
) noexcept
{
    return vec<2, T>(v[xi], v[yi]);
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr vec<3, T> swizzle(
    const vec<L, T>& v,
    size_t xi,
    size_t yi,
    size_t zi
) noexcept
{
    return vec<3, T>(v[xi], v[yi], v[zi]);
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr vec<4, T> swizzle(
    const vec<L, T>& v,
    size_t xi,
    size_t yi,
    size_t zi,
    size_t wi
) noexcept
{
    return vec<4, T>(v[xi], v[yi], v[zi], v[wi]);
}

///////////////////////////////////////////////////////////////////////////////
// compile time
///////////////////////////////////////////////////////////////////////////////

template <size_t xi, size_t L, typename T>
VX_FORCE_INLINE constexpr T swizzle(const vec<L, T>& v) noexcept
{
    VX_STATIC_ASSERT(xi < L, "invalid index");
    return v[xi];
}

template <size_t xi, size_t yi, size_t L, typename T>
VX_FORCE_INLINE constexpr vec<2, T> swizzle(const vec<L, T>& v) noexcept
{
    VX_STATIC_ASSERT(xi < L, "invalid index");
    VX_STATIC_ASSERT(yi < L, "invalid index");
    return vec<2, T>(v[xi], v[yi]);
}

template <size_t xi, size_t yi, size_t zi, size_t L, typename T>
VX_FORCE_INLINE constexpr vec<3, T> swizzle(const vec<L, T>& v) noexcept
{
    VX_STATIC_ASSERT(xi < L, "invalid index");
    VX_STATIC_ASSERT(yi < L, "invalid index");
    VX_STATIC_ASSERT(zi < L, "invalid index");
    return vec<3, T>(v[xi], v[yi], v[zi]);
}

template <size_t xi, size_t yi, size_t zi, size_t wi, size_t L, typename T>
VX_FORCE_INLINE constexpr vec<4, T> swizzle(const vec<L, T>& v) noexcept
{
    VX_STATIC_ASSERT(xi < L, "invalid index");
    VX_STATIC_ASSERT(yi < L, "invalid index");
    VX_STATIC_ASSERT(zi < L, "invalid index");
    VX_STATIC_ASSERT(wi < L, "invalid index");
    return vec<4, T>(v[xi], v[yi], v[zi], v[wi]);
}

} // namespace math
} // namespace vx