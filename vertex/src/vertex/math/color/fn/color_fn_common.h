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
    return c.invert();
}

}
}
