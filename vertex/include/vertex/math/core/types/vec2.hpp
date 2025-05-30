#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"

namespace vx {
namespace math {

template <typename T>
struct vec<2, T>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////
    
    using scalar_type = T;
    static constexpr size_t size = 2;

    using type = vec<size, scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<scalar_type>(0)), y(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(scalar_type scalar) noexcept
        : x(scalar), y(scalar) {}

    VX_FORCE_INLINE constexpr vec(scalar_type vx, scalar_type vy) noexcept
        : x(vx), y(vy) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y>
    VX_FORCE_INLINE constexpr vec(X vx, Y vy) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<2, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<3, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const vec<2, U>& v) noexcept
    {
        x = static_cast<scalar_type>(v.x);
        y = static_cast<scalar_type>(v.y);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr scalar_type& operator[](size_t i) noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0:     return x;
            case 1:     return y;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0:     return x;
            case 1:     return y;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    // equal (==)

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2) noexcept
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    // not equal (!=)

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2) noexcept
    {
        return v1.x != v2.x || v1.y != v2.y;
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        return v1.y > v2.y;
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        return v1.y < v2.y;
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        return v1.y >= v2.y;
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        return v1.y <= v2.y;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+x, +y);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-x, -y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // increment (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        x++;
        y++;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator++(int) noexcept
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    VX_FORCE_INLINE constexpr type& operator--() noexcept
    {
        x--;
        y--;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator--(int) noexcept
    {
        type result(*this);
        --(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr type operator+(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x + scalar, v.y + scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar + v.x, scalar + v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2) noexcept
    {
        return type(v1.x + v2.x, v1.y + v2.y);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x - scalar, v.y - scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar - v.x, scalar - v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2) noexcept
    {
        return type(v1.x - v2.x, v1.y - v2.y);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x * scalar, v.y * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar * v.x, scalar * v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2) noexcept
    {
        return type(v1.x * v2.x, v1.y * v2.y);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x / scalar, v.y / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar / v.x, scalar / v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2) noexcept
    {
        return type(v1.x / v2.x, v1.y / v2.y);
    }

    // modulo (%)

    friend VX_FORCE_INLINE constexpr type operator%(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x % scalar, v.y % scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator%(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar % v.x, scalar % v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator%(const type& v1, const type& v2) noexcept
    {
        return type(v1.x % v2.x, v1.y % v2.y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    friend VX_FORCE_INLINE constexpr type operator&(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x & scalar, v.y & scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator&(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar & v.x, scalar & v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator&(const type& v1, const type& v2) noexcept
    {
        return type(v1.x & v2.x, v1.y & v2.y);
    }

    // or (|)

    friend VX_FORCE_INLINE constexpr type operator|(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x | scalar, v.y | scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator|(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar | v.x, scalar | v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator|(const type& v1, const type& v2) noexcept
    {
        return type(v1.x | v2.x, v1.y | v2.y);
    }

    // xor (^)

    friend VX_FORCE_INLINE constexpr type operator^(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x ^ scalar, v.y ^ scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator^(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar ^ v.x, scalar ^ v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator^(const type& v1, const type& v2) noexcept
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y);
    }

    // left shift (<<)

    friend VX_FORCE_INLINE constexpr type operator<<(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x << scalar, v.y << scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator<<(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar << v.x, scalar << v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator<<(const type& v1, const type& v2) noexcept
    {

        return type(v1.x << v2.x, v1.y << v2.y);
    }

    // right shift (>>)

    friend VX_FORCE_INLINE constexpr type operator>>(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x >> scalar, v.y >> scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator>>(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar >> v.x, scalar >> v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator>>(const type& v1, const type& v2) noexcept
    {
        return type(v1.x >> v2.x, v1.y >> v2.y);
    }

    // not (~)

    VX_FORCE_INLINE constexpr type operator~() const noexcept
    {
        return type(~x, ~y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        x += scalar;
        y += scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    // modulo (%=)

    VX_FORCE_INLINE constexpr type& operator%=(scalar_type scalar) noexcept
    {
        x %= scalar;
        y %= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator%=(const type& v) noexcept
    {
        x %= v.x;
        y %= v.y;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    VX_FORCE_INLINE constexpr type& operator&=(scalar_type scalar) noexcept
    {
        x &= scalar;
        y &= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator&=(const type& v) noexcept
    {
        x &= v.x;
        y &= v.y;
        return *this;
    }

    // or (|=)

    VX_FORCE_INLINE constexpr type& operator|=(scalar_type scalar) noexcept
    {
        x |= scalar;
        y |= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator|=(const type& v) noexcept
    {
        x |= v.x;
        y |= v.y;
        return *this;
    }

    // xor (^=)

    VX_FORCE_INLINE constexpr type& operator^=(scalar_type scalar) noexcept
    {
        x ^= scalar;
        y ^= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator^=(const type& v) noexcept
    {
        x ^= v.x;
        y ^= v.y;
        return *this;
    }

    // left shift (<<=)

    VX_FORCE_INLINE constexpr type& operator<<=(scalar_type scalar) noexcept
    {
        x <<= scalar;
        y <<= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator<<=(const type& v) noexcept
    {
        x <<= v.x;
        y <<= v.y;
        return *this;
    }

    // right shift (>>=)

    VX_FORCE_INLINE constexpr type& operator>>=(scalar_type scalar) noexcept
    {
        x >>= scalar;
        y >>= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator>>=(const type& v) noexcept
    {
        x >>= v.x;
        y >>= v.y;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const type& v, scalar_type scalar) noexcept
    {
        return vec<size, bool>(v.x && scalar, v.y && scalar);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(scalar_type scalar, const type& v) noexcept
    {
        return vec<size, bool>(scalar && v.x, scalar && v.y);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const type& v1, const type& v2) noexcept
    {
        return vec<size, bool>(v1.x && v2.x, v1.y && v2.y);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const type& v, scalar_type scalar) noexcept
    {
        return vec<size, bool>(v.x || scalar, v.y || scalar);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(scalar_type scalar, const type& v) noexcept
    {
        return vec<size, bool>(scalar || v.x, scalar || v.y);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const type& v1, const type& v2) noexcept
    {
        return vec<size, bool>(v1.x || v2.x, v1.y || v2.y);
    }

    // not (!)

    VX_FORCE_INLINE constexpr vec<size, bool> operator!() const noexcept
    {
        return vec<size, bool>(!x, !y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type one() noexcept { return type(static_cast<scalar_type>(1)); }

    static VX_FORCE_INLINE constexpr type right() noexcept { return type(static_cast<scalar_type>(1), static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type left() noexcept { return type(static_cast<scalar_type>(-1), static_cast<scalar_type>(0)); }

    static VX_FORCE_INLINE constexpr type up() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(1)); }
    static VX_FORCE_INLINE constexpr type down() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(-1)); }
};

} // namespace math
} // namespace vx