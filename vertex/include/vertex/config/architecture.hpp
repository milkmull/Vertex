#pragma once

// Architecture detection, derived from Boost.Predef
// (https://github.com/boostorg/predef), Copyright Rene Rivera et al.
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)
//
// Every VX_ARCH_* macro is either undefined (not detected) or defined to a
// version number (VX_VERSION_NUMBER_AVAILABLE when no version can be
// determined). Use `#if defined(VX_ARCH_X)` or `#if VX_ARCH_X`.
//
// Family tags:
//   VX_ARCH_ARM, X86, PPC, RISCV, WASM, MIPS, LOONGARCH, S390, SPARC
//   VX_ARCH_UNKNOWN             defined when no family above was detected
//
// Variant tags (describe the ISA/ABI):
//   VX_ARCH_ARM32 / ARM64
//   VX_ARCH_X86_32 / X86_64     (VX_ARCH_X86 is set for either)
//   VX_ARCH_PPC32 / PPC64
//   VX_ARCH_RISCV32 / RISCV64
//   VX_ARCH_WASM32 / WASM64
//   VX_ARCH_MIPS32 / MIPS64     (follows the pointer width, see below)
//   VX_ARCH_LOONGARCH32 / LOONGARCH64
//   VX_ARCH_S390_32 / S390_64   (VX_ARCH_S390X is an alias for S390_64)
//   VX_ARCH_SPARC32 / SPARC64
// The x86 and s390 variants use an underscore, following Boost.Predef.
//
// Word size (pointer width, use these for sizing decisions):
//   VX_ARCH_WORD_BITS_32 / VX_ARCH_WORD_BITS_64
// Exactly one is defined whenever the width could be determined, and a
// conflicting detection is a hard error.
//
// The variant tags describe the ISA/ABI, while VX_ARCH_WORD_BITS_* describes
// the pointer width. They only differ on ILP32 ABIs running on 64-bit ISAs
// (x32, AArch64 ILP32 / Apple arm64_32, RV64ILP32): e.g. VX_ARCH_ARM64 and
// VX_ARCH_WORD_BITS_32 are both defined there. MIPS tags follow the pointer
// width, because MIPS64 with the n32 ABI has 32-bit pointers.

#include "vertex/config/version_number.hpp"

//=============================================================================
// helpers (private, undefined at the end of this file)
//=============================================================================

// Note: MSVC x64 is LLP64, so neither _LP64 nor __LP64__ is defined there and
// VX_PRIV_ARCH_PTR_64 stays undefined. This is intentional: every use below
// either defaults to 64-bit (x86-64, ARM64), doesn't exist on MSVC (MIPS,
// RISC-V, ...), or is covered by the _WIN64 check in the word size fallback.
// Don't "fix" it by adding _WIN64 here.

#if defined(__ILP32__) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 4))
    #define VX_PRIV_ARCH_PTR_32
#endif

#if defined(__LP64__) || defined(_LP64) || (defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 8))
    #define VX_PRIV_ARCH_PTR_64
#endif

//=============================================================================
// arm
//=============================================================================

#if defined(__ARM_ARCH) || defined(__TARGET_ARCH_ARM) || \
    defined(__TARGET_ARCH_THUMB) || defined(_M_ARM) || \
    defined(__arm__) || defined(__arm64) || defined(__arm64__) || defined(__thumb__) || \
    defined(_M_ARM64) || defined(_M_ARM64EC) || \
    defined(__aarch64__) || defined(__AARCH64EL__) || defined(__AARCH64EB__) || \
    defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__) || \
    defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)

// Detect version

    #if defined(__ARM_ARCH)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(__ARM_ARCH, 0, 0)

    #elif defined(__TARGET_ARCH_ARM)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(__TARGET_ARCH_ARM, 0, 0)

    #elif defined(__TARGET_ARCH_THUMB)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(__TARGET_ARCH_THUMB, 0, 0)

    #elif defined(_M_ARM)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(_M_ARM, 0, 0)

    #elif defined(__arm64) || defined(__arm64__) || defined(_M_ARM64) || defined(_M_ARM64EC) || \
        defined(__aarch64__) || defined(__AARCH64EL__) || defined(__AARCH64EB__)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(8, 0, 0)

    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \
        defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(7, 0, 0)

    #elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || \
        defined(__ARM_ARCH_6KZ__) || defined(__ARM_ARCH_6T2__)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(6, 0, 0)

    #elif defined(__ARM_ARCH_5TE__) || defined(__ARM_ARCH_5TEJ__)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(5, 0, 0)

    #elif defined(__ARM_ARCH_4T__) || defined(__ARM_ARCH_4__)

        #define VX_ARCH_ARM VX_VERSION_NUMBER(4, 0, 0)

    #else

        #define VX_ARCH_ARM VX_VERSION_NUMBER_AVAILABLE

    #endif

// Variant and word size
//
// Not derived from the ARM version: AArch32 builds targeting ARMv8 also
// report __ARM_ARCH == 8, so the version can't tell 32 from 64 bit.

    #if defined(__aarch64__) || defined(__AARCH64EL__) || defined(__AARCH64EB__) || \
        defined(__arm64) || defined(__arm64__) || defined(_M_ARM64) || defined(_M_ARM64EC)

        #define VX_ARCH_ARM64 VX_VERSION_NUMBER_AVAILABLE

        // AArch64 ILP32 and Apple arm64_32 have 32-bit pointers
        #if defined(VX_PRIV_ARCH_PTR_32)
            #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE
        #else
            #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE
        #endif

    #else

        #define VX_ARCH_ARM32        VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // arm

//=============================================================================
// x86
//=============================================================================

//========================================
// x86_32
//========================================

#if defined(i386) || defined(__i386__) || \
    defined(__i486__) || defined(__i586__) || \
    defined(__i686__) || defined(__i386) || \
    defined(_M_IX86) || defined(_X86_) || \
    defined(__THW_INTEL__) || defined(__I86__) || \
    defined(__INTEL__)

// Detect version

    #if defined(__I86__)

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(__I86__, 0, 0)

    #elif defined(_M_IX86)

        // _M_IX86 is 300, 400, 500 or 600
        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(_M_IX86 / 100, 0, 0)

    #elif defined(__i686__)

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(6, 0, 0)

    #elif defined(__i586__)

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(5, 0, 0)

    #elif defined(__i486__)

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(4, 0, 0)

    #elif defined(__i386__)

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER(3, 0, 0)

    #else

        #define VX_ARCH_X86_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

// Word size
//
// Note: __I86__ is also defined by 16-bit Watcom targets, which this reports
// as 32-bit. 16-bit x86 is not supported.

    #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

#endif // x86_32

//========================================
// x86_64
//========================================

// ARM64EC (Windows) defines _M_X64/_M_AMD64 as well as _M_ARM64EC, but its
// code is ARM64, so it must not be detected as x86-64.

#if (defined(__x86_64) || defined(__x86_64__) || \
    defined(__amd64__) || defined(__amd64) || \
    defined(_M_X64) || defined(_M_AMD64)) && \
    !defined(_M_ARM64EC)

    #define VX_ARCH_X86_64 VX_VERSION_NUMBER_AVAILABLE

// Word size
//
// x32 ABI: 64-bit ISA, 32-bit pointers.

    #if defined(VX_PRIV_ARCH_PTR_32)
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE
    #else
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE
    #endif

#endif // x86_64

//========================================
// x86
//========================================

#if defined(VX_ARCH_X86_32) || defined(VX_ARCH_X86_64)

    #define VX_ARCH_X86 VX_VERSION_NUMBER_AVAILABLE

#endif

//=============================================================================
// powerpc
//=============================================================================

// 64-bit PowerPC also defines the 32-bit macros (__powerpc__ etc.), so the
// 64-bit check comes first and 32-bit is "PowerPC but not 64-bit".

#if defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)

    #define VX_ARCH_PPC64        VX_VERSION_NUMBER_AVAILABLE
    #define VX_ARCH_PPC          VX_VERSION_NUMBER_AVAILABLE
    #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

#elif defined(__powerpc__) || defined(__powerpc) || defined(__ppc__) || \
    defined(__PPC__) || defined(__POWERPC__) || defined(_ARCH_PPC) || \
    defined(_M_PPC)

    #define VX_ARCH_PPC32        VX_VERSION_NUMBER_AVAILABLE
    #define VX_ARCH_PPC          VX_VERSION_NUMBER_AVAILABLE
    #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

#endif // powerpc

//=============================================================================
// risc-v
//=============================================================================

// __riscv_xlen is the register width. Only 32 and 64 are recognised; any other
// width (e.g. RV128) gets the family tag only, and the word size fallback at
// the end of this file decides the pointer width. Very old compilers don't
// define __riscv_xlen; they fall back to the pointer model.

#if defined(__riscv)

    #define VX_ARCH_RISCV VX_VERSION_NUMBER_AVAILABLE

    #if defined(__riscv_xlen)

        #if (__riscv_xlen == 64)
            #define VX_ARCH_RISCV64 VX_VERSION_NUMBER_AVAILABLE
            // RV64ILP32 has 32-bit pointers
            #if defined(VX_PRIV_ARCH_PTR_32)
                #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE
            #else
                #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE
            #endif
        #elif (__riscv_xlen == 32)
            #define VX_ARCH_RISCV32      VX_VERSION_NUMBER_AVAILABLE
            #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE
        #endif

    #elif defined(VX_PRIV_ARCH_PTR_64)

        #define VX_ARCH_RISCV64      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_RISCV32      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // risc-v

//=============================================================================
// webassembly
//=============================================================================

#if defined(__wasm__) || defined(__wasm32__) || defined(__wasm64__)

    #define VX_ARCH_WASM VX_VERSION_NUMBER_AVAILABLE

    #if defined(__wasm64__)

        #define VX_ARCH_WASM64       VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_WASM32       VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // wasm

//=============================================================================
// mips
//=============================================================================

// Variant and word size follow the pointer size, not the ISA: MIPS64 with the
// n32 ABI has 32-bit pointers.

#if defined(__mips__) || defined(__mips) || defined(__MIPS__)

    #define VX_ARCH_MIPS VX_VERSION_NUMBER_AVAILABLE

    #if (defined(_MIPS_SZPTR) && (_MIPS_SZPTR == 64)) || \
        (!defined(_MIPS_SZPTR) && defined(VX_PRIV_ARCH_PTR_64))

        #define VX_ARCH_MIPS64       VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_MIPS32       VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // mips

//=============================================================================
// loongarch
//=============================================================================

// __loongarch_grlen is the general-register width (32 or 64).

#if defined(__loongarch__)

    #define VX_ARCH_LOONGARCH VX_VERSION_NUMBER_AVAILABLE

    #if defined(__loongarch_grlen) && (__loongarch_grlen == 32)

        #define VX_ARCH_LOONGARCH32  VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_LOONGARCH64 VX_VERSION_NUMBER_AVAILABLE

        #if defined(VX_PRIV_ARCH_PTR_32)
            #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE
        #else
            #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE
        #endif

    #endif

#endif // loongarch

//=============================================================================
// s390 / z/architecture
//=============================================================================

// __s390x__ is the 64-bit ABI. __zarch__ is only the instruction set and is
// also defined for 31-bit code built with -mzarch, so it doesn't imply 64 bit.

#if defined(__s390__) || defined(__s390x__) || defined(__zarch__)

    #define VX_ARCH_S390 VX_VERSION_NUMBER_AVAILABLE

    #if defined(__s390x__)

        #define VX_ARCH_S390_64      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_S390X        VX_VERSION_NUMBER_AVAILABLE // alias for _64
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_S390_32      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // s390

//=============================================================================
// sparc
//=============================================================================

// __arch64__ (GCC/Clang) and __sparcv9 (Sun/Oracle) mean the 64-bit ABI.
// __sparc_v9__ alone is only the ISA and is also used for 32-bit code.

#if defined(__sparc__) || defined(__sparc) || defined(__sparc_v8__) || \
    defined(__sparc_v9__) || defined(__sparcv8) || defined(__sparcv9)

    #define VX_ARCH_SPARC VX_VERSION_NUMBER_AVAILABLE

    #if defined(__arch64__) || defined(__sparcv9)

        #define VX_ARCH_SPARC64      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #else

        #define VX_ARCH_SPARC32      VX_VERSION_NUMBER_AVAILABLE
        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // sparc

//=============================================================================
// unknown
//=============================================================================

#if !defined(VX_ARCH_ARM) && !defined(VX_ARCH_X86) && !defined(VX_ARCH_PPC) && \
    !defined(VX_ARCH_RISCV) && !defined(VX_ARCH_WASM) && !defined(VX_ARCH_MIPS) && \
    !defined(VX_ARCH_LOONGARCH) && !defined(VX_ARCH_S390) && !defined(VX_ARCH_SPARC)

    #define VX_ARCH_UNKNOWN VX_VERSION_NUMBER_AVAILABLE

#endif // unknown

//=============================================================================
// word size fallback
//=============================================================================

// Architectures without a section above (or with no usable macros) still get
// a word size when the compiler reports the pointer size.

#if !defined(VX_ARCH_WORD_BITS_32) && !defined(VX_ARCH_WORD_BITS_64)

    #if defined(VX_PRIV_ARCH_PTR_64) || defined(_WIN64)

        #define VX_ARCH_WORD_BITS_64 VX_VERSION_NUMBER_AVAILABLE

    #elif defined(VX_PRIV_ARCH_PTR_32) || defined(_WIN32)

        #define VX_ARCH_WORD_BITS_32 VX_VERSION_NUMBER_AVAILABLE

    #endif

#endif // word size fallback

#if defined(VX_ARCH_WORD_BITS_32) && defined(VX_ARCH_WORD_BITS_64)
    #error "vertex/config/arch.hpp: conflicting word size detection"
#endif

//=============================================================================
// cleanup
//=============================================================================

#undef VX_PRIV_ARCH_PTR_32
#undef VX_PRIV_ARCH_PTR_64
