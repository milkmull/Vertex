#pragma once

#include "../detail/base_types.h"
#include "../../math/fn/fn_comparison.h"

namespace vx {
namespace math {

// =============== is_equal_approx ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_equal_approx(
    const rect_t<T>& a,
    const rect_t<T>& b,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_equal_approx(a.position, b.position, epsilon)
        && math::is_equal_approx(a.size, b.size, epsilon);
}

// =============== is_zero_approx ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_zero_approx(
    const rect_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return math::is_zero_approx(x.position, epsilon)
        && math::is_zero_approx(x.size, epsilon);
}

// =============== is_finite ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_finite(const rect_t<T>& x)
{
    return math::is_finite(x.position)
        && math::is_finite(x.size);
}

// =============== is_infinite ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_infinite(const rect_t<T>& x)
{
    return math::is_infinite(x.position)
        || math::is_infinite(x.size);
}

// =============== is_nan ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr bool is_nan(const rect_t<T>& x)
{
    return math::is_nan(x.position)
        || math::is_nan(x.size);
}

// =============== round_if_close ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr auto round_if_close(
    const rect_t<T>& x,
    const T epsilon = math::epsilon<T>
)
{
    return rect_t<T>(
        math::round_if_close(x.position, epsilon),
        math::round_if_close(x.size, epsilon)
    );
}

}
}