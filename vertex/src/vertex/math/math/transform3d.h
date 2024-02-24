#pragma once

#include "../math.h"

namespace vx {
namespace math {

// =============== mat3 pure rotation ===============

// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

/**
 * @brief Creates a 3x3 rotation matrix from an axis and an angle.
 *
 * This function generates a 3x3 rotation matrix representing a rotation around the specified axis by the given angle.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return A 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_axis_angle(const vec<3, T>& axis, T angle)
{
    const vec<3, T> naxis = math::normalize(axis);

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);
    const vec<3, T> t = (static_cast<T>(1) - cosa) * naxis;
    const vec<3, T> sina_naxis = sina * naxis;

    return mat<3, 3, T>(
        (t.x * naxis.x) + cosa,
        (t.x * naxis.y) + sina_naxis.z,
        (t.x * naxis.z) - sina_naxis.y,

        (t.y * naxis.x) - sina_naxis.z,
        (t.y * naxis.y) + cosa,
        (t.y * naxis.z) + sina_naxis.x,

        (t.z * naxis.x) + sina_naxis.y,
        (t.z * naxis.y) - sina_naxis.x,
        (t.z * naxis.z) + cosa
    );
}

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm

/**
 * @brief Creates a 3x3 rotation matrix from a quaternion.
 *
 * This function generates a 3x3 rotation matrix from the given quaternion.
 *
 * @tparam T The type of elements in the matrix and quaternion.
 * @param rotation The quaternion representing the rotation.
 * @return A 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_quat(const quat_t<T>& rotation)
{
    const quat_t<T> qn = math::normalize(rotation);

    const T qxx = qn.x * qn.x;
    const T qyy = qn.y * qn.y;
    const T qzz = qn.z * qn.z;
    const T qxz = qn.x * qn.z;
    const T qxy = qn.x * qn.y;
    const T qyz = qn.y * qn.z;
    const T qwx = qn.w * qn.x;
    const T qwy = qn.w * qn.y;
    const T qwz = qn.w * qn.z;

    return mat<3, 3, T>(
        static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz),
        static_cast<T>(2) * (qxy + qwz),
        static_cast<T>(2) * (qxz - qwy),

        static_cast<T>(2) * (qxy - qwz),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz),
        static_cast<T>(2) * (qyz + qwx),

        static_cast<T>(2) * (qxz + qwy),
        static_cast<T>(2) * (qyz - qwx),
        static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy)
    );
}

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

/**
 * @brief Creates a 3x3 rotation matrix from Euler angles.
 *
 * This function generates a 3x3 rotation matrix from the given Euler angles (in radians).
 * The rotation order is assumed to be X-Y-Z (roll, pitch, yaw).
 *
 * @tparam T The type of elements in the matrix.
 * @param x The angle of rotation around the X-axis (roll).
 * @param y The angle of rotation around the Y-axis (pitch).
 * @param z The angle of rotation around the Z-axis (yaw).
 * @return A 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_from_euler_angles(T x, T y, T z)
{
    const T ct1 = math::cos(x);
    const T ct2 = math::cos(y);
    const T ct3 = math::cos(z);

    const T st1 = math::sin(x);
    const T st2 = math::sin(y);
    const T st3 = math::sin(z);

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

// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

/**
 * @brief Converts a 3x3 rotation matrix to a quaternion.
 *
 * This function converts the given 3x3 rotation matrix to a quaternion.
 *
 * @tparam T The type of elements in the matrix and quaternion.
 * @param m The 3x3 rotation matrix.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> mat3_to_quat(const mat<3, 3, T>& m)
{
    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            s * static_cast<T>(0.25),
            (m.columns[2].y - m.columns[1].z) * invs,
            (m.columns[0].z - m.columns[2].x) * invs,
            (m.columns[1].x - m.columns[0].y) * invs
        );
    }
    else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].y - m.columns[1].z) * invs,
            s * static_cast<T>(0.25),
            (m.columns[0].y + m.columns[1].x) * invs,
            (m.columns[0].z + m.columns[2].x) * invs
        );
    }
    else if (m.columns[1].y > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].x - m.columns[0].z) * invs,
            (m.columns[0].y + m.columns[1].x) * invs,
            s * static_cast<T>(0.25),
            (m.columns[1].z + m.columns[2].y) * invs
        );
    }
    else
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[1].x - m.columns[0].y) * invs,
            (m.columns[0].z + m.columns[2].x) * invs,
            (m.columns[1].z + m.columns[2].y) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

// =============== mat3 pure scale ===============

/**
 * @brief Creates a 3x3 scaling matrix.
 *
 * @tparam T The type of elements in the matrix.
 * @param scale A 3D vector representing the scaling factors along the x, y, and z axes.
 * @return A 3x3 scaling matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_scale(const vec<3, T>& scale)
{
    return mat<3, 3, T>(
        scale.x,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        scale.y,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        scale.z
    );
}

/**
 * @brief Retrieves the scaling factors from a 3x3 scaling matrix.
 *
 * This function calculates and returns the scaling factors represented by
 * the given 3x3 scaling matrix.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 3x3 scaling matrix.
 * @return A 3D vector representing the scaling factors along the x, y, and z axes.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> mat3_get_scale(const mat<3, 3, T>& m)
{
    return vec<3, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1]),
        math::length(m.columns[2])
    );
}

// =============== look at ===============

// https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

/**
 * @brief Creates a left-handed 3x3 view matrix for a look-at transformation.
 *
 * This function generates a left-handed 3x3 view matrix for a look-at transformation,
 * given the eye position, target position, and up vector.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position the camera is looking at.
 * @param up The up vector of the camera  (default is the positive y-axis).
 * @return A left-handed 3x3 view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_look_at_lh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    const vec<3, T> z = math::normalize(target - eye);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<3, 3, T>(x, y, z);
}

/**
 * @brief Creates a right-handed 3x3 view matrix for a look-at transformation.
 *
 * This function generates a right-handed 3x3 view matrix for a look-at transformation,
 * given the eye position, target position, and up vector.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position the camera is looking at.
 * @param up The up vector of the camera  (default is the positive y-axis).
 * @return A right-handed 3x3 view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_look_at_rh(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    const vec<3, T> z = math::normalize(eye - target);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<3, 3, T>(x, y, z);
}

/**
 * @brief Creates a 3x3 view matrix for a look-at transformation.
 *
 * This function generates a 3x3 view matrix for a look-at transformation,
 * given the eye position, target position, and up vector.
 *
 * The handedness of the matrix (left-handed or right-handed) is determined
 * based on the configuration specified in VX_CONFIG_CLIP_CONTROL.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position the camera is looking at.
 * @param up The up vector of the camera  (default is the positive y-axis).
 * @return A 3x3 view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_look_at(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return make_look_at_lh(eye, target, up);
#else
    return make_look_at_rh(eye, target, up);
#endif
}

// =============== 3d translation ===============

/**
 * @brief Creates a 4x4 translation matrix for 3D space.
 *
 * This function generates a 4x4 translation matrix for translating points in 3D space.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param translation A 3D vector representing the translation along the x, y, and z axes.
 * @return A 4x4 translation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_translation_3d(const vec<3, T>& translation)
{
    return mat<4, 4, T>(
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        translation.x,     translation.y,     translation.z,     static_cast<T>(1)
    );
}

/**
 * @brief Retrieves the translation from a 4x4 transformation matrix in 3D space.
 *
 * This function extracts and returns the translation component represented by
 * the last column of the given 4x4 transformation matrix.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 4x4 transformation matrix.
 * @return A 3D vector representing the translation along the x, y, and z axes.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> get_translation_3d(const mat<4, 4, T>& m)
{
    return vec<3, T>(m.columns[3].x, m.columns[3].y, m.columns[3].z);
}

// =============== 3d rotation ===============

/**
 * @brief Creates a 4x4 rotation matrix for 3D space around an axis.
 *
 * This function generates a 4x4 rotation matrix representing a rotation around
 * the specified axis by the given angle.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return A 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_rotation_3d(const vec<3, T>& axis, T angle)
{
    const vec<3, T> naxis = math::normalize(axis);

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);
    const vec<3, T> t = (static_cast<T>(1) - cosa) * naxis;
    const vec<3, T> sina_naxis = sina * naxis;

    return mat<4, 4, T>(
        (t.x * naxis.x) + cosa,
        (t.x * naxis.y) + sina_naxis.z,
        (t.x * naxis.z) - sina_naxis.y,
        static_cast<T>(0),

        (t.y * naxis.x) - sina_naxis.z,
        (t.y * naxis.y) + cosa,
        (t.y * naxis.z) + sina_naxis.x,
        static_cast<T>(0),

        (t.z * naxis.x) + sina_naxis.y,
        (t.z * naxis.y) - sina_naxis.x,
        (t.z * naxis.z) + cosa,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 rotation matrix for 3D space from a quaternion.
 *
 * This function generates a 4x4 rotation matrix from the given quaternion.
 *
 * @tparam T The type of elements in the matrix and quaternion.
 * @param rotation The quaternion representing the rotation.
 * @return A 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_rotation_3d(const quat_t<T>& rotation)
{
    const quat_t<T> qn = math::normalize(rotation);

    const T qxx = qn.x * qn.x;
    const T qyy = qn.y * qn.y;
    const T qzz = qn.z * qn.z;
    const T qxz = qn.x * qn.z;
    const T qxy = qn.x * qn.y;
    const T qyz = qn.y * qn.z;
    const T qwx = qn.w * qn.x;
    const T qwy = qn.w * qn.y;
    const T qwz = qn.w * qn.z;

    return mat<4, 4, T>(
        static_cast<T>(1) - (static_cast<T>(2) * (qyy + qzz)),
        static_cast<T>(2) * (qxy + qwz),
        static_cast<T>(2) * (qxz - qwy),
        static_cast<T>(0),

        static_cast<T>(2) * (qxy - qwz),
        static_cast<T>(1) - (static_cast<T>(2) * (qxx + qzz)),
        static_cast<T>(2) * (qyz + qwx),
        static_cast<T>(0),

        static_cast<T>(2) * (qxz + qwy),
        static_cast<T>(2) * (qyz - qwx),
        static_cast<T>(1) - (static_cast<T>(2) * (qxx + qyy)),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

// https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

/**
 * @brief Creates a 4x4 rotation matrix for 3D space from Euler angles.
 *
 * This function generates a 4x4 rotation matrix from the given Euler angles
 * representing rotations around the x, y, and z axes.
 *
 * @tparam T The type of elements in the matrix.
 * @param x The rotation angle around the x-axis in radians.
 * @param y The rotation angle around the y-axis in radians.
 * @param z The rotation angle around the z-axis in radians.
 * @return A 4x4 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_rotation_3d(T x, T y, T z)
{
    const T ct1 = math::cos(x);
    const T ct2 = math::cos(y);
    const T ct3 = math::cos(z);

    const T st1 = math::sin(x);
    const T st2 = math::sin(y);
    const T st3 = math::sin(z);

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
 * @brief Retrieves the rotation quaternion from a 4x4 transformation matrix in 3D space.
 *
 * This function extracts and returns the rotation component from the given 4x4 transformation matrix.
 *
 * @tparam T The type of elements in the matrix and quaternion.
 * @param m The 4x4 transformation matrix.
 * @return The quaternion representing the rotation.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> get_rotation_3d(const mat<4, 4, T>& m)
{
    const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

    if (trace > static_cast<T>(0))
    {
        const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            s * static_cast<T>(0.25),
            (m.columns[2].y - m.columns[1].z) * invs,
            (m.columns[0].z - m.columns[2].x) * invs,
            (m.columns[1].x - m.columns[0].y) * invs
        );
    }
    else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].y - m.columns[1].z) * invs,
            s * static_cast<T>(0.25),
            (m.columns[0].y + m.columns[1].x) * invs,
            (m.columns[0].z + m.columns[2].x) * invs
        );
    }
    else if (m.columns[1].y > m.columns[2].z)
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[2].x - m.columns[0].z) * invs,
            (m.columns[0].y + m.columns[1].x) * invs,
            s * static_cast<T>(0.25),
            (m.columns[1].z + m.columns[2].y) * invs
        );
    }
    else
    {
        const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
        const T invs = static_cast<T>(1) / s;

        return quat_t<T>(
            (m.columns[1].x - m.columns[0].y) * invs,
            (m.columns[0].z + m.columns[2].x) * invs,
            (m.columns[1].z + m.columns[2].y) * invs,
            s * static_cast<T>(0.25)
        );
    }
}

// =============== 3d scale ===============

/**
 * @brief Creates a 4x4 scale matrix for 3D space.
 *
 * This function generates a 4x4 scale matrix for scaling objects in 3D space.
 *
 * @tparam T The type of elements in the matrix.
 * @param scale A 3D vector representing the scaling factors along the x, y, and z axes.
 * @return A 4x4 scale matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_scale_3d(const vec<3, T>& scale)
{
    return mat<4, 4, T>(
        scale.x,
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        scale.y,
        static_cast<T>(0),
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        scale.z,
        static_cast<T>(0),

        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(0),
        static_cast<T>(1)
    );
}

/**
 * @brief Retrieves the scale vector from a 4x4 transformation matrix in 3D space.
 *
 * This function extracts and returns the scaling component from the given 4x4 transformation matrix.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param m The 4x4 transformation matrix.
 * @return A 3D vector representing the scaling factors along the x, y, and z axes.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> get_scale_3d(const mat<4, 4, T>& m)
{
    return vec<3, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1]),
        math::length(m.columns[2])
    );
}

// =============== trs ===============

/**
 * @brief Creates a 4x4 transformation matrix for translation, rotation, and scaling in 3D space.
 *
 * This function generates a 4x4 transformation matrix combining translation, rotation, and scaling in 3D space.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param translation A 3D vector representing the translation along the x, y, and z axes.
 * @param axis A 3D vector representing the axis of rotation.
 * @param angle The rotation angle in radians.
 * @param scale A 3D vector representing the scaling factors along the x, y, and z axes.
 * @return A 4x4 transformation matrix representing the combined translation, rotation, and scaling.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_trs_3d(
    const vec<3, T>& translation,
    const vec<3, T>& axis, T angle,
    const vec<3, T>& scale
)
{
    const vec<3, T> naxis = math::normalize(axis);

    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);
    const vec<3, T> t = (static_cast<T>(1) - cosa) * naxis;
    const vec<3, T> sina_naxis = sina * naxis;

    return mat<4, 4, T>(
        scale.x * ((t.x * naxis.x) + cosa),
        scale.x * ((t.x * naxis.y) + sina_naxis.z),
        scale.x * ((t.x * naxis.z) - sina_naxis.y),
        static_cast<T>(0),

        scale.y * ((t.y * naxis.x) - sina_naxis.z),
        scale.y * ((t.y * naxis.y) + cosa),
        scale.y * ((t.y * naxis.z) + sina_naxis.x),
        static_cast<T>(0),

        scale.z * ((t.z * naxis.x) + sina_naxis.y),
        scale.z * ((t.z * naxis.y) - sina_naxis.x),
        scale.z * ((t.z * naxis.z) + cosa),
        static_cast<T>(0),

        translation.x,
        translation.y,
        translation.z,
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a 4x4 transformation matrix for translation, rotation, and scaling in 3D space.
 *
 * This function generates a 4x4 transformation matrix combining translation, rotation, and scaling in 3D space.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param translation A 3D vector representing the translation along the x, y, and z axes.
 * @param rotation A quaternion representing the rotation.
 * @param scale A 3D vector representing the scaling factors along the x, y, and z axes.
 * @return A 4x4 transformation matrix representing the combined translation, rotation, and scaling.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> make_trs_3d(
    const vec<3, T>& translation,
    const quat_t<T>& rotation,
    const vec<3, T>& scale
)
{
    const quat_t<T> qn = math::normalize(rotation);

    const T qxx = qn.x * qn.x;
    const T qyy = qn.y * qn.y;
    const T qzz = qn.z * qn.z;
    const T qxz = qn.x * qn.z;
    const T qxy = qn.x * qn.y;
    const T qyz = qn.y * qn.z;
    const T qwx = qn.w * qn.x;
    const T qwy = qn.w * qn.y;
    const T qwz = qn.w * qn.z;

    return mat<4, 4, T>(
        scale.x * (static_cast<T>(1) - (static_cast<T>(2) * (qyy + qzz))),
        scale.x * (static_cast<T>(2) * (qxy + qwz)),
        scale.x * (static_cast<T>(2) * (qxz - qwy)),
        static_cast<T>(0),

        scale.y * (static_cast<T>(2) * (qxy - qwz)),
        scale.y * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qzz))),
        scale.y * (static_cast<T>(2) * (qyz + qwx)),
        static_cast<T>(0),

        scale.z * (static_cast<T>(2) * (qxz + qwy)),
        scale.z * (static_cast<T>(2) * (qyz - qwx)),
        scale.z * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qyy))),
        static_cast<T>(0),

        translation.x,
        translation.y,
        translation.z,
        static_cast<T>(1)
    );
}

// =============== invert ===============

/**
 * @brief Calculates the inverse of an affine transformation matrix.
 *
 * This function computes the inverse of the given 4x4 affine transformation matrix.
 * The inverse of an affine transformation matrix undoes the transformation, effectively
 * reversing the original transformation.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 4x4 affine transformation matrix.
 * @return The inverse of the matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> affine_invert(const mat<4, 4, T>& m)
{
    const mat<3, 3, T> ibasis = math::invert(mat<3, 3, T>(m));

    return mat<4, 4, T>(
        vec<4, T>(ibasis.columns[0], static_cast<T>(0)),
        vec<4, T>(ibasis.columns[1], static_cast<T>(0)),
        vec<4, T>(ibasis.columns[2], static_cast<T>(0)),
        vec<4, T>(ibasis * vec<3, T>(-m.columns[3]), static_cast<T>(1))
    );
}

// =============== transform ===============

/**
 * @brief Transforms a 3D vector using a 4x4 transformation matrix.
 *
 * This function applies the given 4x4 transformation matrix to a 3D vector.
 * The transformation matrix represents a combination of translation, rotation, and scaling.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param m The 4x4 transformation matrix.
 * @param v The 3D vector to be transformed.
 * @return The transformed 3D vector.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> transform(const mat<4, 4, T>& m, const vec<3, T>& v)
{
    return vec<3, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + m.columns[3].x,
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + m.columns[3].y,
        (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + m.columns[3].z
    );
}

// =============== look at ===============

// https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
// https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh

// https://www.3dgep.com/understanding-the-view-matrix/
// https://webglfundamentals.org/webgl/lessons/webgl-3d-camera.html
// https://stackoverflow.com/questions/31040378/whats-the-difference-between-these-two-ways-of-computing-a-lookat-matrix

/**
 * @brief Creates a left-handed view matrix using the look-at transformation.
 *
 * This function creates a left-handed view matrix using the look-at transformation.
 * It transforms a set of objects from world space to view space. The resulting matrix
 * represents the transformation from world space to a camera's view space.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction in world space (default is the positive y-axis).
 * @return The left-handed view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_look_at_lh_3d(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    const vec<3, T> z = math::normalize(target - eye);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<4, 4, T>(
        x.x,
        y.x,
        z.x,
        static_cast<T>(0),

        x.y,
        y.y,
        z.y,
        static_cast<T>(0),

        x.z,
        y.z,
        z.z,
        static_cast<T>(0),

        -math::dot(x, eye),
        -math::dot(y, eye),
        -math::dot(z, eye),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a right-handed view matrix using the look-at transformation.
 *
 * This function creates a right-handed view matrix using the look-at transformation.
 * It transforms a set of objects from world space to view space. The resulting matrix
 * represents the transformation from world space to a camera's view space.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction in world space (default is the positive y-axis).
 * @return The right-handed view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> make_look_at_rh_3d(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
    // In a right handed coordinate system, the camera is looking down the
    // -z axis, so to get a vector from the front of the camera to the
    // target, we reverse the subtraction.
    const vec<3, T> z = math::normalize(eye - target);
    const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
    const vec<3, T> y = math::cross(z, x);

    return mat<4, 4, T>(
        x.x,
        y.x,
        z.x,
        static_cast<T>(0),

        x.y,
        y.y,
        z.y,
        static_cast<T>(0),

        x.z,
        y.z,
        z.z,
        static_cast<T>(0),

        -math::dot(x, eye),
        -math::dot(y, eye),
        +math::dot(z, eye),
        static_cast<T>(1)
    );
}

/**
 * @brief Creates a view matrix using the look-at transformation.
 *
 * This function creates a view matrix using the look-at transformation. It automatically
 * selects between left-handed and right-handed coordinate systems based on the configuration
 * settings. If the left-handed coordinate system is enabled, it returns a left-handed view matrix
 * using the `make_look_at_lh_3d` function. Otherwise, it returns a right-handed view matrix
 * using the `make_look_at_rh_3d` function.
 *
 * @tparam T The type of elements in the matrix and vectors.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction in world space (default is the positive y-axis).
 * @return The view matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> make_look_at_3d(
    const vec<3, T>& eye,
    const vec<3, T>& target,
    const vec<3, T>& up = vec<3, T>::UP()
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return make_look_at_lh(eye, target, up);
#else
    return make_look_at_rh(eye, target, up);
#endif
}

}
}