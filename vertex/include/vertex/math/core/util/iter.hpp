#pragma once

#include "vertex/math/core/types/base.hpp"
#include "vertex/config/language_config.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// vec iterators
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T>
VX_FORCE_INLINE constexpr T* begin(vec<L, T>& v) noexcept
{
    return &v.x;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr T* end(vec<L, T>& v) noexcept
{
    return &v.x + L;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr const T* begin(const vec<L, T>& v) noexcept
{
    return &v.x;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr const T* end(const vec<L, T>& v) noexcept
{
    return &v.x + L;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr const T* cbegin(const vec<L, T>& v) noexcept
{
    return &v.x;
}

template <size_t L, typename T>
VX_FORCE_INLINE constexpr const T* cend(const vec<L, T>& v) noexcept
{
    return &v.x + L;
}

///////////////////////////////////////////////////////////////////////////////
// mat loops
///////////////////////////////////////////////////////////////////////////////

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr vec<N, T>* begin(mat<M, N, T>& m) noexcept
{
    return m.columns;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr vec<N, T>* end(mat<M, N, T>& m) noexcept
{
    return m.columns + M;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr const vec<N, T>* begin(const mat<M, N, T>& m) noexcept
{
    return m.columns;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr const vec<N, T>* end(const mat<M, N, T>& m) noexcept
{
    return m.columns + M;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr const vec<N, T>* cbegin(const mat<M, N, T>& m) noexcept
{
    return m.columns;
}

template <size_t M, size_t N, typename T>
VX_FORCE_INLINE constexpr const vec<N, T>* cend(const mat<M, N, T>& m) noexcept
{
    return m.columns + M;
}

///////////////////////////////////////////////////////////////////////////////
// quat iterators
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T* begin(quat_t<T>& q) noexcept
{
    return &q.w;
}

template <typename T>
VX_FORCE_INLINE constexpr T* end(quat_t<T>& q) noexcept
{
    return &q.w + 4;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* begin(const quat_t<T>& q) noexcept
{
    return &q.w;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* end(const quat_t<T>& q) noexcept
{
    return &q.w + 4;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* cbegin(const quat_t<T>& q) noexcept
{
    return &q.w;
}

template <typename T>
VX_FORCE_INLINE constexpr const T* cend(const quat_t<T>& q) noexcept
{
    return &q.w + 4;
}

} // namespace math
} // namespace vx