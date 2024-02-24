#pragma once

#include "../math.h"

namespace vx {
namespace math {

// =============== mat2 pure rotation ===============

/**
 * @brief Creates a 2x2 rotation matrix.
 *
 * This function generates a 2x2 rotation matrix with the specified angle.
 *
 * @tparam T The type of elements in the matrix.
 * @param angle The angle of rotation in radians.
 * @return A 2x2 rotation matrix.
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
 * This function calculates and returns the rotation angle represented by
 * the given 2x2 rotation matrix.
 *
 * @tparam T The type of elements in the matrix.
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
 * This function generates a 2x2 scaling matrix with the specified scaling factors.
 *
 * @tparam T The type of elements in the matrix.
 * @param scale A 2D vector representing the scaling factors along the x and y axes.
 * @return A 2x2 scaling matrix.
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
 * @brief Retrieves the scaling factors from a 2x2 scaling matrix.
 *
 * This function calculates and returns the scaling factors represented by
 * the given 2x2 scaling matrix.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 2x2 scaling matrix.
 * @return A 2D vector representing the scaling factors along the x and y axes.
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
 * This function generates a 3x3 translation matrix with the specified translation vector in 2D space.

 * @tparam T The type of elements in the matrix (must be floating-point).
 * @param translation A 2D vector representing the translation along the x and y axes.
 * @return A 3x3 translation matrix.
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
 * @brief Retrieves the translation vector from a 2D translation matrix.
 *
 * This function extracts and returns the translation vector represented by
 * the given 3x3 translation matrix in 2D space.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 3x3 translation matrix.
 * @return A 2D vector representing the translation along the x and y axes.
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
 * This function generates a 3x3 rotation matrix with the specified angle of rotation in 2D space.
 * 
 * @tparam T The type of elements in the matrix (must be floating-point).
 * @param angle The angle of rotation in radians.
 * @return A 3x3 rotation matrix.
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
 * This function calculates and returns the rotation angle represented by
 * the given 3x3 rotation matrix in 2D space.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 3x3 rotation matrix.
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
 * This function generates a 3x3 scaling matrix with the specified scaling factors in 2D space.
 *
 * @tparam T The type of elements in the matrix (must be floating-point).
 * @param scale A 2D vector representing the scaling factors along the x and y axes.
 * @return A 3x3 scaling matrix.
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
 * @brief Retrieves the scaling factors from a 2D scaling matrix.
 *
 * This function calculates and returns the scaling factors represented by
 * the given 3x3 scaling matrix in 2D space.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 3x3 scaling matrix.
 * @return A 2D vector representing the scaling factors along the x and y axes.
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
 * @brief Creates a 2D transformation matrix combining translation, rotation, and scaling.
 *
 * This function generates a 3x3 transformation matrix in 2D space by combining translation, rotation, and scaling.
 *
 * @tparam T The type of elements in the matrix (must be floating-point).
 * @param translation A 2D vector representing the translation along the x and y axes.
 * @param angle The angle of rotation in radians.
 * @param scale A 2D vector representing the scaling factors along the x and y axes.
 * @return A 3x3 transformation matrix.
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
 * @brief Calculates the inverse of an affine transformation matrix.
 *
 * This function computes the inverse of the given 3x3 affine transformation matrix.
 * The inverse of an affine transformation matrix undoes the transformation, effectively
 * reversing the original transformation.
 *
 * @tparam T The type of elements in the matrix.
 * @param m The 3x3 affine transformation matrix.
 * @return The inverse of the matrix.
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
 * @brief Transforms a 2D vector using a 3x3 transformation matrix.
 *
 * This function applies the given 3x3 transformation matrix to a 2D vector.
 * The transformation matrix represents a combination of translation, rotation, and scaling.
 *
 * @tparam T The type of elements in the matrix and vector.
 * @param m The 3x3 transformation matrix.
 * @param v The 2D vector to be transformed.
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