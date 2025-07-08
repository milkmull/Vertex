#pragma once

#include <cstdint>
#include <cstddef>

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// basic types
///////////////////////////////////////////////////////////////////////////////

// int
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// uint
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// float
using f32 = float;
using f64 = double;

///////////////////////////////////////////////////////////////////////////////
// vector types
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T>
struct vec;

// bool
using vec2b = vec<2, bool>;
using vec3b = vec<3, bool>;
using vec4b = vec<4, bool>;

// int
using vec2i = vec<2, i32>;
using vec3i = vec<3, i32>;
using vec4i = vec<4, i32>;

// uint
using vec2u = vec<2, u32>;
using vec3u = vec<3, u32>;
using vec4u = vec<4, u32>;

// float
using vec2f = vec<2, f32>;
using vec3f = vec<3, f32>;
using vec4f = vec<4, f32>;

// double
using vec2d = vec<2, f64>;
using vec3d = vec<3, f64>;
using vec4d = vec<4, f64>;

// generic
using vec2 = vec2f;
using vec3 = vec3f;
using vec4 = vec4f;

///////////////////////////////////////////////////////////////////////////////
// matrix types
///////////////////////////////////////////////////////////////////////////////

template <size_t M, size_t N, typename T>
struct mat;

// bool
using mat2x2b = mat<2, 2, bool>;
using mat2x3b = mat<2, 3, bool>;
using mat2x4b = mat<2, 4, bool>;
using mat3x2b = mat<3, 2, bool>;
using mat3x3b = mat<3, 3, bool>;
using mat3x4b = mat<3, 4, bool>;
using mat4x2b = mat<4, 2, bool>;
using mat4x3b = mat<4, 3, bool>;
using mat4x4b = mat<4, 4, bool>;

using mat2b = mat2x2b;
using mat3b = mat3x3b;
using mat4b = mat4x4b;

// int
using mat2x2i = mat<2, 2, i32>;
using mat2x3i = mat<2, 3, i32>;
using mat2x4i = mat<2, 4, i32>;
using mat3x2i = mat<3, 2, i32>;
using mat3x3i = mat<3, 3, i32>;
using mat3x4i = mat<3, 4, i32>;
using mat4x2i = mat<4, 2, i32>;
using mat4x3i = mat<4, 3, i32>;
using mat4x4i = mat<4, 4, i32>;

using mat2i = mat2x2i;
using mat3i = mat3x3i;
using mat4i = mat4x4i;

// uint
using mat2x2u = mat<2, 2, u32>;
using mat2x3u = mat<2, 3, u32>;
using mat2x4u = mat<2, 4, u32>;
using mat3x2u = mat<3, 2, u32>;
using mat3x3u = mat<3, 3, u32>;
using mat3x4u = mat<3, 4, u32>;
using mat4x2u = mat<4, 2, u32>;
using mat4x3u = mat<4, 3, u32>;
using mat4x4u = mat<4, 4, u32>;

using mat2u = mat2x2u;
using mat3u = mat3x3u;
using mat4u = mat4x4u;

// float
using mat2x2f = mat<2, 2, f32>;
using mat2x3f = mat<2, 3, f32>;
using mat2x4f = mat<2, 4, f32>;
using mat3x2f = mat<3, 2, f32>;
using mat3x3f = mat<3, 3, f32>;
using mat3x4f = mat<3, 4, f32>;
using mat4x2f = mat<4, 2, f32>;
using mat4x3f = mat<4, 3, f32>;
using mat4x4f = mat<4, 4, f32>;

using mat2x2 = mat2x2f;
using mat2x3 = mat2x3f;
using mat2x4 = mat2x4f;
using mat3x2 = mat3x2f;
using mat3x3 = mat3x3f;
using mat3x4 = mat3x4f;
using mat4x2 = mat4x2f;
using mat4x3 = mat4x3f;
using mat4x4 = mat4x4f;

using mat2f = mat2x2f;
using mat3f = mat3x3f;
using mat4f = mat4x4f;

// double
using mat2x2d = mat<2, 2, f64>;
using mat2x3d = mat<2, 3, f64>;
using mat2x4d = mat<2, 4, f64>;
using mat3x2d = mat<3, 2, f64>;
using mat3x3d = mat<3, 3, f64>;
using mat3x4d = mat<3, 4, f64>;
using mat4x2d = mat<4, 2, f64>;
using mat4x3d = mat<4, 3, f64>;
using mat4x4d = mat<4, 4, f64>;

using mat2d = mat2x2d;
using mat3d = mat3x3d;
using mat4d = mat4x4d;

// generic
using mat2 = mat2f;
using mat3 = mat3f;
using mat4 = mat4f;

///////////////////////////////////////////////////////////////////////////////
// quaternion types
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct quat_t;

// float
using quatf = quat_t<f32>;

// double
using quatd = quat_t<f64>;

// generic
using quat = quatf;

} // namespace math
} // namespace vx