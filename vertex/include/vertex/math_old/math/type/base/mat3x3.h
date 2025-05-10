#pragma once

#include "../../_priv/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct mat<3, 3, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = mat<3, 3, T>;

    using row_type = vec<3, T>;
    using col_type = vec<3, T>;
    using transpose_type = mat<3, 3, T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(9); }
    static VX_FORCE_INLINE constexpr size_t width() noexcept { return static_cast<size_t>(3); }
    static VX_FORCE_INLINE constexpr size_t height() noexcept { return static_cast<size_t>(3); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    col_type columns[3];

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

    VX_FORCE_INLINE constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    VX_FORCE_INLINE constexpr mat(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit mat(T scalar) noexcept
        : columns{ col_type(scalar, static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), scalar, static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), scalar) } {}

    VX_FORCE_INLINE constexpr mat(
        T x1, T y1, T z1,
        T x2, T y2, T z2,
        T x3, T y3, T z3
    ) noexcept
        : columns{ col_type(x1, y1, z1),
                   col_type(x2, y2, z2),
                   col_type(x3, y3, z3) } {}

    VX_FORCE_INLINE constexpr mat(const col_type& c1, const col_type& c2, const col_type& c3) noexcept
        : columns{ c1, c2, c3 } {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit mat(U scalar)
        : columns{ col_type(static_cast<T>(scalar), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scalar), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(scalar)) } {}

    template <
        typename X1, typename Y1, typename Z1,
        typename X2, typename Y2, typename Z2,
        typename X3, typename Y3, typename Z3,
        typename std::enable_if<
        std::is_arithmetic<X1>::value &&
        std::is_arithmetic<Y1>::value &&
        std::is_arithmetic<Z1>::value &&
        std::is_arithmetic<X2>::value &&
        std::is_arithmetic<Y2>::value &&
        std::is_arithmetic<Z2>::value &&
        std::is_arithmetic<X3>::value &&
        std::is_arithmetic<Y3>::value &&
        std::is_arithmetic<Z3>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr mat(
        X1 x1, Y1 y1, Z1 z1,
        X2 x2, Y2 y2, Z2 z2,
        X3 x3, Y3 y3, Z3 z3
    )
        : columns{ col_type(static_cast<T>(x1), static_cast<T>(y1), static_cast<T>(z1)),
                   col_type(static_cast<T>(x2), static_cast<T>(y2), static_cast<T>(z2)),
                   col_type(static_cast<T>(x3), static_cast<T>(y3), static_cast<T>(z3)) } {}

    template <typename C1, typename C2, typename C3>
    VX_FORCE_INLINE constexpr mat(const vec<3, C1>& c1, const vec<3, C2>& c2, const vec<3, C3>& c3)
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3) } {}

    template <size_t M, size_t N, typename U, typename std::enable_if<(M >= 3 && N >= 3), bool>::type = true>
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
    VX_FORCE_INLINE constexpr type& operator=(const mat<3, 3, U>& m)
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
    friend VX_FORCE_INLINE constexpr vec<3, T> operator*(const type& m, const vec<3, T>& v)
    {
        return vec<3, T>(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr vec<3, T> operator*(const vec<3, T>& v, const type& m)
    {
        return vec<3, T>(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y),
            (v.x * m.columns[0].z) + (v.y * m.columns[1].z) + (v.z * m.columns[2].z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr mat<2, 3, T> operator*(const type& m1, const mat<2, 3, T>& m2)
    {
        return mat<2, 3, T>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& m1, const type& m2)
    {
        return type(
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr mat<4, 3, T> operator*(const type& m1, const mat<4, 3, T>& m2)
    {
        return mat<4, 3, T>(
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr vec<2, T> operator/(const type& m, const vec<3, T>& v)
    {
        return inverse(m) * v;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr vec<2, T> operator/(const vec<3, T>& v, const type& m)
    {
        return v * inverse(m);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& m1, const type& m2)
    {
        return inverse(m1) * m2;
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
    VX_FORCE_INLINE constexpr type& operator*=(const type& m)
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(const type& m)
    {
        (*this) = (*this) / m;
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value&& std::is_same<U, bool>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator!() const
    {
        return type(!columns[0], !columns[1], !columns[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // rotation
    ///////////////////////////////////////////////////////////////////////////////

    // https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a 3x3 rotation matrix from an axis and an angle.
    ///
    /// @param axis The axis of rotation.
    /// @param angle The angle of rotation in radians.
    /// 
    /// @return A 3x3 rotation matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type from_axis_angle(const vec<3, T>& axis, T angle)
    {
        const vec<3, T> naxis = math::normalize(axis);

        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);
        const vec<3, T> t = (static_cast<T>(1) - cosa) * naxis;
        const vec<3, T> sina_naxis = sina * naxis;

        return type(
            (t.x * naxis.x) + cosa,
            (t.x * naxis.y) + sina_naxis.z,
            (t.x * naxis.z) - sina_naxis.y,

            (t.y * naxis.x) - sina_naxis.z,
            (t.y * naxis.y) + cosa,
            (t.y * naxis.z) + sina_naxis.x,

            (t.z * naxis.x) + sina_naxis.y,
            (t.z * naxis.y) - sina_naxis.x,
            (t.z * naxis.z) + cosa
        );
    }

    // https://ntrs.nasa.gov/api/citations/19770024290/downloads/19770024290.pdf

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a 3x3 rotation matrix from Euler angles.
    ///
    /// The rotation order is assumed to be X-Y-Z (roll, pitch, yaw).
    ///
    /// @param x The angle of rotation around the X-axis (roll).
    /// @param y The angle of rotation around the Y-axis (pitch).
    /// @param z The angle of rotation around the Z-axis (yaw).
    /// 
    /// @return A 3x3 rotation matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type from_euler_angles(T x, T y, T z)
    {
        const T ct1 = math::cos(x);
        const T ct2 = math::cos(y);
        const T ct3 = math::cos(z);

        const T st1 = math::sin(x);
        const T st2 = math::sin(y);
        const T st3 = math::sin(z);

        return type(
            +(ct2 * ct3),
            +(st1 * st2 * ct3) + (ct1 * st3),
            -(ct1 * st2 * ct3) + (st1 * st3),

            -(ct2 * st3),
            -(st1 * st2 * st3) + (ct1 * ct3),
            +(ct1 * st2 * st3) + (st1 * ct3),

            +(st2),
            -(st1 * ct2),
            +(ct1 * ct2)
        );
    }

    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a 3x3 rotation matrix from a quaternion.
    ///
    /// @param rotation The quaternion representing the rotation.
    /// 
    /// @return A 3x3 rotation matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value&& type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type from_quat(const quat_t<T>& rotation)
    {
        const quat_t<T> qn = math::normalize(rotation);

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qxz = qn.x * qn.z;
        const T qxy = qn.x * qn.y;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;
        const T qwz = qn.w * qn.z;

        return type(
            static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz),
            static_cast<T>(2) * (qxy + qwz),
            static_cast<T>(2) * (qxz - qwy),

            static_cast<T>(2) * (qxy - qwz),
            static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz),
            static_cast<T>(2) * (qyz + qwx),

            static_cast<T>(2) * (qxz + qwy),
            static_cast<T>(2) * (qyz - qwx),
            static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy)
        );
    }

    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Converts a 3x3 rotation matrix to a quaternion.
    ///
    /// @param m The 3x3 rotation matrix.
    /// 
    /// @return The quaternion representing the rotation.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value&& type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr quat_t<T> to_quat(const type& m)
    {
        const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

        if (trace > static_cast<T>(0))
        {
            const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return quat_t<T>(
                s * static_cast<T>(0.25),
                (m.columns[2].y - m.columns[1].z) * invs,
                (m.columns[0].z - m.columns[2].x) * invs,
                (m.columns[1].x - m.columns[0].y) * invs
            );
        }
        else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return quat_t<T>(
                (m.columns[2].y - m.columns[1].z) * invs,
                s * static_cast<T>(0.25),
                (m.columns[0].y + m.columns[1].x) * invs,
                (m.columns[0].z + m.columns[2].x) * invs
            );
        }
        else if (m.columns[1].y > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return quat_t<T>(
                (m.columns[2].x - m.columns[0].z) * invs,
                (m.columns[0].y + m.columns[1].x) * invs,
                s * static_cast<T>(0.25),
                (m.columns[1].z + m.columns[2].y) * invs
            );
        }
        else
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return quat_t<T>(
                (m.columns[1].x - m.columns[0].y) * invs,
                (m.columns[0].z + m.columns[2].x) * invs,
                (m.columns[1].z + m.columns[2].y) * invs,
                s * static_cast<T>(0.25)
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // scale
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a 3x3 scaling matrix.
    ///
    /// @param scale A 3D vector representing the scaling factors along the
    /// x, y, and z axes.
    /// 
    /// @return A 3x3 scaling matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type make_scale(const vec<3, T>& scale)
    {
        return type(
            scale.x,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            scale.y,
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            scale.z
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the scaling factors from a 3x3 scaling matrix.
    ///
    /// @param m The 3x3 scaling matrix.
    /// 
    /// @return A 3D vector representing the scaling factors along the x, y, and z axes.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr vec<3, T> get_scale(const type& m)
    {
        return vec<3, T>(
            math::length(m.columns[0]),
            math::length(m.columns[1]),
            math::length(m.columns[2])
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // look at
    ///////////////////////////////////////////////////////////////////////////////

    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a left-handed 3x3 view matrix for a look-at transformation,
    /// given the eye position, target position, and up vector.
    ///
    /// @param eye The position of the camera.
    /// @param target The position the camera is looking at.
    /// @param up The up vector of the camera (default is the positive y-axis).
    /// 
    /// @return A left-handed 3x3 view matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type make_look_at_lh(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
        const vec<3, T> z = math::normalize(target - eye);
        const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
        const vec<3, T> y = math::cross(z, x);

        return type(x, y, z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a right-handed 3x3 view matrix for a look-at transformation,
    /// given the eye position, target position, and up vector.
    ///
    /// @param eye The position of the camera.
    /// @param target The position the camera is looking at.
    /// @param up The up vector of the camera (default is the positive y-axis).
    /// 
    /// @return A right-handed 3x3 view matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type make_look_at_rh(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
        const vec<3, T> z = math::normalize(eye - target);
        const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
        const vec<3, T> y = math::cross(z, x);

        return type(x, y, z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a 3x3 view matrix for a look-at transformation, given the
    /// eye position, target position, and up vector.
    ///
    /// The handedness of the matrix (left-handed or right-handed) is determined
    /// based on the configuration specified in VX_CONFIG_CLIP_CONTROL.
    ///
    /// @param eye The position of the camera.
    /// @param target The position the camera is looking at.
    /// @param up The up vector of the camera (default is the positive y-axis).
    /// 
    /// @return A 3x3 view matrix.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type make_look_at(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return make_look_at_lh(eye, target, up);
#else
        return make_look_at_rh(eye, target, up);
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type IDENTITY() { return type(static_cast<T>(1)); }
    static VX_FORCE_INLINE constexpr type ZERO() { return type(static_cast<T>(0)); }

};

VX_PACK_POP()

}
}