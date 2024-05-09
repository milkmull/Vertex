#pragma once

#include "../../_priv/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<3, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scaler_type = T;
    using type = vec<3, T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(3); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    T x, y, z;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z) {}

    VX_FORCE_INLINE constexpr vec(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler), z(scaler) {}

    VX_FORCE_INLINE constexpr vec(T x, T y, T z) noexcept
        : x(x), y(y), z(z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(type& v, T scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler)) {}

    template <typename X, typename Y, typename Z, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, Y y, Z z)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename XY, typename Z, typename std::enable_if<std::is_arithmetic<Z>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z z)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z)) {}
    
    template <typename X, typename YZ, typename std::enable_if<std::is_arithmetic<X>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, const vec<2, YZ>& vyz)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyz.x))
        , z(static_cast<T>(vyz.y)) {}
    
    template <typename U, typename std::enable_if<type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}
    
    template <typename U, typename std::enable_if<type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~vec() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const vec<3, U>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 3);

        switch (i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }

        return static_cast<T>(0);
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 3);

        switch (i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }

        return static_cast<T>(0);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2)
    {
        return !(v1 == v2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator+() const
    {
        return type(+x, +y, +z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-x, -y, -z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // increment (++)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator++()
    {
        x++;
        y++;
        z++;
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
        x--;
        y--;
        z--;
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
    friend VX_FORCE_INLINE constexpr type operator+(const type& v, T scaler)
    {
        return type(v.x + scaler, v.y + scaler, v.z + scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(T scaler, const type& v)
    {
        return type(scaler + v.x, scaler + v.y, scaler + v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler, v.z - scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(T scaler, const type& v)
    {
        return type(scaler - v.x, scaler - v.y, scaler - v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler, v.z * scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(T scaler, const type& v)
    {
        return type(scaler * v.x, scaler * v.y, scaler * v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }

    // division (/)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler, v.z / scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(T scaler, const type& v)
    {
        return type(scaler / v.x, scaler / v.y, scaler / v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(T scaler, const type& v)
    {
        return type(scaler % v.x, scaler % v.y, scaler % v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(T scaler, const type& v)
    {
        return type(scaler & v.x, scaler & v.y, scaler & v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(T scaler, const type& v)
    {
        return type(scaler | v.x, scaler | v.y, scaler | v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(T scaler, const type& v)
    {
        return type(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(T scaler, const type& v)
    {
        return type(scaler << v.x, scaler << v.y, scaler << v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(T scaler, const type& v)
    {
        return type(scaler >> v.x, scaler >> v.y, scaler >> v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator~() const
    {
        return type(~x, ~y, ~z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(T scaler)
    {
        x += scaler;
        y += scaler;
        z += scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(T scaler)
    {
        x -= scaler;
        y -= scaler;
        z -= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(T scaler)
    {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(T scaler)
    {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(T scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, T scaler)
    {
        return type(v.x && scaler, v.y && scaler, v.z && scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(T scaler, const type& v)
    {
        return type(scaler && v.x, scaler && v.y, scaler && v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2)
    {
        return type(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v, T scaler)
    {
        return type(v.x || scaler, v.y || scaler, v.z || scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(T scaler, const type& v)
    {
        return type(scaler || v.x, scaler || v.y, scaler || v.z);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2)
    {
        return type(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator!() const
    {
        return type(!x, !y, !z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO() { return type(static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type ONE() { return type(static_cast<T>(1)); }

    static VX_FORCE_INLINE constexpr type RIGHT() { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type LEFT() { return type(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)); }

    static VX_FORCE_INLINE constexpr type UP() { return type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type DOWN() { return type(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)); }

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)

    static VX_FORCE_INLINE constexpr type FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }
    static VX_FORCE_INLINE constexpr type BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }

#else

    static VX_FORCE_INLINE constexpr type FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }
    static VX_FORCE_INLINE constexpr type BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }

#endif

};

VX_PACK_POP()

}
}