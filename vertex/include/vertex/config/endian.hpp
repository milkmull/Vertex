#pragma once

#include "vertex/config/architecture.hpp"

//=============================================================================
// endianness
//=============================================================================
//
// Always defined (compare numerically, never test with `defined`):
//
//   VX_ORDER_LITTLE_ENDIAN / VX_ORDER_BIG_ENDIAN   distinct nonzero constants
//   VX_ORDER_NATIVE_ENDIAN                         one of the two above
//   VX_ENDIAN_LITTLE / VX_ENDIAN_BIG               1 or 0, safe in `#if`
//
//   #if VX_ENDIAN_LITTLE
//   #if VX_ORDER_NATIVE_ENDIAN == VX_ORDER_BIG_ENDIAN
//
// Mixed-endian (PDP) platforms, conflicting compiler macros and platforms whose
// byte order cannot be determined are a hard error.
//
// Detection tiers, from most to least authoritative:
//   1. Compiler-provided __BYTE_ORDER__ (GCC 4.6+, Clang, ICC, ...)
//   2. Compiler/target flags (__LITTLE_ENDIAN__, __ARMEL__, __MIPSEB, ...)
//   3. System-header macros (__BYTE_ORDER, _LITTLE_ENDIAN, ...). These only
//      count if the header that defines them was already included.
//   4. Architecture guess (x86, WebAssembly and MSVC targets are little-endian;
//      MSVC's _M_PPC is Xbox 360, which is big-endian)

#define VX_ORDER_LITTLE_ENDIAN 1234
#define VX_ORDER_BIG_ENDIAN    4321

// 1. Compiler-provided byte order

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN
#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
    #error "Vertex does not support platforms with PDP endianness"

// 2. Compiler-specific flags

#elif defined(__LITTLE_ENDIAN__) && defined(__BIG_ENDIAN__)
    #error "vertex/config/endian.hpp: conflicting endianness macros"
#elif defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || defined(__MIPSEL) || defined(__MIPSEL__) || defined(_MIPSEL)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || defined(__MIPSEB) || defined(__MIPSEB__) || defined(_MIPSEB)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

// 3. System headers' macros (glibc/BSD/Solaris style)

#elif defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN
#elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN
#elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

// 4. Architectures that are little-endian in practice (including MSVC targets
//    other than Xbox 360's PowerPC)

#elif defined(VX_ARCH_X86) || defined(VX_ARCH_WASM) || \
    defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64) || \
    defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_LITTLE_ENDIAN
#elif defined(_M_PPC)
    #define VX_ORDER_NATIVE_ENDIAN VX_ORDER_BIG_ENDIAN

#else
    #error "Vertex could not determine the endianness of this platform."
#endif

#if VX_ORDER_NATIVE_ENDIAN == VX_ORDER_LITTLE_ENDIAN
    #define VX_ENDIAN_LITTLE 1
    #define VX_ENDIAN_BIG    0
#else
    #define VX_ENDIAN_LITTLE 0
    #define VX_ENDIAN_BIG    1
#endif
