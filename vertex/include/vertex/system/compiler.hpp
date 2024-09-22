#pragma once

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_comp_compiler_macros.html

///////////////////////////////////////////////////////////////////////////////
// Compiler Identification
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)           // Microsoft Visual C++
#   define VX_COMPILER_MSVC
#elif defined(__GNUC__)         // GNU Compiler Collection
#   define VX_COMPILER_GNU
#elif defined(__clang__)        // Clang
#   define VX_COMPILER_CLANG
#else
#   error Unsupported Compiler
#endif

///////////////////////////////////////////////////////////////////////////////
// C++ Standard Detection
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_COMPILER_MSVC)
#   define VX_CPP_STD _MSVC_LANG
#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)
#   define VX_CPP_STD __cplusplus
#else
#   define VX_CPP_STD 0L
#endif

#if VX_CPP_STD >= 202002L
#   define VX_CPP_STANDARD 20
#elif VX_CPP_STD >= 201703L
#   define VX_CPP_STANDARD 17
#elif VX_CPP_STD >= 201402L
#   define VX_CPP_STANDARD 14
#elif VX_CPP_STD >= 201103L
#   define VX_CPP_STANDARD 11
#elif VX_CPP_STD >= 199711L
#   define VX_CPP_STANDARD 98
#else
#   define VX_CPP_STANDARD 1 // Non-standard C++
#endif

#undef VX_CPP_STD

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
// File and Line Information
///////////////////////////////////////////////////////////////////////////////

#define VX_FILE __FILE__
#define VX_LINE __LINE__

///////////////////////////////////////////////////////////////////////////////
// Warning Suppression
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Structure Packing
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Likely/Unlikely Branching
///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// Force Inline
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_COMPILER_MSVC)
#   define VX_FORCE_INLINE __forceinline
#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)
#   define VX_FORCE_INLINE inline __attribute__((always_inline))
#else
#   define VX_FORCE_INLINE inline
#endif

///////////////////////////////////////////////////////////////////////////////
// Fallthrough
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_COMPILER_MSVC)
#   define VX_FALLTHROUGH __fallthrough
#elif defined(VX_COMPILER_GNU) || defined(VX_COMPILER_CLANG)
#   define VX_FALLTHROUGH [[clang::fallthrough]]
#else
#   define VX_FALLTHROUGH do {} while (0)
#endif