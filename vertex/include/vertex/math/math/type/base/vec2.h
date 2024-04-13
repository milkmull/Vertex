#pragma once

#include "../../_priv/common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<2, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scaler_type = T;
    using type = vec<2, T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(2); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    T x, y;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y) {}

    VX_FORCE_INLINE constexpr vec(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler) {}

    VX_FORCE_INLINE constexpr vec(T x, T y) noexcept
        : x(x), y(y) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion matrix constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler)) {}

    template <typename X, typename Y, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr vec(X x, Y y)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<2, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}

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
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const vec<2, U>& v)
    {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y;
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
        return type(+x, +y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-x, -y);
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
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return *this;
    }

    // decrement (--)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator--()
    {
        x--;
        y--;
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
        return type(v.x + scaler, v.y + scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(T scaler, const type& v)
    {
        return type(scaler + v.x, scaler + v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(T scaler, const type& v)
    {
        return type(scaler - v.x, scaler - v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(T scaler, const type& v)
    {
        return type(scaler * v.x, scaler * v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y);
    }

    // division (/)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(T scaler, const type& v)
    {
        return type(scaler / v.x, scaler / v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(T scaler, const type& v)
    {
        return type(scaler % v.x, scaler % v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(T scaler, const type& v)
    {
        return type(scaler & v.x, scaler & v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(T scaler, const type& v)
    {
        return type(scaler | v.x, scaler | v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(T scaler, const type& v)
    {
        return type(scaler ^ v.x, scaler ^ v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(T scaler, const type& v)
    {
        return type(scaler << v.x, scaler << v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(T scaler, const type& v)
    {
        return type(scaler >> v.x, scaler >> v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator~() const
    {
        return type(~x, ~y);
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
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(T scaler)
    {
        x -= scaler;
        y -= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(T scaler)
    {
        x *= scaler;
        y *= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(T scaler)
    {
        x /= scaler;
        y /= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
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
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, T scaler)
    {
        return type(v.x && scaler, v.y && scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(T scaler, const type& v)
    {
        return type(scaler && v.x, scaler && v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2)
    {
        return type(v1.x && v2.x, v1.y && v2.y);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v, T scaler)
    {
        return type(v.x || scaler, v.y || scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(T scaler, const type& v)
    {
        return type(scaler || v.x, scaler || v.y);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2)
    {
        return type(v1.x || v2.x, v1.y || v2.y);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type operator!() const
    {
        return type(!x, !y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // static constructors
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Constructs a vector from an angle.
    ///
    /// @param angle The angle in radians.
    /// 
    /// @return The vector constructed from the angle.
    ///////////////////////////////////////////////////////////////////////////////
    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    static VX_FORCE_INLINE constexpr type from_angle(T angle)
    {
        return type(std::cos(angle), std::sin(angle));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO()  { return type(static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type ONE()   { return type(static_cast<T>(1)); }
    
    static VX_FORCE_INLINE constexpr type RIGHT() { return type(static_cast<T>(1),  static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type LEFT()  { return type(static_cast<T>(-1), static_cast<T>(0)); }
    
    static VX_FORCE_INLINE constexpr type UP()    { return type(static_cast<T>(0), static_cast<T>(1)); }
    static VX_FORCE_INLINE constexpr type DOWN()  { return type(static_cast<T>(0), static_cast<T>(-1)); }

};

VX_PACK_POP()

}
}