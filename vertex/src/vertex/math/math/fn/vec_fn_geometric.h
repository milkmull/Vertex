#pragma once

#include "../detail/base_types.h"
#include "fn_trigonometric.h"
#include "fn_exponential.h"

namespace vx {
namespace math {

// =============== dot ===============

/**
 * @brief Calculates the dot product of two vectors.
 *
 * This function computes the dot product between two vectors 'v' and 'u'.
 *
 * @param v The first vector.
 * @param u The second vector.
 * @return The dot product of the two vectors.
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
 * @brief Calculates the normalized dot product of two vectors.
 *
 * This function computes the dot product between two vectors 'v' and 'u' after normalizing them.
 *
 * @param v The first vector to be normalized and used in the dot product.
 * @param u The second vector to be normalized and used in the dot product.
 * @return The normalized dot product of the two vectors.
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
 * @brief Calculates the cross product of two 2D vectors.
 *
 * This function computes the cross product between two 2D vectors 'v' and 'u'.
 *
 * @param v The first 2D vector.
 * @param u The second 2D vector.
 * @return The cross product of the two 2D vectors.
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
 * @brief Calculates the cross product of two 3D vectors.
 *
 * This function computes the cross product between two 3D vectors 'v' and 'u'.
 *
 * @param v The first 3D vector.
 * @param u The second 3D vector.
 * @return The cross product of the two 3D vectors.
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
 * @brief Scales a vector by a scalar.
 *
 * This function scales the input vector 'v' by the specified scalar value.
 *
 * @param v The vector to be scaled.
 * @param scaler The scalar value to scale the vector by.
 * @return The resulting scaled vector.
 */
template <size_type L, typename T>
inline constexpr vec<L, T> scale(
    const vec<L, T>& v,
    T scaler
)
{
    return v * scaler;
}

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
 * @brief Calculates the squared length of a vector.
 *
 * This function computes the squared length of the input vector 'v'.
 *
 * @param v The vector for which to calculate the squared length.
 * @return The squared length of the vector.
 */
template <size_type L, typename T>
inline constexpr auto length_squared(const vec<L, T>& v)
{
    return dot(v, v);
}

/**
 * @brief Calculates the length of a vector.
 *
 * This function computes the length of the input vector 'v'.
 *
 * @param v The vector for which to calculate the length.
 * @return The length of the vector.
 */
template <size_type L, typename T>
inline constexpr auto length(const vec<L, T>& v)
{
    return sqrt(length_squared(v));
}

// =============== distance ===============

/**
 * @brief Calculates the squared distance between two vectors.
 *
 * This function computes the squared distance between two vectors 'p0' and 'p1'.
 *
 * @param p0 The first vector.
 * @param p1 The second vector.
 * @return The squared distance between the two vectors.
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
 * @brief Calculates the distance between two vectors.
 *
 * This function computes the distance between two vectors 'p0' and 'p1'.
 *
 * @param p0 The first vector.
 * @param p1 The second vector.
 * @return The distance between the two vectors.
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
 * This function normalizes the input vector 'v'.
 *
 * @param v The vector to be normalized.
 * @return The normalized vector. If the length of the input vector is 0,
 * a zero vector will be returned.
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
 * @brief Normalizes a vector.
 *
 * This function normalizes the input vector 'v' without checking if it is 0.
 *
 * @param v The vector to be normalized.
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
 * @brief Check if a vector is normalized.
 *
 * This function takes a vector and checks if it is normalized,
 * meaning its magnitude (length) is approximately equal to 1.
 *
 * @param v The vector to be checked for normalization.
 * @return True if the vector is normalized, false otherwise.
 */
template <size_type L, typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_normalized_approx(const vec<L, T>& v, const T epsilon = math::epsilon<T>)
{
    return (length_squared(v) - static_cast<T>(1)) < epsilon;
}

// =============== clamp_magnitude ===============

/**
 * @brief Clamp the magnitude of the vector within a specified range.
 *
 * This function clamps the magnitude of the vector to be within the specified minimum and maximum values.
 * If the magnitude is below a small epsilon value, the function returns a zero vector.
 *
 * @param min The minimum magnitude allowed.
 * @param max The maximum magnitude allowed.
 * @return A new vector with the clamped magnitude.
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
 * @brief Calculates the angle between two vectors.
 *
 * This function computes the angle (in radians) between two vectors 'from' and 'to'.
 *
 * @param from The first vector.
 * @param to The second vector.
 * @return The angle (in radians) between the two vectors.
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
 * @brief Calculates the signed angle between two vectors.
 *
 * This function computes the signed angle (in radians) between two vectors 'from' and 'to'.
 * The sign indicates the direction of rotation from 'from' to 'to'.
 *
 * @param from The first vector.
 * @param to The second vector.
 * @return The signed angle (in radians) between the two vectors.
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
 * @brief Rotate a 2D vector by a specified angle.
 *
 * This function rotates the given 2D vector by the specified angle using
 * cosine and sine trigonometric functions.
 *
 * @param v The 2D vector to be rotated.
 * @param angle The rotation angle in radians.
 * @return A new 2D vector representing the rotated vector.
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
 * @brief Rotate a 3D vector around a specified axis by a specified angle.
 *
 * This function takes a 3D vector, a rotation axis, and a rotation angle, and applies
 * the rotation using Rodrigues' rotation formula. The formula combines cosine and sine trigonometric
 * functions to compute the rotated vector, ensuring accurate and efficient 3D rotations.
 *
 * @param v The original 3D vector to be rotated.
 * @param axis The rotation axis around which the rotation occurs.
 * @param angle The rotation angle in radians.
 * @return The rotated 3D vector.
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
 * This function projects the input vector 'i' onto the direction of the vector 'n'.
 *
 * @param i The vector to be projected.
 * @param n The vector onto which 'i' is projected.
 * @return The vector 'i' projected onto the direction of 'n'.
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
 * @brief Reflects a vector based on a normal vector.
 *
 * This function reflects the input vector 'i' based on the normal vector 'n'.
 *
 * @param i The vector to be reflected.
 * @param n The normal vector used for reflection.
 * @return The vector 'i' reflected based on the normal vector 'n'.
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
 * @brief Bounces a vector off a surface defined by a normal vector.
 *
 * This function calculates the bounce vector of the input vector 'i' off a surface
 * defined by the normal vector 'n'.
 *
 * @param i The incident vector.
 * @param n The normal vector of the surface.
 * @return The bounce vector of the incident vector off the surface.
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
 * @brief Refracts a vector based on a normal vector and a refraction index.
 *
 * This function refracts the input vector 'i' based on the normal vector 'n'
 * and a refraction index 'eta'.
 *
 * @param i The incident vector.
 * @param n The normal vector.
 * @param eta The refraction index.
 * @return The refracted vector.
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
 * @brief Ensures the correct orientation of a normal vector relative to an incident vector.
 *
 * This function ensures that the input normal vector 'n' has the correct orientation
 * relative to the incident vector 'i' based on the reference normal vector 'nref'.
 *
 * @param n The normal vector to be adjusted.
 * @param i The incident vector.
 * @param nref The reference normal vector.
 * @return The adjusted normal vector.
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
 * @brief Moves a vector towards a target vector by a specified delta.
 *
 * This function moves the input vector 'from' towards the target vector 'to'
 * by a specified delta distance. If the current distance between 'from' and 'to'
 * is less than or equal to the delta, the target vector 'to' is returned.
 * Otherwise, the vector 'from' is moved towards 'to' by the delta distance.
 *
 * @param from The starting vector.
 * @param to The target vector.
 * @param delta The distance to move towards the target.
 * @return The resulting vector after moving towards the target.
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
