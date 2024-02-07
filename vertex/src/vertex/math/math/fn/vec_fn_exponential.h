#pragma once

#include "../detail/vectorize.h"
#include "fn_exponential.h"

namespace vx {
namespace math {

// =============== pow ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto pow(
    const detail::vecf<L, T, Q>& x,
    T y
)
{
    return detail::vec_scaler_binary<L, T, Q, T>::call(math::pow<T>, x, y);
}

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto pow(
    const detail::vecf<L, T, Q>& x,
    const detail::vecf<L, T, Q>& y
)
{
    return detail::vec_binary<L, T, Q, T>::call(math::pow<T>, x, y);
}

// =============== exp ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto exp(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::exp<T>, x);
}

// =============== exp2 ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto exp2(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::exp2<T>, x);
}

// =============== log ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto log(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::log<T>, x);
}

// =============== log2 ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto log2(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::log2<T>, x);
}

// =============== sqrt ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto sqrt(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::sqrt<T>, x);
}

// =============== inverse_sqrt ===============

template <detail::length_type L, typename T, detail::vec_t Q>
inline constexpr auto inverse_sqrt(const detail::vecf<L, T, Q>& x)
{
    return detail::vec_unary<L, T, Q, T>::call(math::inverse_sqrt<T>, x);
}

}
}
