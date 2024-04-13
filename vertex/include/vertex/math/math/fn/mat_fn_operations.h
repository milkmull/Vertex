#pragma once

#include "../_priv/common.h"

#include "../type/mat2x2.h"
#include "../type/mat3x3.h"
#include "../type/mat4x4.h"

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
VX_FORCE_INLINE constexpr mat<2, 2, T> transpose(const mat<2, 2, T>& m)
{
    return mat<2, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<3, 2, T> transpose(const mat<2, 3, T>& m)
{
    return mat<3, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y,
        m.columns[0].z, m.columns[1].z
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<4, 2, T> transpose(const mat<2, 4, T>& m)
{
    return mat<4, 2, T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y,
        m.columns[0].z, m.columns[1].z,
        m.columns[0].w, m.columns[1].w
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<2, 3, T> transpose(const mat<3, 2, T>& m)
{
    return mat<2, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<3, 3, T> transpose(const mat<3, 3, T>& m)
{
    return mat<3, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<4, 3, T> transpose(const mat<3, 4, T>& m)
{
    return mat<4, 3, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z,
        m.columns[0].w, m.columns[1].w, m.columns[2].w
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<2, 4, T> transpose(const mat<4, 2, T>& m)
{
    return mat<2, 4, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x, m.columns[3].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y, m.columns[3].y
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<3, 4, T> transpose(const mat<4, 3, T>& m)
{
    return mat<3, 4, T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x, m.columns[3].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y, m.columns[3].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z, m.columns[3].z
    );
}

template <typename T>
VX_FORCE_INLINE constexpr mat<4, 4, T> transpose(const mat<4, 4, T>& m)
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
VX_FORCE_INLINE constexpr T determinant(const mat<2, 2, T>& m)
{
    return (m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y);
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T determinant(const mat<3, 3, T>& m)
{
    return +m.columns[0].x * ((m.columns[1].y * m.columns[2].z) - (m.columns[2].y * m.columns[1].z))
           -m.columns[1].x * ((m.columns[0].y * m.columns[2].z) - (m.columns[2].y * m.columns[0].z))
           +m.columns[2].x * ((m.columns[0].y * m.columns[1].z) - (m.columns[1].y * m.columns[0].z));
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr T determinant(const mat<4, 4, T>& m)
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

// =============== inverse ===============

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
VX_FORCE_INLINE constexpr mat<2, 2, T> inverse(const mat<2, 2, T>& m)
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
VX_FORCE_INLINE constexpr mat<3, 3, T> inverse(const mat<3, 3, T>& m)
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
// https://github.com/glslify/glsl-inverse/blob/master/index.glsl

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr mat<4, 4, T> inverse(const mat<4, 4, T>& m)
{
    const T a00 = m.columns[0].x;
    const T a01 = m.columns[0].y;
    const T a02 = m.columns[0].z;
    const T a03 = m.columns[0].w;

    const T a10 = m.columns[1].x;
    const T a11 = m.columns[1].y;
    const T a12 = m.columns[1].z;
    const T a13 = m.columns[1].w;

    const T a20 = m.columns[2].x;
    const T a21 = m.columns[2].y;
    const T a22 = m.columns[2].z;
    const T a23 = m.columns[2].w;

    const T a30 = m.columns[3].x;
    const T a31 = m.columns[3].y;
    const T a32 = m.columns[3].z;
    const T a33 = m.columns[3].w;

    const T b00 = (a00 * a11) - (a01 * a10);
    const T b01 = (a00 * a12) - (a02 * a10);
    const T b02 = (a00 * a13) - (a03 * a10);
    const T b03 = (a01 * a12) - (a02 * a11);
    const T b04 = (a01 * a13) - (a03 * a11);
    const T b05 = (a02 * a13) - (a03 * a12);
    const T b06 = (a20 * a31) - (a21 * a30);
    const T b07 = (a20 * a32) - (a22 * a30);
    const T b08 = (a20 * a33) - (a23 * a30);
    const T b09 = (a21 * a32) - (a22 * a31);
    const T b10 = (a21 * a33) - (a23 * a31);
    const T b11 = (a22 * a33) - (a23 * a32);

    const T det = (b00 * b11) - (b01 * b10) + (b02 * b09) + (b03 * b08) - (b04 * b07) + (b05 * b06);
    const T idet = static_cast<T>(1) / det;

    return mat<4, 4, T>(
        ((a11 * b11) - (a12 * b10) + (a13 * b09)) * idet,
        ((a02 * b10) - (a01 * b11) - (a03 * b09)) * idet,
        ((a31 * b05) - (a32 * b04) + (a33 * b03)) * idet,
        ((a22 * b04) - (a21 * b05) - (a23 * b03)) * idet,
        ((a12 * b08) - (a10 * b11) - (a13 * b07)) * idet,
        ((a00 * b11) - (a02 * b08) + (a03 * b07)) * idet,
        ((a32 * b02) - (a30 * b05) - (a33 * b01)) * idet,
        ((a20 * b05) - (a22 * b02) + (a23 * b01)) * idet,
        ((a10 * b10) - (a11 * b08) + (a13 * b06)) * idet,
        ((a01 * b08) - (a00 * b10) - (a03 * b06)) * idet,
        ((a30 * b04) - (a31 * b02) + (a33 * b00)) * idet,
        ((a21 * b02) - (a20 * b04) - (a23 * b00)) * idet,
        ((a11 * b07) - (a10 * b09) - (a12 * b06)) * idet,
        ((a00 * b09) - (a01 * b07) + (a02 * b06)) * idet,
        ((a31 * b01) - (a30 * b03) - (a32 * b00)) * idet,
        ((a20 * b03) - (a21 * b01) + (a22 * b00)) * idet
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
VX_FORCE_INLINE constexpr mat<2, N, T> matrix_comp_mult(
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
VX_FORCE_INLINE constexpr mat<3, N, T> matrix_comp_mult(
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
VX_FORCE_INLINE constexpr mat<4, N, T> matrix_comp_mult(
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
VX_FORCE_INLINE constexpr mat<2, 2, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<2, 3, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<2, 4, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<3, 2, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<3, 3, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<3, 4, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<4, 2, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<4, 3, T> outer_product(
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
VX_FORCE_INLINE constexpr mat<4, 4, T> outer_product(
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