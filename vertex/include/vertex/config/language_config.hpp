#pragma once

#include "vertex/config/util.hpp"
#include "vertex/config/language.hpp"

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_comp_compiler_macros.html
// https://github.com/boostorg/predef/blob/e1211a4ca467bb6512e99025772ca25afa8d6159/include/boost/predef/compiler.h

///////////////////////////////////////////////////////////////////////////////
// Debug/Release
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_CMAKE_DEBUG) || defined(_DEBUG) || defined(DEBUG)
#   define VX_DEBUG 1
#else
#   define VX_DEBUG 0
#endif

#define VX_RELEASE (!VX_DEBUG)

///////////////////////////////////////////////////////////////////////////////
// Null Device
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define VX_NULL_DEVICE L"NUL:"
#else
#   define VX_NULL_DEVICE "/dev/null"
#endif

///////////////////////////////////////////////////////////////////////////////
// Text
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define __VX_TEXT(x) L##x
#else
#   define __VX_TEXT(x) x
#endif

#define VX_TEXT(x) __VX_TEXT(x)

///////////////////////////////////////////////////////////////////////////////
// Line End
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define VX_LINE_END "\r\n"
#else
#   define VX_LINE_END "\n"
#endif

///////////////////////////////////////////////////////////////////////////////
// Null While Loop
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_NULL_WHILE_LOOP_CONDITION (0,0)
#else
#   define VX_NULL_WHILE_LOOP_CONDITION (0)
#endif

///////////////////////////////////////////////////////////////////////////////
// File and Line Information
///////////////////////////////////////////////////////////////////////////////

#define VX_FILE __FILE__
#define VX_LINE __LINE__

///////////////////////////////////////////////////////////////////////////////
// Function Info
///////////////////////////////////////////////////////////////////////////////

#if VX_CPP_STANDARD >= 11 // C++11 and later
#   define VX_FUNCTION __func__
#elif ((defined(__GNUC__) && (__GNUC__ >= 2)) || defined(_MSC_VER) || defined (__WATCOMC__))
#   define VX_FUNCTION __FUNCTION__
#else
#   define VX_FUNCTION "???"
#endif

///////////////////////////////////////////////////////////////////////////////
// builtin
///////////////////////////////////////////////////////////////////////////////

#if defined(__has_builtin)
#   define VX_HAS_BUILTIN(x) __has_builtin(x)
#else
#   define VX_HAS_BUILTIN(x) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// include
///////////////////////////////////////////////////////////////////////////////

#if defined(__has_include)
#   define VX_HAS_INCLUDE(header) __has_include(header)
#else
#   define VX_HAS_INCLUDE(header) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// compiler attribute
///////////////////////////////////////////////////////////////////////////////

#if defined(__has_attribute)
#   define VX_HAS_COMPILER_ATTRIBUTE(x) __has_attribute(x)
#else
#   define VX_HAS_COMPILER_ATTRIBUTE(x) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// cpp attribute
///////////////////////////////////////////////////////////////////////////////

#if defined(__has_cpp_attribute)
#   define VX_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#   define VX_HAS_ATTRIBUTE(x) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// DLL Export/Import
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/0e08d15cca61f59341858f7f239d9bce3dc99f54/include/SDL3/SDL_begin_code.h#L57

#if !defined(VX_STATIC)

#   if defined(_MSC_VER) // Should just be windows

#       define VX_API_EXPORT __declspec(dllexport)
#       define VX_API_IMPORT __declspec(dllimport)

#   elif defined(__GNUC__) && __GNUC__ >= 4

#       define VX_API_EXPORT __attribute__((__visibility__("default")))
#       define VX_API_IMPORT __attribute__((__visibility__("default")))

#   endif

#else

#   define VX_API_EXPORT
#   define VX_API_IMPORT

#endif

#if defined(VX_DLL_EXPORT)
#   define VX_API VX_API_EXPORT
#else
#   define VX_API VX_API_IMPORT
#endif

///////////////////////////////////////////////////////////////////////////////
// Pragma Helper
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define __VX_PRAGMA(x) __pragma(x)
#elif defined(__GNUC__) || defined(__clang__)
#   define __VX_PRAGMA(x) _Pragma(VX_STRINGIFY(x))
#else
#   define __VX_PRAGMA(x)
#endif

///////////////////////////////////////////////////////////////////////////////
// Warning Suppression
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#   define VX_DISABLE_WARNING(warning_name, warning_number) __pragma(warning(disable: warning_number))
#   define VX_DISABLE_WARNING_PUSH()                        __pragma(warning(push, 0))
#   define VX_DISABLE_WARNING_POP()                         __pragma(warning(pop))

#   define VX_DISABLE_MSVC_WARNING(warning_number)          VX_DISABLE_WARNING("", warning_number)
#   define VX_DISABLE_MSVC_WARNING_PUSH()                   VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_MSVC_WARNING_POP()                    VX_DISABLE_WARNING_POP()

#   define VX_DISABLE_CLANG_WARNING(warning_name)
#   define VX_DISABLE_CLANG_WARNING_PUSH()
#   define VX_DISABLE_CLANG_WARNING_POP()

#   define VX_DISABLE_GCC_WARNING(warning_name)
#   define VX_DISABLE_GCC_WARNING_PUSH()
#   define VX_DISABLE_GCC_WARNING_POP()

#   define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
#   define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
#   define VX_DISABLE_GCC_CLANG_WARNING_POP()

#elif defined(__clang__)

#   define VX_DISABLE_WARNING(warning_name, warning_number) __VX_PRAGMA(clang diagnostic ignored warning_name)
#   define VX_DISABLE_WARNING_PUSH()                        _Pragma("clang diagnostic push")
#   define VX_DISABLE_WARNING_POP()                         _Pragma("clang diagnostic pop")

#   define VX_DISABLE_MSVC_WARNING(warning_number)
#   define VX_DISABLE_MSVC_WARNING_PUSH()
#   define VX_DISABLE_MSVC_WARNING_POP()

#   define VX_DISABLE_CLANG_WARNING(warning_name)           VX_DISABLE_WARNING(warning_name, 0)
#   define VX_DISABLE_CLANG_WARNING_PUSH()                  VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_CLANG_WARNING_POP()                   VX_DISABLE_WARNING_POP()

#   define VX_DISABLE_GCC_WARNING(warning_name)
#   define VX_DISABLE_GCC_WARNING_PUSH()
#   define VX_DISABLE_GCC_WARNING_POP()

#   define VX_DISABLE_GCC_CLANG_WARNING(warning_name)       VX_DISABLE_WARNING(warning_name, 0)
#   define VX_DISABLE_GCC_CLANG_WARNING_PUSH()              VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_GCC_CLANG_WARNING_POP()               VX_DISABLE_WARNING_POP()

#elif defined(__GNUC__)

#   define VX_DISABLE_WARNING(warning_name, warning_number) __VX_PRAGMA(GCC diagnostic ignored warning_name)
#   define VX_DISABLE_WARNING_PUSH()                        _Pragma("GCC diagnostic push")
#   define VX_DISABLE_WARNING_POP()                         _Pragma("GCC diagnostic pop")

#   define VX_DISABLE_MSVC_WARNING(warning_number)
#   define VX_DISABLE_MSVC_WARNING_PUSH()
#   define VX_DISABLE_MSVC_WARNING_POP()

#   define VX_DISABLE_CLANG_WARNING(warning_name)
#   define VX_DISABLE_CLANG_WARNING_PUSH()
#   define VX_DISABLE_CLANG_WARNING_POP()

#   define VX_DISABLE_GCC_WARNING(warning_name)             VX_DISABLE_WARNING(warning_name, 0)
#   define VX_DISABLE_GCC_WARNING_PUSH()                    VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_GCC_WARNING_POP()                     VX_DISABLE_WARNING_POP()

#   define VX_DISABLE_GCC_CLANG_WARNING(warning_name)       VX_DISABLE_WARNING(warning_name, 0)
#   define VX_DISABLE_GCC_CLANG_WARNING_PUSH()              VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_GCC_CLANG_WARNING_POP()               VX_DISABLE_WARNING_POP()

#else

#   define VX_DISABLE_WARNING(warning_name, warning_number)
#   define VX_DISABLE_WARNING_PUSH()
#   define VX_DISABLE_WARNING_POP()

#   define VX_DISABLE_MSVC_WARNING(warning_number)
#   define VX_DISABLE_MSVC_WARNING_PUSH()
#   define VX_DISABLE_MSVC_WARNING_POP()

#   define VX_DISABLE_CLANG_WARNING(warning_name)
#   define VX_DISABLE_CLANG_WARNING_PUSH()
#   define VX_DISABLE_CLANG_WARNING_POP()

#   define VX_DISABLE_GCC_WARNING(warning_name)
#   define VX_DISABLE_GCC_WARNING_PUSH()
#   define VX_DISABLE_GCC_WARNING_POP()

#   define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
#   define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
#   define VX_DISABLE_GCC_CLANG_WARNING_POP()

#endif

///////////////////////////////////////////////////////////////////////////////
// Likely/Unlikely Branching
///////////////////////////////////////////////////////////////////////////////

#if VX_CPP_STANDARD >= 20 // C++20 or later

#   define VX_LIKELY(expr) [[likely]] (expr)
#   define VX_UNLIKELY(expr) [[unlikely]] (expr)

#elif defined(__GNUC__) || defined(__clang__)

#   define VX_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#   define VX_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))

#else

#   define VX_LIKELY(expr) (expr)
#   define VX_UNLIKELY(expr) (expr)

#endif

///////////////////////////////////////////////////////////////////////////////
// Force Inline
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_FORCE_INLINE __forceinline
#elif VX_HAS_ATTRIBUTE(always_inline)
#   define VX_FORCE_INLINE inline __attribute__((always_inline))
#else
#   define VX_FORCE_INLINE inline
#endif

///////////////////////////////////////////////////////////////////////////////
// Fallthrough
///////////////////////////////////////////////////////////////////////////////

#if (VX_CPP_STANDARD >= 17)
#   define VX_FALLTHROUGH [[fallthrough]]
#elif defined(_MSC_VER)
#   define VX_FALLTHROUGH __fallthrough
#elif defined(__GNUC__) || defined(__clang__)
#   define VX_FALLTHROUGH [[clang::fallthrough]]
#else
#   define VX_FALLTHROUGH do {} while (0, 0)
#endif

///////////////////////////////////////////////////////////////////////////////
// No Throw
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_NOTHROW __declspec(nothrow)
#elif VX_HAS_ATTRIBUTE(nothrow)
#   define VX_NOTHROW __attribute__((nothrow))
#else
#   define VX_NOTHROW
#endif

///////////////////////////////////////////////////////////////////////////////
// Deprecation
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif VX_HAS_ATTRIBUTE(deprecated)
#   define VX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
#   define VX_DEPRECATED(msg)
#endif

///////////////////////////////////////////////////////////////////////////////
// If Constexpr
///////////////////////////////////////////////////////////////////////////////

#if (VX_CPP_STANDARD >= 17)
#   define VX_IF_CONSTEXPR(x) if constexpr (x)
#else
#   define VX_IF_CONSTEXPR if
#endif

///////////////////////////////////////////////////////////////////////////////
// stdcall
///////////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#   define VX_STDCALL __stdcall
#else
#   define VX_STDCALL
#endif

///////////////////////////////////////////////////////////////////////////////
// Static Assert
///////////////////////////////////////////////////////////////////////////////

#if (VX_CPP_STANDARD >= 11)
#   define VX_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#else
    // Fallback for pre-C++11: generates a compile-time error using typedef
    // Note: This will not show the message but will still fail the build
#   define VX_STATIC_ASSERT(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
#endif