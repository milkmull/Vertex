#pragma once

#include "detail/base_type_defs.h"
#include "fn_common.h"

namespace vx {
namespace math {

// =============== abs ===============

template <typename T>
inline constexpr auto abs(const detail::quat<T>& x)
{
    return detail::quat<T>(
        math::abs(x.w),
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z)
    );
}

// =============== sign ===============

template <typename T>
inline constexpr auto sign(const detail::quat<T>& x)
{
    return detail::quat<T>(
        math::sign(x.w),
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z)
    );
}

// =============== clamp ===============

template <typename T>
inline constexpr auto clamp(
    const detail::quat<T>& x,
    T min,
    T max
)
{
    return detail::quat<T>(
        math::clamp(x.w, min, max),
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::quat<T>& x,
    const detail::quat<T>& min,
    const detail::quat<T>& max
)
{
    return detail::quat<T>(
        math::clamp(x.w, min.w, max.w),
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

}
}
