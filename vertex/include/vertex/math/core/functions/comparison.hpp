#pragma once

#include "vertex/config/language_config.hpp"
#include "../constants.hpp"
#include "../types.hpp"

namespace vx {
namespace math {

//////////////////////////////////////////////////////////////////////////////
// equal
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr bool equal(T x, T y) noexcept
{
    return x == y;
}

//////////////////////////////////////////////////////////////////////////////
// each_equal
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        equal(x.x, y.x),
        equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
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

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal(T x, T y) noexcept
{
    return x != y;
}

//////////////////////////////////////////////////////////////////////////////
// each_not_equal
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_not_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_not_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_not_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
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

template <typename T>
VX_FORCE_INLINE constexpr bool greater(T x, T y) noexcept
{
    return x > y;
}

//////////////////////////////////////////////////////////////////////////////
// each_greater
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_greater(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        greater(x.x, y.x),
        greater(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_greater(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        greater(x.x, y.x),
        greater(x.y, y.y),
        greater(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_greater(const vec<4, T>& x, const vec<4, T>& y) noexcept
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

template <typename T>
VX_FORCE_INLINE constexpr bool less(T x, T y) noexcept
{
    return x < y;
}

//////////////////////////////////////////////////////////////////////////////
// each_less
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_less(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        less(x.x, y.x),
        less(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_less(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        less(x.x, y.x),
        less(x.y, y.y),
        less(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_less(const vec<4, T>& x, const vec<4, T>& y) noexcept
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

template <typename T>
VX_FORCE_INLINE constexpr bool greater_or_equal(T x, T y) noexcept
{
    return x >= y;
}

//////////////////////////////////////////////////////////////////////////////
// each_greater_or_equal
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_greater_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_greater_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        greater_or_equal(x.x, y.x),
        greater_or_equal(x.y, y.y),
        greater_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_greater_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
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

template <typename T>
VX_FORCE_INLINE constexpr bool less_or_equal(T x, T y) noexcept
{
    return x <= y;
}

//////////////////////////////////////////////////////////////////////////////
// each_less_or_equal
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_less_or_equal(const vec<2, T>& x, const vec<2, T>& y) noexcept
{
    return vec2b(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_less_or_equal(const vec<3, T>& x, const vec<3, T>& y) noexcept
{
    return vec3b(
        less_or_equal(x.x, y.x),
        less_or_equal(x.y, y.y),
        less_or_equal(x.z, y.z)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_less_or_equal(const vec<4, T>& x, const vec<4, T>& y) noexcept
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
VX_FORCE_INLINE constexpr bool is_inf(const vec<2, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const vec<3, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y) || is_inf(x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const vec<4, T>& x) noexcept
{
    return is_inf(x.x) || is_inf(x.y) || is_inf(x.z) || is_inf(x.w);
}

// mat

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_inf(const mat<2, N, T>& x) noexcept
{
    return is_inf(x.columns[0])
        || is_inf(x.columns[1]);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_inf(const mat<3, N, T>& x) noexcept
{
    return is_inf(x.columns[0])
        || is_inf(x.columns[1])
        || is_inf(x.columns[2]);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_inf(const mat<4, N, T>& x) noexcept
{
    return is_inf(x.columns[0])
        || is_inf(x.columns[1])
        || is_inf(x.columns[2])
        || is_inf(x.columns[3]);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool is_inf(const quat_t<T>& x) noexcept
{
    return is_inf(x.w) || is_inf(x.x) || is_inf(x.y) || is_inf(x.z);
}

//////////////////////////////////////////////////////////////////////////////
// each_inf
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_is_inf(const vec<2, T>& x) noexcept
{
    return vec2b(is_inf(x.x), is_inf(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_is_inf(const vec<3, T>& x) noexcept
{
    return vec3b(is_inf(x.x), is_inf(x.y), is_inf(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_is_inf(const vec<4, T>& x) noexcept
{
    return vec4b(is_inf(x.x), is_inf(x.y), is_inf(x.z), is_inf(x.w));
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
VX_FORCE_INLINE constexpr bool is_nan(const vec<2, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const vec<3, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y) || is_nan(x.z);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const vec<4, T>& x) noexcept
{
    return is_nan(x.x) || is_nan(x.y) || is_nan(x.z) || is_nan(x.w);
}

// mat

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_nan(const mat<2, N, T>& x) noexcept
{
    return is_nan(x.columns[0])
        || is_nan(x.columns[1]);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_nan(const mat<3, N, T>& x) noexcept
{
    return is_nan(x.columns[0])
        || is_nan(x.columns[1])
        || is_nan(x.columns[2]);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_nan(const mat<4, N, T>& x) noexcept
{
    return is_nan(x.columns[0])
        || is_nan(x.columns[1])
        || is_nan(x.columns[2])
        || is_nan(x.columns[3]);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool is_nan(const quat_t<T>& x) noexcept
{
    return is_nan(x.w) || is_nan(x.x) || is_nan(x.y) || is_nan(x.z);
}

//////////////////////////////////////////////////////////////////////////////
// each_nan
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_is_nan(const vec<2, T>& x) noexcept
{
    return vec2b(is_nan(x.x), is_nan(x.y));
}

template <typename T>
VX_FORCE_INLINE constexpr vec3b each_is_nan(const vec<3, T>& x) noexcept
{
    return vec3b(is_nan(x.x), is_nan(x.y), is_nan(x.z));
}

template <typename T>
VX_FORCE_INLINE constexpr vec4b each_is_nan(const vec<4, T>& x) noexcept
{
    return vec4b(is_nan(x.x), is_nan(x.y), is_nan(x.z), is_nan(x.w));
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

// vec

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const vec<2, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const vec<3, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const vec<4, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon)
        && is_zero_approx(x.w, epsilon);
}

// mat

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const mat<2, N, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.columns[0], epsilon)
        && is_zero_approx(x.columns[1], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const mat<3, N, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.columns[0], epsilon)
        && is_zero_approx(x.columns[1], epsilon)
        && is_zero_approx(x.columns[2], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const mat<4, N, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.columns[0], epsilon)
        && is_zero_approx(x.columns[1], epsilon)
        && is_zero_approx(x.columns[2], epsilon)
        && is_zero_approx(x.columns[3], epsilon);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool is_zero_approx(
    const quat_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return is_zero_approx(x.w, epsilon)
        && is_zero_approx(x.x, epsilon)
        && is_zero_approx(x.y, epsilon)
        && is_zero_approx(x.z, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// each_is_zero_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_is_zero_approx(
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
VX_FORCE_INLINE constexpr vec3b each_is_zero_approx(
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
VX_FORCE_INLINE constexpr vec4b each_is_zero_approx(
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
VX_FORCE_INLINE constexpr bool equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x.x, y.x, epsilon)
        && equal_approx(x.y, y.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
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
VX_FORCE_INLINE constexpr bool equal_approx(
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

// mat

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const mat<2, N, T>& a,
    const mat<2, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(a.columns[0], b.columns[0], epsilon)
        && equal_approx(a.columns[1], b.columns[1], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const mat<3, N, T>& a,
    const mat<3, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(a.columns[0], b.columns[0], epsilon)
        && equal_approx(a.columns[1], b.columns[1], epsilon)
        && equal_approx(a.columns[2], b.columns[2], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const mat<4, N, T>& a,
    const mat<4, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(a.columns[0], b.columns[0], epsilon)
        && equal_approx(a.columns[1], b.columns[1], epsilon)
        && equal_approx(a.columns[2], b.columns[2], epsilon)
        && equal_approx(a.columns[3], b.columns[3], epsilon);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool equal_approx(
    const quat_t<T>& a,
    const quat_t<T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(a.w, b.w, epsilon)
        && equal_approx(a.x, b.x, epsilon)
        && equal_approx(a.y, b.y, epsilon)
        && equal_approx(a.z, b.z, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// each_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_equal_approx(
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
VX_FORCE_INLINE constexpr vec3b each_equal_approx(
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
VX_FORCE_INLINE constexpr vec4b each_equal_approx(
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
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const vec<2, T>& x,
    const vec<2, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.x, y.x, epsilon)
        || not_equal_approx(x.y, y.y, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const vec<3, T>& x,
    const vec<3, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.x, y.x, epsilon)
        || not_equal_approx(x.y, y.y, epsilon)
        || not_equal_approx(x.z, y.z, epsilon);
}

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const vec<4, T>& x,
    const vec<4, T>& y,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(x.x, y.x, epsilon)
        || not_equal_approx(x.y, y.y, epsilon)
        || not_equal_approx(x.z, y.z, epsilon)
        || not_equal_approx(x.w, y.w, epsilon);
}

// mat

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const mat<2, N, T>& a,
    const mat<2, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(a.columns[0], b.columns[0], epsilon)
        || not_equal_approx(a.columns[1], b.columns[1], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const mat<3, N, T>& a,
    const mat<3, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(a.columns[0], b.columns[0], epsilon)
        || not_equal_approx(a.columns[1], b.columns[1], epsilon)
        || not_equal_approx(a.columns[2], b.columns[2], epsilon);
}

template <size_t N, typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const mat<4, N, T>& a,
    const mat<4, N, T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(a.columns[0], b.columns[0], epsilon)
        || not_equal_approx(a.columns[1], b.columns[1], epsilon)
        || not_equal_approx(a.columns[2], b.columns[2], epsilon)
        || not_equal_approx(a.columns[3], b.columns[3], epsilon);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool not_equal_approx(
    const quat_t<T>& a,
    const quat_t<T>& b,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return not_equal_approx(a.w, b.w, epsilon)
        || not_equal_approx(a.x, b.x, epsilon)
        || not_equal_approx(a.y, b.y, epsilon)
        || not_equal_approx(a.z, b.z, epsilon);
}

//////////////////////////////////////////////////////////////////////////////
// each_not_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_not_equal_approx(
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
VX_FORCE_INLINE constexpr vec3b each_not_equal_approx(
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
VX_FORCE_INLINE constexpr vec4b each_not_equal_approx(
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

//////////////////////////////////////////////////////////////////////////////
// each_greater_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_greater_approx(
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
VX_FORCE_INLINE constexpr vec3b each_greater_approx(
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
VX_FORCE_INLINE constexpr vec4b each_greater_approx(
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

//////////////////////////////////////////////////////////////////////////////
// each_less_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_less_approx(
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
VX_FORCE_INLINE constexpr vec3b each_less_approx(
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
VX_FORCE_INLINE constexpr vec4b each_less_approx(
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

//////////////////////////////////////////////////////////////////////////////
// each_greater_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_greater_or_equal_approx(
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
VX_FORCE_INLINE constexpr vec3b each_greater_or_equal_approx(
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
VX_FORCE_INLINE constexpr vec4b each_greater_or_equal_approx(
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
// less_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////
// each_less_or_equal_approx
//////////////////////////////////////////////////////////////////////////////

template <typename T>
VX_FORCE_INLINE constexpr vec2b each_less_or_equal_approx(
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
VX_FORCE_INLINE constexpr vec3b each_less_or_equal_approx(
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
VX_FORCE_INLINE constexpr vec4b each_less_or_equal_approx(
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

//////////////////////////////////////////////////////////////////////////////
// is_identity_approx
//////////////////////////////////////////////////////////////////////////////

template <size_t M, size_t N, typename T, VXM_REQ(M == N)>
VX_FORCE_INLINE constexpr bool is_identity_approx(
    const mat<M, N, T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x, mat<M, N, T>::identity(), epsilon);
}

// quaternion

template <typename T>
VX_FORCE_INLINE constexpr bool is_identity_approx(
    const quat_t<T>& x,
    const T epsilon = constants<T>::epsilon
) noexcept
{
    return equal_approx(x, quat_t<T>::identity(), epsilon);
}

///////////////////////////////////////////////////////////////////////////////
// any
///////////////////////////////////////////////////////////////////////////////

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool any(T x) noexcept
{
    return !!x;
}

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

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool all(T x) noexcept
{
    return !!x;
}

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

template <typename T, VXM_REQ_NUM(T)>
VX_FORCE_INLINE constexpr bool none(T x) noexcept
{
    return !x;
}

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