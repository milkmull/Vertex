#pragma once

#include "vertex/math/config.hpp"
#include "vertex/math/types/vec3b.hpp"

namespace vx {
namespace math {

template <typename T>
struct vec_t<3, T, __detail::component_type::_f>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    static constexpr size_t size = 3;
    static constexpr auto __component_type = __detail::component_type::_f;

    using type = vec_t<size, scalar_type, __component_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y, z;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec_t() noexcept
        : x(static_cast<scalar_type>(0))
        , y(static_cast<scalar_type>(0))
        , z(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr vec_t(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec_t(scalar_type scalar) noexcept
        : x(scalar), y(scalar), z(scalar) {}

    VX_FORCE_INLINE constexpr vec_t(scalar_type vx, scalar_type vy, scalar_type vz) noexcept
        : x(vx), y(vy), z(vz) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar))
        , z(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y, typename Z, VXM_REQ_NUM3(X, Y, Z)>
    VX_FORCE_INLINE constexpr vec_t(X vx, Y vy, Z vz) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy))
        , z(static_cast<scalar_type>(vz)) {}

    template <typename XY, typename Z, VXM_REQ_NUM2(XY, Z)>
    VX_FORCE_INLINE constexpr vec_t(const vec<2, XY>& vxy, Z vz) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(vz)) {}

    template <typename X, typename YZ, VXM_REQ_NUM2(X, YZ)>
    VX_FORCE_INLINE constexpr vec_t(X vx, const vec<2, YZ>& vyz) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vyz.x))
        , z(static_cast<scalar_type>(vyz.y)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(const vec<3, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z)) {}

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

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr type& operator=(const vec<3, U>& v) noexcept
    {
        x = static_cast<scalar_type>(v.x);
        y = static_cast<scalar_type>(v.y);
        z = static_cast<scalar_type>(v.z);
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
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2) noexcept
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2) noexcept
    {
        return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        if (v1.y != v2.y) return v1.y > v2.y;
        return v1.z > v2.z;
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        if (v1.y != v2.y) return v1.y < v2.y;
        return v1.z < v2.z;
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        if (v1.y != v2.y) return v1.y > v2.y;
        return v1.z >= v2.z;
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        if (v1.y != v2.y) return v1.y < v2.y;
        return v1.z <= v2.z;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+x, +y, +z);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-x, -y, -z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // increment (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        x++;
        y++;
        z++;
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
        return type(v.x + scalar, v.y + scalar, v.z + scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar + v.x, scalar + v.y, scalar + v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2) noexcept
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x - scalar, v.y - scalar, v.z - scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar - v.x, scalar - v.y, scalar - v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2) noexcept
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x * scalar, v.y * scalar, v.z * scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar * v.x, scalar * v.y, scalar * v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& v1, const type& v2) noexcept
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x / scalar, v.y / scalar, v.z / scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar / v.x, scalar / v.y, scalar / v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2) noexcept
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
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
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr vec3b operator&&(const type& v1, const type& v2) noexcept
    {
        return vec3b(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr vec3b operator||(const type& v1, const type& v2) noexcept
    {
        return vec3b(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
    }

    // not (!)

    VX_FORCE_INLINE constexpr vec3b operator!() const noexcept
    {
        return vec3b(!x, !y, !z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO() noexcept { return type(static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type ONE() noexcept { return type(static_cast<scalar_type>(1)); }

    static VX_FORCE_INLINE constexpr type RIGHT() noexcept { return type(static_cast<scalar_type>(1), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type LEFT() noexcept { return type(static_cast<scalar_type>(-1), static_cast<scalar_type>(0), static_cast<scalar_type>(0)); }

    static VX_FORCE_INLINE constexpr type UP() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(1), static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type DOWN() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(-1), static_cast<scalar_type>(0)); }

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)

    static VX_FORCE_INLINE constexpr type FORWARD() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(-1)); }
    static VX_FORCE_INLINE constexpr type BACK() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(1)); }

#else

    static VX_FORCE_INLINE constexpr type FORWARD() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(1)); }
    static VX_FORCE_INLINE constexpr type BACK() noexcept { return type(static_cast<scalar_type>(0), static_cast<scalar_type>(0), static_cast<scalar_type>(-1)); }

#endif
};

} // namespace math
} // namespace vx