#pragma once

#include "../detail/common.h"
#include "../../math/fn/fn_common.h"

namespace vx {
namespace math {

// =============== min ===============

template <typename T>
inline constexpr color_t<T> min(
    const color_t<T>& x,
    const color_t<T>& y
)
{
    return color_t<T>(
        math::min(x.r, y.r),
        math::min(x.g, y.g),
        math::min(x.b, y.b),
        math::min(x.a, y.a)
    );
}

// =============== max ===============

template <typename T>
inline constexpr color_t<T> max(
    const color_t<T>& x,
    const color_t<T>& y
)
{
    return color_t<T>(
        math::max(x.r, y.r),
        math::max(x.g, y.g),
        math::max(x.b, y.b),
        math::max(x.a, y.a)
    );
}

// =============== clamp ===============

template <typename T>
inline constexpr color_t<T> clamp(
    const color_t<T>& x,
    const color_t<T>& min = color_t<T>(color_t<T>::MIN_CHANNEL_VALUE),
    const color_t<T>& max = color_t<T>(color_t<T>::MAX_CHANNEL_VALUE)
)
{
    return color_t<T>(
        math::clamp(x.r, min.r, max.r),
        math::clamp(x.g, min.g, max.g),
        math::clamp(x.b, min.b, max.b),
        math::clamp(x.a, min.a, max.a)
    );
}

// =============== invert ===============

template <typename T>
inline constexpr color_t<T> invert(const color_t<T>& c)
{
    return color_t<T>(
        color_t<T>::MAX_CHANNEL_VALUE - c.r,
        color_t<T>::MAX_CHANNEL_VALUE - c.g,
        color_t<T>::MAX_CHANNEL_VALUE - c.b,
        c.a
    );
}

// =============== lighten ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr color_t<T> lighten(const color_t<T>& c, T amount)
{
    return color_t<T>(
        c.r + (static_cast<T>(1) - c.r) * amount,
        c.g + (static_cast<T>(1) - c.g) * amount,
        c.b + (static_cast<T>(1) - c.b) * amount,
        c.a
    );
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr color_t<T> lighten(const color_t<T>& c, typename color_t<T>::float_value_type amount)
{
    using float_type = typename color_t<T>::float_type;
    return lighten(float_type(c), amount);
}

// =============== darken ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr color_t<T> darken(const color_t<T>& c, T amount)
{
    return color_t<T>(
        c.r * (static_cast<T>(1) - amount),
        c.g * (static_cast<T>(1) - amount),
        c.b * (static_cast<T>(1) - amount),
        c.a
    );
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr color_t<T> darken(const color_t<T>& c, typename color_t<T>::float_value_type amount)
{
    using float_type = typename color_t<T>::float_type;
    return darken(float_type(c), amount);
}

// =============== luminance ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr T luminance(const color_t<T>& c)
{
    return (
        (c.r * static_cast<T>(0.2126)) +
        (c.g * static_cast<T>(0.7152)) +
        (c.b * static_cast<T>(0.0722))
    );
}

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline constexpr auto luminance(const color_t<T>& c)
{
    using float_type = typename color_t<T>::float_type;
    return luminance(float_type(c));
}

}
}
