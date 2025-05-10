#pragma once

#include "vertex/math/types.hpp"

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// is_float
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_float { static constexpr bool value = false; };

template <> struct is_float<f32> { static constexpr bool value = true; };
template <> struct is_float<d> { static constexpr bool value = true; };

///////////////////////////////////////////////////////////////////////////////
// is_int
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_int { static constexpr bool value = false; };

template <> struct is_int<b> { static constexpr bool value = true; };

template <> struct is_int<i8> { static constexpr bool value = true; };
template <> struct is_int<i16> { static constexpr bool value = true; };
template <> struct is_int<i32> { static constexpr bool value = true; };
template <> struct is_int<i64> { static constexpr bool value = true; };

template <> struct is_int<u8> { static constexpr bool value = true; };
template <> struct is_int<u16> { static constexpr bool value = true; };
template <> struct is_int<u32> { static constexpr bool value = true; };
template <> struct is_int<u64> { static constexpr bool value = true; };

///////////////////////////////////////////////////////////////////////////////
// is_signed
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_signed { static constexpr bool value = false; };

template <> struct is_signed<i8> { static constexpr bool value = true; };
template <> struct is_signed<i16> { static constexpr bool value = true; };
template <> struct is_signed<i32> { static constexpr bool value = true; };
template <> struct is_signed<i64> { static constexpr bool value = true; };

template <> struct is_float<f32> { static constexpr bool value = true; };
template <> struct is_float<d> { static constexpr bool value = true; };

///////////////////////////////////////////////////////////////////////////////
// is_unsigned
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_unsigned { static constexpr bool value = false; };

template <> struct is_unsigned<b> { static constexpr bool value = true; };

template <> struct is_unsigned<u8> { static constexpr bool value = true; };
template <> struct is_unsigned<u16> { static constexpr bool value = true; };
template <> struct is_unsigned<u32> { static constexpr bool value = true; };
template <> struct is_unsigned<u64> { static constexpr bool value = true; };

///////////////////////////////////////////////////////////////////////////////
// is_numeric
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_numeric { static constexpr bool value = false; };

template <> struct is_numeric<b> { static constexpr bool value = true; };

template <> struct is_numeric<i8> { static constexpr bool value = true; };
template <> struct is_numeric<i16> { static constexpr bool value = true; };
template <> struct is_numeric<i32> { static constexpr bool value = true; };
template <> struct is_numeric<i64> { static constexpr bool value = true; };

template <> struct is_numeric<u8> { static constexpr bool value = true; };
template <> struct is_numeric<u16> { static constexpr bool value = true; };
template <> struct is_numeric<u32> { static constexpr bool value = true; };
template <> struct is_numeric<u64> { static constexpr bool value = true; };

template <> struct is_numeric<f32> { static constexpr bool value = true; };
template <> struct is_numeric<d> { static constexpr bool value = true; };

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