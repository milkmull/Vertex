#pragma once

#include "vertex/config/version_number.hpp"

//==============================================================================
// arm
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/arm.h

#define VX_SIMD_ARM_NEON_VERSION VX_VERSION_NUMBER(1, 0, 0)

#if defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)

#   define VX_SIMD_ARM_NEON VX_SIMD_ARM_NEON_VERSION

#endif // arm

//==============================================================================
// ppc
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/ppc.h

#define VX_SIMD_PPC_VMX_VERSION VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_PPC_VSX_VERSION VX_VERSION_NUMBER(1, 1, 0)
#define VX_SIMD_PPC_QPX_VERSION VX_VERSION_NUMBER(2, 0, 0)

#if defined(__VECTOR4DOUBLE__)

#   define VX_SIMD_PPC VX_SIMD_PPC_QPX_VERSION

#elif defined(__VSX__)

#   define VX_SIMD_PPC VX_SIMD_PPC_VSX_VERSION

#elif defined(__ALTIVEC__) || defined(__VEC__)

#   define VX_SIMD_PPC VX_SIMD_PPC_VMX_VERSION

#endif // ppc

//==============================================================================
// x86
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/x86.h

#define VX_SIMD_X86_MMX_VERSION     VX_VERSION_NUMBER(0, 99, 0)
#define VX_SIMD_X86_SSE_VERSION     VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_X86_SSE2_VERSION    VX_VERSION_NUMBER(2, 0, 0)
#define VX_SIMD_X86_SSE3_VERSION    VX_VERSION_NUMBER(3, 0, 0)
#define VX_SIMD_X86_SSSE3_VERSION   VX_VERSION_NUMBER(3, 1, 0)
#define VX_SIMD_X86_SSE4_1_VERSION  VX_VERSION_NUMBER(4, 1, 0)
#define VX_SIMD_X86_SSE4_2_VERSION  VX_VERSION_NUMBER(4, 2, 0)
#define VX_SIMD_X86_AVX_VERSION     VX_VERSION_NUMBER(5, 0, 0)
#define VX_SIMD_X86_FMA3_VERSION    VX_VERSION_NUMBER(5, 2, 0)
#define VX_SIMD_X86_AVX2_VERSION    VX_VERSION_NUMBER(5, 3, 0)
#define VX_SIMD_X86_MIC_VERSION     VX_VERSION_NUMBER(9, 0, 0)

#if defined(__MIC__)
#   define VX_SIMD_X86 VX_SIMD_X86_MIC_VERSION
#elif defined(__AVX2__)
#   define VX_SIMD_X86 VX_SIMD_X86_AVX2_VERSION
#elif defined(__AVX__)
#   define VX_SIMD_X86 VX_SIMD_X86_AVX_VERSION
#elif defined(__FMA__)
#   define VX_SIMD_X86 VX_SIMD_X86_FMA_VERSION
#elif defined(__SSE4_2__)
#   define VX_SIMD_X86 VX_SIMD_X86_SSE4_2_VERSION
#elif defined(__SSE4_1__)
#   define VX_SIMD_X86 VX_SIMD_X86_SSE4_1_VERSION
#elif defined(__SSSE3__)
#   define VX_SIMD_X86 VX_SIMD_X86_SSSE3_VERSION
#elif defined(__SSE3__)
#   define VX_SIMD_X86 VX_SIMD_X86_SSE3_VERSION
#elif (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#   define VX_SIMD_X86 VX_SIMD_X86_SSE2_VERSION
#elif (defined(__SSE__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1))
#   define VX_SIMD_X86 VX_SIMD_X86_SSE_VERSION
#elif defined(__MMX__)
#   define VX_SIMD_X86 VX_SIMD_X86_MMX_VERSION
#endif // x86

//==============================================================================
// x86_amd
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/x86_amd.h

#define VX_SIMD_X86_AMD_SSE4A_VERSION   VX_VERSION_NUMBER(4, 0, 0)
#define VX_SIMD_X86_AMD_FMA4_VERSION    VX_VERSION_NUMBER(5, 1, 0)
#define VX_SIMD_X86_AMD_XOP_VERSION     VX_VERSION_NUMBER(5, 1, 1)

#if defined(__XOP__)
#   define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_XOP_VERSION
#elif defined(__FMA4__)
#   define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_FMA4_VERSION
#elif defined(__SSE4A__)
#   define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_SSE4A_VERSION
#endif

// At this point, we know that we have an AMD CPU, we do need to check for
// other x86 extensions to determine the final version number.

#if defined(VX_SIMD_X86_AMD) && defined(VX_SIMD_X86) && (VX_SIMD_X86 > VX_SIMD_X86_AMD)

#   undef(VX_SIMD_X86_AMD)
#   define VX_SIMD_X86_AMD VX_SIMD_X86

#endif

//==============================================================================
// finalize
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd.h

#if defined(VX_SIMD_ARM_VERSION) +   \
    defined(VX_SIMD_PPC_VERSION) +   \
    defined(VX_SIMD_X86_VERSION) +   \
    defined(VX_SIMD_X86_AMD_VERSION) \
    > 1
#   error "Multiple SIMD architectures detected!"
#endif

#if defined(VX_SIMD_X86_AMD)
#   define VX_SIMD VX_SIMD_X86_AMD
#elif defined(VX_SIMD_X86)
#   define VX_SIMD VX_SIMD_X86
#elif defined(VX_SIMD_ARM)
#   define VX_SIMD VX_SIMD_ARM
#elif defined(VX_SIMD_PPC)
#   define VX_SIMD VX_SIMD_PPC
#endif
