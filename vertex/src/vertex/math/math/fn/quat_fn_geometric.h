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
 * This function computes the dot product between two quaternions 'q1' and 'q2'.
 *
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
 * This function computes the dot product between two quaternions 'q1' and 'q2' after normalizing them.
 *
 * @param v The first quaternion to be normalized and used in the dot product.
 * @param u The second quaternion to be normalized and used in the dot product.
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

// =============== scale ===============

/**
 * @brief Scales a quaternion by a scalar.
 *
 * This function scales the input quaternion 'q' by the specified scalar value.
 *
 * @param q The quaternion to be scaled.
 * @param scaler The scalar value to scale the quaternion by.
 * @return The resulting scaled quaternion.
 */
template <typename T>
inline constexpr quat_t<T> scale(
    const quat_t<T>& q,
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
 * @param q The quaternion for which to calculate the squared length.
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
 * This function computes the length of the input quaternion 'q'.
 *
 * @param q The quaternion for which to calculate the length.
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
 * This function normalizes the input quaternion 'q'.
 *
 * @param q The quaternion to be normalized.
 * @return The normalized quaternion. If the length of the input quaternion is 0,
 * the unit quaternion will be returned.
 */
template <typename T>
inline constexpr quat_t<T> normalize(const quat_t<T>& q)
{
    const T magsq = length_squared(q);

    if VX_UNLIKELY(magsq < math::epsilon<T>)
        return quat_t<T>();

    return q * math::inverse_sqrt(magsq);
}

/**
 * @brief Normalizes a quaternion.
 *
 * This function normalizes the input quaternion 'q' without checking if it is 0.
 *
 * @param q The quaternion to be normalized.
 * @return The normalized quaternion.
 */
template <typename T>
inline constexpr quat_t<T> fast_normalize(const quat_t<T>& q)
{
    return q * inverse_sqrt(length_squared(q));
}

/**
 * @brief Check if a quaternion is normalized.
 *
 * This function takes a quaternion and checks if it is normalized,
 * meaning its magnitude (length) is approximately equal to 1.
 *
 * @param q The quaternion to be checked for normalization.
 * @return True if the quaternion is normalized, false otherwise.
 */
template <typename T>
inline constexpr bool is_normalized_approx(const quat_t<T>& q, const T epsilon = math::epsilon<T>)
{
    return (length_squared(q) - static_cast<T>(1)) < epsilon;
}

// =============== axis ===============

/**
 * @brief Retrieves the rotation axis of a quaternion.
 *
 * This function calculates and returns the 3D vector representing the rotation axis
 * of the quaternion 'q'. If the quaternion represents no rotation (angle is 0),
 * it returns the positive y-axis.
 *
 * @param q The quaternion for which to extract the axis.
 * @return The rotation axis of the quaternion.
 *
 * @ref https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
 * @ref https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions
 */
template <typename T>
inline constexpr auto axis(const quat_t<T>& q)
{
    const T nw = normalize(q).w;
    const T s2 = static_cast<T>(1) - (nw * nw);

    if (s2 < math::epsilon<T>)
    {
        // This indicates that the angle is 0 degrees and thus,
        // the axis does not matter. We choose the +y axis.
        return vec<3, T>::UP();
    }

    const T invs = math::inverse_sqrt(s2);
    return vec<3, T>(
        q.x * invs,
        q.y * invs,
        q.z * invs
    );
}

// =============== angle ===============

/**
 * @brief Retrieves the rotation angle of a quaternion.
 *
 * This function calculates and returns the rotation angle (in radians) represented
 * by the quaternion 'q'.
 *
 * @param q The quaternion for which to extract the angle.
 * @return The rotation angle of the quaternion in radians.
 */
template <typename T>
inline constexpr T angle(const quat_t<T>& q)
{
    return static_cast<T>(2) * math::acos_clamped(normalize(q).w);
}

/**
 * @brief Calculates the angle between two quaternions.
 *
 * This function computes the angle (in radians) between two quaternions 'from' and 'to'.
 *
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The angle (in radians) between the two quaternions.
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
 * @brief Calculates the signed angle between two quaternions.
 *
 * This function computes the signed angle (in radians) between two quaternions 'from' and 'to'.
 *
 * @param from The first quaternion.
 * @param to The second quaternion.
 * @return The signed angle (in radians) between the two quaternions.
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
 * @brief Calculates the conjugate of a quaternion.
 *
 * The conjugate of a quaternion is obtained by negating the imaginary components
 * (x, y, z) while keeping the real component (w) unchanged.
 *
 * @param q The quaternion for which to calculate the conjugate.
 * @return The conjugate of the quaternion.
 */
template <typename T>
inline constexpr quat_t<T> conjugate(const quat_t<T>& q)
{
    return quat_t<T>(q.w, -q.x, -q.y, -q.z);
}

// =============== invert ===============

/**
 * @brief Calculates the inverse of a quaternion.
 *
 * This function returns the inverse of a quaternion, effectively
 * reversing the encoded rotation.
 *
 * @param q The quaternion to invert.
 * @return The inverse of the quaternion.
 */
template <typename T>
inline constexpr quat_t<T> invert(const quat_t<T>& q)
{
    return conjugate(q) / length_squared(q);
}

}
}
