#pragma once

#include "../detail/base_types.h"
#include "fn_comparison.h"

namespace vx {
namespace math {

// =============== transpose ===============

/**
 * @brief Computes the transpose of a matrix.
 *
 * This function calculates the transpose of the matrix 'm'.
 *
 * @param m The matrix to transpose.
 * @return The transposed matrix.
 */
template <typename T>
inline constexpr detail::mat2x<T> transpose(const detail::mat2x<T>& m)
{
    return detail::mat2x<T>(
        m.columns[0].x, m.columns[1].x,
        m.columns[0].y, m.columns[1].y
    );
}

template <typename T>
inline constexpr detail::mat3x<T> transpose(const detail::mat3x<T>& m)
{
    return detail::mat3x<T>(
        m.columns[0].x, m.columns[1].x, m.columns[2].x,
        m.columns[0].y, m.columns[1].y, m.columns[2].y,
        m.columns[0].z, m.columns[1].z, m.columns[2].z
    );
}

template <typename T>
inline constexpr detail::mat4x<T> transpose(const detail::mat4x<T>& m)
{
    return detail::mat4x<T>(
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
 * This function calculates the determinant of a matrix 'm'.
 *
 * @param m The matrix to compute the determinant of.
 * @return The determinant of the matrix.
 */
template <typename T>
inline constexpr T determinant(const detail::mat2x<T>& m)
{
    return (m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y);
}

template <typename T>
inline constexpr T determinant(const detail::mat3x<T>& m)
{
    return (
        +m.columns[0].x * ((m.columns[1].y * m.columns[2].z) - (m.columns[2].y * m.columns[1].z))
        -m.columns[1].x * ((m.columns[0].y * m.columns[2].z) - (m.columns[2].y * m.columns[0].z))
        +m.columns[2].x * ((m.columns[0].y * m.columns[1].z) - (m.columns[1].y * m.columns[0].z))
    );
}

template <typename T>
inline constexpr T determinant(const detail::mat4x<T>& m)
{
    const T subfac00 = (m.columns[2].z * m.columns[3].w) - (m.columns[2].w * m.columns[3].z);
    const T subfac01 = (m.columns[2].y * m.columns[3].w) - (m.columns[2].w * m.columns[3].y);
    const T subfac02 = (m.columns[2].y * m.columns[3].z) - (m.columns[2].z * m.columns[3].y);
    const T subfac03 = (m.columns[2].x * m.columns[3].w) - (m.columns[2].w * m.columns[3].x);
    const T subfac04 = (m.columns[2].x * m.columns[3].z) - (m.columns[2].z * m.columns[3].x);
    const T subfac05 = (m.columns[2].x * m.columns[3].y) - (m.columns[2].y * m.columns[3].x);

    return (
        +(m.columns[0].x * ((m.columns[1].y * subfac00) - (m.columns[1].z * subfac01) + (m.columns[1].w * subfac02)))
        -(m.columns[0].y * ((m.columns[1].x * subfac00) - (m.columns[1].z * subfac03) + (m.columns[1].w * subfac04)))
        +(m.columns[0].z * ((m.columns[1].x * subfac01) - (m.columns[1].y * subfac03) + (m.columns[1].w * subfac05)))
        -(m.columns[0].w * ((m.columns[1].x * subfac02) - (m.columns[1].y * subfac04) + (m.columns[1].z * subfac05)))
    );
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
inline constexpr detail::mat2x<T> invert(const detail::mat2x<T>& m)
{
    const T det = determinant(m);

    if (math::is_zero_approx(det))
    {
        return detail::mat2x<T>(static_cast<T>(0));
    }

    const T idet = static_cast<T>(1) / det;

    return detail::mat2x<T>(
        +m.columns[1].y * idet,
        -m.columns[0].y * idet,
        -m.columns[1].x * idet,
        +m.columns[0].x * idet
    );
}

template <typename T>
inline constexpr detail::mat3x<T> invert(const detail::mat3x<T>& m)
{
    const T det = determinant(m);

    if (math::is_zero_approx(det))
    {
        return detail::mat3x<T>(static_cast<T>(0));
    }

    const T idet = static_cast<T>(1) / det;

    return detail::mat3x<T>(
        +((m.columns[1].y * m.columns[2].z) - (m.columns[2].y * m.columns[1].z)) * idet,
        -((m.columns[0].y * m.columns[2].z) - (m.columns[2].y * m.columns[0].z)) * idet,
        +((m.columns[0].y * m.columns[1].z) - (m.columns[1].y * m.columns[0].z)) * idet,

        -((m.columns[1].x * m.columns[2].z) - (m.columns[2].x * m.columns[1].z)) * idet,
        +((m.columns[0].x * m.columns[2].z) - (m.columns[2].x * m.columns[0].z)) * idet,
        -((m.columns[0].x * m.columns[1].z) - (m.columns[1].x * m.columns[0].z)) * idet,

        +((m.columns[1].x * m.columns[2].y) - (m.columns[2].x * m.columns[1].y)) * idet,
        -((m.columns[0].x * m.columns[2].y) - (m.columns[2].x * m.columns[0].y)) * idet,
        +((m.columns[0].x * m.columns[1].y) - (m.columns[1].x * m.columns[0].y)) * idet
    );
}

template <typename T>
inline constexpr detail::mat4x<T> invert(const detail::mat4x<T>& m)
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

    const T det = (
        +(m.columns[0].x * ((m.columns[1].y * coef00) - (m.columns[1].z * coef01) + (m.columns[1].w * coef02)))
        -(m.columns[0].y * ((m.columns[1].x * coef00) - (m.columns[1].z * coef03) + (m.columns[1].w * coef04)))
        +(m.columns[0].z * ((m.columns[1].x * coef01) - (m.columns[1].y * coef03) + (m.columns[1].w * coef05)))
        -(m.columns[0].w * ((m.columns[1].x * coef02) - (m.columns[1].y * coef04) + (m.columns[1].z * coef05)))
    );

    if (math::is_zero_approx(det))
    {
        return detail::mat4x<T>(static_cast<T>(0));
    }

    const T idet = static_cast<T>(1) / det;

    return detail::mat4x<T>(
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

}
}