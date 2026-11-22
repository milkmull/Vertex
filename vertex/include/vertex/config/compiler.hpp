#pragma once

// Compiler detection, derived from Boost.Predef
// (https://github.com/boostorg/predef), Copyright Rene Rivera et al.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// Every VX_COMPILER_* macro is either undefined (not detected) or defined to
// a version number (VX_VERSION_NUMBER_AVAILABLE when no version can be
// determined). Use `#if defined(VX_COMPILER_X)` or `#if VX_COMPILER_X`.
//
// Primary tags (at most one is defined; they are mutually exclusive):
//   VX_COMPILER_CLANG, VX_COMPILER_INTEL, VX_COMPILER_GNUC, VX_COMPILER_MSVC
//   VX_COMPILER_UNKNOWN is defined when none of them was detected.
//
// Emulation: compilers that mimic another compiler also define that
// compiler's macros (clang and Intel define __GNUC__ on Linux; clang-cl and
// Intel define _MSC_VER on Windows). A compiler checked later than a primary
// one is reported as VX_COMPILER_<NAME>_EMULATED instead of VX_COMPILER_<NAME>.
// Limitation: only the compilers in this file are recognised, so any other
// GNU-compatible compiler (e.g. nvc++) is reported as VX_COMPILER_GNUC.
//
// Additive tags (may be defined alongside a primary tag):
//   VX_COMPILER_APPLE_CLANG   Apple's clang. Its version numbers follow
//                             Apple's own scheme and do NOT match upstream
//                             LLVM, so compare VX_COMPILER_CLANG only if
//                             VX_COMPILER_APPLE_CLANG is not defined.
//   VX_COMPILER_INTEL_LLVM    Intel oneAPI icx/icpx (clang based; the primary
//                             tag is VX_COMPILER_CLANG)
//   VX_COMPILER_NVCC          NVIDIA nvcc, which wraps a host compiler that
//                             gets its own primary tag
//
// Version notes:
//   - MSVC: VX_COMPILER_MSVC >= VX_VERSION_NUMBER(19, 30, 0) means
//     Visual Studio 2022 or later.
//   - Classic Intel from 2021 and Intel oneAPI report the year as the major
//     component (e.g. 2021, 2024). This relies on VX_VERSION_NUMBER accepting
//     a major above 99.

#include "vertex/config/version_number.hpp"

//=============================================================================
// clang
//=============================================================================

// Clang emulates both gcc and msvc, so it is checked first.

#if defined(__clang__)

    #define VX_COMPILER_CLANG VX_VERSION_NUMBER(__clang_major__, __clang_minor__, __clang_patchlevel__)

// Apple clang

    #if defined(__apple_build_version__)

        #define VX_COMPILER_APPLE_CLANG VX_VERSION_NUMBER(__clang_major__, __clang_minor__, __clang_patchlevel__)

    #endif

// Intel oneAPI (icx/icpx). __INTEL_LLVM_COMPILER is YYYYMMPP, e.g. 20240200

    #if defined(__INTEL_LLVM_COMPILER)

        #define VX_COMPILER_INTEL_LLVM VX_VERSION_NUMBER( \
            __INTEL_LLVM_COMPILER / 10000, \
            (__INTEL_LLVM_COMPILER / 100) % 100, \
            __INTEL_LLVM_COMPILER % 100)

    #endif

    #define VX_PRIV_COMPILER_DETECTED

#endif // clang

//=============================================================================
// intel (classic icc/icl)
//=============================================================================

// Classic Intel defines __GNUC__ on Linux and _MSC_VER on Windows, so it is
// checked before gcc and msvc.

#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)

// Detect version
//
// Up to 2020 the value is VVRP (1910 -> 19.1.0). From 2021 it is the year,
// with the update number in __INTEL_COMPILER_UPDATE.

    #if defined(__INTEL_COMPILER)

        #if (__INTEL_COMPILER >= 2000)

            #if defined(__INTEL_COMPILER_UPDATE)
                #define VX_PRIV_COMPILER_INTEL_DETECTION VX_VERSION_NUMBER(__INTEL_COMPILER, __INTEL_COMPILER_UPDATE, 0)
            #else
                #define VX_PRIV_COMPILER_INTEL_DETECTION VX_VERSION_NUMBER(__INTEL_COMPILER, 0, 0)
            #endif

        #else

            #define VX_PRIV_COMPILER_INTEL_DETECTION VX_VERSION_NUMBER( \
                __INTEL_COMPILER / 100, \
                (__INTEL_COMPILER / 10) % 10, \
                __INTEL_COMPILER % 10)

        #endif

    #else

        #define VX_PRIV_COMPILER_INTEL_DETECTION VX_VERSION_NUMBER_AVAILABLE

    #endif

// Emulated? (Not expected in practice: nothing checked earlier defines the
// Intel macros. Kept so every section follows the same pattern.)

    #if defined(VX_PRIV_COMPILER_DETECTED)
        #define VX_COMPILER_INTEL_EMULATED VX_PRIV_COMPILER_INTEL_DETECTION
    #else
        #define VX_COMPILER_INTEL VX_PRIV_COMPILER_INTEL_DETECTION
        #define VX_PRIV_COMPILER_DETECTED
    #endif

// Cleanup

    #undef VX_PRIV_COMPILER_INTEL_DETECTION

#endif // intel

//=============================================================================
// gcc
//=============================================================================

// Clang and Intel emulate gcc, so they are checked first.

#if defined(__GNUC__)

// Detect version

    #if defined(__GNUC_PATCHLEVEL__)
        #define VX_PRIV_COMPILER_GNUC_DETECTION VX_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
    #else
        #define VX_PRIV_COMPILER_GNUC_DETECTION VX_VERSION_NUMBER(__GNUC__, __GNUC_MINOR__, 0)
    #endif

// Emulated?

    #if defined(VX_PRIV_COMPILER_DETECTED)
        #define VX_COMPILER_GNUC_EMULATED VX_PRIV_COMPILER_GNUC_DETECTION
    #else
        #define VX_COMPILER_GNUC VX_PRIV_COMPILER_GNUC_DETECTION
        #define VX_PRIV_COMPILER_DETECTED
    #endif

// Cleanup

    #undef VX_PRIV_COMPILER_GNUC_DETECTION

#endif // gcc

//=============================================================================
// msvc
//=============================================================================

// Clang (clang-cl) and Intel emulate msvc, so they are checked first.

#if defined(_MSC_VER)

// Detect version
//
// _MSC_VER is VVRR (1938 -> 19.38). The build number comes from
// _MSC_FULL_VER, which has had two layouts: VVRRBBBB (8 digits) and
// VVRRBBBBB (9 digits). Anything unrecognized falls back to build 0.

    #if defined(_MSC_FULL_VER)

        #if ((_MSC_FULL_VER / 10000) == _MSC_VER)
            #define VX_PRIV_COMPILER_MSVC_BUILD (_MSC_FULL_VER % 10000)
        #elif ((_MSC_FULL_VER / 100000) == _MSC_VER)
            #define VX_PRIV_COMPILER_MSVC_BUILD (_MSC_FULL_VER % 100000)
        #else
            #define VX_PRIV_COMPILER_MSVC_BUILD 0
        #endif

    #else

        #define VX_PRIV_COMPILER_MSVC_BUILD 0

    #endif

    #define VX_PRIV_COMPILER_MSVC_DETECTION VX_VERSION_NUMBER(_MSC_VER / 100, _MSC_VER % 100, VX_PRIV_COMPILER_MSVC_BUILD)

// Emulated?

    #if defined(VX_PRIV_COMPILER_DETECTED)
        #define VX_COMPILER_MSVC_EMULATED VX_PRIV_COMPILER_MSVC_DETECTION
    #else
        #define VX_COMPILER_MSVC VX_PRIV_COMPILER_MSVC_DETECTION
        #define VX_PRIV_COMPILER_DETECTED
    #endif

// Cleanup

    #undef VX_PRIV_COMPILER_MSVC_BUILD
    #undef VX_PRIV_COMPILER_MSVC_DETECTION

#endif // msvc

//=============================================================================
// nvcc
//=============================================================================

// Additive: nvcc defines its host compiler's macros, so the host compiler
// still gets its primary tag above.

#if defined(__NVCC__)

    #if defined(__CUDACC_VER_MAJOR__) && defined(__CUDACC_VER_MINOR__) && defined(__CUDACC_VER_BUILD__)
        #define VX_COMPILER_NVCC VX_VERSION_NUMBER(__CUDACC_VER_MAJOR__, __CUDACC_VER_MINOR__, __CUDACC_VER_BUILD__)
    #else
        #define VX_COMPILER_NVCC VX_VERSION_NUMBER_AVAILABLE
    #endif

#endif // nvcc

//=============================================================================
// unknown
//=============================================================================

#if !defined(VX_PRIV_COMPILER_DETECTED)
    #define VX_COMPILER_UNKNOWN VX_VERSION_NUMBER_AVAILABLE
#endif

//=============================================================================
// cleanup
//=============================================================================

#undef VX_PRIV_COMPILER_DETECTED
