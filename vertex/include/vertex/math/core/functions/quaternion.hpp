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
    return (magsq <= constants<T>::epsilon) ? quat_t<T>::identity() : (q * inverse_sqrt(magsq));
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
    return conjugate(q) / length_squared(q);
}

///////////////////////////////////////////////////////////////////////////////
// axis
///////////////////////////////////////////////////////////////////////////////

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
// https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions

template <typename T>
VX_FORCE_INLINE constexpr vec<3, T> axis(const quat_t<T>& q) noexcept
{
    // (assume q is normalized)

    const T s2 = static_cast<T>(1) - (q.w * q.w);
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
    // (assume q is normalized)
    return static_cast<T>(2) * acos_clamped(q.w);
}

template <typename T>
VX_FORCE_INLINE constexpr T angle(
    const quat_t<T>& from,
    const quat_t<T>& to
) noexcept
{
    // (assume from and to are normalized)
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = dot(from, to);
    return acos_clamped(d * d * static_cast<T>(2) - static_cast<T>(1));
}

///////////////////////////////////////////////////////////////////////////////
// axis_angle
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr quat_t<T> axis_angle(const vec<3, T>& axis, T angle) noexcept
{
    // (assume axis is normalized)

    const T sina2 = sin(angle * static_cast<T>(0.5));
    const T cosa2 = cos(angle * static_cast<T>(0.5));

    return quat_t<T>(cosa2, axis * sina2);
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

// https://en.m.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const quat_t<T>& rotation
) noexcept
{
    // (assume rotation is normalized)

    const vec<3, T> qv = rotation.xyz();
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
VX_FORCE_INLINE constexpr quat_t<T> rotate_between(
    const vec<3, T>& from,
    const vec<3, T>& to
) noexcept
{
    // (assume from and to are normalized)

    const T cosa = dot(from, to);

    if (cosa > static_cast<T>(1) - constants<T>::epsilon)
    {
        // In this case, both vectors are pointing in the
        // same direction. We can return the identity
        // quaternion (no rotation).
        return quat_t<T>::identity();
    }

    if (cosa < static_cast<T>(-1) + constants<T>::epsilon)
    {
        // The vectors are pointing in exactly opposite directions.
        // In this case, there are infinitely many valid rotation axes — any axis orthogonal to the vector is valid.
        // To choose a stable one, we attempt to generate an orthogonal axis by crossing with a reference vector.
        // We try the Z axis first; if it's parallel to 'from', we fall back to the X axis.
        // The resulting axis is guaranteed to be orthogonal and used to construct a 180-degree rotation.

        constexpr vec<3, T> z(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));
        constexpr vec<3, T> x(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0));

        vec<3, T> axis = cross(z, from);
        if (length_squared(axis) < constants<T>::epsilon)
        {
            // bad luck, they were parallel, try again!
            axis = cross(x, from);
        }

        return axis_angle(normalize(axis), constants<T>::pi);
    }

    const vec<3, T> axis = cross(from, to);

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
// equivalent_rotation
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool equivalent_rotation(
    const quat_t<T>& a,
    const quat_t<T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return abs(dot(a, b)) > static_cast<T>(1) - epsilon;
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
VX_FORCE_INLINE constexpr quat_t<T> slerp(
    const quat_t<T>& x,
    const quat_t<T>& y,
    T t
)
{
    T cos_half_theta = dot(x, y);
    T xsign = static_cast<T>(1);

    if (cos_half_theta < static_cast<T>(0))
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
        // path around the hypersphere), if cos_half_theta is negative,
        // we make it positive. To account for this, we also have
        // to negate one of the input quaternions to make the new
        // angle accurate.
        // 
        // It doesn't matter which quaternion we invert as the
        // result will still be the same.

        xsign = static_cast<T>(-1);
        cos_half_theta = -cos_half_theta;
    }

    // cos_half_theta is guaranteed to be positive here

    if (cos_half_theta >= static_cast<T>(1) - constants<T>::epsilon)
    {
        // If the angle between the quaternions is super small, we
        // can estimate with linear interpolation. This also helps
        // to avoid dividing by 0 later since acos(1) == sin(0) == 0.

        const quat_t<T> qt = (xsign * x) * (static_cast<T>(1) - t) + y * t;
        return normalize(qt);
    }

    if (cos_half_theta <= constants<T>::epsilon)
    {
        // If theta is 180 degrees (cos_half_theta is about 0) we can
        // rotate about any axis normal to either quaternion.
        // In this edge case, slerp becomes numerically unstable due to
        // division by sin(theta), where theta ~= pi and sin(pi) ~= 0.
        // Although a true spherical interpolation is ambiguous
        // (infinitely many shortest arcs), linear interpolation
        // followed by normalization provides a valid and practical
        // midpoint rotation.

        // In both cases—when the quaternions are nearly identical or
        // exactly opposite—lerp followed by normalize offers a stable,
        // simple, and sufficiently accurate approximation of slerp.

        const quat_t<T> qt = (x + y) * 0.5f;
        return normalize(qt);
    }

    const T half_theta = acos(cos_half_theta);
    const T sin_half_theta = sqrt(static_cast<T>(1) - cos_half_theta * cos_half_theta);
    const T inv_sin_half_theta = static_cast<T>(1) / sin_half_theta;

    const T t1 = sin((static_cast<T>(1) - t) * half_theta);
    const T t2 = sin(t * half_theta);

    const quat_t<T> qt = ((xsign * x * t1) + (y * t2)) * inv_sin_half_theta;
    return normalize(qt);
}

} // namespace math
} // namespace vx