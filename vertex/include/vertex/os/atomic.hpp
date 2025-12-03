#pragma once

#include <atomic>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Atomic
///////////////////////////////////////////////////////////////////////////////

template <typename T>
using atomic = std::atomic<T>;

///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_atomic.h#L346

VX_FORCE_INLINE void cpu_pause() noexcept
{
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))

    // Some assemblers can't do REP NOP, so go with PAUSE
    __asm__ __volatile__("pause\n");

#elif (defined(__arm__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7) || defined(__aarch64__)

    __asm__ __volatile__("yield" ::: "memory");

#elif (defined(__powerpc__) || defined(__powerpc64__))

    __asm__ __volatile__("or 27,27,27");

#elif (defined(__riscv) && __riscv_xlen == 64)

    __asm__ __volatile__(".insn i 0x0F, 0, x0, x0, 0x010");

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

    // this is actually "rep nop" and not a SIMD instruction. No inline asm in MSVC x86-64!
    _mm_pause();

#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))

    __yield();

#elif defined(__WATCOMC__) && defined(__386__)

    // Watcom inline assembly must be declared externally
    extern VX_FORCE_INLINE void cpu_pause();
#   pragma aux cpu_pause = ".686p" ".xmm2" "pause"

#endif
}

} // namespace os
} // namespace vx
