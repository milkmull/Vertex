#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/assert.hpp"
#include "vertex/math/types/traits.hpp"

namespace vx {
namespace math {

template <>
struct vec_t<2, bool, __detail::component_type::_b>
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = bool;
    using type = vec_t<2, scalar_type, __detail::component_type::_b>;
    static constexpr size_t size = 2;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec_t() noexcept
        : x(false), y(false) {}

    VX_FORCE_INLINE constexpr vec_t(const type& v) noexcept
        : x(v.x), y(v.y) {}

    VX_FORCE_INLINE constexpr vec_t(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec_t(scalar_type scalar) noexcept
        : x(scalar), y(scalar) {}

    VX_FORCE_INLINE constexpr vec_t(scalar_type vx, scalar_type vy) noexcept
        : x(vx), y(vy) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y, VXM_REQ_NUM2(X, Y)>
    VX_FORCE_INLINE constexpr vec_t(X vx, Y vy) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(const vec<2, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(const vec<3, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr explicit vec_t(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y)) {}

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
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U, VXM_REQ_NUM(U)>
    VX_FORCE_INLINE constexpr type& operator=(const vec<2, U>& v) noexcept
    {
        x = static_cast<scalar_type>(v.x);
        y = static_cast<scalar_type>(v.y);
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
            case 0:     return x;
            case 1:     return y;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0:     return x;
            case 1:     return y;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2) noexcept
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2) noexcept
    {
        return v1.x != v2.x || v1.y != v2.y;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x && scalar, v.y && scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar && v.x, scalar && v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator&&(const type& v1, const type& v2) noexcept
    {
        return type(v1.x && v2.x, v1.y && v2.y);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr type operator||(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x || scalar, v.y || scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator||(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar || v.x, scalar || v.y);
    }

    friend VX_FORCE_INLINE constexpr type operator||(const type& v1, const type& v2) noexcept
    {
        return type(v1.x || v2.x, v1.y || v2.y);
    }

    // not (!)

    VX_FORCE_INLINE constexpr type operator!() const noexcept
    {
        return type(!x, !y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type TRUE() noexcept { return type(true); }
    static VX_FORCE_INLINE constexpr type FALSE() noexcept { return type(false); }
};

} // namespace math
} // namespace vx