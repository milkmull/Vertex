#pragma once

#include "../../_priv/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct mat<3, 2, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = mat<3, 2, T>;

    using row_type = vec<3, T>;
    using col_type = vec<2, T>;
    using transpose_type = mat<2, 3, T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(6); }
    static VX_FORCE_INLINE constexpr size_t width() noexcept { return static_cast<size_t>(3); }
    static VX_FORCE_INLINE constexpr size_t height() noexcept { return static_cast<size_t>(2); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    col_type columns[3];

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)) } {}

    VX_FORCE_INLINE constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    VX_FORCE_INLINE constexpr mat(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit mat(T scalar) noexcept
        : columns{ col_type(scalar),
                   col_type(scalar),
                   col_type(scalar) } {}

    VX_FORCE_INLINE constexpr mat(
        T x1, T y1,
        T x2, T y2,
        T x3, T y3
    ) noexcept
        : columns{ col_type(x1, y1),
                   col_type(x2, y2),
                   col_type(x3, y3) } {}

    VX_FORCE_INLINE constexpr mat(const col_type& c1, const col_type& c2, const col_type& c3) noexcept
        : columns{ c1, c2, c3 } {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit mat(U scalar)
        : columns{ col_type(static_cast<T>(scalar)),
                   col_type(static_cast<T>(scalar)),
                   col_type(static_cast<T>(scalar)) } {}

    template <
        typename X1, typename Y1,
        typename X2, typename Y2,
        typename X3, typename Y3,
        typename std::enable_if<
        std::is_arithmetic<X1>::value &&
        std::is_arithmetic<Y1>::value &&
        std::is_arithmetic<X2>::value &&
        std::is_arithmetic<Y2>::value &&
        std::is_arithmetic<X3>::value &&
        std::is_arithmetic<Y3>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr mat(
        X1 x1, Y1 y1,
        X2 x2, Y2 y2,
        X3 x3, Y3 y3
    )
        : columns{ col_type(static_cast<T>(x1), static_cast<T>(y1)),
                   col_type(static_cast<T>(x2), static_cast<T>(y2)),
                   col_type(static_cast<T>(x3), static_cast<T>(y3)) } {}

    template <typename C1, typename C2, typename C3>
    VX_FORCE_INLINE constexpr mat(const vec<2, C1>& c1, const vec<2, C2>& c2, const vec<2, C3>& c3)
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3) } {}

    template <size_t M, size_t N, typename U, typename std::enable_if<(M >= 3 && N >= 2), bool>::type = true>
    VX_FORCE_INLINE constexpr explicit mat(const mat<M, N, U>& m)
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]),
                   static_cast<col_type>(m.columns[2]) } {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~mat() noexcept = default;

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

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const mat<3, 2, U>& m)
    {
        columns[0] = static_cast<col_type>(m.columns[0]);
        columns[1] = static_cast<col_type>(m.columns[1]);
        columns[2] = static_cast<col_type>(m.columns[2]);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr col_type& operator[](size_t i)
    {
        assert(i < 3);
        return columns[i];
    }

    VX_FORCE_INLINE constexpr const col_type& operator[](size_t i) const
    {
        assert(i < 3);
        return columns[i];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& m1, const type& m2)
    {
        return (m1.columns[0] == m2.columns[0])
            && (m1.columns[1] == m2.columns[1])
            && (m1.columns[2] == m2.columns[2]);
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& m1, const type& m2)
    {
        return !(m1 == m2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator+() const
    {
        return type(+columns[0], +columns[1], +columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-columns[0], -columns[1], -columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // increment (++)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator++()
    {
        columns[0]++;
        columns[1]++;
        columns[2]++;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator--()
    {
        columns[0]--;
        columns[1]--;
        columns[2]--;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(const type& m, T scalar)
    {
        return type(m.columns[0] + scalar, m.columns[1] + scalar, m.columns[2] + scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(T scalar, const type& m)
    {
        return type(scalar + m.columns[0], scalar + m.columns[1], scalar + m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(const type& m1, const type& m2)
    {
        return type(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1], m1.columns[2] + m2.columns[2]);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& m, T scalar)
    {
        return type(m.columns[0] - scalar, m.columns[1] - scalar, m.columns[2] - scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(T scalar, const type& m)
    {
        return type(scalar - m.columns[0], scalar - m.columns[1], scalar - m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& m1, const type& m2)
    {
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1], m1.columns[2] - m1.columns[2]);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& m, T scalar)
    {
        return type(m.columns[0] * scalar, m.columns[1] * scalar, m.columns[2] * scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(T scalar, const type& m)
    {
        return type(scalar * m.columns[0], scalar * m.columns[1], scalar * m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr vec<2, T> operator*(const type& m, const vec<3, T>& v)
    {
        return vec<2, T>(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr vec<2, T> operator*(const vec<3, T>& v, const type& m)
    {
        return vec<2, T>(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr mat<2, 2, T> operator*(const type& m1, const mat<2, 3, T>& m2)
    {
        return mat<2, 2, T>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& m1, const mat<3, 3, T>& m2)
    {
        return type(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr mat<4, 3, T> operator*(const type& m1, const mat<4, 3, T>& m2)
    {
        return mat<4, 3, T>(
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& m, T scalar)
    {
        return type(m.columns[0] / scalar, m.columns[1] / scalar, m.columns[2] / scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(T scalar, const type& m)
    {
        return type(scalar / m.columns[0], scalar / m.columns[1], scalar / m.columns[2]);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& m, T scalar)
    {
        return type(m.columns[0] % scalar, m.columns[1] % scalar, m.columns[2] % scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(T scalar, const type& m)
    {
        return type(scalar % m.columns[0], scalar % m.columns[1], scalar % m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& m1, const type& m2)
    {
        return type(m1.columns[0] % m2.columns[0], m1.columns[1] % m2.columns[1], m1.columns[2] % m1.columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& m, T scalar)
    {
        return type(m.columns[0] & scalar, m.columns[1] & scalar, m.columns[2] & scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(T scalar, const type& m)
    {
        return type(scalar & m.columns[0], scalar & m.columns[1], scalar & m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& m1, const type& m2)
    {
        return type(m1.columns[0] & m2.columns[0], m1.columns[1] & m2.columns[1], m1.columns[2] & m1.columns[2]);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& m, T scalar)
    {
        return type(m.columns[0] | scalar, m.columns[1] | scalar, m.columns[2] | scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(T scalar, const type& m)
    {
        return type(scalar | m.columns[0], scalar | m.columns[1], scalar | m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& m1, const type& m2)
    {
        return type(m1.columns[0] | m2.columns[0], m1.columns[1] | m2.columns[1], m1.columns[2] | m1.columns[2]);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& m, T scalar)
    {
        return type(m.columns[0] ^ scalar, m.columns[1] ^ scalar, m.columns[2] ^ scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(T scalar, const type& m)
    {
        return type(scalar ^ m.columns[0], scalar ^ m.columns[1], scalar ^ m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& m1, const type& m2)
    {
        return type(m1.columns[0] ^ m2.columns[0], m1.columns[1] ^ m2.columns[1], m1.columns[2] ^ m1.columns[2]);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& m, T scalar)
    {
        return type(m.columns[0] << scalar, m.columns[1] << scalar, m.columns[2] << scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(T scalar, const type& m)
    {
        return type(scalar << m.columns[0], scalar << m.columns[1], scalar << m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& m1, const type& m2)
    {
        return type(m1.columns[0] << m2.columns[0], m1.columns[1] << m2.columns[1], m1.columns[2] << m2.columns[2]);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& m, T scalar)
    {
        return type(m.columns[0] >> scalar, m.columns[1] >> scalar, m.columns[2] >> scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(T scalar, const type& m)
    {
        return type(scalar >> m.columns[0], scalar >> m.columns[1], scalar >> m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& m1, const type& m2)
    {
        return type(m1.columns[0] >> m2.columns[0], m1.columns[1] >> m2.columns[1], m1.columns[2] >> m2.columns[2]);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator~() const
    {
        return type(~columns[0], ~columns[1], ~columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(T scalar)
    {
        columns[0] += scalar;
        columns[1] += scalar;
        columns[2] += scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(const type& m)
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(T scalar)
    {
        columns[0] -= scalar;
        columns[1] -= scalar;
        columns[2] -= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(const type& m)
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(T scalar)
    {
        columns[0] *= scalar;
        columns[1] *= scalar;
        columns[2] *= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(const mat<3, 3, T>& m)
    {
        (*this) = (*this) * m;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(T scalar)
    {
        columns[0] /= scalar;
        columns[1] /= scalar;
        columns[2] /= scalar;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(T scalar)
    {
        columns[0] %= scalar;
        columns[1] %= scalar;
        columns[2] %= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(const type& m)
    {
        columns[0] %= m.columns[0];
        columns[1] %= m.columns[1];
        columns[2] %= m.columns[2];
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(T scalar)
    {
        columns[0] &= scalar;
        columns[1] &= scalar;
        columns[2] &= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(const type& m)
    {
        columns[0] &= m.columns[0];
        columns[1] &= m.columns[1];
        columns[2] &= m.columns[2];
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(T scalar)
    {
        columns[0] |= scalar;
        columns[1] |= scalar;
        columns[2] |= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(const type& m)
    {
        columns[0] |= m.columns[0];
        columns[1] |= m.columns[1];
        columns[2] |= m.columns[2];
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(T scalar)
    {
        columns[0] ^= scalar;
        columns[1] ^= scalar;
        columns[2] ^= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(const type& m)
    {
        columns[0] ^= m.columns[0];
        columns[1] ^= m.columns[1];
        columns[2] ^= m.columns[2];
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(T scalar)
    {
        columns[0] <<= scalar;
        columns[1] <<= scalar;
        columns[2] <<= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(const type& m)
    {
        columns[0] <<= m.columns[0];
        columns[1] <<= m.columns[1];
        columns[2] <<= m.columns[2];
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(T scalar)
    {
        columns[0] >>= scalar;
        columns[1] >>= scalar;
        columns[2] >>= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(const type& m)
    {
        columns[0] >>= m.columns[0];
        columns[1] >>= m.columns[1];
        columns[2] >>= m.columns[2];
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& m, T scalar)
    {
        return type(m.columns[0] && scalar, m.columns[1] && scalar, m.columns[2] && scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(T scalar, const type& m)
    {
        return type(scalar && m.columns[0], scalar && m.columns[1], scalar && m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& m1, const type& m2)
    {
        return type(m1.columns[0] && m2.columns[0], m1.columns[1] && m2.columns[1], m1.columns[2] && m2.columns[2]);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& m, T scalar)
    {
        return type(m.columns[0] || scalar, m.columns[1] || scalar, m.columns[2] || scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(T scalar, const type& m)
    {
        return type(scalar || m.columns[0], scalar || m.columns[1], scalar || m.columns[2]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& m1, const type& m2)
    {
        return type(m1.columns[0] || m2.columns[0], m1.columns[1] || m2.columns[1], m1.columns[2] || m2.columns[2]);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator!() const
    {
        return type(!columns[0], !columns[1], !columns[2]);
    }

};

VX_PACK_POP()

}
}