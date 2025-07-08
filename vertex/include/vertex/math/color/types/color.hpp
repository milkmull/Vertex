#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"
#include "../../core/functions/common.hpp" // for clamp

namespace vx {
namespace math {

template <typename T>
struct color_t
{
    VX_STATIC_ASSERT(is_numeric<T>::value, "type T must be numeric type");

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = color_t<T>;
    static constexpr size_t size = 4;

    static constexpr scalar_type max_channel_value = static_cast<scalar_type>(get_max_channel_value<type>::value);
    static constexpr scalar_type min_channel_value = static_cast<scalar_type>(get_min_channel_value<type>::value);

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
        , a(static_cast<scalar_type>(1)) {}

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

    template <typename T, VXM_REQ_FLOAT(T)>
    VX_FORCE_INLINE constexpr color_t(const vec<3, T>& crgb, T ca = max_channel_value) noexcept
        : r(static_cast<scalar_type>(crgb.x))
        , g(static_cast<scalar_type>(crgb.y))
        , b(static_cast<scalar_type>(crgb.z))
        , a(ca) {}

    template <typename T, VXM_REQ_FLOAT(T)>
    VX_FORCE_INLINE constexpr color_t(const vec<4, T>& v) noexcept
        : r(static_cast<scalar_type>(v.x))
        , g(static_cast<scalar_type>(v.y))
        , b(static_cast<scalar_type>(v.z))
        , a(static_cast<scalar_type>(v.w)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit color_t(U scalar) noexcept
        : r(static_cast<scalar_type>(scalar))
        , g(static_cast<scalar_type>(scalar))
        , b(static_cast<scalar_type>(scalar))
        , a(static_cast<scalar_type>(scalar)) {}

    template <typename R, typename G, typename B, typename A>
    VX_FORCE_INLINE constexpr color_t(R cr, G cg, B cb, A ca) noexcept
        : r(static_cast<scalar_type>(cr))
        , g(static_cast<scalar_type>(cg))
        , b(static_cast<scalar_type>(cb))
        , a(static_cast<scalar_type>(ca)) {}

    template <typename RGB, typename A>
    VX_FORCE_INLINE constexpr color_t(const vec<3, RGB>& crgb, A ca) noexcept
        : r(static_cast<scalar_type>(crgb.r))
        , g(static_cast<scalar_type>(crgb.g))
        , b(static_cast<scalar_type>(crgb.b))
        , a(static_cast<scalar_type>(ca)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit color_t(const vec<4, U>& v) noexcept
        : r(static_cast<scalar_type>(v.r))
        , g(static_cast<scalar_type>(v.g))
        , b(static_cast<scalar_type>(v.b))
        , a(static_cast<scalar_type>(v.a)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // color conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    // int to int

    template <typename U, VXM_REQ(is_int<T>::value && is_int<U>::value)>
    VX_FORCE_INLINE constexpr explicit color_t(const color_t<U>& c)
        : color_t(color(c)) {}

    // int to float

    template <typename U, VXM_REQ(is_int<T>::value && is_float<U>::value)>
    VX_FORCE_INLINE constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r) / static_cast<T>(color_t<U>::max_channel_value))
        , g(static_cast<T>(c.g) / static_cast<T>(color_t<U>::max_channel_value))
        , b(static_cast<T>(c.b) / static_cast<T>(color_t<U>::max_channel_value))
        , a(static_cast<T>(c.a) / static_cast<T>(color_t<U>::max_channel_value)) {}

    // float to int

    template <typename U, VXM_REQ(is_float<T>::value && is_int<U>::value)>
    VX_FORCE_INLINE constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(clamp(c.r, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , g(static_cast<T>(clamp(c.g, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , b(static_cast<T>(clamp(c.b, static_cast<U>(0), static_cast<U>(1)) * max_channel_value))
        , a(static_cast<T>(clamp(c.a, static_cast<U>(0), static_cast<U>(1)) * max_channel_value)) {}

    // float to float

    template <typename U, VXM_REQ(is_float<T>::value && is_float<U>::value)>
    VX_FORCE_INLINE constexpr explicit color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r))
        , g(static_cast<T>(c.g))
        , b(static_cast<T>(c.b))
        , a(static_cast<T>(c.a)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
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

    template <typename __T, VXM_REQ(is_same<color_t<__T>, color8>::value)>
    VX_FORCE_INLINE constexpr explicit color_t(uint32_t hex) noexcept
        : r((hex >>  0) & 0xFF)
        , g((hex >>  8) & 0xFF)
        , b((hex >> 16) & 0xFF)
        , a((hex >> 24) & 0xFF) {}

    template <typename __T, VXM_REQ(is_same<color_t<__T>, color8>::value)>
    VX_FORCE_INLINE constexpr explicit operator uint32_t() const noexcept
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

    VX_FORCE_INLINE constexpr color& operator=(const color& c) noexcept
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr color& operator=(const vec<4, U>& v) noexcept
    {
        r = static_cast<scalar_type>(v.x);
        g = static_cast<scalar_type>(v.y);
        b = static_cast<scalar_type>(v.z);
        a = static_cast<scalar_type>(v.w);
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

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator+(const type& c, T scalar) noexcept
    {
        return type(c.r + scalar, c.g + scalar, c.b + scalar, c.a + scalar);
    }

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator+(T scalar, const type& c) noexcept
    {
        return type(scalar + c.r, scalar + c.g, scalar + c.b, scalar + c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& c1, const type& c2) noexcept
    {
        return type(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
    }

    // subtraction (-)

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator-(const type& c, T scalar) noexcept
    {
        return type(c.r - scalar, c.g - scalar, c.b - scalar, c.a - scalar);
    }

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator-(T scalar, const type& c) noexcept
    {
        return type(scalar - c.r, scalar - c.g, scalar - c.b, scalar - c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& c1, const type& c2) noexcept
    {
        return type(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
    }

    // multiplication (*)

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator*(const type& c, T scalar) noexcept
    {
        return type(c.r * scalar, c.g * scalar, c.b * scalar, c.a * scalar);
    }

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator*(T scalar, const type& c) noexcept
    {
        return type(scalar * c.r, scalar * c.g, scalar * c.b, scalar * c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& c1, const type& c2) noexcept
    {
        return type(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
    }

    // division (/)

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator/(const type& c, T scalar) noexcept
    {
        return type(c.r / scalar, c.g / scalar, c.b / scalar, c.a / scalar);
    }

    template <typename T, VXM_REQ_NUM(T)>
    friend VX_FORCE_INLINE constexpr type operator/(T scalar, const type& c) noexcept
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

    template <typename T, VXM_REQ_NUM(T)>
    VX_FORCE_INLINE constexpr type& operator+=(T scalar) noexcept
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

    template <typename T, VXM_REQ_NUM(T)>
    VX_FORCE_INLINE constexpr type& operator-=(T scalar) noexcept
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

    template <typename T, VXM_REQ_NUM(T)>
    VX_FORCE_INLINE constexpr type& operator*=(T scalar) noexcept
    {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        a *= scalar;
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

    template <typename T, VXM_REQ_NUM(T)>
    VX_FORCE_INLINE constexpr type& operator/=(T scalar) noexcept
    {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
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

    template <typename T = scalar_type>
    VX_FORCE_INLINE constexpr auto rgb() const noexcept
    {
        return vec<3, T>(r, g, b);
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