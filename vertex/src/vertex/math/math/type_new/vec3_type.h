#pragma once

#include "../detail/base_types.h"
#include "scaler.h"

namespace vx {
namespace math {

template <typename T>
struct vec<3, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = vec<3, T>;

    static inline constexpr size_t size() noexcept { return 3; }

    // =============== data ===============

    T x, y, z;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler), z(scaler) {}

    inline constexpr vec(T x, T y, T z) noexcept
        : x(x), y(y), z(z) {}

    // =============== conversion vector constructors ===============

    template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
    inline constexpr vec(vec<3, T>& v, T scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler)) {}

    template <typename X, typename Y, typename Z, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value, bool>::type = true>
    inline constexpr vec(X x, Y y, Z z)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename XY, typename Z, typename std::enable_if<std::is_arithmetic<Z>::value, bool>::type = true>
    inline constexpr vec(const vec<2, XY>& vxy, Z z)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z)) {}
    
    template <typename X, typename YZ, typename std::enable_if<std::is_arithmetic<X>::value, bool>::type = true>
    inline constexpr vec(X x, const vec<2, YZ>& vyz)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyz.x))
        , z(static_cast<T>(vyz.y)) {}
    
    template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
    inline constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}
    
    template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
    inline constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_t i)
    {
        assert(i < 3);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_t i) const
    {
        assert(i < 3);
        return (&x)[i];
    }

};

// =============== comparison operators ===============

template <typename T>
inline constexpr bool operator==(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template <typename T>
inline constexpr bool operator!=(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return !(v1 == v2);
}

// =============== unary constant operators ===============

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator+(const vec<3, T>& v)
{
    return vec<3, T>(+v.x, +v.y, +v.z);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator-(const vec<3, T>& v)
{
    return vec<3, T>(-v.x, -v.y, -v.z);
}

// =============== incrememnt and decrement operators ===============

// increment (++)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator++(vec<3, T>& v)
{
    v.x++;
    v.y++;
    v.z++;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator++(vec<3, T>& v, int)
{
    vec<3, T> result(v);
    ++v;
    return result;
}

// decrement (--)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator--(vec<3, T>& v)
{
    v.x--;
    v.y--;
    v.z--;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator--(vec<3, T>& v, int)
{
    vec<3, T> result(v);
    --v;
    return result;
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator+(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x + scaler, v.y + scaler, v.z + scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator+(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler + v.x, scaler + v.y, scaler + v.z);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator+(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// subtraction (-)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator-(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x - scaler, v.y - scaler, v.z - scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator-(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler - v.x, scaler - v.y, scaler - v.z);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator-(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// multiplication (*)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator*(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x * scaler, v.y * scaler, v.z * scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator*(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler * v.x, scaler * v.y, scaler * v.z);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator*(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

// division (/)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator/(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x / scaler, v.y / scaler, v.z / scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator/(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler / v.x, scaler / v.y, scaler / v.z);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator/(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

// modulo (%)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator%(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x % scaler, v.y % scaler, v.z % scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator%(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler % v.x, scaler % v.y, scaler % v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator%(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
}

// =============== binary bit operators ===============

// and (&)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator&(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x & scaler, v.y & scaler, v.z & scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator&(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler & v.x, scaler & v.y, scaler & v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator&(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
}

// or (|)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator|(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x | scaler, v.y | scaler, v.z | scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator|(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler | v.x, scaler | v.y, scaler | v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator|(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
}

// xor (^)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator^(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator^(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator^(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
}

// left shift (<<)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator<<(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x << scaler, v.y << scaler, v.z << scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator<<(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler << v.x, scaler << v.y, scaler << v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator<<(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
}

// right shift (>>)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator>>(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x >> scaler, v.y >> scaler, v.z >> scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator>>(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler >> v.x, scaler >> v.y, scaler >> v.z);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator>>(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
}

// not (~)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T> operator~(const vec<3, T>& v)
{
    return vec<3, T>(~v.x, ~v.y, ~v.z);
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator+=(vec<3, T>& v, T scaler)
{
    v.x += scaler;
    v.y += scaler;
    v.z += scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator+=(const vec<3, T>& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return v;
}

// subtraction (-=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator-=(vec<3, T>& v, T scaler)
{
    v.x -= scaler;
    v.y -= scaler;
    v.z -= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator-=(const vec<3, T>& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return v;
}

// multiplication (*=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator*=(vec<3, T>& v, T scaler)
{
    v.x *= scaler;
    v.y *= scaler;
    v.z *= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator*=(const vec<3, T>& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return v;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator*=(vec<3, T>& v, const mat<M, 3, T>& m)
{
    v = v * m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator*=(vec<3, T>& v, const mat<3, N, T>& m)
{
    v = v * m;
    return v;
}

// division (/=)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator/=(vec<3, T>& v, T scaler)
{
    v.x /= scaler;
    v.y /= scaler;
    v.z /= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator/=(const vec<3, T>& v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return v;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator/=(vec<3, T>& v, const mat<M, 3, T>& m)
{
    v = v / m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator/=(vec<3, T>& v, const mat<3, N, T>& m)
{
    v = v / m;
    return v;
}

// modulo (%=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator%=(vec<3, T>& v, T scaler)
{
    v.x %= scaler;
    v.y %= scaler;
    v.z %= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator%=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x %= v2.x;
    v1.y %= v2.y;
    v1.z %= v2.z;
    return v1;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator&=(vec<3, T>& v, T scaler)
{
    v.x &= scaler;
    v.y &= scaler;
    v.z &= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator&=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x &= v2.x;
    v1.y &= v2.y;
    v1.z &= v2.z;
    return v1;
}

// or (|=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator|=(vec<3, T>& v, T scaler)
{
    v.x |= scaler;
    v.y |= scaler;
    v.z |= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator|=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x |= v2.x;
    v1.y |= v2.y;
    v1.z |= v2.z;
    return v1;
}

// xor (^=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator^=(vec<3, T>& v, T scaler)
{
    v.x ^= scaler;
    v.y ^= scaler;
    v.z ^= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator^=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x ^= v2.x;
    v1.y ^= v2.y;
    v1.z ^= v2.z;
    return v1;
}

// left shift (<<=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator<<=(vec<3, T>& v, T scaler)
{
    x <<= scaler;
    y <<= scaler;
    z <<= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator<<=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x <<= v2.x;
    v1.y <<= v2.y;
    v1.z <<= v2.z;
    return v1;
}

// right shift (>>=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator>>=(vec<3, T>& v, T scaler)
{
    x >>= scaler;
    y >>= scaler;
    z >>= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<3, T>& operator>>=(vec<3, T>& v1, const vec<3, T>& v2)
{
    v1.x >>= v2.x;
    v1.y >>= v2.y;
    v1.z >>= v2.z;
    return v1;
}

// =============== boolean operators ===============

// and (&&)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator&&(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x && scaler, v.y && scaler, v.z && scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator&&(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler && v.x, scaler && v.y, scaler && v.z);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator&&(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
}

// or (||)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator||(const vec<3, T>& v, T scaler)
{
    return vec<3, T>(v.x || scaler, v.y || scaler, v.z || scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator||(T scaler, const vec<3, T>& v)
{
    return vec<3, T>(scaler || v.x, scaler || v.y, scaler || v.z);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator||(const vec<3, T>& v1, const vec<3, T>& v2)
{
    return vec<3, T>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
}

// not (!)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<3, T> operator!(const vec<3, T>& v)
{
    return vec<3, T>(!v.x, !v.y, !v.z);
}

}
}