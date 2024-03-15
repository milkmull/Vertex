#pragma once

#include "fn_exponential.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// =============== dot ===============

// the dot product of 2 quaternions gives cos(t / 2)

/**
 * @brief Calculates the dot product of two quaternions.
 *
 * @tparam T Type of the quaternion components.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The dot product of the two quaternions.
 */
template <typename T>
inline constexpr T dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
)
{
    return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}

/**
 * @brief Calculates the normalized dot product of two quaternions.
 *
 * @tparam T Type of the quaternion components.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The normalized dot product of the two quaternions.
 */
template <typename T>
inline constexpr T normalized_dot(
    const quat_t<T>& q1,
    const quat_t<T>& q2
)
{
    return dot(normalize(q1), normalize(q2));
}

// =============== length ===============

/**
 * @brief Calculates the squared length of a quaternion.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion.
 * @return The squared length of the quaternion.
 */
template <typename T>
inline constexpr T length_squared(const quat_t<T>& q)
{
    return dot(q, q);
}

/**
 * @brief Calculates the length of a quaternion.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion.
 * @return The length of the quaternion.
 */
template <typename T>
inline constexpr T length(const quat_t<T>& q)
{
    return sqrt(length_squared(q));
}

// =============== normalize ===============

/**
 * @brief Normalizes a quaternion.
 *
 * This function normalizes a quaternion, i.e., it scales the quaternion to have
 * unit length. If the input quaternion is a zero quaternion or very close to it,
 * an identity quaternion is returned.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion to be normalized.
 * @return The normalized quaternion.
 */
template <typename T>
inline constexpr quat_t<T> normalize(const quat_t<T>& q)
{
    const T magsq = length_squared(q);

    if VX_UNLIKELY(magsq < math::epsilon<T>)
    {
        return quat_t<T>();
    }

    return q * math::inverse_sqrt(magsq);
}

/**
 * @brief Fast normalization of a quaternion.
 *
 * This function performs a fast normalization of a quaternion, i.e., it scales the
 * quaternion to have approximately unit length without checking if the length is 0.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion to be fast normalized.
 * @return The fast normalized quaternion.
 */
template <typename T>
inline constexpr quat_t<T> fast_normalize(const quat_t<T>& q)
{
    return q * inverse_sqrt(length_squared(q));
}

/**
 * @brief Checks if a quaternion is normalized.
 *
 * This function checks if a quaternion is normalized by comparing its squared length
 * to one within a specified epsilon value.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion to be checked.
 * @param epsilon The epsilon value used for comparison (default: math::epsilon<T>).
 * @return True if the quaternion is normalized, false otherwise.
 */
template <typename T>
inline constexpr bool is_normalized(const quat_t<T>& q, const T epsilon = math::epsilon<T>)
{
    return (length_squared(q) - static_cast<T>(1)) < epsilon;
}

/**
 * @brief Computes the angle between two quaternions.
 *
 * @tparam T Type of the quaternion components.
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The angle between the two quaternions.
 */
template <typename T>
static inline constexpr T angle(
    const quat_t<T>& from,
    const quat_t<T>& to
)
{
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = normalized_dot(from, to);
    return math::acos_clamped(d * d * static_cast<T>(2) - static_cast<T>(1));
}

/**
 * @brief Computes the signed angle between two quaternions.
 *
 * @tparam T Type of the quaternion components.
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The signed angle between the two quaternions.
 */
template <typename T>
static inline constexpr T signed_angle(
    const quat_t<T>& from,
    const quat_t<T>& to
)
{
    const T a = angle(from, to);
    const T c = (from.w * to.w) - (from.x * to.x) - (from.y * to.y) - (from.z * to.z);
    return (c < static_cast<T>(0)) ? -a : a;
}

// =============== conjugate ===============

/**
 * @brief Computes the conjugate of a quaternion.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion.
 * @return The conjugate of the input quaternion.
 */
template <typename T>
inline constexpr quat_t<T> conjugate(const quat_t<T>& q)
{
    return quat_t<T>(q.w, -q.x, -q.y, -q.z);
}

// =============== invert ===============

/**
 * @brief Computes the inverse of a quaternion.
 *
 * @tparam T Type of the quaternion components.
 * @param q The quaternion.
 * @return The inverse of the input quaternion.
 */
template <typename T>
inline constexpr quat_t<T> invert(const quat_t<T>& q)
{
    return conjugate(q) / magnitude_squared(q);
}

}
}
