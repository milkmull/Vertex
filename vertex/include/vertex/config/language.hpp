#pragma once

//=============================================================================
// C++ standard detection
//=============================================================================
//
// Unlike VX_OS_* and VX_ARCH_*, VX_CPP_STANDARD is always defined, to one of:
// 0 (pre-standard), 98, 11, 14, 17, 20, 23, 26. Compare numerically:
//
//   #if VX_CPP_STANDARD >= 17
//
// MSVC keeps __cplusplus at 199711L unless /Zc:__cplusplus is given, so
// _MSVC_LANG is used when present (this also covers clang-cl).
//
// Thresholds compare with `>` against the previous standard's value so that
// draft modes (-std=c++1z, c++2a, c++2b, which report intermediate values
// such as 201500L, 201709L or 202100L) land on the intended standard.
//
// The result is the language mode, not feature completeness. For individual
// features use the __cpp_* feature-test macros or __has_cpp_attribute.

#if defined(_MSVC_LANG)
    #define VX_PRIV_CPP_STD _MSVC_LANG
#elif defined(__cplusplus)
    #define VX_PRIV_CPP_STD __cplusplus
#else
    #define VX_PRIV_CPP_STD 0L
#endif

#if VX_PRIV_CPP_STD > 202302L
    #define VX_CPP_STANDARD 26
#elif VX_PRIV_CPP_STD > 202002L
    #define VX_CPP_STANDARD 23
#elif VX_PRIV_CPP_STD > 201703L
    #define VX_CPP_STANDARD 20
#elif VX_PRIV_CPP_STD > 201402L
    #define VX_CPP_STANDARD 17
#elif VX_PRIV_CPP_STD > 201103L
    #define VX_CPP_STANDARD 14
#elif VX_PRIV_CPP_STD > 199711L
    #define VX_CPP_STANDARD 11
#elif VX_PRIV_CPP_STD >= 199711L
    #define VX_CPP_STANDARD 98
#else
    #define VX_CPP_STANDARD 0
#endif

#undef VX_PRIV_CPP_STD

// Optional minimum-standard guard:
//
// #if VX_CPP_STANDARD < 17
//     #error "Vertex requires C++17 or later"
// #endif
