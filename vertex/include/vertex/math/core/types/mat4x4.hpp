#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/math/core/types/base.hpp"

#include "vertex/math/simd/mat4f.hpp"

namespace vx {
namespace math {

template <typename T>
struct alignas(simd::vec<4, T>::calulate_alignment()) mat<4, 4, T>
{
#if defined(VX_MATH_SIMD_ENABLED)

#   define _SIMD_OVERLOAD(cond) template <typename _T = scalar_type, VX_MATH_REQ( (simd::vec<4, _T>::cond))>
#   define _SIMD_FALLBACK(cond) template <typename _T = scalar_type, VX_MATH_REQ(!(simd::vec<4, _T>::cond))>

#else

#   define _SIMD_OVERLOAD(cond) template <typename _T = scalar_type, VX_MATH_REQ(!(is_same<_T, _T>::value))>
#   define _SIMD_FALLBACK(cond)

#endif

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    static constexpr size_t width = 4;
    static constexpr size_t height = 4;

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
        : columns{ col_type(static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(1), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(1)) } {}

    VX_FORCE_INLINE constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2], m.columns[3] } {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit mat(scalar_type scalar) noexcept
        : columns{ col_type(scalar, static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), scalar, static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), scalar, static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0), scalar) } {}

    VX_FORCE_INLINE constexpr mat(
        scalar_type x1, scalar_type y1, scalar_type z1, scalar_type w1,
        scalar_type x2, scalar_type y2, scalar_type z2, scalar_type w2,
        scalar_type x3, scalar_type y3, scalar_type z3, scalar_type w3,
        scalar_type x4, scalar_type y4, scalar_type z4, scalar_type w4
    ) noexcept
        : columns{ col_type(x1, y1, z1, w1),
                   col_type(x2, y2, z2, w2),
                   col_type(x3, y3, z3, w3),
                   col_type(x4, y4, z4, w4) } {}

    VX_FORCE_INLINE constexpr mat(
        const col_type& c1,
        const col_type& c2,
        const col_type& c3,
        const col_type& c4
    ) noexcept
        : columns{ c1, c2, c3, c4 } {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VX_MATH_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit mat(U scalar) noexcept
        : columns{ col_type(static_cast<scalar_type>(scalar), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(scalar), static_cast<scalar_type>(0), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(scalar), static_cast<scalar_type>(0)),
                   col_type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(scalar)) } {}

    template <
        typename X1, typename Y1, typename Z1, typename W1,
        typename X2, typename Y2, typename Z2, typename W2,
        typename X3, typename Y3, typename Z3, typename W3,
        typename X4, typename Y4, typename Z4, typename W4
    >
    VX_FORCE_INLINE constexpr mat(
        X1 x1, Y1 y1, Z1 z1, W1 w1,
        X2 x2, Y2 y2, Z2 z2, W2 w2,
        X3 x3, Y3 y3, Z3 z3, W3 w3,
        X4 x4, Y4 y4, Z4 z4, W4 w4
    ) noexcept
        : columns{ col_type(static_cast<scalar_type>(x1), static_cast<scalar_type>(y1), static_cast<scalar_type>(z1), static_cast<scalar_type>(w1)),
                   col_type(static_cast<scalar_type>(x2), static_cast<scalar_type>(y2), static_cast<scalar_type>(z2), static_cast<scalar_type>(w2)),
                   col_type(static_cast<scalar_type>(x3), static_cast<scalar_type>(y3), static_cast<scalar_type>(z3), static_cast<scalar_type>(w3)),
                   col_type(static_cast<scalar_type>(x4), static_cast<scalar_type>(y4), static_cast<scalar_type>(z4), static_cast<scalar_type>(w4)) } {}

    template <typename C1, typename C2, typename C3, typename C4>
    VX_FORCE_INLINE constexpr mat(
        const vec<4, C1>& c1,
        const vec<4, C2>& c2,
        const vec<4, C3>& c3,
        const vec<4, C4>& c4
    ) noexcept
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3),
                   static_cast<col_type>(c4) } {}

    template <size_t M, size_t N, typename U, VX_MATH_REQ(M >= width && N >= height)>
    VX_FORCE_INLINE constexpr explicit mat(const mat<M, N, U>& m) noexcept
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]),
                   static_cast<col_type>(m.columns[2]),
                   static_cast<col_type>(m.columns[3]) } {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        columns[3] = m.columns[3];
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const mat<width, height, U>& m) noexcept
    {
        columns[0] = static_cast<col_type>(m.columns[0]);
        columns[1] = static_cast<col_type>(m.columns[1]);
        columns[2] = static_cast<col_type>(m.columns[2]);
        columns[3] = static_cast<col_type>(m.columns[3]);
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
            && (m1.columns[2] == m2.columns[2])
            && (m1.columns[3] == m2.columns[3]);
    }

    // not equal (!=)

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& m1, const type& m2) noexcept
    {
        return (m1.columns[0] != m2.columns[0])
            || (m1.columns[1] != m2.columns[1])
            || (m1.columns[2] != m2.columns[2])
            || (m1.columns[3] != m2.columns[3]);
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] > m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] > m2.columns[1];
        if (m1.columns[2] != m2.columns[2]) return m1.columns[2] > m2.columns[2];
        return m1.columns[3] > m2.columns[3];
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] < m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] < m2.columns[1];
        if (m1.columns[2] != m2.columns[2]) return m1.columns[2] < m2.columns[2];
        return m1.columns[3] < m2.columns[3];
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] > m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] > m2.columns[1];
        if (m1.columns[2] != m2.columns[2]) return m1.columns[2] > m2.columns[2];
        return m1.columns[3] >= m2.columns[3];
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& m1, const type& m2) noexcept
    {
        if (m1.columns[0] != m2.columns[0]) return m1.columns[0] < m2.columns[0];
        if (m1.columns[1] != m2.columns[1]) return m1.columns[1] < m2.columns[1];
        if (m1.columns[2] != m2.columns[2]) return m1.columns[2] < m2.columns[2];
        return m1.columns[3] <= m2.columns[3];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+columns[0], +columns[1], +columns[2], +columns[3]);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-columns[0], -columns[1], -columns[2], -columns[3]);
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
        columns[3]++;
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
        columns[3]--;
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

    _SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] + scalar, m.columns[1] + scalar, m.columns[2] + scalar, m.columns[3] + scalar);
    }

    _SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar + m.columns[0], scalar + m.columns[1], scalar + m.columns[2], scalar + m.columns[3]);
    }

    _SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& m1, const type& m2) noexcept
    {
        return type(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1], m1.columns[2] + m2.columns[2], m1.columns[3] + m2.columns[3]);
    }

    // subtraction (-)

    _SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] - scalar, m.columns[1] - scalar, m.columns[2] - scalar, m.columns[3] - scalar);
    }

    _SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar - m.columns[0], scalar - m.columns[1], scalar - m.columns[2], scalar - m.columns[3]);
    }

    _SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& m1, const type& m2) noexcept
    {
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1], m1.columns[2] - m2.columns[2], m1.columns[3] - m2.columns[3]);
    }

    // multiplication (*)

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] * scalar, m.columns[1] * scalar, m.columns[2] * scalar, m.columns[3] * scalar);
    }

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar * m.columns[0], scalar * m.columns[1], scalar * m.columns[2], scalar * m.columns[3]);
    }

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr vec<height, scalar_type> operator*(const type& m, const vec<width, scalar_type>& v) noexcept
    {
        return vec<height, scalar_type>(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + (m.columns[3].x * v.w),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + (m.columns[3].y * v.w),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + (m.columns[3].z * v.w),
            (m.columns[0].w * v.x) + (m.columns[1].w * v.y) + (m.columns[2].w * v.z) + (m.columns[3].w * v.w)
        );
    }

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr vec<width, scalar_type> operator*(const vec<height, scalar_type>& v, const type& m) noexcept
    {
        return vec<width, scalar_type>(
            (v.x * m.columns[0].x) + (v.y * m.columns[0].y) + (v.z * m.columns[0].z) + (v.w * m.columns[0].w),
            (v.x * m.columns[1].x) + (v.y * m.columns[1].y) + (v.z * m.columns[1].z) + (v.w * m.columns[1].w),
            (v.x * m.columns[2].x) + (v.y * m.columns[2].y) + (v.z * m.columns[2].z) + (v.w * m.columns[2].w),
            (v.x * m.columns[3].x) + (v.y * m.columns[3].y) + (v.z * m.columns[3].z) + (v.w * m.columns[3].w)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<2, height, scalar_type> operator*(const type& m1, const mat<2, width, scalar_type>& m2) noexcept
    {
        return mat<2, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w)
        );
    }

    friend VX_FORCE_INLINE constexpr mat<3, height, scalar_type> operator*(const type& m1, const mat<3, width, scalar_type>& m2) noexcept
    {
        return mat<3, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z) + (m1.columns[3].x * m2.columns[2].w),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z) + (m1.columns[3].y * m2.columns[2].w),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z) + (m1.columns[3].z * m2.columns[2].w),
            (m1.columns[0].w * m2.columns[2].x) + (m1.columns[1].w * m2.columns[2].y) + (m1.columns[2].w * m2.columns[2].z) + (m1.columns[3].w * m2.columns[2].w)
        );
    }

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE mat<4, height, scalar_type> operator*(const type& m1, const mat<4, width, scalar_type>& m2) noexcept
    {
        return mat<4, height, scalar_type>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z) + (m1.columns[3].x * m2.columns[2].w),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z) + (m1.columns[3].y * m2.columns[2].w),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z) + (m1.columns[3].z * m2.columns[2].w),
            (m1.columns[0].w * m2.columns[2].x) + (m1.columns[1].w * m2.columns[2].y) + (m1.columns[2].w * m2.columns[2].z) + (m1.columns[3].w * m2.columns[2].w),

            (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y) + (m1.columns[2].x * m2.columns[3].z) + (m1.columns[3].x * m2.columns[3].w),
            (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y) + (m1.columns[2].y * m2.columns[3].z) + (m1.columns[3].y * m2.columns[3].w),
            (m1.columns[0].z * m2.columns[3].x) + (m1.columns[1].z * m2.columns[3].y) + (m1.columns[2].z * m2.columns[3].z) + (m1.columns[3].z * m2.columns[3].w),
            (m1.columns[0].w * m2.columns[3].x) + (m1.columns[1].w * m2.columns[3].y) + (m1.columns[2].w * m2.columns[3].z) + (m1.columns[3].w * m2.columns[3].w)
        );
    }

    // division (/)

    _SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(const type& m, scalar_type scalar) noexcept
    {
        return type(m.columns[0] / scalar, m.columns[1] / scalar, m.columns[2] / scalar, m.columns[3] / scalar);
    }

    _SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& m) noexcept
    {
        return type(scalar / m.columns[0], scalar / m.columns[1], scalar / m.columns[2], scalar / m.columns[3]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        columns[0] += scalar;
        columns[1] += scalar;
        columns[2] += scalar;
        columns[3] += scalar;
        return *this;
    }

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator+=(const type& m) noexcept
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        columns[3] += m.columns[3];
        return *this;
    }

    // subtraction (-=)

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        columns[0] -= scalar;
        columns[1] -= scalar;
        columns[2] -= scalar;
        columns[3] -= scalar;
        return *this;
    }

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator-=(const type& m) noexcept
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        columns[3] -= m.columns[3];
        return *this;
    }

    // multiplication (*=)

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        columns[0] *= scalar;
        columns[1] *= scalar;
        columns[2] *= scalar;
        columns[3] *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const mat<width, width, scalar_type>& m) noexcept
    {
        (*this) = (*this) * m;
        return *this;
    }

    // division (/=)

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        columns[0] /= scalar;
        columns[1] /= scalar;
        columns[2] /= scalar;
        columns[3] /= scalar;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type identity() noexcept { return type(static_cast<T>(1)); }
    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<T>(0)); }

    ///////////////////////////////////////////////////////////////////////////////
    // simd conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    using simd_type = typename simd::mat<width, height, scalar_type>;
    using simd_data_type = typename simd_type::data_type;

    VX_FORCE_INLINE mat(const simd_data_type& d) noexcept
        : mat(*(const mat*)(&d)) {}

    VX_FORCE_INLINE operator simd_data_type& () noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(simd_data_type*)(this);
    }

    VX_FORCE_INLINE operator const simd_data_type& () const noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(const simd_data_type*)(this);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    _SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& m, scalar_type scalar) noexcept
    {
        type out;
        simd_type::add(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& m) noexcept
    {
        type out;
        simd_type::add(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& m1, const type& m2) noexcept
    {
        type out;
        simd_type::add(m1, m2, out);
        return out;
    }

    // subtraction (-)

    _SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& m, scalar_type scalar) noexcept
    {
        type out;
        simd_type::sub(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& m) noexcept
    {
        type out;
        simd_type::sub(scalar, m, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& m1, const type& m2) noexcept
    {
        type out;
        simd_type::sub(m1, m2, out);
        return out;
    }

    // multiplication (*)

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(const type& m, scalar_type scalar) noexcept
    {
        type out;
        simd_type::mul(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& m) noexcept
    {
        type out;
        simd_type::mul(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr vec<height, scalar_type> operator*(const type& m, const vec<width, scalar_type>& v) noexcept
    {
        return simd_type::mul(m, v);
    }

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr vec<width, scalar_type> operator*(const vec<height, scalar_type>& v, const type& m) noexcept
    {
        return simd_type::mul(v, m);
    }

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr mat<4, height, scalar_type> operator*(const type& m1, const mat<4, width, scalar_type>& m2) noexcept
    {
        type out;
        simd_type::mul(m1, m2, out);
        return out;
    }

    // division (/)

    _SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(const type& m, scalar_type scalar) noexcept
    {
        type out;
        simd_type::div(m, scalar, out);
        return out;
    }

    _SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& m) noexcept
    {
        type out;
        simd_type::div(scalar, m, out);
        return out;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    _SIMD_OVERLOAD(HAVE_ADD)
    VX_FORCE_INLINE type& operator+=(scalar_type scalar) noexcept
    {
        (*this) = (*this) + scalar;
        return *this;
    }

    _SIMD_OVERLOAD(HAVE_ADD)
    VX_FORCE_INLINE type& operator+=(const type& m) noexcept
    {
        (*this) = (*this) + m;
        return *this;
    }

    // subtraction (-=)

    _SIMD_OVERLOAD(HAVE_SUB)
    VX_FORCE_INLINE type& operator-=(scalar_type scalar) noexcept
    {
        (*this) = (*this) - scalar;
        return *this;
    }

    _SIMD_OVERLOAD(HAVE_SUB)
    VX_FORCE_INLINE type& operator-=(const type& m) noexcept
    {
        (*this) = (*this) - m;
        return *this;
    }

    // multiplication (*=)

    _SIMD_OVERLOAD(HAVE_MUL)
    VX_FORCE_INLINE type& operator*=(scalar_type scalar) noexcept
    {
        (*this) = (*this) * scalar;
        return *this;
    }

    // division (/=)

    _SIMD_OVERLOAD(HAVE_DIV)
    VX_FORCE_INLINE type& operator/=(scalar_type scalar) noexcept
    {
        (*this) = (*this) / scalar;
        return *this;
    }

#   undef _SIMD_OVERLOAD
#   undef _SIMD_FALLBACK
};

} // namespace math
} // namespace vx