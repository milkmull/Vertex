#pragma once

#include "fn_exponential.h"

namespace vx {
namespace math {

// =============== pow ===============

// scaler binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> pow(
    const vec<2, T>& x,
    T y
)
{
    return vec<2, T>(
        math::pow(x.x, y),
        math::pow(x.y, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> pow(
    const vec<3, T>& x,
    T y
)
{
    return vec<3, T>(
        math::pow(x.x, y),
        math::pow(x.y, y),
        math::pow(x.z, y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> pow(
    const vec<4, T>& x,
    T y
)
{
    return vec<4, T>(
        math::pow(x.x, y),
        math::pow(x.y, y),
        math::pow(x.z, y),
        math::pow(x.w, y)
    );
}

// binary

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> pow(
    const vec<2, T>& x,
    const vec<2, T>& y
)
{
    return vec<2, T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> pow(
    const vec<3, T>& x,
    const vec<3, T>& y
)
{
    return vec<3, T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y),
        math::pow(x.z, y.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> pow(
    const vec<4, T>& x,
    const vec<4, T>& y
)
{
    return vec<4, T>(
        math::pow(x.x, y.x),
        math::pow(x.y, y.y),
        math::pow(x.z, y.z),
        math::pow(x.w, y.w)
    );
}

// =============== exp ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> exp(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::exp(x.x),
        math::exp(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> exp(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::exp(x.x),
        math::exp(x.y),
        math::exp(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> exp(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::exp(x.x),
        math::exp(x.y),
        math::exp(x.z),
        math::exp(x.w)
    );
}

// =============== exp2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> exp2(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::exp2(x.x),
        math::exp2(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> exp2(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::exp2(x.x),
        math::exp2(x.y),
        math::exp2(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> exp2(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::exp2(x.x),
        math::exp2(x.y),
        math::exp2(x.z),
        math::exp2(x.w)
    );
}

// =============== log ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> log(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::log(x.x),
        math::log(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> log(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::log(x.x),
        math::log(x.y),
        math::log(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> log(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::log(x.x),
        math::log(x.y),
        math::log(x.z),
        math::log(x.w)
    );
}

// =============== log2 ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> log2(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::log2(x.x),
        math::log2(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> log2(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::log2(x.x),
        math::log2(x.y),
        math::log2(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> log2(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::log2(x.x),
        math::log2(x.y),
        math::log2(x.z),
        math::log2(x.w)
    );
}

// =============== sqrt ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> sqrt(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::sqrt(x.x),
        math::sqrt(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> sqrt(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::sqrt(x.x),
        math::sqrt(x.y),
        math::sqrt(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> sqrt(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::sqrt(x.x),
        math::sqrt(x.y),
        math::sqrt(x.z),
        math::sqrt(x.w)
    );
}

// =============== inverse_sqrt ===============

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<2, T> inverse_sqrt(
    const vec<2, T>& x
)
{
    return vec<2, T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<3, T> inverse_sqrt(
    const vec<3, T>& x
)
{
    return vec<3, T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y),
        math::inverse_sqrt(x.z)
    );
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
VX_FORCE_INLINE constexpr vec<4, T> inverse_sqrt(
    const vec<4, T>& x
)
{
    return vec<4, T>(
        math::inverse_sqrt(x.x),
        math::inverse_sqrt(x.y),
        math::inverse_sqrt(x.z),
        math::inverse_sqrt(x.w)
    );
}

}
}
