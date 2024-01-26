#pragma once

#include <cassert>
#include <cstddef>

// =============== version ===============

#define VX_VERSION_MAJOR 0
#define VX_VERSION_MINOR 1
#define VX_VERSION_PATCH 0

// =============== compilers ===============

#ifdef _MSC_VER                 // Microsoft Visual C++
#   define VX_COMPILER_MSVC
#elif defined(__GNUC__)         // GNU Compiler Collection
#   define VX_COMPILER_GNU
#elif defined(__clang__)        // Clang
#   define VX_COMPILER_CLANG
#else
#   error unsupported compiler
#endif

// =============== warning suppression ===============

#if defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)

#   define VX_DISABLE_WARNING_PUSH()                         _Pragma("GCC diagnostic push")
#   define VX_DISABLE_WARNING_POP()                          _Pragma("GCC diagnostic pop")
#   define VX_DISABLE_WARNING_IMPL(warning)                  _Pragma(#warning)
#   define VX_DISABLE_WARNING(warning_name, warning_number)  VX_DISABLE_WARNING_IMPL(GCC diagnostic ignored warning_name)

#elif defined(VX_COMPILER_MSVC)

#   define VX_DISABLE_WARNING_PUSH()                         __pragma(warning(push, 0))
#   define VX_DISABLE_WARNING_POP()                          __pragma(warning(pop))
#   define VX_DISABLE_WARNING(warning_name, warning_number)  __pragma(warning(disable: warning_number))

#else

#   define VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_WARNING_POP()
#   define VX_DISABLE_WARNING(warning_name, warning_number)

#endif