#pragma once

#include "vertex/math/types/vec/vec4b.hpp"

namespace vx {
namespace math {

template <>
struct vec<4, d>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = d;
    using type = vec<4, d>;
    static constexpr size_t size = 4;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y, z, w;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<scalar_type>(0))
        , y(static_cast<scalar_type>(0))
        , z(static_cast<scalar_type>(0))
        , w(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z), w(v.w) {}

    VX_FORCE_INLINE constexpr vec(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(scalar_type scalar) noexcept
        : x(scalar), y(scalar), z(scalar), w(scalar) {}

    VX_FORCE_INLINE constexpr vec(scalar_type x, scalar_type y, scalar_type z, scalar_type w) noexcept
        : x(x), y(y), z(z), w(w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar))
        , z(static_cast<scalar_type>(scalar))
        , w(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y, typename Z, typename W, VXM_REQ_NUM4(X, Y, Z, W)>
    VX_FORCE_INLINE constexpr vec(X x, Y y, Z z, W w) noexcept
        : x(static_cast<scalar_type>(x))
        , y(static_cast<scalar_type>(y))
        , z(static_cast<scalar_type>(z))
        , w(static_cast<scalar_type>(w)) {}

    template <typename XY, typename Z, typename W, VXM_REQ_NUM3(XY, Z, W)>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z z, W w) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(z))
        , w(static_cast<scalar_type>(w)) {}

    template <typename X, typename Y, typename ZW, VXM_REQ_NUM3(X, Y, ZW)>
    VX_FORCE_INLINE constexpr vec(X x, Y y, const vec<2, ZW>& vzw) noexcept
        : x(static_cast<scalar_type>(x))
        , y(static_cast<scalar_type>(y))
        , z(static_cast<scalar_type>(vzw.x))
        , w(static_cast<scalar_type>(vzw.y)) {}

    template <typename XYZ, typename W, VXM_REQ_NUM2(XYZ, W)>
    VX_FORCE_INLINE constexpr vec(const vec<3, XYZ>& vxyz, W w) noexcept
        : x(static_cast<scalar_type>(vxyz.x))
        , y(static_cast<scalar_type>(vxyz.y))
        , z(static_cast<scalar_type>(vxyz.z))
        , w(static_cast<scalar_type>(w)) {}

    template <typename X, typename YZW, VXM_REQ_NUM2(X, YZW)>
    VX_FORCE_INLINE constexpr vec(X x, const vec<3, YZW>& vyzw) noexcept
        : x(static_cast<scalar_type>(x))
        , y(static_cast<scalar_type>(vyzw.x))
        , z(static_cast<scalar_type>(vyzw.y))
        , w(static_cast<scalar_type>(vyzw.z)) {}

    template <typename XY, typename ZW, VXM_REQ_NUM2(XY, ZW)>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, const vec<2, ZW>& vzw) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(vzw.x))
        , w(static_cast<scalar_type>(vzw.y)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z))
        , w(static_cast<scalar_type>(v.w)) {}

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

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr type& operator=(const vec<4, U>& v) noexcept
    {
        x = static_cast<scalar_type>(v.x);
        y = static_cast<scalar_type>(v.y);
        z = static_cast<scalar_type>(v.z);
        w = static_cast<scalar_type>(v.w);
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
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

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

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2) noexcept
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2) noexcept
    {
        return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+x, +y, +z, +w);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-x, -y, -z, -w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // incrememnt (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        x++;
        y++;
        z++;
        w++;
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
        z--;
        w--;
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
        return type(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2) noexcept
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2) noexcept
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2) noexcept
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2) noexcept
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr vec4b operator&&(const type& v1, const type& v2) noexcept
    {
        return vec4b(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr vec4b operator||(const type& v1, const type& v2) noexcept
    {
        return vec4b(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
    }

    // not (!)

    VX_FORCE_INLINE constexpr vec4b operator!() const noexcept
    {
        return vec4b(!x, !y, !z, !w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO() noexcept { return type(static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type ONE() noexcept { return type(static_cast<scalar_type>(1)); }
};

} // namespace math
} // namespace vx