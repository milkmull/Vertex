#pragma once

#include "../math.h"

namespace vx {
namespace math {

// =============== mat3 pure rotation ===============

// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

/**
 * @brief Creates a 3x3 rotation matrix around an arbitrary axis.
 *
 * This function constructs a 3x3 rotation matrix that rotates vectors
 * around the specified axis by the given angle.
 *
 * @tparam T The data type of the matrix elements.
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_rotation(const vec<3, T>& axis, T angle)
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
 * This function constructs a 3x3 rotation matrix that represents
 * the same rotation as the given quaternion.
 *
 * @tparam T The data type of the matrix elements.
 * @param rotation The quaternion representing the rotation.
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_rotation(const quat_t<T>& rotation)
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
 * This function constructs a 3x3 rotation matrix that represents
 * the rotation specified by the Euler angles (in radians) around
 * the X, Y, and Z axes respectively.
 *
 * @tparam T The data type of the matrix elements.
 * @param x The rotation angle around the X-axis.
 * @param y The rotation angle around the Y-axis.
 * @param z The rotation angle around the Z-axis.
 * @return The 3x3 rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> mat3_make_rotation(T x, T y, T z)
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
 * @brief Extracts the rotation quaternion from a 3x3 rotation matrix.
 *
 * This function extracts the rotation quaternion that represents the
 * rotation encoded in the given 3x3 rotation matrix.
 *
 * @tparam T The data type of the matrix elements and quaternion components.
 * @param m The 3x3 rotation matrix.
 * @return The rotation quaternion.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr quat_t<T> mat3_get_rotation(const mat<3, 3, T>& m)
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
 * @brief Creates a 3x3 scale matrix from the specified scale vector.
 *
 * This function creates a 3x3 scale matrix using the provided scale vector.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param scale The scale vector specifying the scaling factors along each axis.
 * @return The resulting scale matrix.
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
 * @brief Extracts the scale vector from the specified 3x3 matrix.
 *
 * This function extracts the scale vector from the specified 3x3 matrix.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param m The 3x3 matrix from which to extract the scale.
 * @return The scale vector extracted from the matrix.
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
 * @brief Constructs a left-handed look-at view matrix.
 *
 * This function constructs a left-handed look-at view matrix given the eye position,
 * target position, and up direction.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction.
 * @return The left-handed look-at view matrix.
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
 * @brief Constructs a right-handed look-at view matrix.
 *
 * This function constructs a right-handed look-at view matrix given the eye position,
 * target position, and up direction.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction.
 * @return The right-handed look-at view matrix.
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
 * @brief Constructs a look-at view matrix based on the specified clip control configuration.
 *
 * This function constructs either a left-handed or right-handed look-at view matrix based
 * on the specified clip control configuration. If the clip control is set to left-handed,
 * it constructs a left-handed view matrix; otherwise, it constructs a right-handed view matrix.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param eye The position of the camera.
 * @param target The position to look at.
 * @param up The up direction.
 * @return The look-at view matrix.
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
 * @brief Constructs a 3D translation matrix.
 *
 * This function constructs a 3D translation matrix given a translation vector
 * representing the translation along the x, y, and z axes.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param translation The translation vector.
 * @return The 3D translation matrix.
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
 * @brief Retrieves the translation vector from a 3D transformation matrix.
 *
 * This function extracts the translation component from a 3D transformation matrix
 * and returns it as a 3D vector.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param m The 3D transformation matrix.
 * @return The translation vector extracted from the matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> get_translation_3d(const mat<4, 4, T>& m)
{
    return vec<3, T>(m.columns[3].x, m.columns[3].y, m.columns[3].z);
}

// =============== 3d rotation ===============

/**
 * @brief Creates a 3D rotation matrix around an arbitrary axis.
 *
 * This function constructs a 3D rotation matrix representing a rotation
 * around an arbitrary axis in 3D space by the specified angle.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param axis The axis of rotation.
 * @param angle The angle of rotation in radians.
 * @return The 3D rotation matrix.
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
 * @brief Creates a 3D rotation matrix from a quaternion.
 *
 * This function constructs a 3D rotation matrix representing the rotation
 * described by the given quaternion.
 *
 * @tparam T The data type of the matrix elements and quaternion components.
 * @param rotation The quaternion representing the rotation.
 * @return The 3D rotation matrix.
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
 * @brief Creates a 3D rotation matrix from rotation angles around the x, y, and z axes.
 *
 * This function constructs a 3D rotation matrix representing the rotation
 * described by the given rotation angles around the x, y, and z axes.
 *
 * @tparam T The data type of the matrix elements and rotation angles.
 * @param x The rotation angle around the x-axis, in radians.
 * @param y The rotation angle around the y-axis, in radians.
 * @param z The rotation angle around the z-axis, in radians.
 * @return The 3D rotation matrix.
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
 * @brief Extracts the rotation quaternion from a 3D transformation matrix.
 *
 * This function extracts the rotation part from the given 3D transformation matrix
 * and returns it as a unit quaternion representing the rotation component.
 *
 * @tparam T The data type of the matrix elements and quaternion components.
 * @param m The 3D transformation matrix.
 * @return The rotation quaternion extracted from the transformation matrix.
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
 * @brief Creates a 3D scaling transformation matrix.
 *
 * This function generates a 3D transformation matrix that represents
 * a scaling transformation in 3D space along each axis by the given scaling factors.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param scale The scaling factors along each axis.
 * @return The 3D scaling transformation matrix.
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
 * @brief Extracts the scaling factors from a 3D transformation matrix.
 *
 * This function extracts the scaling factors along each axis from a 4x4
 * transformation matrix representing a 3D scaling transformation.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param m The 3D transformation matrix.
 * @return The scaling factors along each axis.
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
 * @brief Creates a 3D transformation matrix from translation, rotation, and scaling components.
 *
 * This function constructs a 4x4 transformation matrix representing a 3D transformation
 * composed of translation, rotation, and scaling components.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param translation The translation component of the transformation.
 * @param axis The axis of rotation for the transformation.
 * @param angle The angle of rotation in radians.
 * @param scale The scaling component of the transformation.
 * @return The 3D transformation matrix.
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
 * @brief Creates a 3D transformation matrix from translation, rotation, and scaling components.
 *
 * This function constructs a 4x4 transformation matrix representing a 3D transformation
 * composed of translation, rotation, and scaling components.
 *
 * @tparam T The data type of the matrix elements and vector components.
 * @param translation The translation component of the transformation.
 * @param rotation The rotation component of the transformation, represented as a quaternion.
 * @param scale The scaling component of the transformation.
 * @return The 3D transformation matrix.
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
 * @brief Computes the inverse of an affine transformation matrix.
 *
 * This function computes the inverse of a 4x4 affine transformation matrix.
 * Affine transformation matrices are used to represent translations, rotations,
 * scaling, and shearing transformations.
 *
 * @tparam T The data type of the matrix elements.
 * @param m The 4x4 affine transformation matrix to invert.
 * @return The inverse of the input affine transformation matrix.
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
 * @brief Transforms a 3D vector by a 4x4 transformation matrix.
 *
 * This function transforms a 3D vector by a 4x4 transformation matrix,
 * which represents translation, rotation, scaling, and perspective transformations.
 *
 * @tparam T The data type of the vector and matrix elements.
 * @param m The 4x4 transformation matrix.
 * @param v The 3D vector to transform.
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
 * @brief Creates a left-handed view matrix for 3D rendering.
 *
 * This function creates a left-handed view matrix for 3D rendering.
 *
 * @tparam T The data type of the matrix elements.
 * @param eye The position of the viewer's eye.
 * @param target The position where the viewer is looking at.
 * @param up The up direction in world space (optional, default is vec<3, T>::UP()).
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
 * @brief Creates a right-handed view matrix for 3D rendering.
 *
 * This function creates a right-handed view matrix for 3D rendering.
 *
 * @tparam T The data type of the matrix elements.
 * @param eye The position of the viewer's eye.
 * @param target The position where the viewer is looking at.
 * @param up The up direction in world space (optional, default is vec<3, T>::UP()).
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
 * @brief Creates a view matrix for 3D rendering.
 *
 * This function creates either a left-handed or right-handed view matrix
 * for 3D rendering based on the configuration.
 *
 * @tparam T The data type of the matrix elements.
 * @param eye The position of the viewer's eye.
 * @param target The position where the viewer is looking at.
 * @param up The up direction in world space (optional, default is vec<3, T>::UP()).
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