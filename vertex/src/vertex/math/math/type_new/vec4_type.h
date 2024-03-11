#pragma once

#include "../detail/base_types.h"
#include "scaler.h"

namespace vx {
namespace math {

template <typename T>
struct vec<4, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = vec<4, T>;

    static inline constexpr size_t size() noexcept { return 4; }

    // =============== data ===============

    T x, y, z, w;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0))
        , w(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z), w(v.w) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler), z(scaler), w(scaler) {}

    inline constexpr vec(T x, T y, T z, T w) noexcept
        : x(x), y(y), z(z), w(w) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler))
        , w(static_cast<T>(scaler)) {}

    template <typename X, typename Y , typename Z, typename W, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value &&
        std::is_arithmetic<W>::value, bool>::type = true>
    inline constexpr vec(X x, Y y, Z z, W w)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}

    template <typename XY, typename Z, typename W, typename std::enable_if<
        std::is_arithmetic<Z>::value &&
        std::is_arithmetic<W>::value, bool>::type = true>
    inline constexpr vec(const vec<2, XY>& vxy, Z z, W w)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}
    
    template <typename X, typename Y, typename ZW, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value, bool>::type = true>
    inline constexpr vec(X x, Y y, const vec<2, ZW>& vzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename XYZ, typename W, typename std::enable_if<std::is_arithmetic<W>::value, bool>::type = true>
    inline constexpr vec(const vec<3, XYZ>& vxyz, W w)
        : x(static_cast<T>(vxyz.x))
        , y(static_cast<T>(vxyz.y))
        , z(static_cast<T>(vxyz.z))
        , w(static_cast<T>(w)) {}
    
    template <typename X, typename YZW, typename std::enable_if<std::is_arithmetic<X>::value, bool>::type = true>
    inline constexpr vec(X x, const vec<3, YZW>& vyzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyzw.x))
        , z(static_cast<T>(vyzw.y))
        , w(static_cast<T>(vyzw.z)) {}
    
    template <typename XY, typename ZW>
    inline constexpr vec(const vec<2, XY>& vxy, const vec<2, ZW>& vzw)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z))
        , w(static_cast<T>(v.w)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_t i)
    {
        assert(i < 4);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);
        return (&x)[i];
    }

};

// =============== boolean operators ===============

template <typename T>
inline constexpr bool operator==(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template <typename T>
inline constexpr bool operator!=(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return !(v1 == v2);
}

// =============== unary constant operators ===============

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator+(const vec<4, T>& v)
{
    return vec<4, T>(+v.x, +v.y, +v.z, +v.w);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator-(const vec<4, T>& v)
{
    return vec<4, T>(-v.x, -v.y, -v.z, -v.w);
}

// =============== incrememnt and decrement operators ===============

// incrememnt (++)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator++(vec<4, T>& v)
{
    v.x++;
    v.y++;
    v.z++;
    v.w++;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator++(vec<4, T>& v, int)
{
    vec<4, T> result(v);
    ++v;
    return result;
}

// decrement (--)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator--(vec<4, T>& v)
{
    x--;
    y--;
    z--;
    w--;
    return v;
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator--(vec<4, T>& v, int)
{
    vec<4, T> result(v);
    --v;
    return result;
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator+(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x + scaler, v.y + scaler, v.z + scaler, v.w + scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator+(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler + v.x, scaler + v.y, scaler + v.z, scaler + v.w);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator+(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

// subtraction (-)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator-(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x - scaler, v.y - scaler, v.z - scaler, v.w - scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator-(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler - v.x, scaler - v.y, scaler - v.z, scaler - v.w);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator-(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

// multiplication (*)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator*(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator*(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler * v.x, scaler * v.y, scaler * v.z, scaler * v.w);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator*(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

// division (/)

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator/(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator/(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler / v.x, scaler / v.y, scaler / v.z, scaler / v.w);
}

template <typename T, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator/(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

// modulo (%)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator%(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x % scaler, v.y % scaler, v.z % scaler, v.w % scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator%(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler % v.x, scaler % v.y, scaler % v.z, scaler % v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator%(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
}

// =============== binary bit operators ===============

// and (&)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator&(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x & scaler, v.y & scaler, v.z & scaler, v.w & scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator&(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler & v.x, scaler & v.y, scaler & v.z, scaler & v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator&(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
}

// or (|)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator|(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x | scaler, v.y | scaler, v.z | scaler, v.w | scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator|(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler | v.x, scaler | v.y, scaler | v.z, scaler | v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator|(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
}

// xor (^)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator^(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler, v.w ^ scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator^(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z, scaler ^ v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator^(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
}

// left shift (<<)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator<<(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x << scaler, v.y << scaler, v.z << scaler, v.w << scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator<<(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler << v.x, scaler << v.y, scaler << v.z, scaler << v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator<<(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
}

// right shift (>>)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator>>(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x >> scaler, v.y >> scaler, v.z >> scaler, v.w >> scaler);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator>>(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler >> v.x, scaler >> v.y, scaler >> v.z, scaler >> v.w);
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator>>(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
}

// not (~)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T> operator~(const vec<4, T>& v)
{
    return vec<4, T>(~v.x, ~v.y, ~v.z, ~v.w);
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator+=(vec<4, T>& v, T scaler)
{
    v.x += scaler;
    v.y += scaler;
    v.z += scaler;
    v.w += scaler;
    return v;
}

template <typename T>
inline constexpr vec<4, T>& operator+=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    v1.w += v2.w;
    return v1;
}

// subtraction (-=)

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator-=(vec<4, T>& v, T scaler)
{
    v.x -= scaler;
    v.y -= scaler;
    v.z -= scaler;
    v.w -= scaler;
    return v;
}

template <typename T>
inline constexpr vec<4, T>& operator-=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    v1.w -= v2.w;
    return v1;
}

// multiplication (*=)

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator*=(vec<4, T>& v, T scaler)
{
    v.x *= scaler;
    v.y *= scaler;
    v.z *= scaler;
    v.w *= scaler;
    return v;
}

template <typename T>
inline constexpr vec<4, T>& operator*=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    v1.w *= v2.w;
    return v1;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator*=(vec<4, T>& v, const mat<M, 4, T>& m)
{
    v = v * m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator*=(vec<4, T>& v, const mat<4, N, T>& m)
{
    v = v * m;
    return v;
}

// division (/=)

template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator/=(vec<4, T>& v, T scaler)
{
    v.x /= scaler;
    v.y /= scaler;
    v.z /= scaler;
    v.w /= scaler;
    return v;
}

template <typename T>
inline constexpr vec<4, T>& operator/=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    v1.z /= v2.z;
    v1.w /= v2.w;
    return v1;
}

template <typename T, size_t M, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator/=(vec<4, T>& v, const mat<M, 4, T>& m)
{
    v = v / m;
    return v;
}

template <typename T, size_t N, typename std::enable_if<is_numeric<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator/=(vec<4, T>& v, const mat<4, N, T>& m)
{
    v = v / m;
    return v;
}

// modulo (%=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator%=(vec<4, T>& v, T scaler)
{
    v.x %= scaler;
    v.y %= scaler;
    v.z %= scaler;
    v.w %= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator%=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x %= v2.x;
    v1.y %= v2.y;
    v1.z %= v2.z;
    v1.w %= v2.w;
    return v1;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator&=(vec<4, T>& v, T scaler)
{
    v.x &= scaler;
    v.y &= scaler;
    v.z &= scaler;
    v.w &= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator&=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x &= v2.x;
    v1.y &= v2.y;
    v1.z &= v2.z;
    v1.w &= v2.w;
    return v1;
}

// or (|=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator|=(vec<4, T>& v, T scaler)
{
    v.x |= scaler;
    v.y |= scaler;
    v.z |= scaler;
    v.w |= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator|=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x |= v2.x;
    v1.y |= v2.y;
    v1.z |= v2.z;
    v1.w |= v2.w;
    return v1;
}

// xor (^=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator^=(vec<4, T>& v, T scaler)
{
    v.x ^= scaler;
    v.y ^= scaler;
    v.z ^= scaler;
    v.w ^= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator^=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x ^= v2.x;
    v1.y ^= v2.y;
    v1.z ^= v2.z;
    v1.w ^= v2.w;
    return v1;
}

// left shift (<<=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator<<=(vec<4, T>& v, T scaler)
{
    v.x <<= scaler;
    v.y <<= scaler;
    v.z <<= scaler;
    v.w <<= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator<<=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x <<= v2.x;
    v1.y <<= v2.y;
    v1.z <<= v2.z;
    v1.w <<= v2.w;
    return v1;
}

// right shift (>>=)

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator>>=(vec<4, T>& v, T scaler)
{
    v.x >>= scaler;
    v.y >>= scaler;
    v.z >>= scaler;
    v.w >>= scaler;
    return v;
}

template <typename T, typename std::enable_if<is_integral<T>::value, bool>::type = true>
inline constexpr vec<4, T>& operator>>=(vec<4, T>& v1, const vec<4, T>& v2)
{
    v1.x >>= v2.x;
    v1.y >>= v2.y;
    v1.z >>= v2.z;
    v1.w >>= v2.w;
    return v;
}

// =============== boolean operators ===============

// and (&&)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator&&(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x && scaler, v.y && scaler, v.z && scaler, v.w && scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator&&(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler && v.x, scaler && v.y, scaler && v.z, scaler && v.w);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator&&(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
}

// or (||)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator||(const vec<4, T>& v, T scaler)
{
    return vec<4, T>(v.x || scaler, v.y || scaler, v.z || scaler, v.w || scaler);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator||(T scaler, const vec<4, T>& v)
{
    return vec<4, T>(scaler || v.x, scaler || v.y, scaler || v.z, scaler || v.w);
}

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator||(const vec<4, T>& v1, const vec<4, T>& v2)
{
    return vec<4, T>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
}

// not (!)

template <typename T, typename std::enable_if<std::is_same<T, bscale>::value, bool>::type = true>
inline constexpr vec<4, T> operator!(const vec<4, T>& v)
{
    return vec<4, T>(!v.x, !v.y, !v.z, !v.w);
}

}
}