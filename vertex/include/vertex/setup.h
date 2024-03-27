#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>

// =============== version ===============

#define VX_VERSION_MAJOR 0
#define VX_VERSION_MINOR 1
#define VX_VERSION_PATCH 0

// =============== compiler ===============

#if defined(_MSC_VER)           // Microsoft Visual C++
#   define VX_COMPILER_MSVC
#elif defined(__GNUC__)         // GNU Compiler Collection
#   define VX_COMPILER_GNU
#elif defined(__clang__)        // Clang
#   define VX_COMPILER_CLANG
#else
#   error unsupported compiler
#endif

// =============== system ===============

#if defined(_WIN32)
#   define VX_SYSTEM_WINDOWS
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_OS_MAC
#       define VX_SYSTEM_MACOS
#   endif
#elif defined(__linux__)
#   define VX_SYSTEM_LINUX
#else
#   error unsupported platform
#endif

// =============== architecture ===============

#if defined(VX_COMPILER_MSVC)

#   if defined(_M_X64) || defined(_M_AMD64)
#       define VX_ARCH_X86_64
#   elif defined(_M_IX86)
#       define VX_ARCH_X86
#   endif

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   if defined(__x86_64__) || defined(__amd64__)
#       define VX_ARCH_X86_64
#   elif defined(__i386__)
#       define VX_ARCH_X86
#   endif

#endif

// =============== c++ standard ===============

#if defined(VX_COMPILER_MSVC)

#   if _MSVC_LANG == 202002L
#      define VX_CPP_STANDARD 20
#   elif _MSVC_LANG == 201703L
#      define VX_CPP_STANDARD 17
#   elif _MSVC_LANG == 201402L
#      define VX_CPP_STANDARD 14
#   elif _MSVC_LANG == 201103L
#      define VX_CPP_STANDARD 11
#   elif _MSVC_LANG == 199711L
#      define VX_CPP_STANDARD 98
#   else
#      define VX_CPP_STANDARD 1
#   endif

#else

#   if __cplusplus == 202002L
#      define VX_CPP_STANDARD 20
#   elif __cplusplus == 201703L
#      define VX_CPP_STANDARD 17
#   elif __cplusplus == 201402L
#      define VX_CPP_STANDARD 14
#   elif __cplusplus == 201103L
#      define VX_CPP_STANDARD 11
#   elif __cplusplus == 199711L
#      define VX_CPP_STANDARD 98
#   else
#      define VX_CPP_STANDARD 1
#   endif

#endif

// =============== dll ===============

#if defined(VX_BUILD_SHARED_LIBS)
#   define VX_DLL_EXPORT __declspec(dllexport)
#else
#   define VX_DLL_EXPORT 
#endif

// =============== file function and line ===============

#if defined(VX_COMPILER_MSVC) || defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_FILE __FILE__
#   define VX_LINE __LINE__

#else

#   define VX_FILE ""
#   define VX_LINE 0

#endif

#if defined(VX_COMPILER_MSVC)

#   define VX_FUNC __FUNCSIG__

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_FUNC __PRETTY_FUNCTION__ 

#else

#   define VX_FUNC ""

#endif

// =============== warning suppression ===============

#if defined(VX_COMPILER_MSVC)

#   define VX_DISABLE_WARNING_PUSH()                         __pragma(warning(push, 0))
#   define VX_DISABLE_WARNING_POP()                          __pragma(warning(pop))
#   define VX_DISABLE_WARNING(warning_name, warning_number)  __pragma(warning(disable: warning_number))

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_DISABLE_WARNING_PUSH()                         _Pragma("GCC diagnostic push")
#   define VX_DISABLE_WARNING_POP()                          _Pragma("GCC diagnostic pop")
#   define VX_DISABLE_WARNING_IMPL(warning)                  _Pragma(#warning)
#   define VX_DISABLE_WARNING(warning_name, warning_number)  VX_DISABLE_WARNING_IMPL(GCC diagnostic ignored warning_name)

#else

#   define VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_WARNING_POP()
#   define VX_DISABLE_WARNING(warning_name, warning_number)

#endif

// =============== packing ===============

#if defined(VX_COMPILER_MSVC)

#   define VX_PACK_PUSH()   __pragma(pack(push, 1))
#   define VX_PACK_POP()    __pragma(pack(pop))

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_PACK_PUSH()   _Pragma("pack(1)")
#   define VX_PACK_POP()    _Pragma("pack()")

#else

#   define VX_PACK_PUSH()
#   define VX_PACK_POP()

#endif

// =============== (un)likely ===============

#if VX_CPP_STANDARD >= 20 // C++20 or later

#   define VX_LIKELY(expr) [[likely]] (expr)
#   define VX_UNLIKELY(expr) [[unlikely]] (expr)

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#   define VX_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))

#else

#   define VX_LIKELY(expr) (expr)
#   define VX_UNLIKELY(expr) (expr)

#endif

// =============== force inline ===============

#if defined(VX_COMPILER_MSVC)
#   define VX_FORCE_INLINE __forceinline
#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)
#   define VX_FORCE_INLINE inline __attribute__((always_inline))
#else
#   define VX_FORCE_INLINE inline
#endif

// =============== assert ===============

#ifdef VX_DEBUG
#   define VX_ENABLE_ASSERTS
#endif

#ifdef VX_ENABLE_ASSERTS
#   define VX_ASSERT(x, msg) { if (!(x)) std::cerr << "[ASSERT] [" << VX_FILE << ":" << VX_LINE << "] " << msg << std::endl; }
#else
#   define VX_ASSERT(x, msg)
#endif

// =============== error ===============

#define VX_ERROR(msg) { std::cerr << "[ERROR] [" << VX_FILE << ":" << VX_LINE << "] " << msg << std::endl; std::exit(EXIT_FAILURE); }
