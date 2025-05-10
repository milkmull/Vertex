#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/math/type_traits.hpp"

namespace vx {
namespace math {

template <>
struct vec<3, b>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = b;
    using type = vec<3, b>;
    static constexpr size_t size = 3;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y, z;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<scalar_type>(0))
        , y(static_cast<scalar_type>(0))
        , z(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z) {}

    VX_FORCE_INLINE constexpr vec(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(scalar_type scalar) noexcept
        : x(scalar), y(scalar), z(scalar) {}

    VX_FORCE_INLINE constexpr vec(scalar_type x, scalar_type y, scalar_type z) noexcept
        : x(x), y(y), z(z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar))
        , z(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y, typename Z, VXM_REQ_NUM3(X, Y, Z)>
    VX_FORCE_INLINE constexpr vec(X x, Y y, Z z) noexcept
        : x(static_cast<scalar_type>(x))
        , y(static_cast<scalar_type>(y))
        , z(static_cast<scalar_type>(z)) {}

    template <typename XY, typename Z, VXM_REQ_NUM2(XY, Z)>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z z) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(z)) {}

    template <typename X, typename YZ, VXM_REQ_NUM2(X, YZ)>
    VX_FORCE_INLINE constexpr vec(X x, const vec<2, YZ>& vyz) noexcept
        : x(static_cast<scalar_type>(x))
        , y(static_cast<scalar_type>(vyz.x))
        , z(static_cast<scalar_type>(vyz.y)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec(const vec<3, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z)) {}

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
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr type& operator=(const vec<3, U>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
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

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x && scalar, v.y && scalar, v.z && scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar && v.x, scalar && v.y, scalar && v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2) noexcept
    {
        return type(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr type operator||(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x || scalar, v.y || scalar, v.z || scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator||(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar || v.x, scalar || v.y, scalar || v.z);
    }

    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2) noexcept
    {
        return type(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z);
    }

    // not (!)

    VX_FORCE_INLINE constexpr type operator!() const noexcept
    {
        return type(!x, !y, !z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type TRUE() noexcept { return type(true); }
    static VX_FORCE_INLINE constexpr type FALSE() noexcept { return type(false); }
};

} // namespace math
} // namespace vx