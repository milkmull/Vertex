#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"

namespace vx {
namespace math {

template <typename T>
struct mat<3, 2, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    static constexpr size_t width = 3;
    static constexpr size_t height = 2;

    using type = mat<width, height, scalar_type>;
    using row_type = vec<width, scalar_type>;
    using col_type = vec<height, scalar_type>;
    using transpose_type = mat<height, width, scalar_type>;

    static constexpr size_t size = width * height;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    col_type columns[width];

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr mat() noexcept
        : columns{ col_type(static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0)) } {}

    VX_FORCE_INLINE constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit mat(scalar_type scalar) noexcept
        : columns{ col_type(scalar),
                   col_type(scalar),
                   col_type(scalar) } {}

    VX_FORCE_INLINE constexpr mat(
        scalar_type x1, scalar_type y1,
        scalar_type x2, scalar_type y2,
        scalar_type x3, scalar_type y3
    ) noexcept
        : columns{ col_type(x1, y1),
                   col_type(x2, y2),
                   col_type(x3, y3) } {}

    VX_FORCE_INLINE constexpr mat(const col_type& c1, const col_type& c2, const col_type& c3) noexcept
        : columns{ c1, c2, c3 } {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit mat(U scalar) noexcept
        : columns{ col_type(static_cast<scalar_type>(scalar)),
                   col_type(static_cast<scalar_type>(scalar)),
                   col_type(static_cast<scalar_type>(scalar)) } {}

    template <
        typename X1, typename Y1,
        typename X2, typename Y2,
        typename X3, typename Y3
    >
    VX_FORCE_INLINE constexpr mat(
        X1 x1, Y1 y1,
        X2 x2, Y2 y2,
        X3 x3, Y3 y3
    ) noexcept
        : columns{ col_type(static_cast<scalar_type>(x1), static_cast<scalar_type>(y1)),
                   col_type(static_cast<scalar_type>(x2), static_cast<scalar_type>(y2)),
                   col_type(static_cast<scalar_type>(x3), static_cast<scalar_type>(y3)) } {}

    template <typename C1, typename C2, typename C3>
    VX_FORCE_INLINE constexpr mat(
        const vec<2, C1>& c1,
        const vec<2, C2>& c2,
        const vec<2, C3>& c3
    ) noexcept
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3) } {}

    template <size_t M, size_t N, typename U, VXM_REQ(M >= width && N >= height)>
    VX_FORCE_INLINE constexpr explicit mat(const mat<M, N, U>& m) noexcept
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]),
                   static_cast<col_type>(m.columns[2]) } {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const mat<width, height, U>& m) noexcept
    {
        columns[0] = static_cast<col_type>(m.columns[0]);
        columns[1] = static_cast<col_type>(m.columns[1]);
        columns[2] = static_cast<col_type>(m.columns[2]);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr col_type& operator[](size_t i) noexcept
    {
        VX_ASSERT(i < width);
        return columns[i];
    }

    VX_FORCE_INLINE constexpr const col_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < width);
        return columns[i];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    // equal (==)

    friend VX_FORCE_INLINE constexpr bool operator==(const type& m1, const type& m2) noexcept
    {
        return (m1.columns[0] == m2.columns[0])
            && (m1.columns[1] == m2.columns[1])
            && (m1.columns[2] == m2.columns[2]);
    }

    // not equal (!=)

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& m1, const type& m2) noexcept
    {
        return (m1.columns[0] != m2.columns[0])
            || (m1.columns[1] != m2.columns[1])
            || (m1.columns[2] != m2.columns[2]);
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] > m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] > m2.columns[1];
        return m1.columns[2] > m2.columns[2];
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] < m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] < m2.columns[1];
        return m1.columns[2] < m2.columns[2];
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] > m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] > m2.columns[1];
        return m1.columns[2] >= m2.columns[2];
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] < m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] < m2.columns[1];
        return m1.columns[2] <= m2.columns[2];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+columns[0], +columns[1], +columns[2]);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-columns[0], -columns[1], -columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // increment (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        columns[0]++;
        columns[1]++;
        columns[2]++;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator++(int) noexcept
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    VX_FORCE_INLINE constexpr type& operator--() noexcept
    {
        columns[0]--;
        columns[1]--;
        columns[2]--;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator--(int) noexcept
    {
        type result(*this);
        --(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr type operator+(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] + scalar, m.columns[1] + scalar, m.columns[2] + scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar + m.columns[0], scalar + m.columns[1], scalar + m.columns[2]);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& m1, const type& m2) noexcept
    {
        return type(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1], m1.columns[2] + m2.columns[2]);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] - scalar, m.columns[1] - scalar, m.columns[2] - scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar - m.columns[0], scalar - m.columns[1], scalar - m.columns[2]);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& m1, const type& m2) noexcept
    {
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1], m1.columns[2] - m2.columns[2]);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] * scalar, m.columns[1] * scalar, m.columns[2] * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar * m.columns[0], scalar * m.columns[1], scalar * m.columns[2]);
    }

    friend VX_FORCE_INLINE constexpr vec<height, scalar_type> operator*(const type& m, const vec<width, scalar_type>& v) noexcept
    {
        return vec<height, scalar_type>(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z)
        );
    }

    friend VX_FORCE_INLINE constexpr vec<width, scalar_type> operator*(const vec<height, scalar_type>& v, const type& m) noexcept
    {
        return vec<width, scalar_type>(
            (v.x * m.columns[0].x) + (v.y * m.columns[0].y),
            (v.x * m.columns[1].x) + (v.y * m.columns[1].y),
            (v.x * m.columns[2].x) + (v.y * m.columns[2].y)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<2, height, scalar_type> operator*(const type& m1, const mat<2, width, scalar_type>& m2) noexcept
    {
        return mat<2, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<3, height, scalar_type> operator*(const type& m1, const mat<3, width, scalar_type>& m2) noexcept
    {
        return mat<3, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<4, height, scalar_type> operator*(const type& m1, const mat<4, width, scalar_type>& m2) noexcept
    {
        return mat<4, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z),

            (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y) + (m1.columns[2].x * m2.columns[3].z),
            (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y) + (m1.columns[2].y * m2.columns[3].z)
        );
    }


    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] / scalar, m.columns[1] / scalar, m.columns[2] / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar / m.columns[0], scalar / m.columns[1], scalar / m.columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        columns[0] += scalar;
        columns[1] += scalar;
        columns[2] += scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& m) noexcept
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        columns[0] -= scalar;
        columns[1] -= scalar;
        columns[2] -= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& m) noexcept
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        columns[0] *= scalar;
        columns[1] *= scalar;
        columns[2] *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const mat<width, width, scalar_type>& m) noexcept
    {
        (*this) = (*this) * m;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        columns[0] /= scalar;
        columns[1] /= scalar;
        columns[2] /= scalar;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<T>(0)); }
};

} // namespace math
} // namespace vx