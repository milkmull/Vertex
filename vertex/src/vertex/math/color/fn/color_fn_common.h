#pragma once

#include "../detail/common.h"
#include "../../math/fn/fn_common.h"

namespace vx {
namespace math {

// =============== clamp ===============

template <typename T>
inline constexpr colorf_t<T> clamp(
    const colorf_t<T>& c,
    const colorf_t<T>& min = colorf_t<T>(colorf_t<T>::MIN_CHANNEL_VALUE),
    const colorf_t<T>& max = colorf_t<T>(colorf_t<T>::MAX_CHANNEL_VALUE)
)
{
    return colorf_t<T>(
        math::clamp(c.r, min.r, max.r),
        math::clamp(c.g, min.g, max.g),
        math::clamp(c.b, min.b, max.b),
        math::clamp(c.a, min.a, max.a)
    );
}

template <typename T>
inline constexpr colori_t<T> clamp(
    const colori_t<T>& c,
    const colori_t<T>& min = colori_t<T>(colori_t<T>::MIN_CHANNEL_VALUE),
    const colori_t<T>& max = colori_t<T>(colori_t<T>::MAX_CHANNEL_VALUE)
)
{
    return colori_t<T>(
        math::clamp(c.r, min.r, max.r),
        math::clamp(c.g, min.g, max.g),
        math::clamp(c.b, min.b, max.b),
        math::clamp(c.a, min.a, max.a)
    );
}

}
}
