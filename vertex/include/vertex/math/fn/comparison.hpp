#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/math/type_traits.hpp"
#include "vertex/math/constants.hpp"

namespace vx {
namespace math {

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
// not_equal
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
// greater
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater(T x, T y) noexcept
{
    return x > y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> greater(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        greater(x.x, y.x),
        greater(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> greater(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        greater(x.x, y.x),
        greater(x.y, y.y),
        greater(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> greater(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        greater(x.x, y.x),
        greater(x.y, y.y),
        greater(x.z, y.z),
        greater(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// less
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less(T x, T y) noexcept
{
    return x < y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> less(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        less(x.x, y.x),
        less(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> less(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        less(x.x, y.x),
        less(x.y, y.y),
        less(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> less(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        less(x.x, y.x),
        less(x.y, y.y),
        less(x.z, y.z),
        less(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// greater_or_equal
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool greater_or_equal(T x, T y) noexcept
{
    return x >= y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> greater_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> greater_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y),
        greater_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> greater_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y),
        greater_or_equal(x.z, y.z),
        greater_or_equal(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// less_or_equal
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool less_or_equal(T x, T y) noexcept
{
    return x <= y;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> less_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec<2, bool>(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> less_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec<3, bool>(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y),
        less_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> less_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec<4, bool>(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y),
        less_or_equal(x.z, y.z),
        less_or_equal(x.w, y.w)
    );
}

//////////////////////////////////////////////////////////////////////////////
// is_inf
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
#if (VX_CPP_STANDARD >= 23)
    return std::isinf(x);
#else
    return x == constants<T>::infinity || x == -constants<T>::infinity;
#endif
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_inf(T x) noexcept
{
    return false;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> is_inf(const vec<2, T>& x) noexcept
{
    return vec<2, bool>(is_inf(x.x), is_inf(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> is_inf(const vec<3, T>& x) noexcept
{
    return vec<3, bool>(is_inf(x.x), is_inf(x.y), is_inf(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> is_inf(const vec<4, T>& x) noexcept
{
    return vec<4, bool>(is_inf(x.x), is_inf(x.y), is_inf(x.z), is_inf(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// any_inf
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool any_inf(const vec<2, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr bool any_inf(const vec<3, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y) || is_inf(x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr bool any_inf(const vec<4, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y) || is_inf(x.z) || is_inf(x.w);
}

//////////////////////////////////////////////////////////////////////////////
// is_nan
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
#if (VX_CPP_STANDARD >= 23)
    return std::isnan(x);
#else
    return x != x;
#endif
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_nan(T x) noexcept
{
    return false;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> is_nan(const vec<2, T>& x) noexcept
{
    return vec<2, bool>(is_nan(x.x), is_nan(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> is_nan(const vec<3, T>& x) noexcept
{
    return vec<3, bool>(is_nan(x.x), is_nan(x.y), is_nan(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> is_nan(const vec<4, T>& x) noexcept
{
    return vec<4, bool>(is_nan(x.x), is_nan(x.y), is_nan(x.z), is_nan(x.w));
}

//////////////////////////////////////////////////////////////////////////////
// any_nan
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool any_nan(const vec<2, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr bool any_nan(const vec<3, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y) || is_nan(x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr bool any_nan(const vec<4, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y) || is_nan(x.z) || is_nan(x.w);
}

//////////////////////////////////////////////////////////////////////////////
// is_zero_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

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

// vec each

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> is_zero_approx(
    const vec<2, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        is_zero_approx(x.x, epsilon),
        is_zero_approx(x.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> is_zero_approx(
    const vec<3, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        is_zero_approx(x.x, epsilon),
        is_zero_approx(x.y, epsilon),
        is_zero_approx(x.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> is_zero_approx(
    const vec<4, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        is_zero_approx(x.x, epsilon),
        is_zero_approx(x.y, epsilon),
        is_zero_approx(x.z, epsilon),
        is_zero_approx(x.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// all_zero_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr b all_zero_approx(
    const vec<2, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr b all_zero_approx(
    const vec<3, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr b all_zero_approx(
    const vec<4, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon)
        && is_zero_approx(x.w, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// equal_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a == b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon),
        equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon),
        equal_approx(x.z, y.z, epsilon),
        equal_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// not_equal_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return !is_zero_approx(a - b, epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool not_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a != b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> not_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        not_equal_approx(x.x, y.x, epsilon),
        not_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> not_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        not_equal_approx(x.x, y.x, epsilon),
        not_equal_approx(x.y, y.y, epsilon),
        not_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> not_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        not_equal_approx(x.x, y.x, epsilon),
        not_equal_approx(x.y, y.y, epsilon),
        not_equal_approx(x.z, y.z, epsilon),
        not_equal_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// greater_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool greater_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool greater_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> greater_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        greater_approx(x.x, y.x, epsilon),
        greater_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> greater_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        greater_approx(x.x, y.x, epsilon),
        greater_approx(x.y, y.y, epsilon),
        greater_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> greater_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        greater_approx(x.x, y.x, epsilon),
        greater_approx(x.y, y.y, epsilon),
        greater_approx(x.z, y.z, epsilon),
        greater_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// less_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool less_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool less_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> less_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        less_approx(x.x, y.x, epsilon),
        less_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> less_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        less_approx(x.x, y.x, epsilon),
        less_approx(x.y, y.y, epsilon),
        less_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> less_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        less_approx(x.x, y.x, epsilon),
        less_approx(x.y, y.y, epsilon),
        less_approx(x.z, y.z, epsilon),
        less_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// greater_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool greater_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a > (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool greater_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a >= b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> greater_or_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> greater_or_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon),
        greater_or_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> greater_or_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon),
        greater_or_equal_approx(x.z, y.z, epsilon),
        greater_or_equal_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// less_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool less_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a < (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool less_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a <= b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec<2, b> less_or_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<2, bool>(
        less_or_equal_approx(x.x, y.x, epsilon),
        less_or_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<3, b> less_or_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<3, bool>(
        less_or_equal_approx(x.x, y.x, epsilon),
        less_or_equal_approx(x.y, y.y, epsilon),
        less_or_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec<4, b> less_or_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec<4, bool>(
        less_or_equal_approx(x.x, y.x, epsilon),
        less_or_equal_approx(x.y, y.y, epsilon),
        less_or_equal_approx(x.z, y.z, epsilon),
        less_or_equal_approx(x.w, y.w, epsilon)
    );
}

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

} // namespace math
} // namespace vx