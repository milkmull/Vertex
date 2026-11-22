#pragma once

//=========================================================================
// Calling Conventions
//=========================================================================

#if defined(_WIN32)

    // Calling conventions (mostly relevant on x86)
    #define VX_STDCALL  __stdcall
    #define VX_CDECL    __cdecl
    #define VX_FASTCALL __fastcall
    #define VX_THISCALL __thiscall

#else

    // Non-Windows: usually ignore calling conventions
    #define VX_STDCALL
    #define VX_CDECL
    #define VX_FASTCALL
    #define VX_THISCALL

#endif

//=========================================================================
// Control Flow & Branch Prediction
//=========================================================================

#if defined(__GNUC__) || defined(__clang__)
    #define VX_LIKELY(expr)   (__builtin_expect(!!(expr), 1))
    #define VX_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#elif VX_CPP_STANDARD >= 20
    #define VX_LIKELY(expr)   (expr) [[likely]]
    #define VX_UNLIKELY(expr) (expr) [[unlikely]]
#else
    #define VX_LIKELY(expr)   (expr)
    #define VX_UNLIKELY(expr) (expr)
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

#if VX_CPP_STANDARD >= 11
    #define VX_NO_RETURN [[noreturn]]
#elif defined(_MSC_VER)
    #define VX_NO_RETURN __declspec(noreturn)
#elif VX_HAS_COMPILER_ATTRIBUTE(noreturn)
    #define VX_NO_RETURN __attribute__((noreturn))
#else
    #define VX_NO_RETURN
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
