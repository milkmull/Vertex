#pragma once

#include <limits>

#include "vertex/config/language_config.hpp"
#include "vertex/math/core/types/base.hpp"

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
// same
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U>  struct is_same : false_t {};
template <typename T> struct is_same<T, T> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_bool
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_bool : false_t {};

template <> struct is_bool<bool> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_float
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_float : false_t {};

template <> struct is_float<f32> : true_t {};
template <> struct is_float<f64> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_int
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_int : false_t {};

template <> struct is_int<bool> : true_t {};

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

template <> struct is_signed<f32> : true_t {};
template <> struct is_signed<f64> : true_t {};

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

template <> struct is_unsigned<bool> : true_t {};

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
template <> struct is_numeric<f64> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// is_scalar
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_scalar : false_t {};

template <> struct is_scalar<bool> : true_t {};
template <> struct is_scalar<i32> : true_t {};
template <> struct is_scalar<u32> : true_t {};
template <> struct is_scalar<f32> : true_t {};
template <> struct is_scalar<f64> : true_t {};

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
template <size_t L> struct is_bool_vec<vec<L, bool>> : true_t {};

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

// is_vec_size
template <typename T, size_t L> struct is_vec_size : false_t {};
template <size_t L_, typename T_, size_t L>
struct is_vec_size<vec<L_, T_>, L> : bool_constant<L_ == L> {};

// is_vec2
template <typename T> struct is_vec2 : is_vec_size<T, 2> {};

// is_vec3
template <typename T> struct is_vec3 : is_vec_size<T, 3> {};

// is_vec4
template <typename T> struct is_vec4 : is_vec_size<T, 4> {};

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

// is_sqare_mat
template <typename T> struct is_sqare_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_sqare_mat<mat<M, N, T>> : bool_constant<M == N> {};

// is_float_mat
template <typename T> struct is_float_mat : false_t {};
template <size_t M, size_t N, typename T> struct is_float_mat<mat<M, N, T>> : is_float<T> {};

// is_bool_mat
template <typename T> struct is_bool_mat : false_t {};
template <size_t M, size_t N> struct is_bool_mat<mat<M, N, bool>> : true_t {};

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

// is_mat_size
template <typename T, size_t M, size_t N> struct is_mat_size : false_t {};
template <size_t M_, size_t N_, typename T_, size_t M, size_t N>
struct is_mat_size<mat<M_, N_, T_>, M, N> : bool_constant<M_ == M && N_ == N> {};

// is_mat2
template <typename T> struct is_mat2 : is_mat_size<T, 2, 2> {};

// is_mat3
template <typename T> struct is_mat3 : is_mat_size<T, 3, 3> {};

// is_mat4
template <typename T> struct is_mat4 : is_mat_size<T, 4, 4> {};

// is_float_mat2
template <typename T> struct is_float_mat2 : bool_constant<is_mat2<T>::value && is_float_mat<T>::value> {};

// is_float_mat3
template <typename T> struct is_float_mat3 : bool_constant<is_mat3<T>::value && is_float_mat<T>::value> {};

// is_float_mat4
template <typename T> struct is_float_mat4 : bool_constant<is_mat4<T>::value && is_float_mat<T>::value> {};

// is_rotation_matrix_compatible
template <typename T> struct is_rotation_matrix_compatible : false_t {};
template <size_t M, size_t N, typename T> struct is_rotation_matrix_compatible<mat<M, N, T>> : bool_constant<M == N> {};

///////////////////////////////////////////////////////////////////////////////
// quaternion helpers
///////////////////////////////////////////////////////////////////////////////

// is_quat
template <typename T> struct is_quat : false_t {};
template <typename T> struct is_quat<quat_t<T>> : true_t {};

///////////////////////////////////////////////////////////////////////////////
// scalar_type
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct scalar_type { using type = void; };

template <> struct scalar_type<bool> { using type = bool; };

template <> struct scalar_type<i8> { using type = i8; };
template <> struct scalar_type<i16> { using type = i16; };
template <> struct scalar_type<i32> { using type = i32; };
template <> struct scalar_type<i64> { using type = i64; };

template <> struct scalar_type<u8> { using type = u8; };
template <> struct scalar_type<u16> { using type = u16; };
template <> struct scalar_type<u32> { using type = u32; };
template <> struct scalar_type<u64> { using type = u64; };

template <> struct scalar_type<f32> { using type = f32; };
template <> struct scalar_type<f64> { using type = f64; };

template <size_t L, typename T> struct scalar_type<vec<L, T>> { using type = T; };
template <size_t M, size_t N, typename T> struct scalar_type<mat<M, N, T>> { using type = T; };
template <typename T> struct scalar_type<quat_t<T>> { using type = T; };

///////////////////////////////////////////////////////////////////////////////
// to_float_type
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct to_float_type { using type = void; };

template <> struct to_float_type<bool> { using type = f64; };

template <> struct to_float_type<i8> { using type = f64; };
template <> struct to_float_type<i16> { using type = f64; };
template <> struct to_float_type<i32> { using type = f64; };
template <> struct to_float_type<i64> { using type = f64; };

template <> struct to_float_type<u8> { using type = f64; };
template <> struct to_float_type<u16> { using type = f64; };
template <> struct to_float_type<u32> { using type = f64; };
template <> struct to_float_type<u64> { using type = f64; };

template <> struct to_float_type<f32> { using type = f32; };
template <> struct to_float_type<f64> { using type = f64; };

///////////////////////////////////////////////////////////////////////////////
// enable_if
///////////////////////////////////////////////////////////////////////////////

template <bool Test, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> { using type = T; };

///////////////////////////////////////////////////////////////////////////////
// static checks
///////////////////////////////////////////////////////////////////////////////

#define VX_MATH_STATIC_ASSERT_SIGNED(type) VX_STATIC_ASSERT(is_signed<type>::value, "Operation not supported for unsigned types.")
#define VX_MATH_STATIC_ASSERT_NUMERIC(type) VX_STATIC_ASSERT(!is_bool<type>::value, "Operation not supported for bool types.")
#define VX_MATH_STATIC_ASSERT_INT(type) VX_STATIC_ASSERT(is_int<type>::value, "Operation not supported for non-integral types.")

#define VX_MATH_STATIC_ASSERT_TRIVIAL(type) VX_STATIC_ASSERT(std::is_trivial<type>::value, #type "should be trivial")

///////////////////////////////////////////////////////////////////////////////
// requires
///////////////////////////////////////////////////////////////////////////////

#define VX_MATH_REQ(condition) typename ::vx::math::enable_if<(condition), int>::type = 0

#define VX_MATH_REQ_SIGNED(type) VX_MATH_REQ(::vx::math::is_signed<type>::value)
#define VX_MATH_REQ_FLOAT(type) VX_MATH_REQ(::vx::math::is_float<type>::value)
#define VX_MATH_REQ_INT(type) VX_MATH_REQ(::vx::math::is_int<type>::value)
#define VX_MATH_REQ_SINT(type) VX_MATH_REQ(::vx::math::is_signed_int<type>::value)
#define VX_MATH_REQ_UINT(type) VX_MATH_REQ(::vx::math::is_unsigned<type>::value)

#define VX_MATH_FLOAT_TYPE(t) typename ::vx::math::to_float_type<t>::type;

#define VX_MATH_RET_TYPE(t, fn)  decltype(fn(std::declval<T>()))
#define VX_MATH_RET_TYPE2(t, fn) decltype(fn(std::declval<T>(), std::declval<T>()))
#define VX_MATH_RET_TYPE3(t, fn) decltype(fn(std::declval<T>(), std::declval<T>(), std::declval<T>()))

#define _IS_NUMERIC_1(t1) is_numeric<t1>::value
#define _IS_NUMERIC_2(t1, t2) _IS_NUMERIC_1(t1) && _IS_NUMERIC_1(t2)
#define _IS_NUMERIC_3(t1, t2, t3) _IS_NUMERIC_2(t1, t2) && _IS_NUMERIC_1(t3)
#define _IS_NUMERIC_4(t1, t2, t3, t4) _IS_NUMERIC_3(t1, t2, t3) && _IS_NUMERIC_1(t4)

#define VX_MATH_REQ_NUM(t1) VX_MATH_REQ(_IS_NUMERIC_1(t1))
#define VX_MATH_REQ_NUM2(t1, t2) VX_MATH_REQ(_IS_NUMERIC_2(t1, t2))
#define VX_MATH_REQ_NUM3(t1, t2, t3) VX_MATH_REQ(_IS_NUMERIC_3(t1, t2, t3))
#define VX_MATH_REQ_NUM4(t1, t2, t3, t4) VX_MATH_REQ(_IS_NUMERIC_4(t1, t2, t3, t4))
#define VX_MATH_REQ_NUM6(t1, t2, t3, t4, t5, t6) VX_MATH_REQ(_IS_NUMERIC_3(t1, t2, t3) && _IS_NUMERIC_3(t4, t5, t6))
#define VX_MATH_REQ_NUM8(t1, t2, t3, t4, t5, t6, t7, t8) VX_MATH_REQ(_IS_NUMERIC_4(t1, t2, t3, t4) && _IS_NUMERIC_4(t5, t6, t7, t8))
#define VX_MATH_REQ_NUM9(t1, t2, t3, t4, t5, t6, t7, t8, t9) VX_MATH_REQ(_IS_NUMERIC_3(t1, t2, t3) && _IS_NUMERIC_3(t4, t5, t6) && _IS_NUMERIC_3(t7, t8, t9))
#define VX_MATH_REQ_NUM12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) VX_MATH_REQ(_IS_NUMERIC_4(t1, t2, t3, t4) && _IS_NUMERIC_4(t5, t6, t7, t8) && _IS_NUMERIC_4(t9, t10, t11, t12))
#define VX_MATH_REQ_NUM16(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15, t16) VX_MATH_REQ(_IS_NUMERIC_4(t1, t2, t3, t4) && _IS_NUMERIC_4(t5, t6, t7, t8) && _IS_NUMERIC_4(t9, t10, t11, t12) && _IS_NUMERIC_4(t13, t14, t15, t16))

///////////////////////////////////////////////////////////////////////////////
// helper macros
///////////////////////////////////////////////////////////////////////////////

#define VX_MATH_FORCE_TEMPLATE() template <typename _T = void>

} // namespace vx
} // namespace math