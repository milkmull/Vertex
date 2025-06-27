#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"

namespace vx {
namespace math {

//struct color
//{
//    ///////////////////////////////////////////////////////////////////////////////
//    // meta
//    ///////////////////////////////////////////////////////////////////////////////
//
//    using scalar_type = f32;
//    static constexpr size_t size = 4;
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // data
//    ///////////////////////////////////////////////////////////////////////////////
//
//    scalar_type r, g, b, a;
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // implicit constructors
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_FORCE_INLINE constexpr color() noexcept
//        : r(static_cast<scalar_type>(0))
//        , g(static_cast<scalar_type>(0))
//        , b(static_cast<scalar_type>(0))
//        , a(static_cast<scalar_type>(1)) {}
//
//    VX_FORCE_INLINE constexpr color(const color& c) noexcept
//        : r(c.r), g(c.g), b(c.b), a(c.a) {}
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // explicit constructors
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_FORCE_INLINE constexpr explicit color(scalar_type scalar) noexcept
//        : r(scalar), g(scalar), b(scalar), a(static_cast<scalar_type>(1)) {}
//
//    VX_FORCE_INLINE constexpr color(
//        scalar_type cr,
//        scalar_type cg,
//        scalar_type cb,
//        scalar_type ca = 1.0f
//    ) noexcept
//        : r(cr), g(cg), b(cb), a(ca) {}
//
//    template <typename T, VXM_REQ_FLOAT(T)>
//    VX_FORCE_INLINE constexpr color(const vec<3, T>& crgb, T ca = static_cast<scalar_type>(1)) noexcept
//        : r(static_cast<scalar_type>(crgb.x))
//        , g(static_cast<scalar_type>(crgb.y))
//        , b(static_cast<scalar_type>(crgb.z))
//        , a(ca) {}
//
//    template <typename T, VXM_REQ_FLOAT(T)>
//    VX_FORCE_INLINE constexpr color(const vec<4, T>& v) noexcept
//        : r(static_cast<scalar_type>(v.x))
//        , g(static_cast<scalar_type>(v.y))
//        , b(static_cast<scalar_type>(v.z))
//        , a(static_cast<scalar_type>(v.w)) {}
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // conversion constructors
//    ///////////////////////////////////////////////////////////////////////////////
//
//    template <typename U>
//    VX_FORCE_INLINE constexpr explicit color(U scalar) noexcept
//        : r(static_cast<scalar_type>(scalar))
//        , g(static_cast<scalar_type>(scalar))
//        , b(static_cast<scalar_type>(scalar))
//        , a(static_cast<scalar_type>(scalar)) {}
//
//    template <typename r, typename g, typename b, typename a>
//    VX_FORCE_INLINE constexpr color(r vx, g vy, b vz, a vw) noexcept
//        : r(static_cast<scalar_type>(vx))
//        , g(static_cast<scalar_type>(vy))
//        , b(static_cast<scalar_type>(vz))
//        , a(static_cast<scalar_type>(vw)) {}
//
//    template <typename XY, typename b, typename a>
//    VX_FORCE_INLINE constexpr color(const vec<2, XY>& vxy, b vz, a vw) noexcept
//        : r(static_cast<scalar_type>(vxy.r))
//        , g(static_cast<scalar_type>(vxy.g))
//        , b(static_cast<scalar_type>(vz))
//        , a(static_cast<scalar_type>(vw)) {}
//
//    template <typename r, typename YZ, typename a>
//    VX_FORCE_INLINE constexpr color(r vx, const vec<2, YZ>& vyz, a vw) noexcept
//        : r(static_cast<scalar_type>(vx))
//        , g(static_cast<scalar_type>(vyz.r))
//        , b(static_cast<scalar_type>(vyz.g))
//        , a(static_cast<scalar_type>(vw)) {}
//
//    template <typename r, typename g, typename ZW>
//    VX_FORCE_INLINE constexpr color(r vx, g vy, const vec<2, ZW>& vzw) noexcept
//        : r(static_cast<scalar_type>(vx))
//        , g(static_cast<scalar_type>(vy))
//        , b(static_cast<scalar_type>(vzw.r))
//        , a(static_cast<scalar_type>(vzw.g)) {}
//
//    template <typename XYZ, typename a>
//    VX_FORCE_INLINE constexpr color(const vec<3, XYZ>& vxyz, a vw) noexcept
//        : r(static_cast<scalar_type>(vxyz.r))
//        , g(static_cast<scalar_type>(vxyz.g))
//        , b(static_cast<scalar_type>(vxyz.b))
//        , a(static_cast<scalar_type>(vw)) {}
//
//    template <typename r, typename YZW>
//    VX_FORCE_INLINE constexpr color(r vx, const vec<3, YZW>& vyzw) noexcept
//        : r(static_cast<scalar_type>(vx))
//        , g(static_cast<scalar_type>(vyzw.r))
//        , b(static_cast<scalar_type>(vyzw.g))
//        , a(static_cast<scalar_type>(vyzw.b)) {}
//
//    template <typename XY, typename ZW>
//    VX_FORCE_INLINE constexpr color(const vec<2, XY>& vxy, const vec<2, ZW>& vzw) noexcept
//        : r(static_cast<scalar_type>(vxy.r))
//        , g(static_cast<scalar_type>(vxy.g))
//        , b(static_cast<scalar_type>(vzw.r))
//        , a(static_cast<scalar_type>(vzw.g)) {}
//
//    template <typename U>
//    VX_FORCE_INLINE constexpr explicit color(const vec<4, U>& v) noexcept
//        : r(static_cast<scalar_type>(c.r))
//        , g(static_cast<scalar_type>(c.g))
//        , b(static_cast<scalar_type>(c.b))
//        , a(static_cast<scalar_type>(c.a)) {}
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // assignment operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_FORCE_INLINE constexpr color& operator=(const color& c) noexcept
//    {
//        r = c.r;
//        g = c.g;
//        b = c.b;
//        a = c.a;
//        return *this;
//    }
//
//    template <typename U>
//    VX_FORCE_INLINE constexpr color& operator=(const vec<4, U>& v) noexcept
//    {
//        r = static_cast<scalar_type>(c.r);
//        g = static_cast<scalar_type>(c.g);
//        b = static_cast<scalar_type>(c.b);
//        a = static_cast<scalar_type>(c.a);
//        return *this;
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // index operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_FORCE_INLINE constexpr scalar_type& operator[](size_t i) noexcept
//    {
//        VX_ASSERT(i < size);
//
//        switch (i)
//        {
//            default:
//            case 0: return r;
//            case 1: return g;
//            case 2: return b;
//            case 3: return a;
//        }
//    }
//
//    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
//    {
//        VX_ASSERT(i < size);
//
//        switch (i)
//        {
//            default:
//            case 0: return r;
//            case 1: return g;
//            case 2: return b;
//            case 3: return a;
//        }
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // comparison operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // equal (==)
//
//    friend VX_FORCE_INLINE constexpr bool operator==(const color& v1, const color& v2) noexcept
//    {
//        return v1.r == v2.r && v1.g == v2.g && v1.b == v2.b && v1.a == v2.a;
//    }
//
//    // not equal (!=)
//
//    friend VX_FORCE_INLINE constexpr bool operator!=(const color& v1, const color& v2) noexcept
//    {
//        return v1.r != v2.r || v1.g != v2.g || v1.b != v2.b || v1.a != v2.a;
//    }
//
//    // greater than (>)
//
//    friend VX_FORCE_INLINE constexpr bool operator>(const color& v1, const color& v2) noexcept
//    {
//        if (v1.r != v2.r) return v1.r > v2.r;
//        if (v1.g != v2.g) return v1.g > v2.g;
//        if (v1.b != v2.b) return v1.b > v2.b;
//        return v1.a > v2.a;
//    }
//
//    // less than (<)
//
//    friend VX_FORCE_INLINE constexpr bool operator<(const color& v1, const color& v2) noexcept
//    {
//        if (v1.r != v2.r) return v1.r < v2.r;
//        if (v1.g != v2.g) return v1.g < v2.g;
//        if (v1.b != v2.b) return v1.b < v2.b;
//        return v1.a < v2.a;
//    }
//
//    // greater than or equal to (>=)
//
//    friend VX_FORCE_INLINE constexpr bool operator>=(const color& v1, const color& v2) noexcept
//    {
//        if (v1.r != v2.r) return v1.r > v2.r;
//        if (v1.g != v2.g) return v1.g > v2.g;
//        if (v1.b != v2.b) return v1.b > v2.b;
//        return v1.a >= v2.a;
//    }
//
//    // less than or equal to (<=)
//
//    friend VX_FORCE_INLINE constexpr bool operator<=(const color& v1, const color& v2) noexcept
//    {
//        if (v1.r != v2.r) return v1.r < v2.r;
//        if (v1.g != v2.g) return v1.g < v2.g;
//        if (v1.b != v2.b) return v1.b < v2.b;
//        return v1.a <= v2.a;
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // unary const operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_FORCE_INLINE constexpr color operator+() const noexcept
//    {
//        return color(+r, +g, +b, +a);
//    }
//
//    VX_FORCE_INLINE constexpr color operator-() const noexcept
//    {
//        return color(-r, -g, -b, -a);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // increment and decrement operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // incrememnt (++)
//
//    VX_FORCE_INLINE constexpr color& operator++() noexcept
//    {
//        r++;
//        g++;
//        b++;
//        a++;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color operator++(int) noexcept
//    {
//        color result(*this);
//        ++(*this);
//        return result;
//    }
//
//    // decrement (--)
//
//    VX_FORCE_INLINE constexpr color& operator--() noexcept
//    {
//        r--;
//        g--;
//        b--;
//        a--;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color operator--(int) noexcept
//    {
//        color result(*this);
//        --(*this);
//        return result;
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // binary arithmetic operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // addition (+)
//
//    friend VX_FORCE_INLINE constexpr color operator+(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r + scalar, c.g + scalar, c.b + scalar, c.a + scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator+(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar + c.r, scalar + c.g, scalar + c.b, scalar + c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator+(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a);
//    }
//
//    // subtraction (-)
//
//    friend VX_FORCE_INLINE constexpr color operator-(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r - scalar, c.g - scalar, c.b - scalar, c.a - scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator-(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar - c.r, scalar - c.g, scalar - c.b, scalar - c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator-(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a);
//    }
//
//    // multiplication (*)
//
//    friend VX_FORCE_INLINE constexpr color operator*(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator*(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar * c.r, scalar * c.g, scalar * c.b, scalar * c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator*(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r * v2.r, v1.g * v2.g, v1.b * v2.b, v1.a * v2.a);
//    }
//
//    // division (/)
//
//    friend VX_FORCE_INLINE constexpr color operator/(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r / scalar, c.g / scalar, c.b / scalar, c.a / scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator/(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar / c.r, scalar / c.g, scalar / c.b, scalar / c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator/(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r / v2.r, v1.g / v2.g, v1.b / v2.b, v1.a / v2.a);
//    }
//
//    // modulo (%)
//
//    friend VX_FORCE_INLINE constexpr color operator%(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r % scalar, c.g % scalar, c.b % scalar, c.a % scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator%(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar % c.r, scalar % c.g, scalar % c.b, scalar % c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator%(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r % v2.r, v1.g % v2.g, v1.b % v2.b, v1.a % v2.a);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // binary bit operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // and (&)
//
//    friend VX_FORCE_INLINE constexpr color operator&(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r & scalar, c.g & scalar, c.b & scalar, c.a & scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator&(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar & c.r, scalar & c.g, scalar & c.b, scalar & c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator&(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r & v2.r, v1.g & v2.g, v1.b & v2.b, v1.a & v2.a);
//    }
//
//    // or (|)
//
//    friend VX_FORCE_INLINE constexpr color operator|(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r | scalar, c.g | scalar, c.b | scalar, c.a | scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator|(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar | c.r, scalar | c.g, scalar | c.b, scalar | c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator|(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r | v2.r, v1.g | v2.g, v1.b | v2.b, v1.a | v2.a);
//    }
//
//    // xor (^)
//
//    friend VX_FORCE_INLINE constexpr color operator^(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r ^ scalar, c.g ^ scalar, c.b ^ scalar, c.a ^ scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator^(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar ^ c.r, scalar ^ c.g, scalar ^ c.b, scalar ^ c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator^(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r ^ v2.r, v1.g ^ v2.g, v1.b ^ v2.b, v1.a ^ v2.a);
//    }
//
//    // left shift (<<)
//
//    friend VX_FORCE_INLINE constexpr color operator<<(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r << scalar, c.g << scalar, c.b << scalar, c.a << scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator<<(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar << c.r, scalar << c.g, scalar << c.b, scalar << c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator<<(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r << v2.r, v1.g << v2.g, v1.b << v2.b, v1.a << v2.a);
//    }
//
//    // right shift (>>)
//
//    friend VX_FORCE_INLINE constexpr color operator>>(const color& c, scalar_type scalar) noexcept
//    {
//        return color(c.r >> scalar, c.g >> scalar, c.b >> scalar, c.a >> scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator>>(scalar_type scalar, const color& c) noexcept
//    {
//        return color(scalar >> c.r, scalar >> c.g, scalar >> c.b, scalar >> c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr color operator>>(const color& v1, const color& v2) noexcept
//    {
//        return color(v1.r >> v2.r, v1.g >> v2.g, v1.b >> v2.b, v1.a >> v2.a);
//    }
//
//    // not (~)
//
//    VX_FORCE_INLINE constexpr color operator~() const noexcept
//    {
//        return color(~r, ~g, ~b, ~a);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // unary arithmetic operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // addition (+=)
//
//    VX_FORCE_INLINE constexpr color& operator+=(scalar_type scalar) noexcept
//    {
//        r += scalar;
//        g += scalar;
//        b += scalar;
//        a += scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator+=(const color& c) noexcept
//    {
//        r += c.r;
//        g += c.g;
//        b += c.b;
//        a += c.a;
//        return *this;
//    }
//
//    // subtraction (-=)
//
//    VX_FORCE_INLINE constexpr color& operator-=(scalar_type scalar) noexcept
//    {
//        r -= scalar;
//        g -= scalar;
//        b -= scalar;
//        a -= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator-=(const color& c) noexcept
//    {
//        r -= c.r;
//        g -= c.g;
//        b -= c.b;
//        a -= c.a;
//        return *this;
//    }
//
//    // multiplication (*=)
//
//    VX_FORCE_INLINE constexpr color& operator*=(scalar_type scalar) noexcept
//    {
//        r *= scalar;
//        g *= scalar;
//        b *= scalar;
//        a *= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator*=(const color& c) noexcept
//    {
//        r *= c.r;
//        g *= c.g;
//        b *= c.b;
//        a *= c.a;
//        return *this;
//    }
//
//    // division (/=)
//
//    VX_FORCE_INLINE constexpr color& operator/=(scalar_type scalar) noexcept
//    {
//        r /= scalar;
//        g /= scalar;
//        b /= scalar;
//        a /= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator/=(const color& c) noexcept
//    {
//        r /= c.r;
//        g /= c.g;
//        b /= c.b;
//        a /= c.a;
//        return *this;
//    }
//
//    // modulo (%=)
//
//    VX_FORCE_INLINE constexpr color& operator%=(scalar_type scalar) noexcept
//    {
//        r %= scalar;
//        g %= scalar;
//        b %= scalar;
//        a %= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator%=(const color& c) noexcept
//    {
//        r %= c.r;
//        g %= c.g;
//        b %= c.b;
//        a %= c.a;
//        return *this;
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // unary bit operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // and (&=)
//
//    VX_FORCE_INLINE constexpr color& operator&=(scalar_type scalar) noexcept
//    {
//        r &= scalar;
//        g &= scalar;
//        b &= scalar;
//        a &= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator&=(const color& c) noexcept
//    {
//        r &= c.r;
//        g &= c.g;
//        b &= c.b;
//        a &= c.a;
//        return *this;
//    }
//
//    // or (|=)
//
//    VX_FORCE_INLINE constexpr color& operator|=(scalar_type scalar) noexcept
//    {
//        r |= scalar;
//        g |= scalar;
//        b |= scalar;
//        a |= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator|=(const color& c) noexcept
//    {
//        r |= c.r;
//        g |= c.g;
//        b |= c.b;
//        a |= c.a;
//        return *this;
//    }
//
//    // xor (^=)
//
//    VX_FORCE_INLINE constexpr color& operator^=(scalar_type scalar) noexcept
//    {
//        r ^= scalar;
//        g ^= scalar;
//        b ^= scalar;
//        a ^= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator^=(const color& c) noexcept
//    {
//        r ^= c.r;
//        g ^= c.g;
//        b ^= c.b;
//        a ^= c.a;
//        return *this;
//    }
//
//    // left shift (<<=)
//
//    VX_FORCE_INLINE constexpr color& operator<<=(scalar_type scalar) noexcept
//    {
//        r <<= scalar;
//        g <<= scalar;
//        b <<= scalar;
//        a <<= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator<<=(const color& c) noexcept
//    {
//        r <<= c.r;
//        g <<= c.g;
//        b <<= c.b;
//        a <<= c.a;
//        return *this;
//    }
//
//    // right shift (>>=)
//
//    VX_FORCE_INLINE constexpr color& operator>>=(scalar_type scalar) noexcept
//    {
//        r >>= scalar;
//        g >>= scalar;
//        b >>= scalar;
//        a >>= scalar;
//        return *this;
//    }
//
//    VX_FORCE_INLINE constexpr color& operator>>=(const color& c) noexcept
//    {
//        r >>= c.r;
//        g >>= c.g;
//        b >>= c.b;
//        a >>= c.a;
//        return *this;
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // boolean operators
//    ///////////////////////////////////////////////////////////////////////////////
//
//    // and (&&)
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const color& c, scalar_type scalar) noexcept
//    {
//        return vec<size, bool>(c.r && scalar, c.g && scalar, c.b && scalar, c.a && scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(scalar_type scalar, const color& c) noexcept
//    {
//        return vec<size, bool>(scalar && c.r, scalar && c.g, scalar && c.b, scalar && c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const color& v1, const color& v2) noexcept
//    {
//        return vec<size, bool>(v1.r && v2.r, v1.g && v2.g, v1.b && v2.b, v1.a && v2.a);
//    }
//
//    // or (||)
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const color& c, scalar_type scalar) noexcept
//    {
//        return vec<size, bool>(c.r || scalar, c.g || scalar, c.b || scalar, c.a || scalar);
//    }
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(scalar_type scalar, const color& c) noexcept
//    {
//        return vec<size, bool>(scalar || c.r, scalar || c.g, scalar || c.b, scalar || c.a);
//    }
//
//    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const color& v1, const color& v2) noexcept
//    {
//        return vec<size, bool>(v1.r || v2.r, v1.g || v2.g, v1.b || v2.b, v1.a || v2.a);
//    }
//
//    // not (!)
//
//    VX_FORCE_INLINE constexpr vec<size, bool> operator!() const noexcept
//    {
//        return vec<size, bool>(!r, !g, !b, !a);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//    // constants
//    ///////////////////////////////////////////////////////////////////////////////
//
//    static VX_FORCE_INLINE constexpr color zero() noexcept { return color(static_cast<scalar_type>(0)); }
//    static VX_FORCE_INLINE constexpr color one() noexcept { return color(static_cast<scalar_type>(1)); }
//};

} // namespace math
} // namespace vx