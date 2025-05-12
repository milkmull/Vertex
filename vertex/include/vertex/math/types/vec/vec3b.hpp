#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/math/type_traits.hpp"

namespace vx {
namespace math {

template <typename T>
struct vec_t<3, T, __detail::component_type::_b>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = vec_t<3, scalar_type, __detail::component_type::_b>;
    static constexpr size_t size = 3;

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

    VX_FORCE_INLINE constexpr vec_t(type&&) noexcept = default;

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
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~vec_t() noexcept = default;

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