#pragma once

#include "./config.hpp"

namespace vx {
namespace math {
namespace simd {

///////////////////////////////////////////////////////////////////////////////
// 128 bit types
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_SIMD_X86) && VX_SIMD_X86 >= VX_SIMD_X86_SSE_VERSION

using f32x4 = __m128;
using i32x4 = __m128i;
using u32x4 = __m128i;

#define VX_HAS_SIMD_F32X4 1
#define VX_HAS_SIMD_I32X4 1
#define VX_HAS_SIMD_U32X4 1

#elif defined(VX_SIMD_ARM)

using f32x4 = float32x4_t;
using i32x4 = int32x4_t;
using u32x4 = uint32x4_t;

#define VX_HAS_SIMD_F32X4 1
#define VX_HAS_SIMD_I32X4 1
#define VX_HAS_SIMD_U32X4 1

#elif defined(VX_SIMD_PPC)

using f32x4 = __vector float;
using i32x4 = __vector int;
using u32x4 = __vector unsigned int;

#define VX_HAS_SIMD_F32X4 1
#define VX_HAS_SIMD_I32X4 1
#define VX_HAS_SIMD_U32X4 1

#else

struct f32x4 { float data[4]; };
struct i32x4 { int data[4]; };
struct u32x4 { unsigned int data[4]; };

#endif

///////////////////////////////////////////////////////////////////////////////
// 256 bit types
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_SIMD_X86) && VX_SIMD_X86 >= VX_SIMD_X86_AVX_VERSION

#define VX_HAS_SIMD_FLOAT8 1
#define VX_HAS_SIMD_INT8   1
#define VX_HAS_SIMD_UINT8  1

using float8 = __m256;
using int8 = __m256i;
using uint8 = __m256i;

#else

struct float8 { float data[8]; };
struct int8 { int data[8]; };
struct uint8 { unsigned int data[8]; };

#endif

///////////////////////////////////////////////////////////////////////////////
// matrix types
///////////////////////////////////////////////////////////////////////////////

using f32x4x4 = f32x4[4];

} // namespace simd
} // namespace math
} // namespace vx