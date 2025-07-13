#pragma once

#include "vertex/math/core/functions/geometric.hpp"
#include "vertex/math/core/functions/matrix.hpp"

namespace vx {
namespace math {
namespace transform2d {

///////////////////////////////////////////////////////////////////////////////
// translation
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> translate(const vec<2, T>& translation) noexcept
{
    return mat<3, 3, T>(
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        translation.x,     translation.y,     static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> extract_translation(const mat<3, 3, T>& m) noexcept
{
    return vec<2, T>(m.columns[2].x, m.columns[2].y);
}

///////////////////////////////////////////////////////////////////////////////
// rotation
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> rotate(T angle) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);

    return mat<3, 3, T>(
         cosa,              sina,              static_cast<T>(0),
        -sina,              cosa,              static_cast<T>(0),
         static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr T extract_rotation(const mat<3, 3, T>& m) noexcept
{
    return atan2(m.columns[0].y, m.columns[0].x);
}

///////////////////////////////////////////////////////////////////////////////
// scale
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> scale(const vec<2, T>& scale) noexcept
{
    return mat<3, 3, T>(
        scale.x,           static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), scale.y,           static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> extract_scale(const mat<3, 3, T>& m) noexcept
{
    return vec<2, T>(
        length(m.columns[0]),
        length(m.columns[1])
    );
}

///////////////////////////////////////////////////////////////////////////////
// trs
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> trs(const vec<2, T>& translation, T angle, const vec<2, T>& scale) noexcept
{
    const T cosa = cos(angle);
    const T sina = sin(angle);

    return mat<3, 3, T>(
         cosa * scale.x, sina * scale.x, static_cast<T>(0),
        -sina * scale.y, cosa * scale.y, static_cast<T>(0),
         translation.x,  translation.y,  static_cast<T>(1)
    );
}

///////////////////////////////////////////////////////////////////////////////
// transform
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<2, T> transform(const mat<3, 3, T>& m, const vec<2, T>& v) noexcept
{
    return vec<2, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + m.columns[2].x,
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + m.columns[2].y
    );
}

///////////////////////////////////////////////////////////////////////////////
// inverse
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr mat<3, 3, T> inverse(const mat<3, 3, T>& m) noexcept
{
    const mat<2, 2, T> ibasis = inverse(mat<2, 2, T>(m));

    return mat<3, 3, T>(
        vec<3, T>(ibasis.columns[0], static_cast<T>(0)),
        vec<3, T>(ibasis.columns[1], static_cast<T>(0)),
        vec<3, T>(ibasis * vec<2, T>(-m.columns[2]), static_cast<T>(1))
    );
}

} // namespace vx
} // namespace math
} // namespace transform2d