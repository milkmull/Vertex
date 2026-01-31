#pragma once

#include "vertex/config/language.hpp"
#include "vertex/config/util.hpp"

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_comp_compiler_macros.html
// https://github.com/boostorg/predef/blob/e1211a4ca467bb6512e99025772ca25afa8d6159/include/boost/predef/compiler.h

//=========================================================================
// Build Configuration
//=========================================================================

#if defined(VX_CMAKE_DEBUG) || defined(_DEBUG) || defined(DEBUG)
    #define VX_DEBUG 1
#else
    #define VX_DEBUG 0
#endif

#define VX_RELEASE (!VX_DEBUG)

//=========================================================================
// Platform Basics
//=========================================================================

#if defined(_WIN32)
    #define VX_NULL_DEVICE L"NUL:"
    #define VX_LINE_END    "\r\n"

    // Calling conventions (mostly relevant on x86)
    #define VX_STDCALL  __stdcall
    #define VX_CDECL    __cdecl
    #define VX_FASTCALL __fastcall
    #define VX_THISCALL __thiscall

#else
    #define VX_NULL_DEVICE "/dev/null"
    #define VX_LINE_END    "\n"

    // Non-Windows: usually ignore calling conventions
    #define VX_STDCALL
    #define VX_CDECL
    #define VX_FASTCALL
    #define VX_THISCALL
#endif


//=========================================================================
// Text / Encoding
//=========================================================================

#if defined(_WIN32)
    #define _VX_TEXT(x) L##x
#else
    #define _VX_TEXT(x) x
#endif

#define VX_TEXT(x) _VX_TEXT(x)

#define VX_EMPTY_DEFINE(def) defined(def) && ((def) + 0 != -14) && (7 - (def) - 7 == 14)

//=========================================================================
// Source Location
//=========================================================================

#define VX_FILE __FILE__
#define VX_LINE __LINE__

// Basic (standardized) function name
#if VX_CPP_STANDARD >= 11
    #define VX_FUNCTION __func__
#elif defined(__FUNCTION__)
    #define VX_FUNCTION __FUNCTION__
#else
    #define VX_FUNCTION "unknown function"
#endif

// Full signature (compiler-specific, includes templates, namespaces, etc.)
#if defined(__clang__) || defined(__GNUC__)
    #define VX_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define VX_PRETTY_FUNCTION __FUNCSIG__
#else
    #define VX_PRETTY_FUNCTION VX_FUNCTION
#endif

//=========================================================================
// Feature Detection Helpers
//=========================================================================

#if defined(__has_builtin)
    #define VX_HAS_BUILTIN(x) __has_builtin(x)
#else
    #define VX_HAS_BUILTIN(x) 0
#endif

#if defined(__has_include)
    #define VX_HAS_INCLUDE(header) __has_include(header)
#else
    #define VX_HAS_INCLUDE(header) 0
#endif

#if defined(__has_attribute)
    #define VX_HAS_COMPILER_ATTRIBUTE(x) __has_attribute(x)
#else
    #define VX_HAS_COMPILER_ATTRIBUTE(x) 0
#endif

#if defined(__has_cpp_attribute)
    #define VX_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
    #define VX_HAS_ATTRIBUTE(x) 0
#endif

//=========================================================================
// DLL Import / Export
//=========================================================================

#if !defined(VX_BUILD_STATIC)

    #if defined(_MSC_VER)
        #define VX_API_EXPORT __declspec(dllexport)
        #define VX_API_IMPORT __declspec(dllimport)
    #elif defined(__GNUC__) && __GNUC__ >= 4
        #define VX_API_EXPORT __attribute__((visibility("default")))
        #define VX_API_IMPORT __attribute__((visibility("default")))
    #else
        #define VX_API_EXPORT
        #define VX_API_IMPORT
    #endif

#else
    #define VX_API_EXPORT
    #define VX_API_IMPORT
#endif

#if defined(VX_BUILD_SHARED)
    #define VX_API VX_API_EXPORT
#else
    #define VX_API VX_API_IMPORT
#endif

//=========================================================================
// Pragma Utilities
//=========================================================================

#if defined(_MSC_VER)
    #define _VX_PRAGMA(x) __pragma(x)
#elif defined(__GNUC__) || defined(__clang__)
    #define _VX_PRAGMA(x) _Pragma(VX_STRINGIFY(x))
#else
    #define _VX_PRAGMA(x)
#endif

//=========================================================================
// Warning Control
//=========================================================================

#if defined(_MSC_VER)

    #define VX_DISABLE_WARNING_PUSH()                        __pragma(warning(push, 0))
    #define VX_DISABLE_WARNING(warning_name, warning_number) __pragma(warning(disable : warning_number))
    #define VX_DISABLE_WARNING_POP()                         __pragma(warning(pop))

    #define VX_DISABLE_MSVC_WARNING_PUSH()          VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number) VX_DISABLE_WARNING("", warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()           VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()

#elif defined(__clang__)

    #define VX_DISABLE_WARNING_PUSH()                        _Pragma("clang diagnostic push")
    #define VX_DISABLE_WARNING(warning_name, warning_number) _VX_PRAGMA(clang diagnostic ignored warning_name)
    #define VX_DISABLE_WARNING_POP()                         _Pragma("clang diagnostic pop")

    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

#elif defined(__GNUC__)

    #define VX_DISABLE_WARNING_PUSH()                        _Pragma("GCC diagnostic push")
    #define VX_DISABLE_WARNING(warning_name, warning_number) _VX_PRAGMA(GCC diagnostic ignored warning_name)
    #define VX_DISABLE_WARNING_POP()                         _Pragma("GCC diagnostic pop")

    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_WARNING_POP()         VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()        VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name) VX_DISABLE_WARNING(warning_name, 0)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()         VX_DISABLE_WARNING_POP()

#else

    #define VX_DISABLE_WARNING_PUSH()
    #define VX_DISABLE_WARNING(warning_name, warning_number)
    #define VX_DISABLE_WARNING_POP()

    #define VX_DISABLE_MSVC_WARNING_PUSH()
    #define VX_DISABLE_MSVC_WARNING(warning_number)
    #define VX_DISABLE_MSVC_WARNING_POP()

    #define VX_DISABLE_CLANG_WARNING_PUSH()
    #define VX_DISABLE_CLANG_WARNING(warning_name)
    #define VX_DISABLE_CLANG_WARNING_POP()

    #define VX_DISABLE_GCC_WARNING_PUSH()
    #define VX_DISABLE_GCC_WARNING(warning_name)
    #define VX_DISABLE_GCC_WARNING_POP()

    #define VX_DISABLE_GCC_CLANG_WARNING_PUSH()
    #define VX_DISABLE_GCC_CLANG_WARNING(warning_name)
    #define VX_DISABLE_GCC_CLANG_WARNING_POP()

#endif

//=========================================================================
// Control Flow & Branch Prediction
//=========================================================================

#if VX_CPP_STANDARD >= 20
    #define VX_LIKELY(expr)    (expr) [[likely]]
    #define VX_UNLIKELY(expr)  (expr) [[unlikely]]
    #define _VX_LIKELY_DEFINED 1
#elif defined(__GNUC__) || defined(__clang__)
    #define VX_LIKELY(expr)    (__builtin_expect(!!(expr), 1))
    #define VX_UNLIKELY(expr)  (__builtin_expect(!!(expr), 0))
    #define _VX_LIKELY_DEFINED 1
#else
    #define VX_LIKELY(expr)    (expr)
    #define VX_UNLIKELY(expr)  (expr)
    #define _VX_LIKELY_DEFINED 0
#endif

#if _VX_LIKELY_DEFINED && 0

    #define VX_UNLIKELY_COLD_PATH(cond, action) \
        do \
        { \
            if VX_UNLIKELY(cond) \
            { \
                action; \
            } \
        } while (0)

#else

    #define VX_UNLIKELY_COLD_PATH(cond, action) \
        do \
        { \
            if (!(cond)) \
            { \
                break; \
            } \
            do \
            { \
                action; \
            } while (0); \
        } while (0)

#endif

#undef _VX_LIKELY_DEFINED

#if defined(_MSC_VER)
    #define VX_ASSUME(expr) __assume(expr)
#elif defined(__clang__) || defined(__GNUC__)
    #define VX_ASSUME(expr) ((expr) ? static_cast<void>(0) : __builtin_unreachable())
#else
    #define VX_ASSUME(expr)
#endif

//=========================================================================
// Function Attributes
//=========================================================================

#if defined(_MSC_VER)
    #define VX_FORCE_INLINE __forceinline
    #define VX_NO_INLINE    __declspec(noinline)
#elif VX_HAS_COMPILER_ATTRIBUTE(always_inline)
    #define VX_FORCE_INLINE inline __attribute__((always_inline))
    #define VX_NO_INLINE    __attribute__((noinline))
#else
    #define VX_FORCE_INLINE inline
    #define VX_NO_INLINE
#endif

#if VX_CPP_STANDARD >= 17
    #define VX_NO_DISCARD [[nodiscard]]
#elif defined(_MSC_VER)
    #define VX_NO_DISCARD _Check_return_
#elif VX_HAS_COMPILER_ATTRIBUTE(warn_unused_result)
    #define VX_NO_DISCARD __attribute__((warn_unused_result))
#else
    #define VX_NO_DISCARD
#endif

#if defined(_MSC_VER)
    #define VX_NO_THROW __declspec(nothrow)
#elif VX_HAS_COMPILER_ATTRIBUTE(nothrow)
    #define VX_NO_THROW __attribute__((nothrow))
#else
    #define VX_NO_THROW
#endif

#if defined(_MSC_VER)
    #define VX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif VX_HAS_COMPILER_ATTRIBUTE(deprecated)
    #define VX_DEPRECATED(msg) __attribute__((deprecated(msg)))
#else
    #define VX_DEPRECATED(msg)
#endif

#if defined(_MSC_VER)
    #define VX_NO_ALIAS __declspec(noalias)
#elif VX_HAS_COMPILER_ATTRIBUTE(noalias)
    #define VX_NO_ALIAS __attribute__((noalias))
#else
    #define VX_NO_ALIAS
#endif

//=========================================================================
// Additional Attributes (Added)
//=========================================================================

#if VX_HAS_COMPILER_ATTRIBUTE(pure)
    #define VX_PURE __attribute__((pure))
#else
    #define VX_PURE
#endif

#if VX_HAS_COMPILER_ATTRIBUTE(const)
    #define VX_CONST_FN __attribute__((const))
#else
    #define VX_CONST_FN
#endif

#if VX_HAS_COMPILER_ATTRIBUTE(hot)
    #define VX_HOT __attribute__((hot))
#else
    #define VX_HOT
#endif

#if VX_HAS_COMPILER_ATTRIBUTE(cold)
    #define VX_COLD __attribute__((cold))
#else
    #define VX_COLD
#endif

#if defined(_MSC_VER)
    #define VX_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
    #define VX_RESTRICT __restrict__
#else
    #define VX_RESTRICT
#endif

//=========================================================================
// Parameter Attributes
//=========================================================================

#if VX_CPP_STANDARD >= 17
    #define VX_MAYBE_UNUSED [[maybe_unused]]
#elif VX_HAS_COMPILER_ATTRIBUTE(unused)
    #define VX_MAYBE_UNUSED __attribute__((unused))
#else
    #define VX_MAYBE_UNUSED
#endif

#define VX_UNUSED(x) ((void)(x))

//=========================================================================
// Language Utilities
//=========================================================================

#if VX_CPP_STANDARD >= 17
    #define VX_FALLTHROUGH [[fallthrough]]
#elif defined(__clang__)
    #define VX_FALLTHROUGH [[clang::fallthrough]]
#elif defined(_MSC_VER)
    #define VX_FALLTHROUGH __fallthrough
#else
    #define VX_FALLTHROUGH \
        do \
        { \
        } while (0)
#endif

#if defined(_MSC_VER)
    #define VX_NULL_WHILE_LOOP_CONDITION (0, 0)
#else
    #define VX_NULL_WHILE_LOOP_CONDITION (0)
#endif

//=========================================================================
// constexpr
//=========================================================================

#if VX_CPP_STANDARD >= 11
    #define VX_CONSTEXPR constexpr
#else
    #define VX_CONSTEXPR inline
#endif

#if VX_CPP_STANDARD >= 11
    #define VX_CONSTEXPR11 VX_CONSTEXPR
#else
    #define VX_CONSTEXPR11 inline
#endif

#if VX_CPP_STANDARD >= 14
    #define VX_CONSTEXPR14 VX_CONSTEXPR
#else
    #define VX_CONSTEXPR14 inline
#endif

#if VX_CPP_STANDARD >= 17
    #define VX_CONSTEXPR17 VX_CONSTEXPR
#else
    #define VX_CONSTEXPR17 inline
#endif

#if VX_CPP_STANDARD >= 20
    #define VX_CONSTEXPR20 VX_CONSTEXPR
#else
    #define VX_CONSTEXPR20 inline
#endif

#if VX_CPP_STANDARD >= 23
    #define VX_CONSTEXPR23 VX_CONSTEXPR
#else
    #define VX_CONSTEXPR23 inline
#endif

#if VX_CPP_STANDARD >= 17
    #define VX_IF_CONSTEXPR(x) if constexpr (x)
#else
    #define VX_IF_CONSTEXPR(x) if (x)
#endif

#if defined(_MSC_VER)
    #define VX_NULL_WHILE_LOOP_CONDITION (0, 0)
#else
    #define VX_NULL_WHILE_LOOP_CONDITION (0)
#endif

#if VX_CPP_STANDARD >= 20
    #define VX_IS_CONSTANT_EVALUATED() std::is_constant_evaluated()
#elif VX_CPP_STANDARD >= 17
    #if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
        #define VX_IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
    #else
        #define VX_IS_CONSTANT_EVALUATED() false
    #endif
#else
    #define VX_IS_CONSTANT_EVALUATED() false
#endif

//=========================================================================
// Static Assert
//=========================================================================

#if VX_CPP_STANDARD >= 11
    #define VX_STATIC_ASSERT(cond) static_assert(cond)
    #define VX_STATIC_ASSERT_MSG(cond, msg) static_assert(cond, msg)
#else
    #define VX_STATIC_ASSERT(cond) typedef char static_assertion[(cond) ? 1 : -1]
    #define VX_STATIC_ASSERT_MSG(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
#endif

//=========================================================================
// Trap / Unreachable
//=========================================================================

#if defined(_MSC_VER)
    #define VX_GENERATE_TRAP() ::__fastfail(7)
#else
    #define VX_GENERATE_TRAP() ::__builtin_trap()
#endif

#define VX_UNREACHABLE() VX_ASSUME(0)

//=========================================================================
// Allocator Annotation
//=========================================================================

#ifndef VX_ALLOCATOR
    #if defined(_MSC_VER)
        #define VX_ALLOCATOR __declspec(allocator)
    #elif defined(__GNUC__) || defined(__clang__)
        #define VX_ALLOCATOR __attribute__((malloc))
    #else
        #define VX_ALLOCATOR
    #endif
#endif
