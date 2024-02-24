#pragma once

#include "../math.h"

namespace vx {
namespace math {

// =============== mat2 pure rotation ===============

/**
 * @brief Constructs a 2x2 rotation matrix.
 *
 * This function constructs a 2x2 rotation matrix given an angle.
 *
 * @tparam T Type of the matrix components.
 * @param angle The rotation angle in radians.
 * @return The rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<2, 2, T> mat2_make_rotation(T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<2, 2, T>(cosa, sina, -sina, cosa);
}

/**
 * @brief Retrieves the rotation angle from a 2x2 rotation matrix.
 *
 * This function extracts the rotation angle in radians from a 2x2 rotation matrix.
 *
 * @tparam T Type of the matrix components.
 * @param m The 2x2 rotation matrix.
 * @return The rotation angle in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T mat2_get_rotation(const mat<2, 2, T>& m)
{
    return math::atan2(m.columns[0].y, m.columns[0].x);
}

// =============== mat2 pure scale ===============

/**
 * @brief Creates a 2x2 scaling matrix.
 *
 * This function creates a 2x2 scaling matrix from the specified scale vector.
 *
 * @tparam T Type of the matrix components.
 * @param scale The scale vector.
 * @return The resulting 2x2 scaling matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<2, 2, T> mat2_make_scale(const vec<2, T>& scale)
{
    return mat<2, 2, T>(
        scale.x, static_cast<T>(0),
        static_cast<T>(0), scale.y
    );
}

/**
 * @brief Extracts the scale vector from a 2x2 matrix.
 *
 * This function extracts the scale vector from the specified 2x2 matrix.
 *
 * @tparam T Type of the matrix components.
 * @param m The input 2x2 matrix.
 * @return The scale vector extracted from the matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> mat2_get_scale(const mat<2, 2, T>& m)
{
    return vec<2, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1])
    );
}

// =============== 2d translation ===============

/**
 * @brief Creates a 2D translation matrix.
 *
 * This function creates a 3x3 matrix representing a 2D translation
 * transformation.
 *
 * @tparam T Type of the matrix components.
 * @param translation The translation vector.
 * @return The translation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_translation_2d(const vec<2, T>& translation)
{
    return mat<3, 3, T>(
        static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
        translation.x,     translation.y,     static_cast<T>(1)
    );
}

/**
 * @brief Extracts the translation vector from a 2D translation matrix.
 *
 * This function extracts the translation vector from a 3x3 matrix representing
 * a 2D translation transformation.
 *
 * @tparam T Type of the matrix components.
 * @param m The 2D translation matrix.
 * @return The translation vector.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> get_translation_2d(const mat<3, 3, T>& m)
{
    return vec<2, T>(m.columns[2].x, m.columns[2].y);
}

// =============== 2d rotation ===============

/**
 * @brief Creates a 2D rotation matrix.
 *
 * This function creates a 3x3 matrix representing a 2D rotation transformation.
 *
 * @tparam T Type of the matrix components.
 * @param angle The rotation angle in radians.
 * @return The 2D rotation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_rotation_2d(T angle)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<3, 3, T>(
        cosa,              sina,              static_cast<T>(0),
       -sina,              cosa,              static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

/**
 * @brief Retrieves the rotation angle from a 2D rotation matrix.
 *
 * This function calculates the rotation angle in radians from a 2D rotation matrix.
 *
 * @tparam T Type of the matrix components.
 * @param m The 2D rotation matrix.
 * @return The rotation angle in radians.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T get_rotation_2d(const mat<3, 3, T>& m)
{
    return math::atan2(m.columns[0].y, m.columns[0].x);
}

// =============== 2d scale ===============

/**
 * @brief Creates a 2D scaling matrix.
 *
 * This function creates a 2D scaling matrix based on the given scale vector.
 *
 * @tparam T Type of the matrix and vector components.
 * @param scale The scaling factors along the x and y axes.
 * @return The resulting 2D scaling matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_scale_2d(const vec<2, T>& scale)
{
    return mat<3, 3, T>(
        scale.x,           static_cast<T>(0), static_cast<T>(0),
        static_cast<T>(0), scale.y,           static_cast<T>(0),
        static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
    );
}

/**
 * @brief Extracts the scaling factors from a 2D scaling matrix.
 *
 * This function extracts the scaling factors along the x and y axes from
 * the given 2D scaling matrix.
 *
 * @tparam T Type of the matrix and vector components.
 * @param m The 2D scaling matrix.
 * @return The scaling factors along the x and y axes.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> get_scale_2d(const mat<3, 3, T>& m)
{
    return vec<2, T>(
        math::length(m.columns[0]),
        math::length(m.columns[1])
    );
}

// =============== trs ===============

/**
 * @brief Constructs a 2D transformation matrix from translation, rotation, and scaling components.
 *
 * This function constructs a 2D transformation matrix from the given translation, rotation angle,
 * and scaling factors along the x and y axes.
 *
 * @tparam T Type of the matrix and vector components.
 * @param translation The translation vector.
 * @param angle The rotation angle in radians.
 * @param scale The scaling factors along the x and y axes.
 * @return The 2D transformation matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> make_trs_2d(const vec<2, T>& translation, T angle, vec<2, T>& scale)
{
    const T cosa = math::cos(angle);
    const T sina = math::sin(angle);

    return mat<3, 3, T>(
        cosa * scale.x, sina * scale.x, static_cast<T>(0),
       -sina * scale.y, cosa * scale.y, static_cast<T>(0),
        translation.x,  translation.y,  static_cast<T>(1)
    );
}

// =============== invert ===============

/**
 * @brief Computes the inverse of a 3x3 affine transformation matrix.
 *
 * This function computes the inverse of a 3x3 affine transformation matrix.
 * The inverse transformation matrix effectively undoes the original
 * transformation.
 *
 * @tparam T Type of the matrix and vector components.
 * @param m The input 3x3 affine transformation matrix.
 * @return The inverse of the input matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> affine_invert(const mat<3, 3, T>& m)
{
    const mat<2, 2, T> ibasis = math::invert(mat<2, 2, T>(m));

    return mat<3, 3, T>(
        vec<3, T>(ibasis.columns[0],                 static_cast<T>(0)),
        vec<3, T>(ibasis.columns[1],                 static_cast<T>(0)),
        vec<3, T>(ibasis * vec<2, T>(-m.columns[2]), static_cast<T>(1))
    );
}

// =============== transform ===============

/**
 * @brief Transforms a 2D vector by an affine transformation matrix.
 *
 * This function applies the 2D affine transformation specified by the given matrix
 * to the input 2D vector. The transformation consists of rotation, scaling, and translation.
 *
 * @tparam T The data type of the matrix and vector elements.
 * @param m The affine transformation matrix.
 * @param v The 2D vector to transform.
 * @return The transformed 2D vector.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<2, T> transform(const mat<3, 3, T>& m, const vec<2, T>& v)
{
    return vec<2, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + m.columns[2].x,
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + m.columns[2].y
    );
}

}
}