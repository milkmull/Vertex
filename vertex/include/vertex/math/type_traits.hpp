#pragma once

#include "vertex/math/types.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// integral constant
///////////////////////////////////////////////////////////////////////////////

template <class T, T val>
struct integral_constant
{
    static constexpr T value = val;
    using value_type = T;
};

template <bool val>
using bool_constant = integral_constant<bool, val>;

using true_t = bool_constant<true>;
using false_t = bool_constant<false>;

///////////////////////////////////////////////////////////////////////////////
// is_float
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_float : false_t {};

template <> struct is_float<f32> : true_t {};
template <> struct is_float<d> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_int
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_int : false_t {};

template <> struct is_int<b> : true_t {};

template <> struct is_int<i8> : true_t {};
template <> struct is_int<i16> : true_t {};
template <> struct is_int<i32> : true_t {};
template <> struct is_int<i64> : true_t {};

template <> struct is_int<u8> : true_t {};
template <> struct is_int<u16> : true_t {};
template <> struct is_int<u32> : true_t {};
template <> struct is_int<u64> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_signed
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_signed : false_t {};

template <> struct is_signed<i8> : true_t {};
template <> struct is_signed<i16> : true_t {};
template <> struct is_signed<i32> : true_t {};
template <> struct is_signed<i64> : true_t {};

template <> struct is_float<f32> : true_t {};
template <> struct is_float<d> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_signed_int
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_signed_int : false_t {};

template <> struct is_signed_int<i8> : true_t {};
template <> struct is_signed_int<i16> : true_t {};
template <> struct is_signed_int<i32> : true_t {};
template <> struct is_signed_int<i64> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_unsigned
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_unsigned : false_t {};

template <> struct is_unsigned<b> : true_t {};

template <> struct is_unsigned<u8> : true_t {};
template <> struct is_unsigned<u16> : true_t {};
template <> struct is_unsigned<u32> : true_t {};
template <> struct is_unsigned<u64> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_numeric
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_numeric : false_t {};

template <> struct is_numeric<i8> : true_t {};
template <> struct is_numeric<i16> : true_t {};
template <> struct is_numeric<i32> : true_t {};
template <> struct is_numeric<i64> : true_t {};

template <> struct is_numeric<u8> : true_t {};
template <> struct is_numeric<u16> : true_t {};
template <> struct is_numeric<u32> : true_t {};
template <> struct is_numeric<u64> : true_t {};

template <> struct is_numeric<f32> : true_t {};
template <> struct is_numeric<d> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_scaler
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_scaler : false_t {};

template <> struct is_scaler<b> : true_t {};
template <> struct is_numeric<i32> : true_t {};
template <> struct is_numeric<u32> : true_t {};
template <> struct is_numeric<f32> : true_t {};
template <> struct is_numeric<d> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// vector helpers
///////////////////////////////////////////////////////////////////////////////

// is_vec
template <typename T> struct is_vec : false_t {};
template <size_t L, typename T> struct is_vec<vec<L, T>> : true_t {};

// vec_size
template <typename T> struct vec_size : integral_constant<size_t, 0> {};
template <size_t L, typename T> struct vec_size<vec<L, T>> : integral_constant<size_t, L> {};

// vec_type
template <typename T> struct vec_type { using type = void; };
template <size_t L, typename T> struct vec_type<vec<L, T>> { using type = T; };

// is_float_vec
template <typename T> struct is_float_vec : false_t {};
template <size_t L, typename T> struct is_float_vec<vec<L, T>> : is_float<T> {};

// is_bool_vec
template <typename T> struct is_bool_vec : false_t {};
template <size_t L> struct is_bool_vec<vec<L, b>> : true_t {};

// is_int_vec
template <typename T> struct is_int_vec : false_t {};
template <size_t L, typename T> struct is_int_vec<vec<L, T>> : is_int<T> {};

// is_signed_vec
template <typename T> struct is_signed_vec : false_t {};
template <size_t L, typename T> struct is_signed_vec<vec<L, T>> : is_signed<T> {};

// is_signed_int_vec
template <typename T> struct is_signed_int_vec : false_t {};
template <size_t L, typename T> struct is_signed_int_vec<vec<L, T>> : is_signed_int<T> {};

// is_unsigned_vec
template <typename T> struct is_unsigned_vec : false_t {};
template <size_t L, typename T> struct is_unsigned_vec<vec<L, T>> : is_unsigned<T> {};

///////////////////////////////////////////////////////////////////////////////
// matrix helpers
///////////////////////////////////////////////////////////////////////////////

// is_mat
template <typename T> struct is_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_mat<mat<M, N, T>> : true_t {};

// mat_width
template <typename T> struct mat_width : integral_constant<size_t, 0> {};
template <size_t M, size_t N, typename T> struct mat_width<mat<M, N, T>> : integral_constant<size_t, M> {};

// mat_height
template <typename T> struct mat_height : integral_constant<size_t, 0> {};
template <size_t M, size_t N, typename T> struct mat_height<mat<M, N, T>> : integral_constant<size_t, N> {};

// mat_type
template <typename T> struct mat_type { using type = void; };
template <size_t M, size_t N, typename T> struct mat_type<mat<M, N, T>> { using type = T; };

// is_float_mat
template <typename T> struct is_float_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_float_mat<mat<M, N, T>> : is_float<T> {};

// is_bool_mat
template <typename T> struct is_bool_mat : false_t {};
template <size_t M, size_t N> struct is_bool_mat<mat<M, N, b>> : true_t {};

// is_int_mat
template <typename T> struct is_int_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_int_mat<mat<M, N, T>> : is_int<T> {};

// is_signed_mat
template <typename T> struct is_signed_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_signed_mat<mat<M, N, T>> : is_signed<T> {};

// is_signed_int_mat
template <typename T> struct is_signed_int_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_signed_int_mat<mat<M, N, T>> : is_signed_int<T> {};

// is_unsigned_mat
template <typename T> struct is_unsigned_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_unsigned_mat<mat<M, N, T>> : is_unsigned<T> {};

///////////////////////////////////////////////////////////////////////////////
// quaternion helpers
///////////////////////////////////////////////////////////////////////////////

// is_quat
template <typename T> struct is_quat : false_t {};
template <typename T> struct is_quat<quat_t<T>> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// requires
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <bool Test, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> { using type = T; };

} // namespace __detail

#define VXM_REQ(condition) typename __detail::enable_if<(condition), int>::type = 0
#define VXM_REQ_FLOAT(type) VXM_REQ(is_float<type>::value)
#define VXM_REQ_INT(type) VXM_REQ(is_int<type>::value)
#define VXM_REQ_NUM(type) VXM_REQ(is_numeric<type>::value)

} // namespace vx
} // namespace math