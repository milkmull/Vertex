#pragma once

#include "detail/base_type_defs.h"
#include "fn_trigonometric.h"
#include "fn_exponential.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== to_float ===============

/**
 * @brief Converts an integral vector to a floating-point vector.
 *
 * This function converts an integral vector 'v' to a floating-point vector.
 *
 * @tparam T Element type of the vector.
 * @param v The integral vector to be converted.
 * @return The resulting floating-point vector.
 *
 * @note This function is applicable for vectors of type 'veci'.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr typename auto to_float(const detail::veci<L, T, Q>& v)
{
    return typename detail::veci<L, T, Q>::float_type(*this);
}

// =============== dot ===============

/**
 * @brief Calculates the dot product of two vectors.
 *
 * This function computes the dot product between two vectors 'v' and 'u'.
 *
 * @tparam T Element type of the vectors.
 * @param v The first vector.
 * @param u The second vector.
 * @return The dot product of the two vectors.
 */
template <typename T>
inline constexpr T dot(
    const detail::vecx<2, T>& v,
    const detail::vecx<2, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y);
}

template <typename T>
inline constexpr T dot(
    const detail::vecx<3, T>& v,
    const detail::vecx<3, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z);
}

template <typename T>
inline constexpr T dot(
    const detail::vecx<4, T>& v,
    const detail::vecx<4, T>& u
)
{
    return (v.x * u.x) + (v.y * u.y) + (v.z * u.z) + (v.w * u.w);
}

template <typename T>
inline constexpr T dot(
    const detail::col<T>& v,
    const detail::col<T>& u
)
{
    return (v.r * u.r) + (v.g * u.g) + (v.b * u.b) + (v.a * u.a);
}

/**
 * @brief Calculates the normalized dot product of two vectors.
 *
 * This function computes the dot product between two vectors 'v' and 'u' after normalizing them.
 *
 * @tparam T Element type of the vectors.
 * @param v The first vector to be normalized and used in the dot product.
 * @param u The second vector to be normalized and used in the dot product.
 * @return The normalized dot product of the two vectors.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr T normalized_dot(
    const detail::vecf<L, T, Q>& v,
    const detail::vecf<L, T, Q>& u
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
 * @tparam T Element type of the vectors.
 * @param v The first 2D vector.
 * @param u The second 2D vector.
 * @return The cross product of the two 2D vectors.
 */
template <typename T>
inline constexpr T cross(
    const detail::vecx<2, T>& v,
    const detail::vecx<2, T>& u
)
{
    return (v.x * u.y) - (v.y * u.x);
}

/**
 * @brief Calculates the cross product of two 3D vectors.
 *
 * This function computes the cross product between two 3D vectors 'v' and 'u'.
 *
 * @tparam T Element type of the vectors.
 * @param v The first 3D vector.
 * @param u The second 3D vector.
 * @return The cross product of the two 3D vectors.
 */
template <typename T>
inline constexpr detail::vecx<3, T> cross(
    const detail::vecx<3, T>& v,
    const detail::vecx<3, T>& u
)
{
    return detail::vecx<3, T>(
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
 * @tparam T Element type of the vector.
 * @param v The vector to be scaled.
 * @param scaler The scalar value to scale the vector by.
 * @return The resulting scaled vector.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vec<L, T, Q> scale(
    const detail::vec<L, T, Q>& v,
    T scaler
)
{
    return v * scaler;
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vec<L, T, Q> scale(
    const detail::vec<L, T, Q>& v,
    const detail::vec<L, T, Q>& scaler
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
 * @tparam T Element type of the vector.
 * @param v The vector for which to calculate the squared length.
 * @return The squared length of the vector.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length_squared(const detail::vecf<L, T, Q>& v)
{
    return dot(v, v);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length_squared(const detail::veci<L, T, Q>& v)
{
    return static_cast<detail::int_float_type>(dot(v, v));
}

/**
 * @brief Calculates the length of a vector.
 *
 * This function computes the length of the input vector 'v'.
 *
 * @tparam T Element type of the vector.
 * @param v The vector for which to calculate the length.
 * @return The length of the vector.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto length(const detail::vec<L, T, Q>& v)
{
    return sqrt(length_squared(v));
}

// =============== distance ===============

/**
 * @brief Calculates the squared distance between two vectors.
 *
 * This function computes the squared distance between two vectors 'p0' and 'p1'.
 *
 * @tparam T Element type of the vectors.
 * @param p0 The first vector.
 * @param p1 The second vector.
 * @return The squared distance between the two vectors.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto distance_squared(
    const detail::vec<L, T, Q>& p0,
    const detail::vec<L, T, Q>& p1
)
{
    return length_squared(p1 - p0);
}

/**
 * @brief Calculates the distance between two vectors.
 *
 * This function computes the distance between two vectors 'p0' and 'p1'.
 *
 * @tparam T Element type of the vectors.
 * @param p0 The first vector.
 * @param p1 The second vector.
 * @return The distance between the two vectors.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto distance(
    const detail::vec<L, T, Q>& p0,
    const detail::vec<L, T, Q>& p1
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
 * @tparam T Element type of the vector.
 * @param v The vector to be normalized.
 * @return The normalized vector. If the length of the input vector is 0,
 * a zero vector will be returned.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> normalize(const detail::vecf<L, T, Q>& v)
{
    const T magsq = length_squared(v);
    if (magsq < math::epsilon<T>)
        return detail::vecf<L, T, Q>(0);
    return v * inverse_sqrt(magsq);
}

/**
 * @brief Check if a vector is normalized.
 *
 * This function takes a vector and checks if it is normalized,
 * meaning its magnitude (length) is approximately equal to 1.
 *
 * @tparam T The type of the vector components.
 * @param v The vector to be checked for normalization.
 * @return True if the vector is normalized, false otherwise.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr bool is_normalized(const detail::vecf<L, T, Q>& v)
{
    return math::is_equal_approx(length_squared(v), static_cast<T>(1));
}

// =============== clamp_magnitude ===============

/**
 * @brief Clamp the magnitude of the vector within a specified range.
 *
 * This function clamps the magnitude of the vector to be within the specified minimum and maximum values.
 * If the magnitude is below a small epsilon value, the function returns a zero vector.
 *
 * @tparam T Element type of the vector.
 * @param min The minimum magnitude allowed.
 * @param max The maximum magnitude allowed.
 * @return A new vector with the clamped magnitude.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> clamp_magnitude(
    const detail::vecf<L, T, Q>& v,
    T min,
    T max
)
{
    const T mag = length(v);

    if (mag < math::epsilon<T>)
        return detail::vecf<L, T, Q>(0);

    const T new_mag = math::clamp(
        mag,
        static_cast<T>(min),
        static_cast<T>(max)
    );

    return (v * (static_cast<T>(1) / mag)) * new_mag;
}

// =============== angle ===============

/**
 * @brief Calculates the angle between two 2D vectors.
 *
 * This function computes the angle (in radians) between two 2D vectors 'from' and 'to'.
 *
 * @tparam T Element type of the vectors.
 * @param from The first 2D vector.
 * @param to The second 2D vector.
 * @return The angle (in radians) between the two 2D vectors.
 */
template <typename T>
static inline constexpr T angle(
    const detail::vecf<2, T>& from,
    const detail::vecf<2, T>& to
)
{
    return math::acos(normalized_dot(from, to));
}

/**
 * @brief Calculates the signed angle between two 2D vectors.
 *
 * This function computes the signed angle (in radians) between two 2D vectors 'from' and 'to'.
 * The sign indicates the direction of rotation from 'from' to 'to'.
 *
 * @tparam T Element type of the vectors.
 * @param from The first 2D vector.
 * @param to The second 2D vector.
 * @return The signed angle (in radians) between the two 2D vectors.
 */
template <typename T>
static inline constexpr T signed_angle(
    const detail::vecf<2, T>& from,
    const detail::vecf<2, T>& to
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
 * @tparam T The type of the vector components.
 * @param v The 2D vector to be rotated.
 * @param angle The rotation angle in radians.
 * @return A new 2D vector representing the rotated vector.
 */
template <typename T>
inline constexpr detail::vecf<2, T> rotate(const detail::vecf<2, T>& v, T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return detail::vecf<2, T>(
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
template <typename T>
inline constexpr detail::vecf<3, T> rotate(
    const detail::vecf<3, T>& v, 
    const detail::vecf<3, T>& axis, 
    T angle
)
{
    const detail::vecf<3, T> naxis(normalize(axis));

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return v * cosa + cross(naxis, v) * sina + naxis * dot(naxis, v) * (static_cast<T>(1) - cosa);
}

/**
 * @brief Rotate a 3D vector around the X-axis.
 *
 * This function takes a 3D vector and rotates it around the X-axis by a specified angle.
 * The rotation is achieved using cosine and sine trigonometric functions to compute the new vector.
 *
 * @param v The original 3D vector to be rotated.
 * @param angle The rotation angle in radians.
 * @return The rotated 3D vector.
 */
template <typename T>
inline constexpr detail::vecf<3, T> rotate_x(
    const detail::vecf<3, T>& v,
    T angle
)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return detail::vecf<3, T>(
        v.x,
        (v.y * cosa) - (v.z * sina),
        (v.y * sina) + (v.z * cosa)
    );
}

/**
 * @brief Rotate a 3D vector around the Y-axis.
 *
 * This function takes a 3D vector and rotates it around the Y-axis by a specified angle.
 * The rotation is achieved using cosine and sine trigonometric functions to compute the new vector.
 *
 * @param v The original 3D vector to be rotated.
 * @param angle The rotation angle in radians.
 * @return The rotated 3D vector.
 */
template <typename T>
inline constexpr detail::vecf<3, T> rotate_y(
    const detail::vecf<3, T>& v,
    T angle
)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return detail::vecf<3, T>(
        (v.x * cosa) + (v.z * sina),
        v.y,
        -(v.x * sina) + (v.z * cosa)
    );
}

/**
 * @brief Rotate a 3D vector around the Z-axis.
 *
 * This function takes a 3D vector and rotates it around the Z-axis by a specified angle.
 * The rotation is achieved using cosine and sine trigonometric functions to compute the new vector.
 *
 * @param v The original 3D vector to be rotated.
 * @param angle The rotation angle in radians.
 * @return The rotated 3D vector.
 */
template <typename T>
inline constexpr detail::vecf<3, T> rotate_z(
    const detail::vecf<3, T>& v,
    T angle
)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return detail::vecf<3, T>(
        (v.x * cosa) - (v.y * sina),
        (v.x * sina) + (v.y * cosa),
        v.z
    );
}

// =============== project ===============

/**
 * @brief Projects a vector onto another vector.
 *
 * This function projects the input vector 'i' onto the direction of the vector 'n'.
 *
 * @tparam T Element type of the vectors.
 * @param i The vector to be projected.
 * @param n The vector onto which 'i' is projected.
 * @return The vector 'i' projected onto the direction of 'n'.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> project(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
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
 * @tparam T Element type of the vectors.
 * @param i The vector to be reflected.
 * @param n The normal vector used for reflection.
 * @return The vector 'i' reflected based on the normal vector 'n'.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> reflect(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
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
 * @tparam T Element type of the vectors.
 * @param i The incident vector.
 * @param n The normal vector of the surface.
 * @return The bounce vector of the incident vector off the surface.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> bounce(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n
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
 * @tparam T Element type of the vectors.
 * @param i The incident vector.
 * @param n The normal vector.
 * @param eta The refraction index.
 * @return The refracted vector.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> refract(
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& n,
    T eta
)
{
    const T d = normalized_dot(n, i);
    const T k = static_cast<T>(1) - eta * eta * (static_cast<T>(1) - d * d);

    if (k < math::epsilon<T>)
        return detail::vec<L, T, Q>(0);

    return eta * i - (eta * d + sqrt(k)) * n;
}

// =============== face_forward ===============

/**
 * @brief Ensures the correct orientation of a normal vector relative to an incident vector.
 *
 * This function ensures that the input normal vector 'n' has the correct orientation
 * relative to the incident vector 'i' based on the reference normal vector 'nref'.
 *
 * @tparam T Element type of the vectors.
 * @param n The normal vector to be adjusted.
 * @param i The incident vector.
 * @param nref The reference normal vector.
 * @return The adjusted normal vector.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> face_forward(
    const detail::vecf<L, T, Q>& n,
    const detail::vecf<L, T, Q>& i,
    const detail::vecf<L, T, Q>& nref
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
 * @tparam T Element type of the vectors.
 * @param from The starting vector.
 * @param to The target vector.
 * @param delta The distance to move towards the target.
 * @return The resulting vector after moving towards the target.
 */
template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr detail::vecf<L, T, Q> move_toward(
    const detail::vecf<L, T, Q>& from,
    const detail::vecf<L, T, Q>& to,
    T delta
)
{
    const detail::vecf<L, T, Q> vd(to - from);
    const T d = length(vd);
    return (d <= delta) ? to : (from + (vd / d * delta));
}

}
}
