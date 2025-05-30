#pragma once

#include "./geometric.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// dot
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
) noexcept
{
    return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}

///////////////////////////////////////////////////////////////////////////////
// length_squared
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T length_squared(const quat_t<T>& q) noexcept
{
    return dot(q, q);
}

template <typename T>
VX_FORCE_INLINE constexpr T magnitude_squared(const quat_t<T>& q) noexcept
{
    return length_squared(q);
}

///////////////////////////////////////////////////////////////////////////////
// length
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T length(const quat_t<T>& q) noexcept
{
    return sqrt(length_squared(q));
}

template <typename T>
VX_FORCE_INLINE constexpr T magnitude(const quat_t<T>& q) noexcept
{
    return length(q);
}

///////////////////////////////////////////////////////////////////////////////
// normalize
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> normalize(const quat_t<T>& q) noexcept
{
    const T magsq = length_squared(q);
    return (magsq <= constants<T>::epsilon) ? quat_t<T>(0, 0, 0, 0) : (q * inverse_sqrt(magsq));
}

///////////////////////////////////////////////////////////////////////////////
// normalize_fast
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> normalize_fast(const quat_t<T>& q) noexcept
{
    return q * inverse_sqrt(length_squared(q));
}

///////////////////////////////////////////////////////////////////////////////
// is_normalized
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool is_normalized(
    const quat_t<T>& q,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(length_squared(q), static_cast<T>(1), epsilon);
}

///////////////////////////////////////////////////////////////////////////////
// normalized_dot
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T normalized_dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
) noexcept
{
    return dot(normalize(q1), normalize(q2));
}

///////////////////////////////////////////////////////////////////////////////
// conjugate
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> conjugate(const quat_t<T>& q) noexcept
{
    return quat_t<T>(q.w, -q.x, -q.y, -q.z);
}

///////////////////////////////////////////////////////////////////////////////
// inverse
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> inverse(const quat_t<T>& q) noexcept
{
    return conjugate(q) / magnitude_squared(q);
}

///////////////////////////////////////////////////////////////////////////////
// axis
///////////////////////////////////////////////////////////////////////////////

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
// https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> axis(const quat_t<T>& q) noexcept
{
    const T nw = normalize(q).w;
    const T s2 = static_cast<T>(1) - (nw * nw);

    if (s2 <= constants<T>::epsilon)
    {
        // This indicates that the angle is 0 degrees and thus,
        // the axis does not matter. We choose the +y axis.
        return vec<3, T>::up();
    }

    const T invs = inverse_sqrt(s2);
    return vec<3, T>(
        q.x * invs,
        q.y * invs,
        q.z * invs
    );
}

///////////////////////////////////////////////////////////////////////////////
// angle
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T angle(const quat_t<T>& q) noexcept
{
    return static_cast<T>(2) * acos_clamped(normalize(q).w);
}

template <typename T>
VX_FORCE_INLINE constexpr T angle(
    const quat_t<T>& from,
    const quat_t<T>& to
) noexcept
{
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = normalized_dot(from, to);
    return acos_clamped(d * d * static_cast<T>(2) - static_cast<T>(1));
}

///////////////////////////////////////////////////////////////////////////////
// signed_angle
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr T signed_angle(
    const quat_t<T>& from,
    const quat_t<T>& to
) noexcept
{
    const T a = angle(from, to);
    const T c = (from.w * to.w) - (from.x * to.x) - (from.y * to.y) - (from.z * to.z);
    return (c < static_cast<T>(0)) ? -a : a;
}

///////////////////////////////////////////////////////////////////////////////
// axis_angle
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> axis_angle(const vec<3, T>& axis, T angle) noexcept
{
    const vec<3, T> naxis = normalize(axis);

    const T sina2 = sin(angle * static_cast<T>(0.5));
    const T cosa2 = cos(angle * static_cast<T>(0.5));

    return quat_t<T>(cosa2, naxis * sina2);
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

// https://en.m.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula

template <size_t L, typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const quat_t<T>& rotation
) noexcept
{
    const vec<3, T> qv = rotation.vector();
    const vec<3, T> uv = cross(qv, v);
    const vec<3, T> uuv = cross(qv, uv);

    return v + ((uv * rotation.w) + uuv) * static_cast<T>(2);
}

///////////////////////////////////////////////////////////////////////////////
// rotate_between
///////////////////////////////////////////////////////////////////////////////

// https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/quaternion.inl
// https://www.cesarkallas.net/arquivos/livros/informatica/game/Game%20Programming%20Gems%201.pdf (page 215 (pdf page 211))

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> rotate_between(const vec<3, T>& from, const vec<3, T>& to) noexcept
{
    const vec<3, T> fn = normalize(from);
    const vec<3, T> tn = normalize(to);

    const T cosa = dot(fn, tn);

    if (cosa > static_cast<T>(1) - constants<T>::epsilon)
    {
        // In this case, both vectors are pointing in the
        // same direction. We can return the identity
        // quaternion (no rotation).
        return quat_t<T>(1, 0, 0, 0);
    }

    const vec<3, T> axis = normalize(cross(fn, tn));

    if (cosa < static_cast<T>(-1) + constants<T>::epsilon)
    {
        // In this case, the vectors are pointing in opposite
        // directions. There is no ideal axis to rotate around,
        // so we choose one. In this case we just use the cross
        // product of the 2 input vectors and a rotation angle
        // of pi.
        return axis_angle(axis, constants<T>::pi);
    }

    const T s = sqrt((static_cast<T>(1) + cosa) * static_cast<T>(2));
    const T invs = static_cast<T>(1) / s;

    return quat_t<T>(
        s * static_cast<T>(0.5),
        axis.x * invs,
        axis.y * invs,
        axis.z * invs
    );
}

///////////////////////////////////////////////////////////////////////////////
// lerp
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> lerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
) noexcept
{
    return x * (static_cast<T>(1) - t) + y * t;
}

template <typename T>
VX_FORCE_INLINE constexpr quat_t<T> mix(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
) noexcept
{
    return lerp(x, y, t);
}

///////////////////////////////////////////////////////////////////////////////
// slerp
///////////////////////////////////////////////////////////////////////////////

// https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
// https://en.wikipedia.org/wiki/Slerp

// assuming qx and qy are normalized
// theta = arccos(dot(qx, qy))
// qm = { qx * sin((1 - t) * theta) + qy * sin(t * theta) } / sin(theta)

template <typename T>
inline constexpr quat_t<T> slerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
)
{
    T cos_alpha = normalized_dot(x, y);
    T xsign = static_cast<T>(1);

    if (cos_alpha < static_cast<T>(0))
    {
        // Since q(w,x,y,z) and q(-w,-x,-y,-z) represent the same
        // rotation we should make sure the result is not sensitive 
        // to whether the positive or inverted form of the
        // quaternion is used.
        // 
        // acos will return an angle between:
        // 
        // 0-90 degrees for positive inputs
        // 90-180 degrees for negative inputs
        // 
        // To ensure acos returns the smallest angle (or shortest
        // path around the hypersphere), if cos_alpha is negative,
        // we make it positive. To account for this, we also have
        // to negate one of the input quaternions to make the new
        // angle accurate.
        // 
        // It doesn't matter which quaternion we invert as the
        // result will still be the same.

        xsign = static_cast<T>(-1);
        cos_alpha = -cos_alpha;
    }

    if (cos_alpha >= static_cast<T>(1) - constants<T>::epsilon)
    {
        // If the angle between the quaternions is super small, we
        // can estimate with linear interpolation. This also helps
        // to avoid dividing by 0 later since acos(1) == sin(0) == 0.

        return lerp(xsign * x, y, t);
    }

    const T alpha = acos(cos_alpha);
    const T sin_alpha = sin(alpha);
    const T inv_sin_alpha = static_cast<T>(1) / sin_alpha;

    const T t1 = sin((static_cast<T>(1) - t) * alpha) * inv_sin_alpha;
    const T t2 = sin(t * alpha) * inv_sin_alpha;

    return (xsign * x * t1) + (y * t2);
}

} // namespace math
} // namespace vx