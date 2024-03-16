#pragma once

#include "../detail/base_types.h"

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
    return mat<2, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y
    );
}

template <typename T>
inline constexpr mat<3, 2, T> transpose(const mat<2, 3, T>& m)
{
    return mat<3, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y,
        m.columns[0].z, m.columns[1].z
    );
}

template <typename T>
inline constexpr mat<4, 2, T> transpose(const mat<2, 4, T>& m)
{
    return mat<4, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y,
        m.columns[0].z, m.columns[1].z,
        m.columns[0].w, m.columns[1].w
    );
}

template <typename T>
inline constexpr mat<2, 3, T> transpose(const mat<3, 2, T>& m)
{
    return mat<2, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y
    );
}

template <typename T>
inline constexpr mat<3, 3, T> transpose(const mat<3, 3, T>& m)
{
    return mat<3, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z
    );
}

template <typename T>
inline constexpr mat<4, 3, T> transpose(const mat<3, 4, T>& m)
{
    return mat<4, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z,
        m.columns[0].w, m.columns[1].w, m.columns[2].w
    );
}

template <typename T>
inline constexpr mat<2, 4, T> transpose(const mat<4, 2, T>& m)
{
    return mat<2, 4, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x, m.columns[3].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y, m.columns[3].y
    );
}

template <typename T>
inline constexpr mat<3, 4, T> transpose(const mat<4, 3, T>& m)
{
    return mat<3, 4, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x, m.columns[3].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y, m.columns[3].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z, m.columns[3].z
    );
}

template <typename T>
inline constexpr mat<4, 4, T> transpose(const mat<4, 4, T>& m)
{
    return mat<4, 4, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x, m.columns[3].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y, m.columns[3].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z, m.columns[3].z,
        m.columns[0].w, m.columns[1].w, m.columns[2].w, m.columns[3].w
    );
}

// =============== determinant ===============

/**
 * @brief Computes the determinant of a matrix.
 *
 * @param m The matrix to compute the determinant of.
 * @return The determinant of the matrix.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T determinant(const mat<2, 2, T>& m)
{
    return (m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T determinant(const mat<3, 3, T>& m)
{
    return +m.columns[0].x * ((m.columns[1].y * m.columns[2].z) - (m.columns[2].y * m.columns[1].z))
           -m.columns[1].x * ((m.columns[0].y * m.columns[2].z) - (m.columns[2].y * m.columns[0].z))
           +m.columns[2].x * ((m.columns[0].y * m.columns[1].z) - (m.columns[1].y * m.columns[0].z));
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T determinant(const mat<4, 4, T>& m)
{
    const T subfac00 = (m.columns[2].z * m.columns[3].w) - (m.columns[2].w * m.columns[3].z);
    const T subfac01 = (m.columns[2].y * m.columns[3].w) - (m.columns[2].w * m.columns[3].y);
    const T subfac02 = (m.columns[2].y * m.columns[3].z) - (m.columns[2].z * m.columns[3].y);
    const T subfac03 = (m.columns[2].x * m.columns[3].w) - (m.columns[2].w * m.columns[3].x);
    const T subfac04 = (m.columns[2].x * m.columns[3].z) - (m.columns[2].z * m.columns[3].x);
    const T subfac05 = (m.columns[2].x * m.columns[3].y) - (m.columns[2].y * m.columns[3].x);

    return +(m.columns[0].x * ((m.columns[1].y * subfac00) - (m.columns[1].z * subfac01) + (m.columns[1].w * subfac02)))
           -(m.columns[0].y * ((m.columns[1].x * subfac00) - (m.columns[1].z * subfac03) + (m.columns[1].w * subfac04)))
           +(m.columns[0].z * ((m.columns[1].x * subfac01) - (m.columns[1].y * subfac03) + (m.columns[1].w * subfac05)))
           -(m.columns[0].w * ((m.columns[1].x * subfac02) - (m.columns[1].y * subfac04) + (m.columns[1].z * subfac05)));
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<2, 2, T> invert(const mat<2, 2, T>& m)
{
    const T det = (m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y);

    const T idet = static_cast<T>(1) / det;

    return mat<2, 2, T>(
        +m.columns[1].y * idet,
        -m.columns[0].y * idet,
        -m.columns[1].x * idet,
        +m.columns[0].x * idet
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> invert(const mat<3, 3, T>& m)
{
    const T coef00 = (m.columns[1].y * m.columns[2].z) - (m.columns[2].y * m.columns[1].z);
    const T coef01 = (m.columns[0].y * m.columns[2].z) - (m.columns[2].y * m.columns[0].z);
    const T coef02 = (m.columns[0].y * m.columns[1].z) - (m.columns[1].y * m.columns[0].z);

    const T coef03 = (m.columns[1].x * m.columns[2].z) - (m.columns[2].x * m.columns[1].z);
    const T coef04 = (m.columns[0].x * m.columns[2].z) - (m.columns[2].x * m.columns[0].z);
    const T coef05 = (m.columns[0].x * m.columns[1].z) - (m.columns[1].x * m.columns[0].z);

    const T coef06 = (m.columns[1].x * m.columns[2].y) - (m.columns[2].x * m.columns[1].y);
    const T coef07 = (m.columns[0].x * m.columns[2].y) - (m.columns[2].x * m.columns[0].y);
    const T coef08 = (m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y);

    const T det = +m.columns[0].x * coef00
                  -m.columns[1].x * coef01
                  +m.columns[2].x * coef02;

    const T idet = static_cast<T>(1) / det;

    return mat<3, 3, T>(
        +coef00 * idet,
        -coef01 * idet,
        +coef02 * idet,

        -coef03 * idet,
        +coef04 * idet,
        -coef05 * idet,

        +coef06 * idet,
        -coef07 * idet,
        +coef08 * idet
    );
}

// https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> invert(const mat<4, 4, T>& m)
{
    const T coef00 = (m.columns[2].z * m.columns[3].w) - (m.columns[3].z * m.columns[2].w);
    const T coef01 = (m.columns[2].y * m.columns[3].w) - (m.columns[3].y * m.columns[2].w);
    const T coef02 = (m.columns[2].y * m.columns[3].z) - (m.columns[3].y * m.columns[2].z);

    const T coef03 = (m.columns[2].x * m.columns[3].w) - (m.columns[2].w * m.columns[3].x);
    const T coef04 = (m.columns[2].x * m.columns[3].z) - (m.columns[2].z * m.columns[3].x);
    const T coef05 = (m.columns[2].x * m.columns[3].y) - (m.columns[2].y * m.columns[3].x);

    const T coef06 = (m.columns[1].z * m.columns[3].w) - (m.columns[1].w * m.columns[3].z);
    const T coef07 = (m.columns[1].y * m.columns[3].w) - (m.columns[1].w * m.columns[3].y);
    const T coef08 = (m.columns[1].y * m.columns[3].z) - (m.columns[1].z * m.columns[3].y);

    const T coef09 = (m.columns[1].z * m.columns[2].w) - (m.columns[1].w * m.columns[2].z);
    const T coef10 = (m.columns[1].y * m.columns[2].w) - (m.columns[1].w * m.columns[2].y);
    const T coef11 = (m.columns[1].y * m.columns[2].z) - (m.columns[1].z * m.columns[2].y);

    const T coef12 = (m.columns[1].x * m.columns[3].w) - (m.columns[1].w * m.columns[3].x);
    const T coef13 = (m.columns[1].x * m.columns[3].z) - (m.columns[1].z * m.columns[3].x);
    const T coef14 = (m.columns[1].x * m.columns[2].w) - (m.columns[1].w * m.columns[2].x);

    const T coef15 = (m.columns[1].x * m.columns[2].z) - (m.columns[1].z * m.columns[2].x);
    const T coef16 = (m.columns[1].x * m.columns[3].y) - (m.columns[1].y * m.columns[3].x);
    const T coef17 = (m.columns[1].x * m.columns[2].y) - (m.columns[1].y * m.columns[2].x);

    const T det = +(m.columns[0].x * ((m.columns[1].y * coef00) - (m.columns[1].z * coef01) + (m.columns[1].w * coef02)))
                  -(m.columns[0].y * ((m.columns[1].x * coef00) - (m.columns[1].z * coef03) + (m.columns[1].w * coef04)))
                  +(m.columns[0].z * ((m.columns[1].x * coef01) - (m.columns[1].y * coef03) + (m.columns[1].w * coef05)))
                  -(m.columns[0].w * ((m.columns[1].x * coef02) - (m.columns[1].y * coef04) + (m.columns[1].z * coef05)));

    const T idet = static_cast<T>(1) / det;

    return mat<4, 4, T>(
        +((m.columns[1].y * coef00) - (m.columns[1].z * coef01) + (m.columns[1].w * coef02)) * idet,
        -((m.columns[0].y * coef00) - (m.columns[0].z * coef01) + (m.columns[0].w * coef02)) * idet,
        +((m.columns[0].y * coef06) - (m.columns[0].z * coef07) + (m.columns[0].w * coef08)) * idet,
        -((m.columns[0].y * coef09) - (m.columns[0].z * coef10) + (m.columns[0].w * coef11)) * idet,

        -((m.columns[1].x * coef00) - (m.columns[1].z * coef03) + (m.columns[1].w * coef04)) * idet,
        +((m.columns[0].x * coef00) - (m.columns[0].z * coef03) + (m.columns[0].w * coef04)) * idet,
        -((m.columns[0].x * coef06) - (m.columns[0].z * coef12) + (m.columns[0].w * coef13)) * idet,
        +((m.columns[0].x * coef09) - (m.columns[0].z * coef14) + (m.columns[0].w * coef15)) * idet,

        +((m.columns[1].x * coef01) - (m.columns[1].y * coef03) + (m.columns[1].w * coef05)) * idet,
        -((m.columns[0].x * coef01) - (m.columns[0].y * coef03) + (m.columns[0].w * coef05)) * idet,
        +((m.columns[0].x * coef07) - (m.columns[0].y * coef12) + (m.columns[0].w * coef16)) * idet,
        -((m.columns[0].x * coef10) - (m.columns[0].y * coef14) + (m.columns[0].w * coef17)) * idet,

        -((m.columns[1].x * coef02) - (m.columns[1].y * coef04) + (m.columns[1].z * coef05)) * idet,
        +((m.columns[0].x * coef02) - (m.columns[0].y * coef04) + (m.columns[0].z * coef05)) * idet,
        -((m.columns[0].x * coef08) - (m.columns[0].y * coef13) + (m.columns[0].z * coef16)) * idet,
        +((m.columns[0].x * coef11) - (m.columns[0].y * coef15) + (m.columns[0].z * coef17)) * idet
    );
}

// =============== component multiplication ===============

/**
 * @brief Performs component-wise multiplication of two matrices of size 2xN.
 *
 * This function computes the component-wise multiplication of two matrices and returns the result.
 *
 * @tparam N The number of columns in the matrices.
 * @tparam T The data type of the matrix elements.
 * @param x The first matrix operand.
 * @param y The second matrix operand.
 * @return The resulting matrix after component-wise multiplication.
 */
template <size_t N, typename T>
inline constexpr mat<2, N, T> matrix_comp_mult(
    const mat<2, N, T>& x,
    const mat<2, N, T>& y
)
{
    return mat<2, N, T>(
        x.colums[0] * y.columns[0],
        x.colums[1] * y.columns[1]
    );
}

template <size_t N, typename T>
inline constexpr mat<3, N, T> matrix_comp_mult(
    const mat<3, N, T>& x,
    const mat<3, N, T>& y
)
{
    return mat<3, N, T>(
        x.colums[0] * y.columns[0],
        x.colums[1] * y.columns[1],
        x.colums[2] * y.columns[2]
    );
}

template <size_t N, typename T>
inline constexpr mat<4, N, T> matrix_comp_mult(
    const mat<4, N, T>& x,
    const mat<4, N, T>& y
)
{
    return mat<4, N, T>(
        x.colums[0] * y.columns[0],
        x.colums[1] * y.columns[1],
        x.colums[2] * y.columns[2],
        x.colums[3] * y.columns[3]
    );
}

// =============== outer product ===============

/**
 * @brief Computes the outer product of two vectors.
 *
 * This function computes the outer product of two vectors and returns the resulting matrix.
 *
 * @tparam T The data type of the vector elements.
 * @param c The first vector operand (column vector).
 * @param r The second vector operand (row vector).
 * @return The resulting matrix after computing the outer product.
 */
template <typename T>
inline constexpr mat<2, 2, T> outer_product(
    const vec<2, T>& c,
    const vec<2, T>& r
)
{
    return mat<2, 2, T>(
        c.x * r.x, c.y * r.x,
        c.x * r.y, c.y * r.y
    );
}

template <typename T>
inline constexpr mat<2, 3, T> outer_product(
    const vec<3, T>& c,
    const vec<2, T>& r
)
{
    return mat<2, 3, T>(
        c.x * r.x, c.y * r.x, c.z * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y
    );
}

template <typename T>
inline constexpr mat<2, 4, T> outer_product(
    const vec<4, T>& c,
    const vec<2, T>& r
)
{
    return mat<2, 4, T>(
        c.x * r.x, c.y * r.x, c.z * r.x, c.w * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y, c.w * r.y
    );
}

template <typename T>
inline constexpr mat<3, 2, T> outer_product(
    const vec<2, T>& c,
    const vec<3, T>& r
)
{
    return mat<3, 2, T>(
        c.x * r.x, c.y * r.x,
        c.x * r.y, c.y * r.y,
        c.x * r.z, c.y * r.z
    );
}

template <typename T>
inline constexpr mat<3, 3, T> outer_product(
    const vec<3, T>& c,
    const vec<3, T>& r
)
{
    return mat<3, 3, T>(
        c.x * r.x, c.y * r.x, c.z * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y,
        c.x * r.z, c.y * r.z, c.z * r.z
    );
}

template <typename T>
inline constexpr mat<3, 4, T> outer_product(
    const vec<4, T>& c,
    const vec<3, T>& r
)
{
    return mat<3, 4, T>(
        c.x * r.x, c.y * r.x, c.z * r.x, c.w * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y, c.w * r.y,
        c.x * r.z, c.y * r.z, c.z * r.z, c.w * r.z
    );
}

template <typename T>
inline constexpr mat<4, 2, T> outer_product(
    const vec<2, T>& c,
    const vec<4, T>& r
)
{
    return mat<4, 2, T>(
        c.x * r.x, c.y * r.x,
        c.x * r.y, c.y * r.y,
        c.x * r.z, c.y * r.z,
        c.x * r.w, c.y * r.w
    );
}

template <typename T>
inline constexpr mat<4, 3, T> outer_product(
    const vec<3, T>& c,
    const vec<4, T>& r
)
{
    return mat<4, 3, T>(
        c.x * r.x, c.y * r.x, c.z * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y,
        c.x * r.z, c.y * r.z, c.z * r.z,
        c.x * r.w, c.y * r.w, c.z * r.w
    );
}

template <typename T>
inline constexpr mat<4, 4, T> outer_product(
    const vec<4, T>& c,
    const vec<4, T>& r
)
{
    return mat<4, 4, T>(
        c.x * r.x, c.y * r.x, c.z * r.x, c.w * r.x,
        c.x * r.y, c.y * r.y, c.z * r.y, c.w * r.y,
        c.x * r.z, c.y * r.z, c.z * r.z, c.w * r.z,
        c.x * r.w, c.y * r.w, c.z * r.w, c.w * r.w
    );
}

}
}