#pragma once

#include "../../core/types/traits.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// color types
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct color_t;

using color = color_t<f32>;
using color8 = color_t<u8>;

///////////////////////////////////////////////////////////////////////////////
// is_color
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_color : false_t {};
template <typename T> struct is_color<color_t<T>> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// scalar_type
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct scalar_type<color_t<T>> { using type = T; };

///////////////////////////////////////////////////////////////////////////////
// max/min_channel_value
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <typename T, bool is_float> struct get_max_channel_value_impl {};
template <typename T> struct get_max_channel_value_impl<T, true> : integral_constant<size_t, 1> {};
template <typename T> struct get_max_channel_value_impl<T, false> : integral_constant<size_t, std::numeric_limits<T>::max()> {};

template <typename T, bool is_float> struct get_min_channel_value_impl {};
template <typename T> struct get_min_channel_value_impl<T, true> : integral_constant<size_t, 0> {};
template <typename T> struct get_min_channel_value_impl<T, false> : integral_constant<size_t, std::numeric_limits<T>::min()> {};

} // namespace __detail

template <typename T> struct get_max_channel_value {};
template <typename T> struct get_max_channel_value<color_t<T>> : __detail::get_max_channel_value_impl<T, is_float<T>::value> {};

template <typename T> struct get_min_channel_value {};
template <typename T> struct get_min_channel_value<color_t<T>> : __detail::get_min_channel_value_impl<T, is_float<T>::value> {};

///////////////////////////////////////////////////////////////////////////////
// color_vec_scalar_type
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct color_vec_scalar_type {};
template <> struct color_vec_scalar_type<color> { using type = f32; };
template <> struct color_vec_scalar_type<color8> { using type = i32; };

} // namespace vx
} // namespace math