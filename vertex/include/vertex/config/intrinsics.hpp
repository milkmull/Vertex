#pragma once

// Compiler intrinsics: optimizer hints, traps and compiler-only barriers.
//
// One construct, different spelling per compiler. Nothing here depends on the
// CPU architecture; architecture-specific operations (hardware fences, CPU
// pause, prefetch) belong in the atomic/thread modules, not in config.
//
//   VX_ASSUME(expr)               optimizer hint; expr must be side-effect free
//                                 and true. Use as a statement only.
//   VX_UNREACHABLE()              control never gets here (UB if it does)
//   VX_GENERATE_TRAP()            abort the process immediately
//   VX_GENERATE_TRAP_CODE(code)   same, with a fast-fail code where supported
//   VX_COMPILER_BARRIER()         stops the compiler reordering memory accesses
//                                 across it. Emits NO CPU fence: it does not
//                                 order anything between threads or cores.
//
// Requires: VX_HAS_BUILTIN / VX_HAS_ATTRIBUTE (feature_detection.hpp),
// VX_UNUSED (macro.hpp, must expand to a void expression), VX_CPP_STANDARD
// (language.hpp).

#include "vertex/config/feature_detection.hpp"
#include "vertex/config/language.hpp"
#include "vertex/config/macro.hpp"

// __fastfail, _ReadWriteBarrier
#if defined(_MSC_VER)
    #include <intrin.h>
#endif

//=========================================================================
// Assume
//=========================================================================

// Attribute form is statement-only, so every branch is used as a statement.
// The attribute is only used in C++23 mode to avoid pedantic warnings, even
// though some compilers accept it earlier.
//
// GCC has no __builtin_assume; the conditional form below is the idiom. Note
// that it (unlike the others) evaluates expr, which is another reason expr
// must be side-effect free.

#if (VX_CPP_STANDARD >= 23) && VX_HAS_ATTRIBUTE(assume)
    #define VX_ASSUME(expr) [[assume(expr)]]
#elif defined(_MSC_VER)
    #define VX_ASSUME(expr) __assume(expr)
#elif VX_HAS_BUILTIN(__builtin_assume)
    #define VX_ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
    #define VX_ASSUME(expr) ((expr) ? static_cast<void>(0) : __builtin_unreachable())
#else
    // sizeof is unevaluated: no side effects, no "unused" warnings.
    #define VX_ASSUME(expr) static_cast<void>(sizeof(expr))
#endif

//=========================================================================
// Trap
//=========================================================================

#if !defined(VX_TRAP_CODE_DEFAULT)
    #define VX_TRAP_CODE_DEFAULT 7 // FAST_FAIL_FATAL_APP_EXIT
#endif

// The code is only meaningful for __fastfail; elsewhere it is ignored.
// (No `::` before builtins: they are not namespace members.)

#if defined(_MSC_VER)
    #define VX_GENERATE_TRAP_CODE(code) __fastfail(static_cast<unsigned int>(code))
#elif VX_HAS_BUILTIN(__builtin_trap) || defined(__GNUC__)
    #define VX_GENERATE_TRAP_CODE(code) (VX_UNUSED(code), __builtin_trap())
#else
    #include <cstdlib>
    #define VX_GENERATE_TRAP_CODE(code) (VX_UNUSED(code), ::std::abort())
#endif

#define VX_GENERATE_TRAP() VX_GENERATE_TRAP_CODE(VX_TRAP_CODE_DEFAULT)

//=========================================================================
// Unreachable
//=========================================================================

// Uses the dedicated intrinsic rather than VX_ASSUME(0): where VX_ASSUME has
// no real implementation (older GCC), VX_ASSUME(0) would be a no-op and
// control would fall through.

#if VX_HAS_BUILTIN(__builtin_unreachable) || defined(__GNUC__)
    #define VX_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define VX_UNREACHABLE() __assume(0)
#else
    #define VX_UNREACHABLE() VX_GENERATE_TRAP()
#endif

//=========================================================================
// Compiler Barrier
//=========================================================================

#if defined(__INTEL_COMPILER)
    #define VX_COMPILER_BARRIER() __memory_barrier()
#elif defined(__clang__)
    #define VX_COMPILER_BARRIER() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#elif defined(_MSC_VER) && !defined(_WIN32_WCE)
    #define VX_COMPILER_BARRIER() _ReadWriteBarrier()
#endif

#ifndef VX_COMPILER_BARRIER
    #define VX_COMPILER_BARRIER()
#endif
