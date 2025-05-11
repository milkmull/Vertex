#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/math/type_traits.hpp"

namespace vx {
namespace math {

template <>
struct vec<4, b>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = b;
    using type = vec<4, b>;
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

    VX_FORCE_INLINE constexpr vec(scalar_type vx, scalar_type vy, scalar_type vz, scalar_type vw) noexcept
        : x(vx), y(vy), z(vz), w(vw) {}

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
    VX_FORCE_INLINE constexpr vec(X vx, Y vy, Z vz, W vw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy))
        , z(static_cast<scalar_type>(vz))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename XY, typename Z, typename W, VXM_REQ_NUM3(XY, Z, W)>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z vz, W vw) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(vz))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename YZ, typename W, VXM_REQ_NUM3(X, YZ, W)>
    VX_FORCE_INLINE constexpr vec(X vx, const vec<2, YZ>& vyz, W vw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vyz.x))
        , z(static_cast<scalar_type>(vyz.y))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename Y, typename ZW, VXM_REQ_NUM3(X, Y, ZW)>
    VX_FORCE_INLINE constexpr vec(X vx, Y vy, const vec<2, ZW>& vzw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy))
        , z(static_cast<scalar_type>(vzw.x))
        , w(static_cast<scalar_type>(vzw.y)) {}

    template <typename XYZ, typename W, VXM_REQ_NUM2(XYZ, W)>
    VX_FORCE_INLINE constexpr vec(const vec<3, XYZ>& vxyz, W vw) noexcept
        : x(static_cast<scalar_type>(vxyz.x))
        , y(static_cast<scalar_type>(vxyz.y))
        , z(static_cast<scalar_type>(vxyz.z))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename YZW, VXM_REQ_NUM2(X, YZW)>
    VX_FORCE_INLINE constexpr vec(X vx, const vec<3, YZW>& vyzw) noexcept
        : x(static_cast<scalar_type>(vx))
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
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x && scalar, v.y && scalar, v.z && scalar, v.w && scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar && v.x, scalar && v.y, scalar && v.z, scalar && v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2) noexcept
    {
        return type(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr type operator||(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x || scalar, v.y || scalar, v.z || scalar, v.w || scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator||(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar || v.x, scalar || v.y, scalar || v.z, scalar || v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2) noexcept
    {
        return type(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
    }

    // not (!)

    VX_FORCE_INLINE constexpr type operator!() const noexcept
    {
        return type(!x, !y, !z, !w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type TRUE() noexcept { return type(true); }
    static VX_FORCE_INLINE constexpr type FALSE() noexcept { return type(false); }
};

} // namespace math
} // namespace vx