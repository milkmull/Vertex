#pragma once

#include "fn_common.h"

#include "../type/quat.h"

namespace vx {
namespace math {

// =============== abs ===============

template <typename T>
VX_FORCE_INLINE constexpr auto abs(const quat_t<T>& x)
{
    return quat_t<T>(
        math::abs(x.w),
        math::abs(x.x),
        math::abs(x.y),
        math::abs(x.z)
    );
}

// =============== sign ===============

template <typename T>
VX_FORCE_INLINE constexpr auto sign(const quat_t<T>& x)
{
    return quat_t<T>(
        math::sign(x.w),
        math::sign(x.x),
        math::sign(x.y),
        math::sign(x.z)
    );
}

// =============== clamp ===============

template <typename T>
VX_FORCE_INLINE constexpr auto clamp(
    const quat_t<T>& x,
    T min,
    T max
)
{
    return quat_t<T>(
        math::clamp(x.w, min, max),
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
VX_FORCE_INLINE constexpr auto clamp(
    const quat_t<T>& x,
    const quat_t<T>& min,
    const quat_t<T>& max
)
{
    return quat_t<T>(
        math::clamp(x.w, min.w, max.w),
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

}
}
