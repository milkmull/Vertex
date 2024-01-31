#pragma once

#include <cassert>
#include <cstddef>

// =============== version ===============

#define VX_VERSION_MAJOR 0
#define VX_VERSION_MINOR 1
#define VX_VERSION_PATCH 0

// =============== compiler ===============

#ifdef _MSC_VER                 // Microsoft Visual C++
#   define VX_COMPILER_MSVC
#elif defined(__GNUC__)         // GNU Compiler Collection
#   define VX_COMPILER_GNU
#elif defined(__clang__)        // Clang
#   define VX_COMPILER_CLANG
#else
#   error unsupported compiler
#endif

// =============== platform ===============

#if defined(_WIN32)
#   define VX_PLATFORM_WINDOWS
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_OS_MAC
#       define VX_PLATFORM_MACOS
#   endif
#elif defined(__linux__)
#   define VX_PLATFORM_LINUX
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

#ifdef VX_COMPILER_MSVC

#   define VX_PACK_PUSH()   __pragma(pack(push, 1))
#   define VX_PACK_POP()    __pragma(pack(pop))

#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_PACK_PUSH()   _Pragma("pack(1)")
#   define VX_PACK_POP()    _Pragma("pack()")

#else

#   define VX_PACK_PUSH()
#   define VX_PACK_POP()

#endif
