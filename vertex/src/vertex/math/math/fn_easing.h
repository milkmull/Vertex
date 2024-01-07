#pragma once

namespace vx {
namespace math {

// https://easings.net/

// =============== linear ===============

template <typename T>
inline constexpr T ease_linear(T x);

// =============== quad ===============

template <typename T>
inline constexpr T ease_in_quad(T x);

template <typename T>
inline constexpr T ease_out_quad(T x);

template <typename T>
inline constexpr T ease_in_out_quad(T x);

// =============== cubic ===============

template <typename T>
inline constexpr T ease_in_cubic(T x);

template <typename T>
inline constexpr T ease_out_cubic(T x);

template <typename T>
inline constexpr T ease_in_out_cubic(T x);

// =============== quart ===============

template <typename T>
inline constexpr T ease_in_quart(T x);

template <typename T>
inline constexpr T ease_out_quart(T x);

template <typename T>
inline constexpr T ease_in_out_quart(T x);

// =============== quint ===============

template <typename T>
inline constexpr T ease_in_quint(T x);

template <typename T>
inline constexpr T ease_out_quint(T x);

template <typename T>
inline constexpr T ease_in_out_quint(T x);

// =============== sine ===============

template <typename T>
inline constexpr T ease_in_sine(T x);

template <typename T>
inline constexpr T ease_out_sine(T x);

template <typename T>
inline constexpr T ease_in_out_sine(T x);

// =============== expo ===============

template <typename T>
inline constexpr T ease_in_expo(T x);

template <typename T>
inline constexpr T ease_out_expo(T x);

template <typename T>
inline constexpr T ease_in_out_expo(T x);

// =============== circ ===============

template <typename T>
inline constexpr T ease_in_circ(T x);

template <typename T>
inline constexpr T ease_out_circ(T x);

template <typename T>
inline constexpr T ease_in_out_circ(T x);

// =============== back ===============

template <typename T>
inline constexpr T ease_in_back(T x);

template <typename T>
inline constexpr T ease_out_back(T x);

template <typename T>
inline constexpr T ease_in_out_back(T x);

// =============== elastic ===============

template <typename T>
inline constexpr T ease_in_elastic(T x);

template <typename T>
inline constexpr T ease_out_elastic(T x);

template <typename T>
inline constexpr T ease_in_out_elastic(T x);

// =============== bounce ===============

template <typename T>
inline constexpr T ease_in_bounce(T x);

template <typename T>
inline constexpr T ease_out_bounce(T x);

template <typename T>
inline constexpr T ease_in_out_bounce(T x);

}
}

#include "_impl/fn_easing.inl"
