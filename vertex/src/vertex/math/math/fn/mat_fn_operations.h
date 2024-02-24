#pragma once

#include "../detail/base_types.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== transpose ===============

/**
 * @brief Transposes a matrix.
 *
 * @tparam T Type of the matrix components.
 * @param m The input matrix.
 * @return The transposed matrix.
 */
template <typename T>
inline constexpr mat<2, 2, T> transpose(const mat<2, 2, T>& m)
{
    return m.transpose();
}

template <typename T>
inline constexpr mat<3, 3, T> transpose(const mat<3, 3, T>& m)
{
    return m.transpose();
}

template <typename T>
inline constexpr mat<4, 4, T> transpose(const mat<4, 4, T>& m)
{
    return m.transpose();
}

// =============== determinant ===============

/**
 * @brief Computes the determinant of a matrix.
 *
 * @param m The matrix to compute the determinant of.
 * @return The determinant of the matrix.
 */
template <typename T>
inline constexpr T determinant(const mat<2, 2, T>& m)
{
    return m.determinant();
}

template <typename T>
inline constexpr T determinant(const mat<3, 3, T>& m)
{
    return m.determinant();
}

template <typename T>
inline constexpr T determinant(const mat<4, 4, T>& m)
{
    return m.determinant();
}

// =============== invert ===============

/**
 * @brief Computes the inverse of a matrix.
 *
 * This function calculates the inverse matrix for the matrix 'm' if it exists.
 * If the matrix is not invertible (determinant is approximately zero), it returns a matrix with zeros.
 *
 * @param m The matrix to invert.
 * @return The inverted matrix if invertible, otherwise a matrix with zeros.
 */
template <typename T>
inline constexpr mat<2, 2, T> invert(const mat<2, 2, T>& m)
{
    return m.invert();
}

template <typename T>
inline constexpr mat<3, 3, T> invert(const mat<3, 3, T>& m)
{
    return m.invert();
}

template <typename T>
inline constexpr mat<4, 4, T> invert(const mat<4, 4, T>& m)
{
    return m.invert();
}

}
}