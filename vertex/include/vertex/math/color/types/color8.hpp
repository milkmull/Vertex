#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/math/color/types/base.hpp"
#include "vertex/math/core/functions/common.hpp" // for clamp

namespace vx {
namespace math {

template <>
struct color_t<u8>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = u8;
    using type = color_t<scalar_type>;
    static constexpr size_t size = 4;
    using vec_type = vec<4, int32_t>;

    static constexpr scalar_type max_channel_value = static_cast<scalar_type>(255);
    static constexpr scalar_type min_channel_value = static_cast<scalar_type>(0);

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type r, g, b, a;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr color_t() noexcept
        : r(min_channel_value)
        , g(min_channel_value)
        , b(min_channel_value)
        , a(max_channel_value) {}

    VX_FORCE_INLINE constexpr color_t(const type& c) noexcept
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit color_t(scalar_type scalar) noexcept
        : r(scalar), g(scalar), b(scalar), a(max_channel_value) {}

    VX_FORCE_INLINE constexpr color_t(
        scalar_type cr,
        scalar_type cg,
        scalar_type cb,
        scalar_type ca = max_channel_value
    ) noexcept
        : r(cr), g(cg), b(cb), a(ca) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VX_MATH_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit color_t(U scalar) noexcept
        : r(static_cast<scalar_type>(scalar))
        , g(static_cast<scalar_type>(scalar))
        , b(static_cast<scalar_type>(scalar))
        , a(max_channel_value) {}

    template <typename R, typename G, typename B>
    VX_FORCE_INLINE constexpr color_t(R cr, G cg, B cb) noexcept
        : r(static_cast<scalar_type>(cr))
        , g(static_cast<scalar_type>(cg))
        , b(static_cast<scalar_type>(cb))
        , a(max_channel_value) {}

    template <typename R, typename G, typename B, typename A>
    VX_FORCE_INLINE constexpr color_t(R cr, G cg, B cb, A ca) noexcept
        : r(static_cast<scalar_type>(cr))
        , g(static_cast<scalar_type>(cg))
        , b(static_cast<scalar_type>(cb))
        , a(static_cast<scalar_type>(ca)) {}

    template <typename RGB>
    VX_FORCE_INLINE constexpr explicit color_t(const vec<3, RGB>& crgb) noexcept
        : r(static_cast<scalar_type>(crgb.x))
        , g(static_cast<scalar_type>(crgb.y))
        , b(static_cast<scalar_type>(crgb.z))
        , a(max_channel_value) {}

    template <typename RGB, typename A>
    VX_FORCE_INLINE constexpr color_t(const vec<3, RGB>& crgb, A ca) noexcept
        : r(static_cast<scalar_type>(crgb.x))
        , g(static_cast<scalar_type>(crgb.y))
        , b(static_cast<scalar_type>(crgb.z))
        , a(static_cast<scalar_type>(ca)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit color_t(const vec<4, U>& v) noexcept
        : r(static_cast<scalar_type>(v.x))
        , g(static_cast<scalar_type>(v.y))
        , b(static_cast<scalar_type>(v.z))
        , a(static_cast<scalar_type>(v.w)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // color conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    // int to int

    template <typename U, VX_MATH_REQ(is_int<U>::value)>
    VX_FORCE_INLINE constexpr explicit color_t(const color_t<U>& c)
        : color_t(color(c)) {}

    // float to int

    template <typename U, VX_MATH_REQ(is_float<U>::value)>
    VX_FORCE_INLINE constexpr color_t(const color_t<U>& c)
        : r(static_cast<scalar_type>(clamp(c.r, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , g(static_cast<scalar_type>(clamp(c.g, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , b(static_cast<scalar_type>(clamp(c.b, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , a(static_cast<scalar_type>(clamp(c.a, static_cast<U>(0), static_cast<U>(1)) * max_channel_value)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U = scalar_type>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const noexcept
    {
        return vec<4, U>(
            static_cast<U>(r),
            static_cast<U>(g),
            static_cast<U>(b),
            static_cast<U>(a)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // hex conversion
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type from_hex(uint32_t hex) noexcept
    {
        return type(
            (hex >>  0) & 0xFF,
            (hex >>  8) & 0xFF,
            (hex >> 16) & 0xFF,
            (hex >> 24) & 0xFF
        );
    }

    VX_FORCE_INLINE constexpr uint32_t to_hex() const noexcept
    {
        return (
            (static_cast<uint32_t>(r) <<  0) |
            (static_cast<uint32_t>(g) <<  8) |
            (static_cast<uint32_t>(b) << 16) |
            (static_cast<uint32_t>(a) << 24)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& c) noexcept
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(const vec<4, scalar_type>& v) noexcept
    {
        r = v.x;
        g = v.y;
        b = v.z;
        a = v.w;
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
            case 0: return r;
            case 1: return g;
            case 2: return b;
            case 3: return a;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0: return r;
            case 1: return g;
            case 2: return b;
            case 3: return a;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    // equal (==)

    friend VX_FORCE_INLINE constexpr bool operator==(const type& c1, const type& c2) noexcept
    {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
    }

    // not equal (!=)

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& c1, const type& c2) noexcept
    {
        return c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a;
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& c1, const type& c2) noexcept
    {
        if (c1.r != c2.r) return c1.r > c2.r;
        if (c1.g != c2.g) return c1.g > c2.g;
        if (c1.b != c2.b) return c1.b > c2.b;
        return c1.a > c2.a;
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& c1, const type& c2) noexcept
    {
        if (c1.r != c2.r) return c1.r < c2.r;
        if (c1.g != c2.g) return c1.g < c2.g;
        if (c1.b != c2.b) return c1.b < c2.b;
        return c1.a < c2.a;
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& c1, const type& c2) noexcept
    {
        if (c1.r != c2.r) return c1.r > c2.r;
        if (c1.g != c2.g) return c1.g > c2.g;
        if (c1.b != c2.b) return c1.b > c2.b;
        return c1.a >= c2.a;
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& c1, const type& c2) noexcept
    {
        if (c1.r != c2.r) return c1.r < c2.r;
        if (c1.g != c2.g) return c1.g < c2.g;
        if (c1.b != c2.b) return c1.b < c2.b;
        return c1.a <= c2.a;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+r, +g, +b, +a);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-r, -g, -b, -a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // incrememnt (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        r++;
        g++;
        b++;
        a++;
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
        r--;
        g--;
        b--;
        a--;
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

    friend VX_FORCE_INLINE constexpr type operator+(const type& c, scalar_type scalar) noexcept
    {
        return type(c.r + scalar, c.g + scalar, c.b + scalar, c.a + scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& c) noexcept
    {
        return type(scalar + c.r, scalar + c.g, scalar + c.b, scalar + c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& c1, const type& c2) noexcept
    {
        return type(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& c, scalar_type scalar) noexcept
    {
        return type(c.r - scalar, c.g - scalar, c.b - scalar, c.a - scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& c) noexcept
    {
        return type(scalar - c.r, scalar - c.g, scalar - c.b, scalar - c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& c1, const type& c2) noexcept
    {
        return type(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& c, scalar_type scalar) noexcept
    {
        return type(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& c) noexcept
    {
        return type(scalar * c.r, scalar * c.g, scalar * c.b, scalar * c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& c, f32 scalar) noexcept
    {
        return type(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(f32 scalar, const type& c) noexcept
    {
        return type(scalar * c.r, scalar * c.g, scalar * c.b, scalar * c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& c1, const type& c2) noexcept
    {
        return type(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& c, scalar_type scalar) noexcept
    {
        return type(c.r / scalar, c.g / scalar, c.b / scalar, c.a / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& c) noexcept
    {
        return type(scalar / c.r, scalar / c.g, scalar / c.b, scalar / c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& c, f32 scalar) noexcept
    {
        return type(c.r / scalar, c.g / scalar, c.b / scalar, c.a / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(f32 scalar, const type& c) noexcept
    {
        return type(scalar / c.r, scalar / c.g, scalar / c.b, scalar / c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& c1, const type& c2) noexcept
    {
        return type(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b, c1.a / c2.a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        r += scalar;
        g += scalar;
        b += scalar;
        a += scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& c) noexcept
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        r -= scalar;
        g -= scalar;
        b -= scalar;
        a -= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& c) noexcept
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(f32 scalar) noexcept
    {
        (*this) = (*this) * scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& c) noexcept
    {
        r *= c.r;
        g *= c.g;
        b *= c.b;
        a *= c.a;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(f32 scalar) noexcept
    {
        (*this) = (*this) / scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& c) noexcept
    {
        r /= c.r;
        g /= c.g;
        b /= c.b;
        a /= c.a;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // channels
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U = int32_t>
    VX_FORCE_INLINE constexpr vec<3, U> rgb() const noexcept
    {
        return vec<3, U>(
            static_cast<U>(r),
            static_cast<U>(g),
            static_cast<U>(b)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // colors
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(min_channel_value, min_channel_value, min_channel_value, min_channel_value); }
    static VX_FORCE_INLINE constexpr type one() noexcept { return type(max_channel_value, max_channel_value, max_channel_value, max_channel_value); }

    static VX_FORCE_INLINE constexpr type clear() noexcept { return zero(); }
    static VX_FORCE_INLINE constexpr type white() noexcept { return type(max_channel_value); }
    static VX_FORCE_INLINE constexpr type black() noexcept { return type(min_channel_value); }

    static VX_FORCE_INLINE constexpr type gray() noexcept { return type(max_channel_value / static_cast<scalar_type>(2)); }
    static VX_FORCE_INLINE constexpr type grey() noexcept { return gray(); }

    static VX_FORCE_INLINE constexpr type red() noexcept { return type(max_channel_value, min_channel_value, min_channel_value); }
    static VX_FORCE_INLINE constexpr type green() noexcept { return type(min_channel_value, max_channel_value, min_channel_value); }
    static VX_FORCE_INLINE constexpr type blue() noexcept { return type(min_channel_value, min_channel_value, max_channel_value); }

    static VX_FORCE_INLINE constexpr type yellow() noexcept { return type(max_channel_value, max_channel_value, min_channel_value); }
    static VX_FORCE_INLINE constexpr type magenta() noexcept { return type(max_channel_value, min_channel_value, max_channel_value); }
    static VX_FORCE_INLINE constexpr type cyan() noexcept { return type(min_channel_value, max_channel_value, max_channel_value); }
};

} // namespace math
} // namespace vx