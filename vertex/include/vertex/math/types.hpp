#pragma once

#include <cstdint>

namespace vx {
namespace math {

///////////////////////////////////////////////////////////////////////////////
// basic types
///////////////////////////////////////////////////////////////////////////////

// bool
using b = bool;

// int
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using i = i32;

// uint
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using u = u32;

// float
using f32 = float;
using f = f32;

// double
using f64 = double;
using d = f64;

///////////////////////////////////////////////////////////////////////////////
// vector types
///////////////////////////////////////////////////////////////////////////////

template <size_t L, typename T> struct vec;

// bool
using vec2b = vec<2, b>;
using vec3b = vec<3, b>;
using vec4b = vec<4, b>;

// int
using vec2i = vec<2, i>;
using vec3i = vec<3, i>;
using vec4i = vec<4, i>;

// uint
using vec2u = vec<2, u>;
using vec3u = vec<3, u>;
using vec4u = vec<4, u>;

// float
using vec2f = vec<2, f>;
using vec3f = vec<3, f>;
using vec4f = vec<4, f>;

// double
using vec2d = vec<2, d>;
using vec3d = vec<3, d>;
using vec4d = vec<4, d>;

// generic
using vec2 = vec2f;
using vec3 = vec3f;
using vec4 = vec4f;

///////////////////////////////////////////////////////////////////////////////
// matrix types
///////////////////////////////////////////////////////////////////////////////

template <size_t M, size_t N, typename T> struct mat;

// bool
using mat2x2b = mat<2, 2, b>;
using mat2x3b = mat<2, 3, b>;
using mat2x4b = mat<2, 4, b>;
using mat3x2b = mat<3, 2, b>;
using mat3x3b = mat<3, 3, b>;
using mat3x4b = mat<3, 4, b>;
using mat4x2b = mat<4, 2, b>;
using mat4x3b = mat<4, 3, b>;
using mat4x4b = mat<4, 4, b>;

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

using mat2f = mat2x2f;
using mat3f = mat3x3f;
using mat4f = mat4x4f;

// double
using mat2x2d = mat<2, 2, d>;
using mat2x3d = mat<2, 3, d>;
using mat2x4d = mat<2, 4, d>;
using mat3x2d = mat<3, 2, d>;
using mat3x3d = mat<3, 3, d>;
using mat3x4d = mat<3, 4, d>;
using mat4x2d = mat<4, 2, d>;
using mat4x3d = mat<4, 3, d>;
using mat4x4d = mat<4, 4, d>;

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

template <typename T> struct quat_t;

// float
using quatf = quat_t<f32>;

// double
using quatd = quat_t<d>;

// generic
using quat = quatf;

} // namespace vx
} // namespace math