#pragma once

#include "../detail/base_types.h"
#include "scaler.h"

namespace vx {
namespace math {

template <typename T>
struct vec<2, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = vec<2, T>;

    static inline constexpr size_t size() noexcept { return 2; }

    // =============== data ===============

    T x, y;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler) {}

    inline constexpr vec(T x, T y) noexcept
        : x(x), y(y) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler)) {}

    template <typename X, typename Y, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value, bool>::type = true>
    inline constexpr vec(X x, Y y)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y)) {}

    template <typename U>
    inline constexpr explicit vec(const vec<2, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_t i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_t i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

};

// =============== comparison operators ===============

template <typename T>
inline constexpr bool operator==(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

template <typename T>
inline constexpr bool operator!=(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return !(v1 == v2);
}

// =============== unary constant operators ===============

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator+(const vec<2, T>& v)
{
    return vec<2, T>(+v.x, +v.y);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator-(const vec<2, T>& v)
{
    return vec<2, T>(-v.x, -v.y);
}

// =============== incrememnt and decrement operators ===============

// increment (++)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator++(vec<2, T>& v)
{
    v.x++;
    v.y++;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator++(vec<2, T>& v, int)
{
    vec<2, T> result(v);
    ++v;
    return result;
}

// decrement (--)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator--(vec<2, T>& v)
{
    v.x--;
    v.y--;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator--(vec<2, T>& v, int)
{
    vec<2, T> result(v);
    --v;
    return result;
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator+(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x + scaler, v.y + scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator+(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler + v.x, scaler + v.y);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator+(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x + v2.x, v1.y + v2.y);
}

// subtraction (-)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator-(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x - scaler, v.y - scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator-(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler - v.x, scaler - v.y);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator-(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x - v2.x, v1.y - v2.y);
}

// multiplication (*)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator*(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x * scaler, v.y * scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator*(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler * v.x, scaler * v.y);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator*(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x * v2.x, v1.y * v2.y);
}

// division (/)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator/(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x / scaler, v.y / scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator/(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler / v.x, scaler / v.y);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator/(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x / v2.x, v1.y / v2.y);
}

// modulo (%)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator%(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x % scaler, v.y % scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator%(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler % v.x, scaler % v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator%(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x % v2.x, v1.y % v2.y);
}

// =============== binary bit operators ===============

// and (&)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator&(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x & scaler, v.y & scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator&(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler & v.x, scaler & v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator&(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x & v2.x, v1.y & v2.y);
}

// or (|)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator|(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x | scaler, v.y | scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator|(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler | v.x, scaler | v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator|(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x | v2.x, v1.y | v2.y);
}

// xor (^)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator^(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x ^ scaler, v.y ^ scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator^(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler ^ v.x, scaler ^ v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator^(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x ^ v2.x, v1.y ^ v2.y);
}

// left shift (<<)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator<<(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x << scaler, v.y << scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator<<(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler << v.x, scaler << v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator<<(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x << v2.x, v1.y << v2.y);
}

// right shift (>>)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator>>(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x >> scaler, v.y >> scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator>>(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler >> v.x, scaler >> v.y);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator>>(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x >> v2.x, v1.y >> v2.y);
}

// not (~)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T> operator~(const vec<2, T>& v)
{
    return vec<2, T>(~v.x, ~v.y);
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator+=(vec<2, T>& v, T scaler)
{
    v.x += scaler;
    v.y += scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator+=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

// subtraction (-=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator-=(vec<2, T>& v, T scaler)
{
    v.x -= scaler;
    v.y -= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator-=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}

// multiplication (*=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator*=(vec<2, T>& v, T scaler)
{
    v.x *= scaler;
    v.y *= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator*=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator*=(vec<2, T>& v, const mat<M, 2, T>& m)
{
    v = v * m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator*=(vec<2, T>& v, const mat<2, N, T>& m)
{
    v = v * m;
    return v;
}

// division (/=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator/=(vec<2, T>& v, T scaler)
{
    v.x /= scaler;
    v.y /= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator/=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator/=(vec<2, T>& v, const mat<M, 2, T>& m)
{
    v = v / m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator/=(vec<2, T>& v, const mat<2, N, T>& m)
{
    v = v / m;
    return v;
}

// modulo (%=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator%=(vec<2, T>& v, T scaler)
{
    v.x %= scaler;
    v.y %= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator%=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x %= v2.x;
    v1.y %= v2.y;
    return v1;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator&=(vec<2, T>& v, T scaler)
{
    v.x &= scaler;
    v.y &= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator&=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x &= v2.x;
    v1.y &= v2.y;
    return v1;
}

// or (|=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator|=(vec<2, T>& v, T scaler)
{
    v.x |= scaler;
    v.y |= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator|=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x |= v2.x;
    v1.y |= v2.y;
    return v1;
}

// xor (^=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator^=(vec<2, T>& v, T scaler)
{
    v.x ^= scaler;
    v.y ^= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator^=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x ^= v2.x;
    v1.y ^= v2.y;
    return v1;
}

// left shift (<<=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator<<=(vec<2, T>& v, T scaler)
{
    v.x <<= scaler;
    v.y <<= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator<<=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x <<= v2.x;
    v1.y <<= v2.y;
    return v1;
}

// right shift (>>=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator>>=(vec<2, T>& v, T scaler)
{
    v.x >>= scaler;
    v.y >>= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<2, T>& operator>>=(vec<2, T>& v1, const vec<2, T>& v2)
{
    v1.x >>= v2.x;
    v1.y >>= v2.y;
    return v1;
}

// =============== boolean operators ===============

// and (&&)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator&&(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x && scaler, v.y && scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator&&(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler && v.x, scaler && v.y);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator&&(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x && v2.x, v1.y && v2.y);
}

// or (||)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator||(const vec<2, T>& v, T scaler)
{
    return vec<2, T>(v.x || scaler, v.y || scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator||(T scaler, const vec<2, T>& v)
{
    return vec<2, T>(scaler || v.x, scaler || v.y);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator||(const vec<2, T>& v1, const vec<2, T>& v2)
{
    return vec<2, T>(v1.x || v2.x, v1.y || v2.y);
}

// not (!)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<2, T> operator!(const vec<2, T>& v)
{
    return vec<2, T>(!v.x, !v.y);
}

}
}