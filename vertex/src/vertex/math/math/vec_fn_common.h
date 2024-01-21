#pragma once

#include "detail/vectorize.h"
#include "fn_common.h"

namespace vx {
namespace math {

// =============== min ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto min(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::min<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto min(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::min<T>>(x, y);
}

// =============== max ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto max(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::max<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto max(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::max<T>>(x, y);
}

// =============== abs ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto abs(const detail::vec<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::abs<T>>(x);
}

// =============== round ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto round(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::round<T>>(x);
}

// =============== trunc ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto trunc(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::trunc<T>>(x);
}

// =============== floor ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto floor(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::floor<T>>(x);
}

// =============== ceil ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto ceil(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::ceil<T>>(x);
}

// =============== sign ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto sign(const detail::vec<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::sign<T>>(x);
}

// =============== fmod ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fmod(
    const detail::vecf<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::fmod<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fmod(
    const detail::vecf<L, T, Q>& x,
    const detail::vecf<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::fmod<T>>(x, y);
}

// =============== mod ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto mod(
    const detail::vec<L, T, Q>& x,
    T y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return detail::vec_scaler_binary<L, T, Q, R>::call<math::mod<T>>(x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto mod(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& y
)
{
    using R = decltype(math::mod(std::declval<T>(), std::declval<T>()));
    return detail::vec_binary<L, T, Q, R>::call<math::mod<T>>(x, y);
}

// =============== fract ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto fract(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call<math::fract<T>>(x);
}

// =============== modf ===============

template <typename T>
inline constexpr auto modf(
    const detail::vecf<2, T>& x,
    detail::vecf<2, T>& intpart
)
{
    return detail::vecf<2, T>(
        math::modf(x.x, &intpart.x),
        math::modf(x.y, &intpart.y)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::vecf<3, T>& x,
    detail::vecf<3, T>& intpart
)
{
    return detail::vecf<3, T>(
        math::modf(x.x, &intpart.x),
        math::modf(x.y, &intpart.y),
        math::modf(x.z, &intpart.z)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::vecf<4, T>& x,
    detail::vecf<4, T>& intpart
)
{
    return detail::vecf<4, T>(
        math::modf(x.x, &intpart.x),
        math::modf(x.y, &intpart.y),
        math::modf(x.z, &intpart.z),
        math::modf(x.w, &intpart.w)
    );
}

template <typename T>
inline constexpr auto modf(
    const detail::col<T>& x,
    detail::col<T>& intpart
)
{
    return detail::col<T>(
        math::modf(x.r, &intpart.r),
        math::modf(x.g, &intpart.g),
        math::modf(x.b, &intpart.b),
        math::modf(x.a, &intpart.a)
    );
}

// =============== frexp ===============

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<2, T>& x,
    detail::vec<2, int, Q>& exp
)
{
    return detail::vecf<2, T>(
        math::frexp(x.x, &exp.x),
        math::frexp(x.y, &exp.y)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<3, T>& x,
    detail::vec<3, int, Q>& exp
)
{
    return detail::vecf<3, T>(
        math::frexp(x.x, &exp.x),
        math::frexp(x.y, &exp.y),
        math::frexp(x.z, &exp.z)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::vecf<4, T>& x,
    detail::vec<4, int, Q>& exp
)
{
    return detail::vecf<4, T>(
        math::frexp(x.x, &exp.x),
        math::frexp(x.y, &exp.y),
        math::frexp(x.z, &exp.z),
        math::frexp(x.w, &exp.w)
    );
}

template <typename T, detail::vec_t Q>
inline constexpr auto frexp(
    const detail::col<T>& x,
    detail::vec<4, int, Q>& exp
)
{
    return detail::col<T>(
        math::frexp(x.r, &exp.r),
        math::frexp(x.g, &exp.g),
        math::frexp(x.b, &exp.b),
        math::frexp(x.a, &exp.a)
    );
}

// =============== snap ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto snap(
    const detail::vec<L, T, Q>& x,
    T interval
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call<math::snap<T>>(x, interval);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto snap(
    const detail::vec<L, T, Q>& x,
    const detail::vec<L, T, Q>& interval
)
{
    return detail::vec_binary<L, T, Q, T>::call<math::snap<T>>(x, interval);
}

// =============== clamp ===============

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<2, T>& x,
    T min,
    T max
)
{
    return detail::vecx<2, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<3, T>& x,
    T min,
    T max
)
{
    return detail::vecx<3, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<4, T>& x,
    T min,
    T max
)
{
    return detail::vecx<4, T>(
        math::clamp(x.x, min, max),
        math::clamp(x.y, min, max),
        math::clamp(x.z, min, max),
        math::clamp(x.w, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::col<T>& x,
    T min,
    T max
)
{
    return detail::col<T>(
        math::clamp(x.r, min, max),
        math::clamp(x.g, min, max),
        math::clamp(x.b, min, max),
        math::clamp(x.a, min, max)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<2, T>& x,
    const detail::vecx<2, T>& min,
    const detail::vecx<2, T>& max
)
{
    return detail::vecx<2, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<3, T>& x,
    const detail::vecx<3, T>& min,
    const detail::vecx<3, T>& max
)
{
    return detail::vecx<3, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::vecx<4, T>& x,
    const detail::vecx<4, T>& min,
    const detail::vecx<4, T>& max
)
{
    return detail::vecx<4, T>(
        math::clamp(x.x, min.x, max.x),
        math::clamp(x.y, min.y, max.y),
        math::clamp(x.z, min.z, max.z),
        math::clamp(x.w, min.w, max.w)
    );
}

template <typename T>
inline constexpr auto clamp(
    const detail::col<T>& v,
    const detail::col<T>& min,
    const detail::col<T>& max
)
{
    return detail::col<T>(
        math::clamp(v.r, min.r, max.r),
        math::clamp(v.g, min.g, max.g),
        math::clamp(v.b, min.b, max.b),
        math::clamp(v.a, min.a, max.a)
    );
}

}
}
