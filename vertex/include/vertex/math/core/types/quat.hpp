#pragma once

#include "vertex/config/assert.hpp"
#include "vertex/math/core/types/base.hpp"

#include "vertex/math/simd/quatf.hpp"

namespace vx {
namespace math {

// key formuals:
// q = s + v = cos(t / 2) + sin(t / 2) * v
// acos(s) * 2 = t
// 1 - (s * s) = sin2(t / 2)
// |cross(u, v)| = |u||v|sin(t)

template <typename T>
struct alignas(simd::quat_t<T>::calulate_alignment()) quat_t
{
    VX_STATIC_ASSERT(is_float<T>::value, "type T must be floating point type");

#if defined(VXM_ENABLE_SIMD)

#   define _SIMD_OVERLOAD(cond) template <typename _T = scalar_type, VXM_REQ( (simd::quat_t<_T>::cond))>
#   define _SIMD_FALLBACK(cond) template <typename _T = scalar_type, VXM_REQ(!(simd::quat_t<_T>::cond))>

#else

#   define _SIMD_OVERLOAD(cond) template <typename _T = scalar_type, VXM_REQ(!(is_same<_T, _T>::value))>
#   define _SIMD_FALLBACK(cond)

#endif

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

    _SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& q1, const type& q2) noexcept
    {
        return type(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
    }

    // subtraction (-)

    _SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& q1, const type& q2) noexcept
    {
        return type(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
    }

    // multiplication (*)

    _SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(const type& q, scalar_type scalar) noexcept
    {
        return type(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
    }

    _SIMD_FALLBACK(HAVE_MUL)
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

    _SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(const type& q, scalar_type scalar) noexcept
    {
        return type(q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    _SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator+=(const type& q) noexcept
    {
        w += q.w;
        x += q.x;
        y += q.y;
        z += q.z;
        return *this;
    }

    // subtraction (-=)

    _SIMD_FALLBACK(HAVE_SUB)
    VX_FORCE_INLINE constexpr type& operator-=(const type& q) noexcept
    {
        w -= q.w;
        x -= q.x;
        y -= q.y;
        z -= q.z;
        return *this;
    }

    // multiplication (*=)

    _SIMD_FALLBACK(HAVE_MUL)
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

    VX_FORCE_INLINE constexpr vec<3, scalar_type> xyz() const noexcept{ return vec<3, scalar_type>(x, y, z); }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type identity() noexcept { return type(static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }

        ///////////////////////////////////////////////////////////////////////////////
    // simd conversion
    ///////////////////////////////////////////////////////////////////////////////

    using simd_type = typename simd::quat_t<scalar_type>;
    using simd_data_type = typename simd_type::data_type;

    VX_FORCE_INLINE quat_t(const simd_data_type& d) noexcept
        : quat_t(*(const quat_t*)(&d)) {}

    VX_FORCE_INLINE operator simd_data_type& () noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(simd_data_type*)(this);
    }

    VX_FORCE_INLINE operator const simd_data_type& () const noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(const simd_data_type*)(this);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    _SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE type operator+(const type& q1, const type& q2) noexcept
    {
        return simd_type::add(q1, q2);
    }

    // subtraction (-)

    _SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE type operator-(const type& q1, const type& q2) noexcept
    {
        return simd_type::sub(q1, q2);
    }

    // multiplication (*)

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(const type& q, scalar_type scalar) noexcept
    {
        return simd_type::mul(q, scalar);
    }

    _SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(scalar_type scalar, const type& q) noexcept
    {
        return simd_type::mul(q, scalar);
    }

    // diqision (/)

    _SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE type operator/(const type& q, scalar_type scalar) noexcept
    {
        return simd_type::div(q, scalar);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    _SIMD_OVERLOAD(HAVE_ADD)
    VX_FORCE_INLINE type& operator+=(const type& q) noexcept
    {
        (*this) = (*this) + q;
        return *this;
    }

    // subtraction (-=)

    _SIMD_OVERLOAD(HAVE_SUB)
    VX_FORCE_INLINE type& operator-=(const type& q) noexcept
    {
        (*this) = (*this) - q;
        return *this;
    }

    // multiplication (*=)

    _SIMD_OVERLOAD(HAVE_MUL)
    VX_FORCE_INLINE type& operator*=(scalar_type scalar) noexcept
    {
        (*this) = (*this) * scalar;
        return *this;
    }

    // division (/=)

    _SIMD_OVERLOAD(HAVE_DIV)
    VX_FORCE_INLINE type& operator/=(scalar_type scalar) noexcept
    {
        (*this) = (*this) / scalar;
        return *this;
    }

#   undef _SIMD_OVERLOAD
#   undef _SIMD_FALLBACK
};

} // namespace math
} // namespace vx