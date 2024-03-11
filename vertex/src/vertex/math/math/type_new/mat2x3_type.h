#pragma once

#include "../detail/base_types.h"
#include "scaler.h"

namespace vx {
namespace math {

template <typename T>
struct mat<2, 3, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = mat<2, 3, T>;

    using row_type = vec<2, T>;
    using col_type = vec<3, T>;
    using transpose_type = mat<3, 2, T>;

    static inline constexpr size_t size() noexcept { return 6; }
    static inline constexpr size_t width() noexcept { return 2; }
    static inline constexpr size_t height() noexcept { return 3; }

    // =============== data ===============

    col_type columns[2];

    // =============== implicit constructors ===============

    inline constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)) } {}

    inline constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1] } {}

    inline constexpr mat(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit mat(T scaler) noexcept
        : columns{ col_type(scaler),
                   col_type(scaler) } {}

    inline constexpr mat(
        T x1, T y1, T z1,
        T x2, T y2, T z2
    ) noexcept
        : columns{ col_type(x1, y1, z1), col_type(x2, y2, z2) } {}

    inline constexpr mat(const col_type& c1, const col_type& c2) noexcept
        : columns{ c1, c2 } {}

    // =============== conversion matrix constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler)),
                   col_type(static_cast<T>(scaler)) } {}

    template <
        typename X1, typename Y1, typename Z1,
        typename X2, typename Y2, typename Z2,
        typename std::enable_if<
        std::is_arithmetic<X1>::value &&
        std::is_arithmetic<Y1>::value &&
        std::is_arithmetic<Z1>::value &&
        std::is_arithmetic<X2>::value &&
        std::is_arithmetic<Y2>::value &&
        std::is_arithmetic<Z2>::value, bool>::type = true>
    inline constexpr mat(
        X1 x1, Y1 y1, Z1 z1,
        X2 x2, Y2 y2, Z2 z2
    )
        : columns{ col_type(static_cast<T>(x1), static_cast<T>(y1), static_cast<T>(z1)),
                   col_type(static_cast<T>(x2), static_cast<T>(y2), static_cast<T>(z2)) } {}

    template <typename C1, typename C2>
    inline constexpr mat(const vec<3, C1>& c1, const vec<3, C2>& c2)
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2) } {}

    template <typename T>
    inline constexpr explicit mat(const mat<2, 3, T>& m)
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]) } {}

    // =============== destructor ===============

    ~mat() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr col_type& operator[](size_t i)
    {
        assert(i < 2);
        return columns[i];
    }

    inline constexpr const col_type& operator[](size_t i) const
    {
        assert(i < 2);
        return columns[i];
    }

};

// =============== boolean operators ===============

template <typename T>
inline constexpr bool operator==(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return (m1.columns[0] == m2.columns[0])
        && (m1.columns[1] == m2.columns[1]);
}

template <typename T>
inline constexpr bool operator!=(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return !(m1 == m2);
}

// =============== unary constant operators ===============

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator+(const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(+m.columns[0], +m.columns[1]);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator-(const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(-m.columns[0], -m.columns[1]);
}

// =============== incrememnt and decrement operators ===============

// increment (++)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator++(mat<2, 3, T>& m)
{
    m.columns[0]++;
    m.columns[1]++;
    return m;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator++(mat<2, 3, T>& m, int)
{
    mat<2, 3, T> result(m);
    ++m;
    return result;
}

// decrement (--)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator--(mat<2, 3, T>& m)
{
    m.columns[0]--;
    m.columns[1]--;
    return m;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator--(mat<2, 3, T>& m, int)
{
    mat<2, 3, T> result(m);
    --m;
    return result;
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator+(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] + scaler, m.columns[1] + scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator+(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler + m.columns[0], scaler + m.columns[1]);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator+(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1]);
}

// subtraction (-)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator-(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] - scaler, m.columns[1] - scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator-(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler - m.columns[0], scaler - m.columns[1]);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator-(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1]);
}

// multiplication (*)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator*(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] * scaler, m.columns[1] * scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator*(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler * m.columns[0], scaler * m.columns[1]);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator*(const mat<2, 3, T>& m, const vec<2, T>& v)
{
    return vec<3, T>(
        (m.columns[0].x * v.x) + (m.columns[1].x * v.y),
        (m.columns[0].y * v.x) + (m.columns[1].y * v.y),
        (m.columns[0].z * v.x) + (m.columns[1].z * v.y)
    );
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator*(const vec<2, T>& v, const mat<2, 3, T>& m)
{
    return vec<3, T>(
        (v.x * m.columns[0].x) + (v.y * m.columns[1].x),
        (v.x * m.columns[0].y) + (v.y * m.columns[1].y),
        (v.x * m.columns[0].z) + (v.y * m.columns[1].z)
    );
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator*(const mat<2, 3, T>& m1, const mat<2, 2, T>& m2)
{
    return mat<2, 3, T>(
        (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y),
        (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y),
        (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y),

        (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y),
        (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y),
        (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y)
    );
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<3, 3, T> operator*(const mat<2, 3, T>& m1, const mat<3, 2, T>& m2)
{
    return mat<3, 3, T>(
        (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y),
        (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y),
        (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y),

        (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y),
        (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y),
        (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y),

        (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y),
        (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y),
        (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y)
    );
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<4, 3, T> operator*(const mat<2, 3, T>& m1, const mat<4, 2, T>& m2)
{
    return mat<4, 3, T>(
        (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y),
        (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y),
        (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y),

        (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y),
        (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y),
        (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y),

        (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y),
        (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y),
        (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y),

        (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y),
        (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y),
        (m1.columns[0].z * m2.columns[3].x) + (m1.columns[1].z * m2.columns[3].y)
    );
}

// division (/)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator/(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] / scaler, m.columns[1] / scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator/(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler / m.columns[0], scaler / m.columns[1]);
}

// modulo (%)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator%(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] % scaler, m.columns[1] % scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator%(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler % m.columns[0], scaler % m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator%(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] % m2.columns[0], m1.columns[1] % m2.columns[1]);
}

// =============== binary bit operators ===============

// and (&)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] & scaler, m.columns[1] & scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler & m.columns[0], scaler & m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] & m2.columns[0], m1.columns[1] & m2.columns[1]);
}

// or (|)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator|(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] | scaler, m.columns[1] | scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator|(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler | m.columns[0], scaler | m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator|(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] | m2.columns[0], m1.columns[1] | m2.columns[1]);
}

// xor (^)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator^(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] ^ scaler, m.columns[1] ^ scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator^(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler ^ m.columns[0], scaler ^ m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator^(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] ^ m2.columns[0], m1.columns[1] ^ m2.columns[1]);
}

// left shift (<<)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator<<(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] << scaler, m.columns[1] << scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator<<(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler << m.columns[0], scaler << m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator<<(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] << m2.columns[0], m1.columns[1] << m2.columns[1]);
}

// right shift (>>)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator>>(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] >> scaler, m.columns[1] >> scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator>>(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler >> m.columns[0], scaler >> m.columns[1]);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator>>(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] >> m2.columns[0], m1.columns[1] >> m2.columns[1]);
}

// not (~)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator~(const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(~m.columns[0], ~m.columns[1]);
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator+=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] += scaler;
    m.columns[1] += scaler;
    return m;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator+=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] += m2.columns[0];
    m1.columns[1] += m2.columns[1];
    return m1;
}

// subtraction (-=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator-=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] -= scaler;
    m.columns[1] -= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator-=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] -= m2.columns[0];
    m1.columns[1] -= m2.columns[1];
    return m1;
}

// multiplication (*=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator*=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] *= scaler;
    m.columns[1] *= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator*=(mat<2, 3, T>& m1, const mat<2, 2, T>& m2)
{
    m1 = m1 * m2;
    return m1;
}

// division (/=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator/=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] /= scaler;
    m.columns[1] /= scaler;
    return m;
}

// modulo (%=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator%=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] %= scaler;
    m.columns[1] %= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator%=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] %= m2.columns[0];
    m1.columns[1] %= m2.columns[1];
    return m1;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator&=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] &= scaler;
    m.columns[1] &= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator&=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] &= m2.columns[0];
    m1.columns[1] &= m2.columns[1];
    return m1;
}

// or (|=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator|=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] |= scaler;
    m.columns[1] |= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator|=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] |= m2.columns[0];
    m1.columns[1] |= m2.columns[1];
    return m1;
}

// xor (^=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator^=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] ^= scaler;
    m.columns[1] ^= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator^=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] ^= m2.columns[0];
    m1.columns[1] ^= m2.columns[1];
    return m1;
}

// left shift (<<=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator<<=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] <<= scaler;
    m.columns[1] <<= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator<<=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] <<= m2.columns[0];
    m1.columns[1] <<= m2.columns[1];
    return m1;
}

// right shift (>>=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator>>=(mat<2, 3, T>& m, T scaler)
{
    m.columns[0] >>= scaler;
    m.columns[1] >>= scaler;
    return m;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr mat<2, 3, T>& operator>>=(mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    m1.columns[0] >>= m2.columns[0];
    m1.columns[1] >>= m2.columns[1];
    return m1;
}

// =============== boolean operators ===============

// and (&&)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&&(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] && scaler, m.columns[1] && scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&&(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler && m.columns[0], scaler && m.columns[1]);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator&&(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] && m2.columns[0], m1.columns[1] && m2.columns[1]);
}

// or (||)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator||(const mat<2, 3, T>& m, T scaler)
{
    return mat<2, 3, T>(m.columns[0] || scaler, m.columns[1] || scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator||(T scaler, const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(scaler || m.columns[0], scaler || m.columns[1]);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator||(const mat<2, 3, T>& m1, const mat<2, 3, T>& m2)
{
    return mat<2, 3, T>(m1.columns[0] || m2.columns[0], m1.columns[1] || m2.columns[1]);
}

// not (!)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr mat<2, 3, T> operator!(const mat<2, 3, T>& m)
{
    return mat<2, 3, T>(!m.columns[0], !m.columns[1]);
}

}
}