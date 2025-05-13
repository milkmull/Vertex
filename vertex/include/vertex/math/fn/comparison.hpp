#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/math/constants.hpp"
#include "vertex/math/types.hpp"

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
VX_FORCE_INLINE constexpr vec2b equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        equal(x.x, y.x),
        equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr vec2b not_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b not_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b not_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr vec2b greater(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        greater(x.x, y.x),
        greater(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b greater(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        greater(x.x, y.x),
        greater(x.y, y.y),
        greater(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b greater(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr vec2b less(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        less(x.x, y.x),
        less(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b less(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        less(x.x, y.x),
        less(x.y, y.y),
        less(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b less(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr vec2b greater_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b greater_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y),
        greater_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b greater_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr vec2b less_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b less_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y),
        less_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b less_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr bool is_inf(T) noexcept
{
    return false;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b is_inf(const vec<2, T>& x) noexcept
{
    return vec2b(is_inf(x.x), is_inf(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b is_inf(const vec<3, T>& x) noexcept
{
    return vec3b(is_inf(x.x), is_inf(x.y), is_inf(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b is_inf(const vec<4, T>& x) noexcept
{
    return vec4b(is_inf(x.x), is_inf(x.y), is_inf(x.z), is_inf(x.w));
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
VX_FORCE_INLINE constexpr bool is_nan(T) noexcept
{
    return false;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b is_nan(const vec<2, T>& x) noexcept
{
    return vec2b(is_nan(x.x), is_nan(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b is_nan(const vec<3, T>& x) noexcept
{
    return vec3b(is_nan(x.x), is_nan(x.y), is_nan(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b is_nan(const vec<4, T>& x) noexcept
{
    return vec4b(is_nan(x.x), is_nan(x.y), is_nan(x.z), is_nan(x.w));
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
    return ((x < static_cast<T>(0)) ? -x : x) <= epsilon;
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool is_zero_approx(T x, const T epsilon = constants<T>::epsilon) noexcept
{
    (void)epsilon; // suppress unused parameter warning
    return x == static_cast<T>(0);
}

// vec each

template <typename T>
VX_FORCE_INLINE constexpr vec2b is_zero_approx(
    const vec<2, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        is_zero_approx(x.x, epsilon),
        is_zero_approx(x.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b is_zero_approx(
    const vec<3, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        is_zero_approx(x.x, epsilon),
        is_zero_approx(x.y, epsilon),
        is_zero_approx(x.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b is_zero_approx(
    const vec<4, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
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
VX_FORCE_INLINE constexpr bool all_zero_approx(
    const vec<2, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_zero_approx(
    const vec<3, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_zero_approx(
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
    (void)epsilon; // suppress unused parameter warning
    return a == b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon),
        equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
        equal_approx(x.x, y.x, epsilon),
        equal_approx(x.y, y.y, epsilon),
        equal_approx(x.z, y.z, epsilon),
        equal_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// all_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool all_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.x, y.x, epsilon)
        && equal_approx(x.y, y.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.x, y.x, epsilon)
        && equal_approx(x.y, y.y, epsilon)
        && equal_approx(x.z, y.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.x, y.x, epsilon)
        && equal_approx(x.y, y.y, epsilon)
        && equal_approx(x.z, y.z, epsilon)
        && equal_approx(x.w, y.w, epsilon);
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
    (void)epsilon; // suppress unused parameter warning
    return a != b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b not_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        not_equal_approx(x.x, y.x, epsilon),
        not_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b not_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        not_equal_approx(x.x, y.x, epsilon),
        not_equal_approx(x.y, y.y, epsilon),
        not_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b not_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
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
    (void)epsilon; // suppress unused parameter warning
    return a > b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b greater_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        greater_approx(x.x, y.x, epsilon),
        greater_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b greater_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        greater_approx(x.x, y.x, epsilon),
        greater_approx(x.y, y.y, epsilon),
        greater_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b greater_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
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
    (void)epsilon; // suppress unused parameter warning
    return a < b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b less_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        less_approx(x.x, y.x, epsilon),
        less_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b less_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        less_approx(x.x, y.x, epsilon),
        less_approx(x.y, y.y, epsilon),
        less_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b less_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
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
    return a >= (b - epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool greater_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    (void)epsilon; // suppress unused parameter warning
    return a >= b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b greater_or_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b greater_or_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon),
        greater_or_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b greater_or_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
        greater_or_equal_approx(x.x, y.x, epsilon),
        greater_or_equal_approx(x.y, y.y, epsilon),
        greater_or_equal_approx(x.z, y.z, epsilon),
        greater_or_equal_approx(x.w, y.w, epsilon)
    );
}

//////////////////////////////////////////////////////////////////////////////
// all_greater_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool all_greater_or_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_greater_or_equal_approx(x.x, y.x, epsilon)
        && is_greater_or_equal_approx(x.y, y.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_greater_or_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_greater_or_equal_approx(x.x, y.x, epsilon)
        && is_greater_or_equal_approx(x.y, y.y, epsilon)
        && is_greater_or_equal_approx(x.z, y.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool all_greater_or_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_greater_or_equal_approx(x.x, y.x, epsilon)
        && is_greater_or_equal_approx(x.y, y.y, epsilon)
        && is_greater_or_equal_approx(x.z, y.z, epsilon)
        && is_greater_or_equal_approx(x.w, y.w, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// less_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

// scalar

template <typename T, VXM_REQ_FLOAT(T)>
VX_FORCE_INLINE constexpr bool less_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    return a <= (b + epsilon);
}

template <typename T, VXM_REQ_INT(T)>
VX_FORCE_INLINE constexpr bool less_or_equal_approx(T a, T b, const T epsilon = constants<T>::epsilon) noexcept
{
    (void)epsilon; // suppress unused parameter warning
    return a <= b;
}

// vec

template <typename T>
VX_FORCE_INLINE constexpr vec2b less_or_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec2b(
        less_or_equal_approx(x.x, y.x, epsilon),
        less_or_equal_approx(x.y, y.y, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b less_or_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec3b(
        less_or_equal_approx(x.x, y.x, epsilon),
        less_or_equal_approx(x.y, y.y, epsilon),
        less_or_equal_approx(x.z, y.z, epsilon)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b less_or_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return vec4b(
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