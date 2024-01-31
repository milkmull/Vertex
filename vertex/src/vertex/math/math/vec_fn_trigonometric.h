#pragma once

#include "detail/vectorize.h"
#include "fn_trigonometric.h"

namespace vx {
namespace math {

// =============== radians ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto radians(const detail::vec<L, T, Q>& x)
{
    using R = decltype(math::radians(std::declval<T>()));
    return detail::vec_unary<L, T, Q, R>::call(math::radians<T>, x);
}

// =============== degrees ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto degrees(const detail::vec<L, T, Q>& x)
{
    using R = decltype(math::degrees(std::declval<T>()));
    return detail::vec_unary<L, T, Q, R>::call(math::degrees<T>, x);
}

// =============== sin ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto sin(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::sin<T>, x);
}

// =============== cos ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto cos(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::cos<T>, x);
}

// =============== tan ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto tan(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::tan<T>, x);
}

// =============== asin ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto asin(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::asin<T>, x);
}

// =============== asin clamped ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto asin_clamped(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::asin_clamped<T>, x);
}

// =============== acos ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto acos(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::acos<T>, x);
}

// =============== acos clamped ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto acos_clamped(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::acos_clamped<T>, x);
}

// =============== atan ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto atan(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::atan<T>, x);
}

// =============== atan2 ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto atan2(
    const detail::vecf<L, T, Q>& x1,
    const detail::vecf<L, T, Q>& x2
)
{
    return detail::vec_binary<L, T, Q, T>::call(math::atan2<T>, x1, x2);
}

// =============== sinh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto sinh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::sinh<T>, x);
}

// =============== cosh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto cosh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::cosh<T>, x);
}

// =============== tanh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto tanh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::tanh<T>, x);
}

// =============== asinh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto asinh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::asinh<T>, x);
}

// =============== acosh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto acosh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::acosh<T>, x);
}

// =============== atanh ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto atanh(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::atanh<T>, x);
}

}
}