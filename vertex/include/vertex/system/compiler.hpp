#pragma once

// https://www.boost.org/doc/libs/1_55_0/libs/predef/doc/html/predef/reference/boost_comp_compiler_macros.html

///////////////////////////////////////////////////////////////////////////////
// Compiler Identification
///////////////////////////////////////////////////////////////////////////////

#if defined(__clang__)
#   define VX_COMPILER_CLANG
#elif defined(__GNUC__)
#   define VX_COMPILER_GNU
#elif defined(_MSC_VER)
#   define VX_COMPILER_MSVC
#else
#   error Unknown Compiler
#endif

///////////////////////////////////////////////////////////////////////////////
// C++ Standard Detection
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && defined(_MSVC_LANG)
#   define VX_CPP_STD _MSVC_LANG
#elif defined(__cplusplus)
#   define VX_CPP_STD __cplusplus
#else
#   define VX_CPP_STD 0L
#endif

#if VX_CPP_STD >= 202302L
#   define VX_CPP_STANDARD 23
#elif VX_CPP_STD >= 202002L
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
#   define VX_CPP_STANDARD 0
#endif

#undef VX_CPP_STD

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
// Helper Macros
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#   define __VX_PRAGMA(x) __pragma(x)

#elif defined(__GNUC__) || defined(__clang__)

#   define __VX_PRAGMA(x) _Pragma(#x)

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

#   define VX_DISABLE_WARNING(warning_name, warning_number) __VX_PRAGMA(clang diagnostic ignored "-W" warning_name)
#   define VX_DISABLE_WARNING_PUSH()                            _Pragma("clang diagnostic push")
#   define VX_DISABLE_WARNING_POP()                             _Pragma("clang diagnostic pop")

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

#   define VX_DISABLE_WARNING(warning_name, warning_number) __VX_PRAGMA(GCC diagnostic ignored "-W" warning_name)
#   define VX_DISABLE_WARNING_PUSH()                            _Pragma("GCC diagnostic push")
#   define VX_DISABLE_WARNING_POP()                             _Pragma("GCC diagnostic pop")

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
// Structure Packing
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#   define VX_PACK_PUSH()   __pragma(pack(push, 1))
#   define VX_PACK_POP()    __pragma(pack(pop))

#elif defined(__GNUC__) || defined(__clang__)

#   define VX_PACK_PUSH()   _Pragma("pack(push, 1)")
#   define VX_PACK_POP()    _Pragma("pack(pop)")

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
#elif defined(__GNUC__) || defined(__clang__)
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
#elif defined(__GNUC__) || defined(__clang__)
#   define VX_NOTHROW __attribute__((nothrow))
#else
#   define VX_NOTHROW
#endif

///////////////////////////////////////////////////////////////////////////////
// Deprecation
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define VX_DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__GNUC__) || defined(__clang__)
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
// builtin
///////////////////////////////////////////////////////////////////////////////

#if defined(__has_builtin)
#   define VX_HAS_BUILTIN(x) __has_builtin(x)
#else
#   define VX_HAS_BUILTIN(x) 0
#endif

///////////////////////////////////////////////////////////////////////////////
// stdcall
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#   define VX_STDCALL __stdcall
#else
#   define VX_STDCALL
#endif