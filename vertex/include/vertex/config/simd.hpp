#pragma once

// SIMD extension detection, derived from Boost.Predef hardware/simd
// (https://github.com/boostorg/predef), Copyright Rene Rivera et al.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// VX_SIMD_<FAMILY> and VX_SIMD are undefined when not detected, otherwise
// defined to the version number of the highest detected extension. The
// VX_SIMD_<FAMILY>_<EXT>_VERSION constants are always defined. Version numbers
// are only comparable within one family.
//
//   #if defined(VX_SIMD_X86) && (VX_SIMD_X86 >= VX_SIMD_X86_AVX2_VERSION)
//
// Families: ARM, PPC, X86, X86_AMD, WASM, RISCV, MIPS, LOONGARCH.
// VX_SIMD_X86_AMD is a refinement of X86 for AMD-specific extensions; VX_SIMD
// prefers it when set.
//
// Notes:
//   - Detection is based on compiler-defined feature macros, so it reflects
//     the flags the code is compiled with, not the CPU it runs on.
//   - AArch64 is assumed to have NEON. Builds that disable it
//     (-mgeneral-regs-only: kernels, bare metal) are still reported as NEON.
//   - ARM64EC defines _M_X64/_M_AMD64 but its SIMD is NEON, so it is
//     detected as ARM, never as x86.
//   - WebAssembly targets are never detected as x86, even when Emscripten's
//     SSE emulation macros (-msse2, ...) are defined.
//   - MSVC does not define __SSE3__ through __SSE4_2__ or __FMA__, so MSVC
//     builds report SSE2 until /arch:AVX (or higher) is used.
//   - AVX-512: VX_SIMD_X86_AVX512F_VERSION means the foundation subset only.
//     VX_SIMD_X86_AVX512_SKX_VERSION means F + BW + DQ + VL (the common
//     baseline since Skylake-X).

#include "vertex/config/version_number.hpp"

//==============================================================================
// arm
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/arm.h

#define VX_SIMD_ARM_NEON_VERSION VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_ARM_SVE_VERSION  VX_VERSION_NUMBER(2, 0, 0)
#define VX_SIMD_ARM_SVE2_VERSION VX_VERSION_NUMBER(2, 1, 0)

// SVE implies AArch64 (and therefore NEON), so it is checked first.
#if defined(__ARM_FEATURE_SVE2)

    #define VX_SIMD_ARM VX_SIMD_ARM_SVE2_VERSION

#elif defined(__ARM_FEATURE_SVE)

    #define VX_SIMD_ARM VX_SIMD_ARM_SVE_VERSION

#elif defined(__ARM_NEON__) || defined(__ARM_NEON) || defined(__aarch64__) || \
    defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)

    #define VX_SIMD_ARM VX_SIMD_ARM_NEON_VERSION

#endif // arm

//==============================================================================
// ppc
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/ppc.h

#define VX_SIMD_PPC_VMX_VERSION VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_PPC_VSX_VERSION VX_VERSION_NUMBER(1, 1, 0)
#define VX_SIMD_PPC_QPX_VERSION VX_VERSION_NUMBER(2, 0, 0)

#if defined(__VECTOR4DOUBLE__)

    #define VX_SIMD_PPC VX_SIMD_PPC_QPX_VERSION

#elif defined(__VSX__)

    #define VX_SIMD_PPC VX_SIMD_PPC_VSX_VERSION

#elif defined(__ALTIVEC__) || defined(__VEC__)

    #define VX_SIMD_PPC VX_SIMD_PPC_VMX_VERSION

#endif // ppc

//==============================================================================
// x86
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/x86.h

#define VX_SIMD_X86_MMX_VERSION        VX_VERSION_NUMBER(0, 99, 0)
#define VX_SIMD_X86_SSE_VERSION        VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_X86_SSE2_VERSION       VX_VERSION_NUMBER(2, 0, 0)
#define VX_SIMD_X86_SSE3_VERSION       VX_VERSION_NUMBER(3, 0, 0)
#define VX_SIMD_X86_SSSE3_VERSION      VX_VERSION_NUMBER(3, 1, 0)
#define VX_SIMD_X86_SSE4_1_VERSION     VX_VERSION_NUMBER(4, 1, 0)
#define VX_SIMD_X86_SSE4_2_VERSION     VX_VERSION_NUMBER(4, 2, 0)
#define VX_SIMD_X86_AVX_VERSION        VX_VERSION_NUMBER(5, 0, 0)
#define VX_SIMD_X86_FMA_VERSION        VX_VERSION_NUMBER(5, 2, 0)
#define VX_SIMD_X86_AVX2_VERSION       VX_VERSION_NUMBER(5, 3, 0)
#define VX_SIMD_X86_AVX512F_VERSION    VX_VERSION_NUMBER(6, 0, 0)
#define VX_SIMD_X86_AVX512_SKX_VERSION VX_VERSION_NUMBER(6, 1, 0)
#define VX_SIMD_X86_MIC_VERSION        VX_VERSION_NUMBER(9, 0, 0)

// WebAssembly (including Emscripten's SSE emulation) is not x86.
#if !defined(__wasm__) && !defined(__wasm32__) && !defined(__wasm64__)

    // MSVC x64 always has SSE2. ARM64EC defines _M_X64 too, but is not x86.
    #if defined(_M_X64) && !defined(_M_ARM64EC)
        #define VX_PRIV_SIMD_MSVC_X64
    #endif

    // AVX-512 implies AVX2, so it is checked first. "SKX" is F + BW + DQ + VL.
    #if defined(__AVX512F__) && defined(__AVX512BW__) && defined(__AVX512DQ__) && defined(__AVX512VL__)
        #define VX_PRIV_SIMD_AVX512_SKX
    #endif

    // Checked from highest to lowest version so that the result is always the
    // highest detected extension (FMA is checked before AVX for this reason).
    #if defined(__MIC__)
        #define VX_SIMD_X86 VX_SIMD_X86_MIC_VERSION
    #elif defined(VX_PRIV_SIMD_AVX512_SKX)
        #define VX_SIMD_X86 VX_SIMD_X86_AVX512_SKX_VERSION
    #elif defined(__AVX512F__)
        #define VX_SIMD_X86 VX_SIMD_X86_AVX512F_VERSION
    #elif defined(__AVX2__)
        #define VX_SIMD_X86 VX_SIMD_X86_AVX2_VERSION
    #elif defined(__FMA__)
        #define VX_SIMD_X86 VX_SIMD_X86_FMA_VERSION
    #elif defined(__AVX__)
        #define VX_SIMD_X86 VX_SIMD_X86_AVX_VERSION
    #elif defined(__SSE4_2__)
        #define VX_SIMD_X86 VX_SIMD_X86_SSE4_2_VERSION
    #elif defined(__SSE4_1__)
        #define VX_SIMD_X86 VX_SIMD_X86_SSE4_1_VERSION
    #elif defined(__SSSE3__)
        #define VX_SIMD_X86 VX_SIMD_X86_SSSE3_VERSION
    #elif defined(__SSE3__)
        #define VX_SIMD_X86 VX_SIMD_X86_SSE3_VERSION
    #elif defined(__SSE2__) || defined(VX_PRIV_SIMD_MSVC_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
        #define VX_SIMD_X86 VX_SIMD_X86_SSE2_VERSION
    #elif defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)
        #define VX_SIMD_X86 VX_SIMD_X86_SSE_VERSION
    #elif defined(__MMX__)
        #define VX_SIMD_X86 VX_SIMD_X86_MMX_VERSION
    #endif // x86

    #undef VX_PRIV_SIMD_MSVC_X64
    #undef VX_PRIV_SIMD_AVX512_SKX

//==============================================================================
// x86_amd
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd/x86_amd.h

    #define VX_SIMD_X86_AMD_SSE4A_VERSION VX_VERSION_NUMBER(4, 0, 0)
    #define VX_SIMD_X86_AMD_FMA4_VERSION  VX_VERSION_NUMBER(5, 1, 0)
    #define VX_SIMD_X86_AMD_XOP_VERSION   VX_VERSION_NUMBER(5, 1, 1)

    #if defined(__XOP__)
        #define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_XOP_VERSION
    #elif defined(__FMA4__)
        #define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_FMA4_VERSION
    #elif defined(__SSE4A__)
        #define VX_SIMD_X86_AMD VX_SIMD_X86_AMD_SSE4A_VERSION
    #endif

// An AMD-specific extension implies an AMD CPU, but the generic x86 extensions
// may be newer than the AMD-specific ones, so the final version is the higher
// of the two. (AMD-specific macros are assumed to appear only alongside the
// base x86 macros.)

    #if defined(VX_SIMD_X86_AMD) && defined(VX_SIMD_X86) && (VX_SIMD_X86 > VX_SIMD_X86_AMD)

        #undef VX_SIMD_X86_AMD
        #define VX_SIMD_X86_AMD VX_SIMD_X86

    #endif

#endif // !wasm

//==============================================================================
// wasm
//==============================================================================

#define VX_SIMD_WASM_SIMD128_VERSION VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_WASM_RELAXED_VERSION VX_VERSION_NUMBER(1, 1, 0)

#if defined(__wasm_relaxed_simd__)

    #define VX_SIMD_WASM VX_SIMD_WASM_RELAXED_VERSION

#elif defined(__wasm_simd128__)

    #define VX_SIMD_WASM VX_SIMD_WASM_SIMD128_VERSION

#endif // wasm

//==============================================================================
// riscv
//==============================================================================

#define VX_SIMD_RISCV_RVV_VERSION VX_VERSION_NUMBER(1, 0, 0)

#if defined(__riscv_vector)

    #define VX_SIMD_RISCV VX_SIMD_RISCV_RVV_VERSION

#endif // riscv

//==============================================================================
// mips
//==============================================================================

#define VX_SIMD_MIPS_MSA_VERSION VX_VERSION_NUMBER(1, 0, 0)

#if defined(__mips_msa)

    #define VX_SIMD_MIPS VX_SIMD_MIPS_MSA_VERSION

#endif // mips

//==============================================================================
// loongarch
//==============================================================================

#define VX_SIMD_LOONGARCH_LSX_VERSION  VX_VERSION_NUMBER(1, 0, 0)
#define VX_SIMD_LOONGARCH_LASX_VERSION VX_VERSION_NUMBER(2, 0, 0)

#if defined(__loongarch_asx)

    #define VX_SIMD_LOONGARCH VX_SIMD_LOONGARCH_LASX_VERSION

#elif defined(__loongarch_sx)

    #define VX_SIMD_LOONGARCH VX_SIMD_LOONGARCH_LSX_VERSION

#endif // loongarch

//==============================================================================
// finalize
//==============================================================================

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/hardware/simd.h

#if defined(VX_SIMD_ARM) + \
        defined(VX_SIMD_PPC) + \
        defined(VX_SIMD_X86) + \
        defined(VX_SIMD_WASM) + \
        defined(VX_SIMD_RISCV) + \
        defined(VX_SIMD_MIPS) + \
        defined(VX_SIMD_LOONGARCH) > \
    1
    #error "vertex/config/simd.hpp: multiple SIMD architectures detected"
#endif

#if defined(VX_SIMD_X86_AMD)
    #define VX_SIMD VX_SIMD_X86_AMD
#elif defined(VX_SIMD_X86)
    #define VX_SIMD VX_SIMD_X86
#elif defined(VX_SIMD_ARM)
    #define VX_SIMD VX_SIMD_ARM
#elif defined(VX_SIMD_PPC)
    #define VX_SIMD VX_SIMD_PPC
#elif defined(VX_SIMD_WASM)
    #define VX_SIMD VX_SIMD_WASM
#elif defined(VX_SIMD_RISCV)
    #define VX_SIMD VX_SIMD_RISCV
#elif defined(VX_SIMD_MIPS)
    #define VX_SIMD VX_SIMD_MIPS
#elif defined(VX_SIMD_LOONGARCH)
    #define VX_SIMD VX_SIMD_LOONGARCH
#endif
