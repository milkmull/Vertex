#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/math/types/vec3i.hpp"

namespace vx {
namespace math {

template <typename T>
struct mat_t<3, 3, T, __detail::component_type::_i>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    static constexpr size_t width = 3;
    static constexpr size_t height = 3;
    static constexpr auto __component_type = __detail::component_type::_i;

    using type = mat_t<width, height, scalar_type, __component_type>;
    using row_type = vec_t<width, scalar_type, __component_type>;
    using col_type = vec_t<height, scalar_type, __component_type>;
    using transpose_type = mat_t<width, height, scalar_type, __component_type>;

    static constexpr size_t size = width * height;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    col_type columns[width];

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr mat_t() noexcept
        : columns{ col_type(static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(1), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(1)) } {}

    VX_FORCE_INLINE constexpr mat_t(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    VX_FORCE_INLINE constexpr mat_t(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit mat_t(scalar_type scalar) noexcept
        : columns{ col_type(scalar, static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), scalar, static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), scalar) } {}

    VX_FORCE_INLINE constexpr mat_t(
        scalar_type x1, scalar_type y1, scalar_type z1,
        scalar_type x2, scalar_type y2, scalar_type z2,
        scalar_type x3, scalar_type y3, scalar_type z3
    ) noexcept
        : columns{ col_type(x1, y1),
                   col_type(x2, y2),
                   col_type(x3, y3) } {}

    VX_FORCE_INLINE constexpr mat_t(const col_type& c1, const col_type& c2, const col_type& c3) noexcept
        : columns{ c1, c2, c3 } {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit mat_t(U scalar) noexcept
        : columns{ col_type(static_cast<scalar_type>(scalar), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(scalar), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(scalar)) } {}

    template <
        typename X1, typename Y1, typename Z1,
        typename X2, typename Y2, typename Z2,
        typename X3, typename Y3, typename Z3,
        VXM_REQ_NUM9(X1, Y1, Z1, X2, Y2, Z2, X3, Y3, Z3)>
    VX_FORCE_INLINE constexpr mat_t(
        X1 x1, Y1 y1, Z1 z1,
        X2 x2, Y2 y2, Z2 z2,
        X3 x3, Y3 y3, Z3 z3
    ) noexcept
        : columns{ col_type(static_cast<scalar_type>(x1), static_cast<scalar_type>(y1), static_cast<scalar_type>(z1)),
                   col_type(static_cast<scalar_type>(x2), static_cast<scalar_type>(y2), static_cast<scalar_type>(z2)),
                   col_type(static_cast<scalar_type>(x3), static_cast<scalar_type>(y3), static_cast<scalar_type>(z3)) } {}

    template <typename C1, typename C2, typename C3, VXM_REQ_NUM3(C1, C2, C3)>
    VX_FORCE_INLINE constexpr mat_t(
        const vec<2, C1>& c1,
        const vec<2, C2>& c2,
        const vec<2, C3>& c3
    ) noexcept
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3) } {}

    template <size_t M, size_t N, typename U, VXM_REQ((M >= width && N >= height) && is_numeric<U>::value)>
    VX_FORCE_INLINE constexpr explicit mat_t(const mat<M, N, U>& m) noexcept
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]),
                   static_cast<col_type>(m.columns[2]) } {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~mat_t() noexcept = default;

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

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U, VXM_REQ_NUM(U)>
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
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1], m1.columns[2] - m1.columns[2]);
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
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z)
        );
    }

    friend VX_FORCE_INLINE constexpr vec<height, scalar_type> operator*(const vec<width, scalar_type>& v, const type& m) noexcept
    {
        return vec<height, scalar_type>(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y),
            (v.x * m.columns[0].z) + (v.y * m.columns[1].z) + (v.z * m.columns[2].z)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<2, height, scalar_type> operator*(const type& m1, const mat<2, width, scalar_type>& m2) noexcept
    {
        return mat<2, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<3, height, scalar_type> operator*(const type& m1, const mat<3, width, scalar_type>& m2) noexcept
    {
        return mat<3, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<4, height, scalar_type> operator*(const type& m1, const mat<4, width, scalar_type>& m2) noexcept
    {
        return mat<4, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z),

            (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y) + (m1.columns[2].x * m2.columns[3].z),
            (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y) + (m1.columns[2].y * m2.columns[3].z),
            (m1.columns[0].z * m2.columns[3].x) + (m1.columns[1].z * m2.columns[3].y) + (m1.columns[2].z * m2.columns[3].z)
        );
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

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type IDENTITY() noexcept { return type(static_cast<T>(1)); }
    static VX_FORCE_INLINE constexpr type ZERO() noexcept { return type(static_cast<T>(0)); }
};

} // namespace math
} // namespace vx