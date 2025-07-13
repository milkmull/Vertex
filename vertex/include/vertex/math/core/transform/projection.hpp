#pragma once

#include "vertex/math/core/functions/matrix.hpp"
#include "vertex/math/core/functions/trigonometric.hpp"

namespace vx {
namespace math {
namespace projection {

///////////////////////////////////////////////////////////////////////////////
// ortho
///////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixorthooffcenterlh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_lh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        static_cast<T>(2) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(2) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1) / (zfar - znear),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -znear / (zfar - znear),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_lh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        static_cast<T>(2) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(2) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(2) / (zfar - znear),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -(zfar + znear) / (zfar - znear),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixorthooffcenterrh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_rh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        static_cast<T>(2) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(2) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(-1) / (zfar - znear),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -znear / (zfar - znear),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_rh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        static_cast<T>(2) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(2) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(-2) / (zfar - znear),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -(zfar + znear) / (zfar - znear),
        static_cast<T>(1)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_lh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho_rh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> ortho(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// frustum
///////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectiveoffcenterlh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_lh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        (static_cast<T>(2) * znear) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        (static_cast<T>(2) * znear) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        zfar / (zfar - znear),
        static_cast<T>(1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_lh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        (static_cast<T>(2) * znear) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        (static_cast<T>(2) * znear) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        (zfar + znear) / (zfar - znear),
        static_cast<T>(1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectiveoffcenterrh

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_rh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        (static_cast<T>(2) * znear) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        (static_cast<T>(2) * znear) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        (right + left) / (right - left),
        (top + bottom) / (top - bottom),
        -zfar / (zfar - znear),
        static_cast<T>(-1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_rh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
    return mat<4, 4, T>(
        (static_cast<T>(2) * znear) / (right - left),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        (static_cast<T>(2) * znear) / (top - bottom),
        static_cast<T>(0),
        static_cast<T>(0),

        (right + left) / (right - left),
        (top + bottom) / (top - bottom),
        -(zfar + znear) / (zfar - znear),
        static_cast<T>(-1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return frustum_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustum_rh_zo(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return frustum_lh_no(left, right, bottom, top, znear, zfar);
#else
    return frustum_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_lh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return frustum_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustum_lh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum_rh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return frustum_rh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustum_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> frustum(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return frustum_lh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return frustum_lh_no(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return frustum_rh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return frustum_rh_no(left, right, bottom, top, znear, zfar);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// perspective
///////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_lh_zo(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
    const T tanfovy2 = tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
        static_cast<T>(1) / (aspect * tanfovy2),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(1) / tanfovy2,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        zfar / (zfar - znear),
        static_cast<T>(1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_lh_no(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
    const T tanfovy2 = tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
        static_cast<T>(1) / (aspect * tanfovy2),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(1) / tanfovy2,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        (zfar + znear) / (zfar - znear),
        static_cast<T>(1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovrh


template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_rh_zo(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
    const T tanfovy2 = tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
        static_cast<T>(1) / (aspect * tanfovy2),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(1) / tanfovy2,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        -zfar / (zfar - znear),
        static_cast<T>(-1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_rh_no(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
    const T tanfovy2 = tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
        static_cast<T>(1) / (aspect * tanfovy2),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(1) / tanfovy2,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        -(zfar + znear) / (zfar - znear),
        static_cast<T>(-1),

        static_cast<T>(0),
        static_cast<T>(0),
        -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
        static_cast<T>(0)
    );
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_zo(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
static VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_no(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return perspective_lh_no(fovy, aspect, znear, zfar);
#else
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_lh(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_lh_no(fovy, aspect, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> perspective_rh(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<4, 4, T> perspective(
    T fovy, T aspect,
    T znear, T zfar
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return perspective_lh_no(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// project
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> project_zo(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
    vec<4, T> v4(object, static_cast<T>(1));
    v4 = projection * (model * v4);

    const T invw = static_cast<T>(1) / v4.w;
    v4 *= invw;

    v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);

    v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
    v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

    return vec<3, T>(v4);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> project_no(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
    vec<4, T> v4(object, static_cast<T>(1));
    v4 = projection * (model * v4);

    const T invw = static_cast<T>(1) / v4.w;
    v4 *= invw;

    v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.z = v4.z * static_cast<T>(0.5) + static_cast<T>(0.5);

    v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
    v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

    return vec<3, T>(v4);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> project(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return project_zo(object, model, projection, viewport);
#else
    return project_no(object, model, projection, viewport);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// unproject
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> unproject_zo(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
    const mat<4, 4, T> inv = inverse(projection * model);
    vec<4, T> v4(object, static_cast<T>(1));

    v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.z);
    v4.y = (v4.y - static_cast<T>(viewport.y)) / static_cast<T>(viewport.w);

    v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
    v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);

    v4 = inv * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> unproject_no(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
    const mat<4, 4, T> inv = inverse(projection * model);
    vec<4, T> v4(object, static_cast<T>(1));

    v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.z);
    v4.y = (v4.y - static_cast<T>(viewport.y)) / static_cast<T>(viewport.w);

    v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
    v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);
    v4.z = v4.z * static_cast<T>(2) - static_cast<T>(1);

    v4 = inv * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename U, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> unproject(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
) noexcept
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return unproject_zo(object, model, projection, viewport);
#else
    return unproject_no(object, model, projection, viewport);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// projection
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> project(const mat<4, 4, T>& m, const vec<3, T>& v) noexcept
{
    vec<4, T> v4(v, static_cast<T>(1));
    v4 = m * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> project(const mat<4, 4, T>& m, const vec<4, T>& v) noexcept
{
    const vec<4, T> v4 = m * v;
    const T invw = static_cast<T>(1) / v4.w;
    return v4 * invw;
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> unproject(const mat<4, 4, T>& m, const vec<3, T>& v) noexcept
{
    vec<4, T> v4(v, static_cast<T>(1));
    v4 = inverse(m) * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<4, T> unproject(const mat<4, 4, T>& m, const vec<4, T>& v) noexcept
{
    const vec<4, T> v4 = inverse(m) * v;
    const T invw = static_cast<T>(1) / v4.w;
    return v4 * invw;
}

} // namespace projection
} // namespace math
} // namespace vx