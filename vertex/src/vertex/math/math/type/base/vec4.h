#pragma once

#include "../../detail/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<4, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = vec<4, T>;

    static inline constexpr size_t size() noexcept { return static_cast<size_t>(4); }

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

    template <typename U>
    inline constexpr type& operator=(const vec<4, U>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

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

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }

    friend inline constexpr bool operator!=(const type& v1, const type& v2)
    {
        return !(v1 == v2);
    }

    // =============== unary constant operators ===============

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator+() const
    {
        return type(+x, +y, +z, +w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator-() const
    {
        return type(-x, -y, -z, -w);
    }

    // =============== incrememnt and decrement operators ===============

    // incrememnt (++)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator++()
    {
        x++;
        y++;
        z++;
        w++;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator--()
    {
        x--;
        y--;
        z--;
        w--;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    // =============== binary arithmetic operators ===============

    // addition (+)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(const type& v, T scaler)
    {
        return type(v.x + scaler, v.y + scaler, v.z + scaler, v.w + scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(T scaler, const type& v)
    {
        return type(scaler + v.x, scaler + v.y, scaler + v.z, scaler + v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler, v.z - scaler, v.w - scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(T scaler, const type& v)
    {
        return type(scaler - v.x, scaler - v.y, scaler - v.z, scaler - v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(T scaler, const type& v)
    {
        return type(scaler * v.x, scaler * v.y, scaler * v.z, scaler * v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    // division (/)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator/(T scaler, const type& v)
    {
        return type(scaler / v.x, scaler / v.y, scaler / v.z, scaler / v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler, v.w % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return type(scaler % v.x, scaler % v.y, scaler % v.z, scaler % v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler, v.w & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return type(scaler & v.x, scaler & v.y, scaler & v.z, scaler & v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler, v.w | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return type(scaler | v.x, scaler | v.y, scaler | v.z, scaler | v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler, v.w ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return type(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z, scaler ^ v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler, v.w << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return type(scaler << v.x, scaler << v.y, scaler << v.z, scaler << v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler, v.w >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return type(scaler >> v.x, scaler >> v.y, scaler >> v.z, scaler >> v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type operator~() const
    {
        return type(~x, ~y, ~z, ~w);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator+=(T scaler)
    {
        x += scaler;
        y += scaler;
        z += scaler;
        w += scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator-=(T scaler)
    {
        x -= scaler;
        y -= scaler;
        z -= scaler;
        w -= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator*=(T scaler)
    {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        w *= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    template <size_t M, typename std::enable_if<type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator*=(const mat<M, 4, T>& m)
    {
        (*this) = (*this) * m;
        return *this;
    }

    template <size_t N, typename std::enable_if<type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator*=(const mat<4, N, T>& m)
    {
        (*this) = (*this) * m;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator/=(T scaler)
    {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        w /= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    inline constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<T>::value, bool>::type = true>
    inline constexpr type& operator/=(const mat<4, 4, T>& m)
    {
        (*this) = (*this) / m;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        w %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        w %= v.w;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(T scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        w &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        w &= v.w;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        w |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        w |= v.w;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        w ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        w ^= v.w;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        w <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        w <<= v.w;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        w >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
        w >>= v.w;
        return *this;
    }

    // =============== boolean operators ===============

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(const type& v, T scaler)
    {
        return type(v.x && scaler, v.y && scaler, v.z && scaler, v.w && scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(T scaler, const type& v)
    {
        return type(scaler && v.x, scaler && v.y, scaler && v.z, scaler && v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(const type& v1, const type& v2)
    {
        return type(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(const type& v, T scaler)
    {
        return type(v.x || scaler, v.y || scaler, v.z || scaler, v.w || scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(T scaler, const type& v)
    {
        return type(scaler || v.x, scaler || v.y, scaler || v.z, scaler || v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(const type& v1, const type& v2)
    {
        return type(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    inline constexpr type operator!() const
    {
        return type(!x, !y, !z, !w);
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(static_cast<T>(0)); }
    static inline constexpr type ONE() { return type(static_cast<T>(1)); }

};

VX_PACK_POP()

}
}