#pragma once

#include "fn_trigonometric.h"
#include "fn_exponential.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== dot ===============

// the dot product of 2 quaternions gives cos(t / 2)

/**
 * @brief Calculates the dot product of two quaternions.
 *
 * This function computes the dot product between two quaternions 'q1' and 'q2'.
 *
 * @tparam T Element type of the quaternions.
 * @param q1 The first quaternion.
 * @param q2 The second quaternion.
 * @return The dot product of the two quaternions.
 */
template <typename T>
inline constexpr T dot(
    const detail::quat<T>& q1,
    const detail::quat<T>& q2
)
{
    return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}

/**
 * @brief Calculates the normalized dot product of two quaternions.
 *
 * This function computes the dot product between two quaternions 'q1' and 'q2' after normalizing them.
 *
 * @tparam T Element type of the quaternions.
 * @param v The first quaternion to be normalized and used in the dot product.
 * @param u The second quaternion to be normalized and used in the dot product.
 * @return The normalized dot product of the two quaternions.
 */
template <typename T>
inline constexpr T normalized_dot(
    const detail::quat<T>& q1,
    const detail::quat<T>& q2
)
{
    return dot(normalize(q1), normalize(q2));
}

// =============== scale ===============

/**
 * @brief Scales a quaternion by a scalar.
 *
 * This function scales the input quaternion 'q' by the specified scalar value.
 *
 * @tparam T Element type of the quaternion.
 * @param q The quaternion to be scaled.
 * @param scaler The scalar value to scale the quaternion by.
 * @return The resulting scaled quaternion.
 */
template <typename T>
inline constexpr detail::quat<T> scale(
    const detail::quat<T>& q,
    T scaler
)
{
    return q * scaler;
}

// =============== length ===============

/**
 * @brief Calculates the squared length of a quaternion.
 *
 * This function computes the squared length of the input quaternion 'q'.
 *
 * @tparam T Element type of the quaternion.
 * @param q The quaternion for which to calculate the squared length.
 * @return The squared length of the quaternion.
 */
template <typename T>
inline constexpr T length_squared(const detail::quat<T>& q)
{
    return dot(q, q);
}

/**
 * @brief Calculates the length of a quaternion.
 *
 * This function computes the length of the input quaternion 'q'.
 *
 * @tparam T Element type of the quaternion.
 * @param q The quaternion for which to calculate the length.
 * @return The length of the quaternion.
 */
template <typename T>
inline constexpr T length(const detail::quat<T>& q)
{
    return sqrt(length_squared(q));
}

// =============== normalize ===============

/**
 * @brief Normalizes a quaternion.
 *
 * This function normalizes the input quaternion 'q'.
 *
 * @tparam T Element type of the quaternion.
 * @param q The quaternion to be normalized.
 * @return The normalized quaternion. If the length of the input quaternion is 0,
 * the unit quaternion will be returned.
 */
template <typename T>
inline constexpr auto normalize(const detail::quat<T>& q)
{
    const T magsq = length_squared(q);
    if (magsq < math::epsilon<T>) return detail::quat<T>();
    return q * math::inverse_sqrt(magsq);
}

/**
 * @brief Check if a quaternion is normalized.
 *
 * This function takes a quaternion and checks if it is normalized,
 * meaning its magnitude (length) is approximately equal to 1.
 *
 * @tparam T The type of the quaternion components.
 * @param q The quaternion to be checked for normalization.
 * @return True if the quaternion is normalized, false otherwise.
 */
template <typename T>
inline constexpr bool is_normalized(const detail::quat<T>& q)
{
    return math::is_equal_approx(length_squared(q), static_cast<T>(1));
}

// =============== angle ===============

/**
 * @brief Calculates the angle between two quaternions.
 *
 * This function computes the angle (in radians) between two quaternions 'from' and 'to'.
 *
 * @tparam T Element type of the quaternions.
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The angle (in radians) between the two quaternions.
 */
template <typename T>
static inline constexpr T angle(
    const detail::quat<T>& from,
    const detail::quat<T>& to
)
{
    // We use the half angle identity:
    // cos(t / 2) = sqrt[(1 + cos(t)) / 2]
    // cos(t) = cos2(t / 2) * 2 - 1
    const T d = normalized_dot(from, to);
    return math::acos_clamped(d * d * static_cast<T>(2) - static_cast<T>(1));
}

/**
 * @brief Calculates the signed angle between two quaternions.
 *
 * This function computes the signed angle (in radians) between two quaternions 'from' and 'to'.
 *
 * @tparam T Element type of the quaternions.
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The signed angle (in radians) between the two quaternions.
 */
template <typename T>
static inline constexpr T signed_angle(
    const detail::quat<T>& from,
    const detail::quat<T>& to
)
{
    const T a = angle(from, to);
    const T c = (from.w * to.w) - (from.x * to.x) - (from.y * to.y) - (from.z * to.z);
    return (c < static_cast<T>(0)) ? -a : a;
}

}
}
