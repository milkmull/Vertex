#pragma once

#include "fn_common.h"
#include "fn_trigonometric.h"
#include "fn_exponential.h"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// dot product
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the dot product of two vectors.
///
/// @tparam T Type of the vector components.
/// @param v The first vector.
/// @param u The second vector.
/// 
/// @return The dot product of v and u.
///////////////////////////////////////////////////////////////////////////////
template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<2, T>& v,
    const vec<2, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y);
}

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<3, T>& v,
    const vec<3, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

template <typename T>
VX_FORCE_INLINE constexpr T dot(
    const vec<4, T>& v,
    const vec<4, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z) + (v.w * u.w);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the dot product of two vectors after normalizing them.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param v The first vector.
/// @param u The second vector.
/// 
/// @return The normalized dot product of v and u.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr auto normalized_dot(
    const vec<L, T>& v,
    const vec<L, T>& u
)
{
    return dot(normalize(v), normalize(u));
}

///////////////////////////////////////////////////////////////////////////////
// cross product
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Compute an orthogonal vector to the input 2D vector.
///
/// The input vector is rotated by 90 degrees counter-clockwise to obtain the
/// orthogonal vector.
///
/// @tparam T The type of elements in the vector.
/// @param v The input 2D vector.
/// @return A new 2D vector that is orthogonal to the input vector.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> orthogonal(const vec<2, T>& v)
{
    return vec<2, T>(-v.y, v.x);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the cross product of two 2D vectors.
///
/// @tparam T Type of the vector components.
/// @param v The first vector.
/// @param u The second vector.
/// 
/// @return The cross product of v and u.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T cross(
    const vec<2, T>& v,
    const vec<2, T>& u
)
{
    return (v.x * u.y) - (v.y * u.x);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the cross product of two 3D vectors.
///
/// @tparam T Type of the vector components.
/// @param v The first vector.
/// @param u The second vector.
/// 
/// @return The cross product of v and u.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> cross(
    const vec<3, T>& v,
    const vec<3, T>& u
)
{
    return vec<3, T>(
        (v.y * u.z) - (v.z * u.y),
        (v.z * u.x) - (v.x * u.z),
        (v.x * u.y) - (v.y * u.x)
    );
}

///////////////////////////////////////////////////////////////////////////////
// length
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the squared length of a vector.
///
/// @tparam L Dimension of the vector.
/// @tparam T Type of the vector components.
/// @param v The vector.
/// 
/// @return The squared length of v.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T length_squared(const vec<L, T>& v)
{
    return dot(v, v);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the length of a vector.
///
/// @tparam L Dimension of the vector.
/// @tparam T Type of the vector components.
/// @param v The vector.
/// 
/// @return The length of v.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T length(const vec<L, T>& v)
{
    return sqrt(length_squared(v));
}

///////////////////////////////////////////////////////////////////////////////
// distance
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the squared distance between two points.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param p0 The first point.
/// @param p1 The second point.
/// 
/// @return The squared distance between p0 and p1.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T distance_squared(
    const vec<L, T>& p0,
    const vec<L, T>& p1
)
{
    return length_squared(p1 - p0);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the distance between two points.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param p0 The first point.
/// @param p1 The second point.
/// 
/// @return The distance between p0 and p1.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T distance(
    const vec<L, T>& p0,
    const vec<L, T>& p1
)
{
    return length(p1 - p0);
}

///////////////////////////////////////////////////////////////////////////////
// normalize
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Normalizes a vector.
///
/// This function normalizes a vector, i.e., it scales the vector to have unit
/// length. If the input vector is a zero vector or very close to it, a zero
/// vector is returned.
///
/// @tparam L Dimension of the vector.
/// @tparam T Type of the vector components.
/// @param v The vector to normalize.
/// 
/// @return The normalized vector if the input is not the zero vector;
/// otherwise, returns the zero vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> normalize(const vec<L, T>& v)
{
    const T magsq = length_squared(v);

    if VX_UNLIKELY(magsq < math::epsilon<T>())
    {
        return vec<L, T>(0);
    }

    return v * inverse_sqrt(magsq);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Fast normalization of a vector.
///
/// This function performs a fast normalization of a vector, i.e., it scales the
/// vector to have approximately unit length without checking if the length is 0.
///
/// @tparam L Dimension of the vector.
/// @tparam T Type of the vector components.
/// @param v The vector to normalize.
/// 
/// @return The normalized vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> fast_normalize(const vec<L, T>& v)
{
    return v * inverse_sqrt(length_squared(v));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Checks if a vector is normalized.
///
/// @tparam L Number of components in the vector.
/// @tparam T Type of the vector components.
/// @param v The vector to be checked.
/// @param epsilon The epsilon value used for comparison (default: math::epsilon<T>()).
/// 
/// @return True if the vector is normalized, false otherwise.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr bool is_normalized(const vec<L, T>& v, const T epsilon = math::epsilon<T>())
{
    return (length_squared(v) - static_cast<T>(1)) < epsilon;
}

///////////////////////////////////////////////////////////////////////////////
// clamp_magnitude
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Clamps the magnitude of a vector within a specified range.
///
/// This function clamps the magnitude of a vector within a specified range. If
/// the input vector is the zero vector or very close to it, the zero vector is
/// returned.
///
/// @tparam L Dimension of the vector.
/// @tparam T Type of the vector components.
/// @param v The vector whose magnitude to clamp.
/// @param min The minimum allowed magnitude.
/// @param max The maximum allowed magnitude.
/// 
/// @return The clamped vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> clamp_magnitude(
    const vec<L, T>& v,
    T min,
    T max
)
{
    const T mag = length(v);

    if (mag < math::epsilon<T>())
    {
        return vec<L, T>(0);
    }

    const T new_mag = math::clamp(
        mag,
        static_cast<T>(min),
        static_cast<T>(max)
    );

    return (v * (static_cast<T>(1) / mag)) * new_mag;
}

///////////////////////////////////////////////////////////////////////////////
// aspect
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Compute the aspect ratio of a 2D vector.
///
/// The aspect represents the ratio of its x-component to its y-component. If
/// the y-component is close to zero, it returns zero to avoid division by zero.
///
/// @tparam T The type of elements in the vector.
/// @param v The input 2D vector.
/// 
/// @return The aspect ratio of the input vector.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T aspect(const vec<2, T>& v)
{
    return static_cast<T>(math::abs(v.y) > math::epsilon<T>()) * (v.x / v.y);
}

///////////////////////////////////////////////////////////////////////////////
// angle
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Compute the angle of a 2D vector relative to the positive x-axis.
///
/// @tparam T The type of elements in the vector.
/// @param v The input 2D vector.
/// 
/// @return The angle of the input vector in radians.
///////////////////////////////////////////////////////////////////////////////
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T angle(const vec<2, T>& v)
{
    return math::atan2(v.y, v.x);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the angle between two vectors.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param from The first vector.
/// @param to The second vector.
/// 
/// @return The angle between from and to in radians.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T angle(
    const vec<L, T>& from,
    const vec<L, T>& to
)
{
    return math::acos(normalized_dot(from, to));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the signed angle between two vectors.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param from The first vector.
/// @param to The second vector.
/// 
/// @return The signed angle between from and to in radians.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T signed_angle(
    const vec<L, T>& from,
    const vec<L, T>& to
)
{
    const T a = angle(from, to);
    return (cross(from, to) < static_cast<T>(0)) ? -a : a;
}

///////////////////////////////////////////////////////////////////////////////
// rotate
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotates a 2D vector by a specified angle.
///
/// @tparam T Type of the vector components.
/// @param v The 2D vector to rotate.
/// @param angle The angle of rotation in radians.
/// 
/// @return The rotated vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> rotate(const vec<2, T>& v, T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return vec<2, T>(
        (v.x * cosa) - (v.y * sina),
        (v.x * sina) + (v.y * cosa)
    );
}

// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotates a 3D vector around a specified axis by a given angle.
///
/// @tparam T Type of the vector components.
/// @param v The 3D vector to rotate.
/// @param axis The axis of rotation.
/// @param angle The angle of rotation in radians.
/// 
/// @return The rotated vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const vec<3, T>& axis,
    T angle
)
{
    const vec<3, T> naxis = normalize(axis);

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return v * cosa + cross(naxis, v) * sina + naxis * dot(naxis, v) * (static_cast<T>(1) - cosa);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Rotates a 3D vector by a quaternion rotation.
///
/// @tparam T Type of the vector components.
/// @param v The 3D vector to rotate.
/// @param rotation The quaternion representing the rotation.
/// 
/// @return The rotated vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const quat_t<T>& rotation
)
{
    return rotation * v;
}

///////////////////////////////////////////////////////////////////////////////
// project
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Projects a vector onto another vector.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param i The vector to project.
/// @param n The vector onto which to project.
/// 
/// @return The projected vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> project(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return n * (dot(i, n) / length_squared(n));
}

///////////////////////////////////////////////////////////////////////////////
// reflect
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml

///////////////////////////////////////////////////////////////////////////////
/// @brief Reflects a vector off a surface with a given normal.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param i The incident vector.
/// @param n The normal vector of the surface.
/// 
/// @return The reflected vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> reflect(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return i - static_cast<T>(2) * normalized_dot(n, i) * n;
}

///////////////////////////////////////////////////////////////////////////////
// bounce
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the bounce direction of a vector off a surface with a given
/// normal.
/// 
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param i The incident vector.
/// @param n The normal vector of the surface.
/// 
/// @return The bounced vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> bounce(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return -reflect(i, n);
}

///////////////////////////////////////////////////////////////////////////////
// refract
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/refract.xhtml

///////////////////////////////////////////////////////////////////////////////
/// @brief Computes the refraction of a vector through a surface with a given
/// normal and refraction index.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param i The incident vector.
/// @param n The normal vector of the surface.
/// @param eta The ratio of refraction indices (refraction index of the medium
/// the incident ray is leaving divided by the refraction index of the medium
/// it is entering).
/// 
/// @return The refracted vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> refract(
    const vec<L, T>& i,
    const vec<L, T>& n,
    T eta
)
{
    const T d = normalized_dot(n, i);
    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);

    if (k < math::epsilon<T>())
    {
        return vec<L, T>(0);
    }

    return eta * i - (eta * d + sqrt(k)) * n;
}

///////////////////////////////////////////////////////////////////////////////
// face_forward
///////////////////////////////////////////////////////////////////////////////

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/faceforward.xhtml

///////////////////////////////////////////////////////////////////////////////
/// @brief Ensures the normal vector is facing in the same direction as the
/// incident vector relative to a reference normal.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param n The original normal vector.
/// @param i The incident vector.
/// @param nref The reference normal vector.
/// 
/// @return The adjusted normal vector.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> face_forward(
    const vec<L, T>& n,
    const vec<L, T>& i,
    const vec<L, T>& nref
)
{
    return (normalized_dot(nref, i) < static_cast<T>(0)) ? n : -n;
}

///////////////////////////////////////////////////////////////////////////////
// move_toward
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// @brief Moves a vector towards a target vector by a specified distance.
///
/// @tparam L Dimension of the vectors.
/// @tparam T Type of the vector components.
/// @param from The initial vector position.
/// @param to The target vector position.
/// @param delta The maximum distance to move towards the target.
/// 
/// @return The new vector position.
///////////////////////////////////////////////////////////////////////////////
template <size_t L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<L, T> move_toward(
    const vec<L, T>& from,
    const vec<L, T>& to,
    T delta
)
{
    const vec<L, T> vd = to - from;
    const T d = length(vd);
    return (d <= delta) ? to : (from + (vd / d * delta));
}

}
}
