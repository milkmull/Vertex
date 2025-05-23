#pragma once

#include "../../_priv/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<4, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = vec<4, T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(4); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    T x, y, z, w;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0))
        , w(static_cast<T>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z), w(v.w) {}

    VX_FORCE_INLINE constexpr vec(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(T scalar) noexcept
        : x(scalar), y(scalar), z(scalar), w(scalar) {}

    VX_FORCE_INLINE constexpr vec(T x, T y, T z, T w) noexcept
        : x(x), y(y), z(z), w(w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit vec(U scalar)
        : x(static_cast<T>(scalar))
        , y(static_cast<T>(scalar))
        , z(static_cast<T>(scalar))
        , w(static_cast<T>(scalar)) {}

    template <typename X, typename Y , typename Z, typename W, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value &&
        std::is_arithmetic<W>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, Y y, Z z, W w)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}

    template <typename XY, typename Z, typename W, typename std::enable_if<
        std::is_arithmetic<Z>::value &&
        std::is_arithmetic<W>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z z, W w)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}
    
    template <typename X, typename Y, typename ZW, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, Y y, const vec<2, ZW>& vzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename XYZ, typename W, typename std::enable_if<std::is_arithmetic<W>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(const vec<3, XYZ>& vxyz, W w)
        : x(static_cast<T>(vxyz.x))
        , y(static_cast<T>(vxyz.y))
        , z(static_cast<T>(vxyz.z))
        , w(static_cast<T>(w)) {}
    
    template <typename X, typename YZW, typename std::enable_if<std::is_arithmetic<X>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, const vec<3, YZW>& vyzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyzw.x))
        , z(static_cast<T>(vyzw.y))
        , w(static_cast<T>(vyzw.z)) {}
    
    template <typename XY, typename ZW>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, const vec<2, ZW>& vzw)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z))
        , w(static_cast<T>(v.w)) {}

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
        w = v.w;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const vec<4, U>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
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
        return type(+x, +y, +z, +w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-x, -y, -z, -w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // incrememnt (++)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator++()
    {
        x++;
        y++;
        z++;
        w++;
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
        w--;
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
    friend VX_FORCE_INLINE constexpr type operator+(const type& v, T scalar)
    {
        return type(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(T scalar, const type& v)
    {
        return type(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v, T scalar)
    {
        return type(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(T scalar, const type& v)
    {
        return type(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v, T scalar)
    {
        return type(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(T scalar, const type& v)
    {
        return type(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    // division (/)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v, T scalar)
    {
        return type(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(T scalar, const type& v)
    {
        return type(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v, T scalar)
    {
        return type(v.x % scalar, v.y % scalar, v.z % scalar, v.w % scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(T scalar, const type& v)
    {
        return type(scalar % v.x, scalar % v.y, scalar % v.z, scalar % v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v, T scalar)
    {
        return type(v.x & scalar, v.y & scalar, v.z & scalar, v.w & scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(T scalar, const type& v)
    {
        return type(scalar & v.x, scalar & v.y, scalar & v.z, scalar & v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v, T scalar)
    {
        return type(v.x | scalar, v.y | scalar, v.z | scalar, v.w | scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(T scalar, const type& v)
    {
        return type(scalar | v.x, scalar | v.y, scalar | v.z, scalar | v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v, T scalar)
    {
        return type(v.x ^ scalar, v.y ^ scalar, v.z ^ scalar, v.w ^ scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(T scalar, const type& v)
    {
        return type(scalar ^ v.x, scalar ^ v.y, scalar ^ v.z, scalar ^ v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v, T scalar)
    {
        return type(v.x << scalar, v.y << scalar, v.z << scalar, v.w << scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(T scalar, const type& v)
    {
        return type(scalar << v.x, scalar << v.y, scalar << v.z, scalar << v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v, T scalar)
    {
        return type(v.x >> scalar, v.y >> scalar, v.z >> scalar, v.w >> scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(T scalar, const type& v)
    {
        return type(scalar >> v.x, scalar >> v.y, scalar >> v.z, scalar >> v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator~() const
    {
        return type(~x, ~y, ~z, ~w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(T scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(T scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(T scalar)
    {
        x %= scalar;
        y %= scalar;
        z %= scalar;
        w %= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        w %= v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(T scalar)
    {
        x &= scalar;
        y &= scalar;
        z &= scalar;
        w &= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        w &= v.w;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(T scalar)
    {
        x |= scalar;
        y |= scalar;
        z |= scalar;
        w |= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        w |= v.w;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(T scalar)
    {
        x ^= scalar;
        y ^= scalar;
        z ^= scalar;
        w ^= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        w ^= v.w;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(T scalar)
    {
        x <<= scalar;
        y <<= scalar;
        z <<= scalar;
        w <<= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        w <<= v.w;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(T scalar)
    {
        x >>= scalar;
        y >>= scalar;
        z >>= scalar;
        w >>= scalar;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
        w >>= v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, T scalar)
    {
        return type(v.x && scalar, v.y && scalar, v.z && scalar, v.w && scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(T scalar, const type& v)
    {
        return type(scalar && v.x, scalar && v.y, scalar && v.z, scalar && v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2)
    {
        return type(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v, T scalar)
    {
        return type(v.x || scalar, v.y || scalar, v.z || scalar, v.w || scalar);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(T scalar, const type& v)
    {
        return type(scalar || v.x, scalar || v.y, scalar || v.z, scalar || v.w);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2)
    {
        return type(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator!() const
    {
        return type(!x, !y, !z, !w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO() { return type(static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type ONE() { return type(static_cast<T>(1)); }

};

VX_PACK_POP()

}
}