#pragma once

#include "../detail/base_types.h"
#include "fn_trigonometric.h"
#include "fn_exponential.h"

namespace vx {
namespace math {

// =============== dot ===============

/**
 * @brief Computes the dot product of two vectors.
 *
 * This function computes the dot product of two vectors.
 *
 * @tparam T Type of the vector components.
 * @param v The first vector.
 * @param u The second vector.
 * @return The dot product of v and u.
 */
template <typename T>
inline constexpr auto dot(
    const vec<2, T>& v,
    const vec<2, T>& u
)
{
    using FT = typename detail::to_float_type<T>::type;
    return static_cast<FT>((v.x * u.x) + (v.y * u.y));
}

template <typename T>
inline constexpr auto dot(
    const vec<3, T>& v,
    const vec<3, T>& u
)
{
    using FT = typename detail::to_float_type<T>::type;
    return static_cast<FT>((v.x * u.x) + (v.y * u.y) + (v.z * u.z));
}

template <typename T>
inline constexpr auto dot(
    const vec<4, T>& v,
    const vec<4, T>& u
)
{
    using FT = typename detail::to_float_type<T>::type;
    return static_cast<FT>((v.x * u.x) + (v.y * u.y) + (v.z * u.z) + (v.w * u.w));
}

/**
 * @brief Computes the normalized dot product of two vectors.
 *
 * This function computes the dot product of two vectors after normalizing them.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param v The first vector.
 * @param u The second vector.
 * @return The normalized dot product of v and u.
 */
template <size_type L, typename T>
inline constexpr auto normalized_dot(
    const vec<L, T>& v,
    const vec<L, T>& u
)
{
    return dot(normalize(v), normalize(u));
}

// =============== cross ===============

/**
 * @brief Computes the cross product of two 2D vectors.
 *
 * This function computes the cross product of two 2D vectors.
 *
 * @tparam T Type of the vector components.
 * @param v The first vector.
 * @param u The second vector.
 * @return The cross product of v and u.
 */
template <typename T>
inline constexpr auto cross(
    const vec<2, T>& v,
    const vec<2, T>& u
)
{
    using FT = typename detail::to_float_type<T>::type;
    return static_cast<FT>((v.x * u.y) - (v.y * u.x));
}

/**
 * @brief Computes the cross product of two 3D vectors.
 *
 * This function computes the cross product of two 3D vectors.
 *
 * @tparam T Type of the vector components.
 * @param v The first vector.
 * @param u The second vector.
 * @return The cross product of v and u.
 */
template <typename T>
inline constexpr auto cross(
    const vec<3, T>& v,
    const vec<3, T>& u
)
{
    using FT = typename detail::to_float_type<T>::type;
    return vec<3, FT>(
        (v.y * u.z) - (v.z * u.y),
        (v.z * u.x) - (v.x * u.z),
        (v.x * u.y) - (v.y * u.x)
    );
}

// =============== scale ===============

/**
 * @brief Scales a vector by a scalar value.
 *
 * This function scales a vector by a scalar value.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector to scale.
 * @param scaler The scalar value.
 * @return The scaled vector.
 */
template <size_type L, typename T>
inline constexpr vec<L, T> scale(
    const vec<L, T>& v,
    T scaler
)
{
    return v * scaler;
}

/**
 * @brief Scales a vector component-wise by another vector.
 *
 * This function scales each component of a vector by the corresponding component of another vector.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param v The vector to scale.
 * @param scaler The vector to scale by.
 * @return The scaled vector.
 */
template <size_type L, typename T>
inline constexpr vec<L, T> scale(
    const vec<L, T>& v,
    const vec<L, T>& scaler
)
{
    return v * scaler;
}

// =============== length ===============

/**
 * @brief Computes the squared length of a vector.
 *
 * This function computes the squared length of a vector.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector.
 * @return The squared length of v.
 */
template <size_type L, typename T>
inline constexpr auto length_squared(const vec<L, T>& v)
{
    return dot(v, v);
}

/**
 * @brief Computes the length of a vector.
 *
 * This function computes the length of a vector.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector.
 * @return The length of v.
 */
template <size_type L, typename T>
inline constexpr auto length(const vec<L, T>& v)
{
    return sqrt(length_squared(v));
}

// =============== distance ===============

/**
 * @brief Computes the squared distance between two points.
 *
 * This function computes the squared distance between two points represented by vectors.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param p0 The first point.
 * @param p1 The second point.
 * @return The squared distance between p0 and p1.
 */
template <size_type L, typename T>
inline constexpr auto distance_squared(
    const vec<L, T>& p0,
    const vec<L, T>& p1
)
{
    return length_squared(p1 - p0);
}

/**
 * @brief Computes the distance between two points.
 *
 * This function computes the distance between two points represented by vectors.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param p0 The first point.
 * @param p1 The second point.
 * @return The distance between p0 and p1.
 */
template <size_type L, typename T>
inline constexpr auto distance(
    const vec<L, T>& p0,
    const vec<L, T>& p1
)
{
    return length(p1 - p0);
}

// =============== normalize ===============

/**
 * @brief Normalizes a vector.
 *
 * This function normalizes a vector, i.e., it scales the vector to have unit
 * length. If the input vector is a zero vector or very close to it, a zero
 * vector is returned.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector to normalize.
 * @return The normalized vector if the input is not the zero vector;
 * otherwise, returns the zero vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> normalize(const vec<L, T>& v)
{
    const T magsq = length_squared(v);

    if VX_UNLIKELY(magsq < math::epsilon<T>)
    {
        return vec<L, T>(0);
    }

    return v * inverse_sqrt(magsq);
}

template <size_type L, typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto normalize(const vec<L, T>& v)
{
    using FT = typename detail::to_float_type<T>::type;
    return normalize(vec<L, FT>(v));
}

/**
 * @brief Fast normalization of a vector.
 *
 * This function performs a fast normalization of a vector, i.e., it scales the
 * vector to have approximately unit length without checking if the length is 0.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector to normalize.
 * @return The normalized vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> fast_normalize(const vec<L, T>& v)
{
    return v * inverse_sqrt(length_squared(v));
}

template <size_type L, typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto fast_normalize(const vec<L, T>& v)
{
    using FT = typename detail::to_float_type<T>::type;
    return fast_normalize(vec<L, FT>(v));
}

/**
 * @brief Checks if a vector is normalized.
 *
 * This function checks if a vector is normalized by comparing its squared length
 * to one within a specified epsilon value.
 *
 * @tparam L Number of components in the vector.
 * @tparam T Type of the vector components.
 * @param v The vector to be checked.
 * @param epsilon The epsilon value used for comparison (default: math::epsilon<T>).
 * @return True if the vector is normalized, false otherwise.
 */
template <size_type L, typename T>
inline constexpr bool is_normalized(const vec<L, T>& v, const T epsilon = math::epsilon<T>)
{
    return (length_squared(v) - static_cast<T>(1)) < epsilon;
}

// =============== clamp_magnitude ===============

/**
 * @brief Clamps the magnitude of a vector within a specified range.
 *
 * This function clamps the magnitude of a vector within a specified range. If
 * the input vector is the zero vector or very close to it, the zero vector is
 * returned.
 *
 * @tparam L Dimension of the vector.
 * @tparam T Type of the vector components.
 * @param v The vector whose magnitude to clamp.
 * @param min The minimum allowed magnitude.
 * @param max The maximum allowed magnitude.
 * @return The clamped vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> clamp_magnitude(
    const vec<L, T>& v,
    T min,
    T max
)
{
    const T mag = length(v);

    if (mag < math::epsilon<T>)
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

// =============== angle ===============

/**
 * @brief Computes the angle between two vectors.
 *
 * This function computes the angle between two vectors.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param from The first vector.
 * @param to The second vector.
 * @return The angle between from and to in radians.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr T angle(
    const vec<L, T>& from,
    const vec<L, T>& to
)
{
    return math::acos(normalized_dot(from, to));
}

/**
 * @brief Computes the signed angle between two vectors.
 *
 * This function computes the signed angle between two vectors. The sign
 * indicates the direction of rotation from the "from" vector to the "to"
 * vector.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param from The first vector.
 * @param to The second vector.
 * @return The signed angle between from and to in radians.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr T signed_angle(
    const vec<L, T>& from,
    const vec<L, T>& to
)
{
    const T a = angle(from, to);
    return (cross(from, to) < static_cast<T>(0)) ? -a : a;
}

// =============== rotate ===============

/**
 * @brief Rotates a 2D vector by a specified angle.
 *
 * This function rotates a 2D vector by a specified angle.
 *
 * @tparam T Type of the vector components.
 * @param v The 2D vector to rotate.
 * @param angle The angle of rotation in radians.
 * @return The rotated vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> rotate(const vec<2, T>& v, T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return vec<2, T>(
        (v.x * cosa) - (v.y * sina),
        (v.x * sina) + (v.y * cosa)
    );
}

/**
 * @brief Rotates a 3D vector around a specified axis by a given angle.
 *
 * This function rotates a 3D vector around a specified axis by a given angle.
 *
 * @tparam T Type of the vector components.
 * @param v The 3D vector to rotate.
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return The rotated vector.
 * 
 * @ref https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> rotate(
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

/**
 * @brief Rotates a 3D vector by a quaternion rotation.
 *
 * This function rotates a 3D vector by a quaternion rotation.
 *
 * @tparam T Type of the vector components.
 * @param v The 3D vector to rotate.
 * @param rotation The quaternion representing the rotation.
 * @return The rotated vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> rotate(
    const vec<3, T>& v,
    const quat_t<T>& rotation
)
{
    return rotation * v;
}

// =============== project ===============

/**
 * @brief Projects a vector onto another vector.
 *
 * This function projects a vector onto another vector.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param i The vector to project.
 * @param n The vector onto which to project.
 * @return The projected vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> project(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return n * (dot(i, n) / length_squared(n));
}

// =============== reflect ===============

/**
 * @brief Reflects a vector off a surface with a given normal.
 *
 * This function reflects a vector off a surface with a given normal.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param i The incident vector.
 * @param n The normal vector of the surface.
 * @return The reflected vector.
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> reflect(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return i - static_cast<T>(2) * normalized_dot(n, i) * n;
}

// =============== bounce ===============

/**
 * @brief Computes the bounce direction of a vector off a surface with a given normal.
 *
 * This function computes the bounce direction of a vector off a surface with a given normal.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param i The incident vector.
 * @param n The normal vector of the surface.
 * @return The bounced vector.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> bounce(
    const vec<L, T>& i,
    const vec<L, T>& n
)
{
    return -reflect(i, n);
}

// =============== refract ===============

/**
 * @brief Computes the refraction of a vector through a surface with a given
 * normal and refraction index.
 *
 * This function computes the refraction of a vector through a surface with a
 * given normal and refraction index.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param i The incident vector.
 * @param n The normal vector of the surface.
 * @param eta The ratio of refraction indices (refraction index of the medium
 * the incident ray is leaving divided by the refraction index of the medium
 * it is entering).
 * @return The refracted vector.
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/refract.xhtml
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> refract(
    const vec<L, T>& i,
    const vec<L, T>& n,
    T eta
)
{
    const T d = normalized_dot(n, i);
    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);

    if (k < math::epsilon<T>)
    {
        return vec<L, T>(0);
    }

    return eta * i - (eta * d + sqrt(k)) * n;
}

// =============== face_forward ===============

/**
 * @brief Ensures the normal vector is facing in the same direction as the
 * incident vector relative to a reference normal.
 *
 * This function ensures the normal vector is facing in the same direction as
 * the incident vector relative to a reference normal.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param n The original normal vector.
 * @param i The incident vector.
 * @param nref The reference normal vector.
 * @return The adjusted normal vector.
 * 
 * @ref https://registry.khronos.org/OpenGL-Refpages/gl4/html/faceforward.xhtml
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> face_forward(
    const vec<L, T>& n,
    const vec<L, T>& i,
    const vec<L, T>& nref
)
{
    return (normalized_dot(nref, i) < static_cast<T>(0)) ? n : -n;
}

// =============== move_toward ===============

/**
 * @brief Moves a vector towards a target vector by a specified distance.
 *
 * This function moves a vector towards a target vector by a specified distance.
 *
 * @tparam L Dimension of the vectors.
 * @tparam T Type of the vector components.
 * @param from The initial vector position.
 * @param to The target vector position.
 * @param delta The maximum distance to move towards the target.
 * @return The new vector position.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<L, T> move_toward(
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
