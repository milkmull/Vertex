#pragma once

#include "../math.h"

namespace vx {
namespace math {
namespace transform3d {
namespace euler {

// =============== quaternion ===============

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_(in_3-2-1_sequence)_to_quaternion_conversion
// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf
// https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/euler_angles.inl

// quat_from_euler

/**
 * @brief Creates a quaternion from Euler angles in XYZ order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the X, Y, and Z axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the X-axis in radians.
 * @param t2 The rotation angle around the Y-axis in radians.
 * @param t3 The rotation angle around the Z-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_XYZ(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in XZY order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the X, Z, and Y axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the X-axis in radians.
 * @param t2 The rotation angle around the Z-axis in radians.
 * @param t3 The rotation angle around the Y-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_XZY(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1),
        -(st1 * st2 * ct3) + (st1 * ct1 * ct2),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in XYX order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the X, Y, and X axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the X-axis in radians.
 * @param t2 The rotation angle around the Y-axis in radians.
 * @param t3 The rotation angle around the X-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_XYX(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        +ct2 * math::sin(t1p3),
        +st2 * math::cos(t1m3),
        +st2 * math::sin(t1m3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in XZX order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the X, Y, and X axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the X-axis in radians.
 * @param t2 The rotation angle around the Z-axis in radians.
 * @param t3 The rotation angle around the X-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_XZX(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        +ct2 * math::sin(t1p3),
        -st2 * math::sin(t1m3),
        +st2 * math::cos(t1m3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in YXZ order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Y, X, and Z axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Y-axis in radians.
 * @param t2 The rotation angle around the X-axis in radians.
 * @param t3 The rotation angle around the Z-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_YXZ(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1),
        -(st1 * st2 * ct3) + (st3 * ct1 * ct2)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in YZX order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Y, Z, and X axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Y-axis in radians.
 * @param t2 The rotation angle around the Z-axis in radians.
 * @param t3 The rotation angle around the X-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_YZX(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct2)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in YXY order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Y, X, and Y axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Y-axis in radians.
 * @param t2 The rotation angle around the X-axis in radians.
 * @param t3 The rotation angle around the Y-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_YXY(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::cos(t1m3),
        +ct2 * math::sin(t1p3),
        -st2 * math::sin(t1m3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in YZY order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Y, Z, and Y axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Y-axis in radians.
 * @param t2 The rotation angle around the Z-axis in radians.
 * @param t3 The rotation angle around the Y-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_YZY(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::sin(t1m3),
        +ct2 * math::sin(t1p3),
        +st2 * math::cos(t1m3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in ZXY order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Z, X, and Y axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Z-axis in radians.
 * @param t2 The rotation angle around the X-axis in radians.
 * @param t3 The rotation angle around the Y-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_ZXY(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        -(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        -(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * ct2 * ct3) + (st2 * st3 * ct1)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in ZYX order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Z, Y, and X axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Z-axis in radians.
 * @param t2 The rotation angle around the Y-axis in radians.
 * @param t3 The rotation angle around the X-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_ZYX(T t1, T t2, T t3)
{
    t1 *= static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);
    t3 *= static_cast<T>(0.5);

    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return quat_t<T>(
        +(st1 * st2 * st3) + (ct1 * ct2 * ct3),
        -(st1 * st2 * ct3) + (st3 * ct1 * ct2),
        +(st1 * st3 * ct2) + (st2 * ct1 * ct3),
        +(st1 * ct2 * ct3) - (st2 * st3 * ct1)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in ZXZ order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Z, X, and Z axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Z-axis in radians.
 * @param t2 The rotation angle around the X-axis in radians.
 * @param t3 The rotation angle around the Z-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        +st2 * math::cos(t1m3),
        +st2 * math::sin(t1m3),
        +ct2 * math::sin(t1p3)
    );
}

/**
 * @brief Creates a quaternion from Euler angles in ZYZ order.
 *
 * This function takes three Euler angles (t1, t2, t3) in radians representing rotations around
 * the Z, Y, and Z axes, respectively. It computes and returns the corresponding quaternion.
 *
 * @param t1 The rotation angle around the Z-axis in radians.
 * @param t2 The rotation angle around the Y-axis in radians.
 * @param t3 The rotation angle around the Z-axis in radians.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> quat_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T t1p3 = (t1 + t3) * static_cast<T>(0.5);
    const T t1m3 = (t1 - t3) * static_cast<T>(0.5);
    t2 *= static_cast<T>(0.5);

    const T ct2 = math::cos(t2);
    const T st2 = math::sin(t2);

    return quat_t<T>(
        +ct2 * math::cos(t1p3),
        -st2 * math::sin(t1m3),
        +st2 * math::cos(t1m3),
        +ct2 * math::sin(t1p3)
    );
}

// quat_to_euler

/**
 * @brief Converts a quaternion to Euler angles in XYZ order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the X, Y, and Z axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_XYZ(const quat_t<T>& q)
{
    return mat3_to_euler_XYZ(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in XZY order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the X, Z, and Y axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_XZY(const quat_t<T>& q)
{
    return mat3_to_euler_XZY(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in XYX order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the X, Y, and X axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_XYX(const quat_t<T>& q)
{
    return mat3_to_euler_XYX(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in XZX order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the X, Z, and X axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_XZX(const quat_t<T>& q)
{
    return mat3_to_euler_XZX(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in YXZ order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Y, X, and Z axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_YXZ(const quat_t<T>& q)
{
    return mat3_to_euler_YXZ(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in YZX order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Y, Z, and X axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_YZX(const quat_t<T>& q)
{
    return mat3_to_euler_YZX(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in YXY order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Y, X, and Y axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_YXY(const quat_t<T>& q)
{
    return mat3_to_euler_YXY(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in YZY order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Y, Z, and Y axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_YZY(const quat_t<T>& q)
{
    return mat3_to_euler_YZY(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in ZXY order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Z, X, and Y axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_ZXY(const quat_t<T>& q)
{
    return mat3_to_euler_ZXY(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in ZYX order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Z, Y, and X axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_ZYX(const quat_t<T>& q)
{
    return mat3_to_euler_ZYX(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in ZXZ order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Z, X, and Z axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_ZXZ(const quat_t<T>& q)
{
    return mat3_to_euler_ZXZ(q.to_mat3());
}

/**
 * @brief Converts a quaternion to Euler angles in ZYZ order.
 *
 * This function takes a quaternion 'q' and computes the corresponding Euler angles (t1, t2, t3)
 * in radians representing rotations around the Z, Y, and Z axes, respectively.
 *
 * @param q The quaternion to convert.
 * @return A vector containing the Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> quat_to_euler_ZYZ(const quat_t<T>& q)
{
    return mat3_to_euler_ZYZ(q.to_mat3());
}

// =============== mat3 ===============

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

// mat3_from_euler

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in XYZ order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the X, Y, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_XYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),

        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),

        +(st2),
        -(st1 * ct2),
        +(ct1 * ct2)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in XZY order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the X, Z, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_XZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),

        -(st2),
        +(ct1 * ct2),
        +(st1 * ct2),

        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in XYX order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the X, Y, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_XYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct2),
        +(st1 * st2),
        -(ct1 * st2),

        +(st2 * st3),
        +(ct1 * ct3) - (st1 * ct2 * st3),
        +(st1 * ct3) + (ct1 * ct2 * st3),

        +(st2 * ct3),
        -(ct1 * st3) - (st1 * ct2 * ct3),
        -(st1 * st3) + (ct1 * ct2 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in XZX order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the X, Z, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_XZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct2),
        +(ct1 * st2),
        +(st1 * st2),

        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),

        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in YXZ order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Y, X, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_YXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),

        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),

        +(st1 * ct2),
        -(st2),
        +(ct1 * ct2)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in YZX order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Y, Z, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_YZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct1 * ct2),
        +(st2),
        -(st1 * ct2),

        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),

        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in YXY order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Y, X, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_YXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),

        +(st1 * st2),
        +(ct2),
        +(ct1 * st2),

        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in YZY order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Y, Z, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_YZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        -(st1 * ct2 * ct3) - (ct1 * st3),

        -(ct1 * st2),
        +(ct2),
        +(st1 * st2),

        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        -(st1 * ct2 * st3) + (ct1 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in ZXY order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Z, X, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_ZXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),

        -(st1 * ct2),
        +(ct1 * ct2),
        +(st2),

        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in ZYX order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Z, Y, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_ZYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct1 * ct2),
        +(st1 * ct2),
        -(st2),

        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),

        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in ZXZ order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Z, X, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),

        -(st1 * ct2 * ct3) - (ct1 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),

        +(st1 * st2),
        -(ct1 * st2),
        +(ct2)
    );
}

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles in ZYZ order.
 *
 * This function constructs a 3x3 rotation matrix representing rotations around the Z, Y, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<3, 3, T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),

        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),

        +(ct1 * st2),
        +(st1 * st2),
        +(ct2)
    );
}

// mat3_to_euler

/**
 * @brief Extracts Euler angles in XYZ order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in XYZ order. The resulting angles represent rotations around the X, Y, and Z axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_XYZ(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[1].x * m.columns[1].x);
    const T t2 = math::atan2(-m.columns[2].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].z - c1 * m.columns[0].y, c1 * m.columns[1].y - s1 * m.columns[1].z);

    return vec<3, T>(
        -t1,
        -t2,
        -t3
    );
}

/**
 * @brief Extracts Euler angles in XZY order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in XZY order. The resulting angles represent rotations around the X, Z, and Y axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_XZY(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(-m.columns[1].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].y - c1 * m.columns[0].z, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in XYX order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in XYX order. The resulting angles represent rotations around the X, Y, and X axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_XYX(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, -m.columns[0].z);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[2].y - s1 * m.columns[2].z, c1 * m.columns[1].y + s1 * m.columns[1].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in XZX order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in XZX order. The resulting angles represent rotations around the X, Z, and X axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_XZX(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[0].z, m.columns[0].y);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[1].z - s1 * m.columns[1].y, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YXZ order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in YXZ order. The resulting angles represent rotations around the Y, X, and Z axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_YXZ(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[1].y * m.columns[1].y);
    const T t2 = math::atan2(-m.columns[2].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].z - c1 * m.columns[1].x, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YZX order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in YZX order. The resulting angles represent rotations around the Y, Z, and X axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_YZX(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(-m.columns[0].z, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].y * m.columns[1].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(m.columns[0].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].x + c1 * m.columns[1].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YXY order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in YXY order. The resulting angles represent rotations around the Y, X, and Y axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_YXY(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[1].x, m.columns[1].z);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x - s1 * m.columns[2].z, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YZY order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in YZY order. The resulting angles represent rotations around the Y, Z, and Y axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_YZY(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, -m.columns[1].x);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-s1 * m.columns[0].x - c1 * m.columns[0].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZXY order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in ZXY order. The resulting angles represent rotations around the Z, X, and Y axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_ZXY(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(-m.columns[1].x, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(m.columns[1].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x + s1 * m.columns[2].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZYX order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in ZYX order. The resulting angles represent rotations around the Z, Y, and X axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_ZYX(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].z * m.columns[1].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(-m.columns[0].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[2].x - c1 * m.columns[2].y, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZXZ order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in ZXZ order. The resulting angles represent rotations around the Z, X, and Z axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_ZXZ(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, -m.columns[2].y);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[1].x - s1 * m.columns[1].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZYZ order from a 3x3 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 3x3 rotation matrix 'm'
 * in ZYZ order. The resulting angles represent rotations around the Z, Y, and Z axes.
 *
 * @param m The 3x3 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_to_euler_ZYZ(const mat<3, 3, T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].x);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[0].y - s1 * m.columns[0].x, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

// =============== mat4 ===============

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

// mat4_from_euler

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in XYZ order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the X, Y, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_XYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        static_cast<T>(0),

        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        static_cast<T>(0),

        +(st2),
        -(st1 * ct2),
        +(ct1 * ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in XZY order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the X, Z, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_XZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        static_cast<T>(0),

        -(st2),
        +(ct1 * ct2),
        +(st1 * ct2),
        static_cast<T>(0),

        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in XYX order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the X, Y, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_XYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct2),
        +(st1 * st2),
        -(ct1 * st2),
        static_cast<T>(0),

        +(st2 * st3),
        +(ct1 * ct3) - (st1 * ct2 * st3),
        +(st1 * ct3) + (ct1 * ct2 * st3),
        static_cast<T>(0),

        +(st2 * ct3),
        -(ct1 * st3) - (st1 * ct2 * ct3),
        -(st1 * st3) + (ct1 * ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in XZX order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the X, Z, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the X-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_XZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct2),
        +(ct1 * st2),
        +(st1 * st2),
        static_cast<T>(0),

        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        static_cast<T>(0),

        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in YXZ order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Y, X, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_YXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        +(ct1 * st2 * st3) - (st1 * ct3),
        static_cast<T>(0),

        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        +(ct1 * st2 * ct3) + (st1 * st3),
        static_cast<T>(0),

        +(st1 * ct2),
        -(st2),
        +(ct1 * ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in YZX order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Y, Z, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_YZX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct1 * ct2),
        +(st2),
        -(st1 * ct2),
        static_cast<T>(0),

        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        +(st1 * st2 * ct3) + (ct1 * st3),
        static_cast<T>(0),

        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        -(st1 * st2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in YXY order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Y, X, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_YXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        -(ct1 * ct2 * st3) - (st1 * ct3),
        static_cast<T>(0),

        +(st1 * st2),
        +(ct2),
        +(ct1 * st2),
        static_cast<T>(0),

        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in YZY order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Y, Z, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Y-axis.
 * @param t2 The rotation angle around the Z-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_YZY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        -(st1 * ct2 * ct3) - (ct1 * st3),
        static_cast<T>(0),

        -(ct1 * st2),
        +(ct2),
        +(st1 * st2),
        static_cast<T>(0),

        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in ZXY order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Z, X, and Y axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Y-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_ZXY(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        -(st1 * st2 * st3) + (ct1 * ct3),
        +(ct1 * st2 * st3) + (st1 * ct3),
        -(ct2 * st3),
        static_cast<T>(0),

        -(st1 * ct2),
        +(ct1 * ct2),
        +(st2),
        static_cast<T>(0),

        +(st1 * st2 * ct3) + (ct1 * st3),
        -(ct1 * st2 * ct3) + (st1 * st3),
        +(ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in ZYX order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Z, Y, and X axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the X-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_ZYX(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct1 * ct2),
        +(st1 * ct2),
        -(st2),
        static_cast<T>(0),

        +(ct1 * st2 * st3) - (st1 * ct3),
        +(st1 * st2 * st3) + (ct1 * ct3),
        +(ct2 * st3),
        static_cast<T>(0),

        +(ct1 * st2 * ct3) + (st1 * st3),
        +(st1 * st2 * ct3) - (ct1 * st3),
        +(ct2 * ct3),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in ZXZ order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Z, X, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the X-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_ZXZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(ct1 * ct2 * st3) + (st1 * ct3),
        +(st2 * st3),
        static_cast<T>(0),

        -(st1 * ct2 * ct3) - (ct1 * ct3),
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st2 * ct3),
        static_cast<T>(0),

        +(st1 * st2),
        -(ct1 * st2),
        +(ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix from Euler angles in ZYZ order.
 *
 * This function constructs a 4x4 rotation matrix representing rotations around the Z, Y, and Z axes,
 * using the provided Euler angles (t1, t2, t3) in radians.
 *
 * @param t1 The rotation angle around the Z-axis.
 * @param t2 The rotation angle around the Y-axis.
 * @param t3 The rotation angle around the Z-axis.
 *
 * @return The 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> mat4_from_euler_ZYZ(T t1, T t2, T t3)
{
    const T ct1 = math::cos(t1);
    const T ct2 = math::cos(t2);
    const T ct3 = math::cos(t3);

    const T st1 = math::sin(t1);
    const T st2 = math::sin(t2);
    const T st3 = math::sin(t3);

    return mat<4, 4, T>(
        +(ct1 * ct2 * ct3) - (st1 * st3),
        +(st1 * ct2 * ct3) + (ct1 * st3),
        -(st2 * ct3),
        static_cast<T>(0),

        -(ct1 * ct2 * st3) - (st1 * ct3),
        -(st1 * ct2 * st3) + (ct1 * ct3),
        +(st2 * st3),
        static_cast<T>(0),

        +(ct1 * st2),
        +(st1 * st2),
        +(ct2),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

// mat4_to_euler

/**
 * @brief Extracts Euler angles in XYZ order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in XYZ order. The resulting angles represent rotations around the X, Y, and Z axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_XYZ(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[1].x * m.columns[1].x);
    const T t2 = math::atan2(-m.columns[2].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].z - c1 * m.columns[0].y, c1 * m.columns[1].y - s1 * m.columns[1].z);

    return vec<3, T>(
        -t1,
        -t2,
        -t3
    );
}

/**
 * @brief Extracts Euler angles in XZY order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in XZY order. The resulting angles represent rotations around the X, Z, and Y axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_XZY(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].x * m.columns[0].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(-m.columns[1].x, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[0].y - c1 * m.columns[0].z, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in XYX order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in XYX order. The resulting angles represent rotations around the X, Y, and X axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_XYX(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, -m.columns[0].z);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[2].y - s1 * m.columns[2].z, c1 * m.columns[1].y + s1 * m.columns[1].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in XZX order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in XZX order. The resulting angles represent rotations around the X, Z, and X axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_XZX(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[0].z, m.columns[0].y);
    const T s2 = math::sqrt(m.columns[1].x * m.columns[1].x + m.columns[2].x * m.columns[2].x);
    const T t2 = math::atan2(s2, m.columns[0].x);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[1].z - s1 * m.columns[1].y, c1 * m.columns[2].z - s1 * m.columns[2].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YXZ order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in YXZ order. The resulting angles represent rotations around the Y, X, and Z axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_YXZ(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, m.columns[2].z);
    const T c2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[1].y * m.columns[1].y);
    const T t2 = math::atan2(-m.columns[2].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].z - c1 * m.columns[1].x, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YZX order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in YZX order. The resulting angles represent rotations around the Y, Z, and X axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_YZX(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(-m.columns[0].z, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].y * m.columns[1].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(m.columns[0].y, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[1].x + c1 * m.columns[1].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YXY order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in YXY order. The resulting angles represent rotations around the Y, X, and Y axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_YXY(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[1].x, m.columns[1].z);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x - s1 * m.columns[2].z, c1 * m.columns[0].x - s1 * m.columns[0].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in YZY order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in YZY order. The resulting angles represent rotations around the Y, Z, and Y axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_YZY(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[1].z, -m.columns[1].x);
    const T s2 = math::sqrt(m.columns[0].y * m.columns[0].y + m.columns[2].y * m.columns[2].y);
    const T t2 = math::atan2(s2, m.columns[1].y);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-s1 * m.columns[0].x - c1 * m.columns[0].z, s1 * m.columns[2].x + c1 * m.columns[2].z);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZXY order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in ZXY order. The resulting angles represent rotations around the Z, X, and Y axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_ZXY(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(-m.columns[1].x, m.columns[1].y);
    const T c2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(m.columns[1].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[2].x + s1 * m.columns[2].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZYX order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in ZYX order. The resulting angles represent rotations around the Z, Y, and X axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_ZYX(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[0].y, m.columns[0].x);
    const T c2 = math::sqrt(m.columns[1].z * m.columns[1].z + m.columns[2].z * m.columns[2].z);
    const T t2 = math::atan2(-m.columns[0].z, c2);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(s1 * m.columns[2].x - c1 * m.columns[2].y, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZXZ order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in ZXZ order. The resulting angles represent rotations around the Z, X, and Z axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_ZXZ(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[2].x, -m.columns[2].y);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(-c1 * m.columns[1].x - s1 * m.columns[1].y, c1 * m.columns[0].x + s1 * m.columns[0].y);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

/**
 * @brief Extracts Euler angles in ZYZ order from a 4x4 rotation matrix.
 *
 * This function calculates the Euler angles (t1, t2, t3) in radians from the given 4x4 rotation matrix 'm'
 * in ZYZ order. The resulting angles represent rotations around the Z, Y, and Z axes.
 *
 * @param m The 4x4 rotation matrix.
 * @return The Euler angles (t1, t2, t3) in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat4_to_euler_ZYZ(const mat<4, 4, T>& m)
{
    const T t1 = math::atan2(m.columns[2].y, m.columns[2].x);
    const T s2 = math::sqrt(m.columns[0].z * m.columns[0].z + m.columns[1].z * m.columns[1].z);
    const T t2 = math::atan2(s2, m.columns[2].z);
    const T s1 = math::sin(t1);
    const T c1 = math::cos(t1);
    const T t3 = math::atan2(c1 * m.columns[0].y - s1 * m.columns[0].x, c1 * m.columns[1].y - s1 * m.columns[1].x);

    return vec<3, T>(
        t1,
        t2,
        t3
    );
}

}
}
}
}