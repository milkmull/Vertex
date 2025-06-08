#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"

namespace vx {
namespace math {

// key formuals:
// q = s + v = cos(t / 2) + sin(t / 2) * v
// acos(s) * 2 = t
// 1 - (s * s) = sin2(t / 2)
// |cross(u, v)| = |u||v|sin(t)

template <typename T>
struct quat_t
{
    VX_STATIC_ASSERT(is_float<T>::value, "type T must be floating point type");

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = quat_t<scalar_type>;
    static constexpr size_t size = 4;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type w, x, y, z;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr quat_t() noexcept
        : w(static_cast<scalar_type>(1))
        , x(static_cast<scalar_type>(0))
        , y(static_cast<scalar_type>(0))
        , z(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr quat_t(const type& q) noexcept
        : w(q.w), x(q.x), y(q.y), z(q.z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr quat_t(scalar_type qw, const vec<3, scalar_type>& v) noexcept
        : w(qw), x(v.x), y(v.y), z(v.z) {}

    VX_FORCE_INLINE constexpr quat_t(
        scalar_type qw,
        scalar_type qx,
        scalar_type qy,
        scalar_type qz
    ) noexcept
        : w(qw), x(qx), y(qy), z(qz) {}

    VX_FORCE_INLINE constexpr quat_t(const vec<4, scalar_type>& v) noexcept
        : w(v.w), x(v.x), y(v.y), z(v.z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename W, typename XYZ>
    VX_FORCE_INLINE constexpr quat_t(W qw, const vec<3, XYZ>& qxyz) noexcept
        : w(static_cast<scalar_type>(qw))
        , x(static_cast<scalar_type>(qxyz.x))
        , y(static_cast<scalar_type>(qxyz.y))
        , z(static_cast<scalar_type>(qxyz.z)) {}

    template <typename W, typename X, typename Y, typename Z>
    VX_FORCE_INLINE constexpr quat_t(W qw, X qx, Y qy, Z qz) noexcept
        : w(static_cast<scalar_type>(qw))
        , x(static_cast<scalar_type>(qx))
        , y(static_cast<scalar_type>(qy))
        , z(static_cast<scalar_type>(qz)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit quat_t(const quat_t<U>& q) noexcept
        : w(static_cast<scalar_type>(q.w))
        , x(static_cast<scalar_type>(q.x))
        , y(static_cast<scalar_type>(q.y))
        , z(static_cast<scalar_type>(q.z)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit quat_t(const vec<4, U>& v) noexcept
        : w(static_cast<scalar_type>(v.w))
        , x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& q) noexcept
    {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const quat_t<U>& q) noexcept
    {
        w = static_cast<scalar_type>(q.w);
        x = static_cast<scalar_type>(q.x);
        y = static_cast<scalar_type>(q.y);
        z = static_cast<scalar_type>(q.z);
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
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const noexcept
    {
        return vec<4, U>(
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z),
            static_cast<U>(w)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& q1, const type& q2) noexcept
    {
        return q1.w == q2.w && q1.x == q2.x && q1.y == q2.y && q1.z == q2.z;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& q1, const type& q2) noexcept
    {
        return q1.w != q2.w || q1.x != q2.x || q1.y != q2.y || q1.z != q2.z;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+w, +x, +y, +z);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-w, -x, -y, -z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr type operator+(const type& q1, const type& q2) noexcept
    {
        return type(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& q1, const type& q2) noexcept
    {
        return type(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& q, scalar_type scalar) noexcept
    {
        return type(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& q) noexcept
    {
        return type(scalar * q.w, scalar * q.x, scalar * q.y, scalar * q.z);
    }

    // https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm

    friend VX_FORCE_INLINE constexpr type operator*(const type& q1, const type& q2) noexcept
    {
        return type(
            (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z),
            (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
            (q1.w * q2.y) + (q1.y * q2.w) + (q1.z * q2.x) - (q1.x * q2.z),
            (q1.w * q2.z) + (q1.z * q2.w) + (q1.x * q2.y) - (q1.y * q2.x)
        );
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& q, scalar_type scalar) noexcept
    {
        return type(q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(const type& q) noexcept
    {
        w += q.w;
        x += q.x;
        y += q.y;
        z += q.z;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(const type& q) noexcept
    {
        w -= q.w;
        x -= q.x;
        y -= q.y;
        z -= q.z;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        w *= scalar;
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& q) noexcept
    {
        (*this) = (*this) * q;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        w /= scalar;
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // operations
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec<3, scalar_type> vector() const noexcept{ return vec<3, scalar_type>(x, y, z); }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type identity() noexcept { return type(static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }

};

} // namespace math
} // namespace vx