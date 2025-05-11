#pragma once

#include <cmath>

#include "vertex/config/language_config.hpp"
#include "vertex/math/type_traits.hpp"
#include "vertex/math/constants.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// any
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool any(const vec<2, T>& x) noexcept
{
    return x.x || x.y;
}

template <typename T>
VX_FORCE_INLINE constexpr bool any(const vec<3, T>& x) noexcept
{
    return x.x || x.y || x.z;
}

template <typename T>
VX_FORCE_INLINE constexpr bool any(const vec<4, T>& x) noexcept
{
    return x.x || x.y || x.z || x.w;
}

///////////////////////////////////////////////////////////////////////////////
// all
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool all(const vec<2, T>& x) noexcept
{
    return x.x && x.y;
}

template <typename T>
VX_FORCE_INLINE constexpr bool all(const vec<3, T>& x) noexcept
{
    return x.x && x.y && x.z;
}

template <typename T>
VX_FORCE_INLINE constexpr bool all(const vec<4, T>& x) noexcept
{
    return x.x && x.y && x.z && x.w;
}

///////////////////////////////////////////////////////////////////////////////
// none
///////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool none(const vec<2, T>& x) noexcept
{
    return !x.x && !x.y;
}

template <typename T>
VX_FORCE_INLINE constexpr bool none(const vec<3, T>& x) noexcept
{
    return !x.x && !x.y && !x.z;
}

template <typename T>
VX_FORCE_INLINE constexpr bool none(const vec<4, T>& x) noexcept
{
    return !x.x && !x.y && !x.z && !x.w;
}

//////////////////////////////////////////////////////////////////////////////
// equal
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool equal(T x, T y) noexcept
{
    return x == y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z),
        equal(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// not equal
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool not_equal(T x, T y) noexcept
{
    return x != y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> not_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> not_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> not_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z),
        not_equal(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// greater than
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater_than(T x, T y) noexcept
{
    return x > y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> greater_than(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> greater_than(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y),
        greater_than(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> greater_than(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y),
        greater_than(x.z, y.z),
        greater_than(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// greater than or equal
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater_than_or_equal(T x, T y) noexcept
{
    return x >= y;
}

//////////////////////////////////////////////////////////////////////////////
// less than
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less_than(T x, T y) noexcept
{
    return x < y;
}

//////////////////////////////////////////////////////////////////////////////
// less than or equal
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less_than_or_equal(T x, T y) noexcept
{
    return x <= y;
}
//////////////////////////////////////////////////////////////////////////////
// is_inf
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
    return std::isinf(x);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// is_nan
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
    return std::isnan(x);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// is_zero_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = constants<T>::epsilon) noexcept
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = constants<T>::epsilon) noexcept
{
    return x == static_cast<T>(0);
}

//////////////////////////////////////////////////////////////////////////////
// is_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a == b;
}

//////////////////////////////////////////////////////////////////////////////
// is_not_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return !is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a != b;
}

//////////////////////////////////////////////////////////////////////////////
// is_greater_than_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > b;
}

//////////////////////////////////////////////////////////////////////////////
// is_greater_than_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_greater_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a >= b;
}

//////////////////////////////////////////////////////////////////////////////
// is_less_than_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < b;
}

//////////////////////////////////////////////////////////////////////////////
// is_less_than_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_less_than_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a <= b;
}

} // namespace math
} // namespace vx