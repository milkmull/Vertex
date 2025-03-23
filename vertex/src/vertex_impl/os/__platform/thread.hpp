#pragma once

#include "vertex/system/platform_config.hpp"
#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_thread.hpp"
#else
#   include "vertex_impl/os/__platform/dummy/dummy_thread.hpp"
#endif

namespace vx {
namespace os {

// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_atomic.h#L346

VX_FORCE_INLINE void thread_yield() noexcept
{
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))

    // x86/x86-64: Reduces power consumption while spinning (SSE2, "rep nop")
    __asm__ __volatile__("pause\n");

#elif (defined(__arm__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7) || defined(__aarch64__)

    // ARM/ARM64: Hint to the processor that the thread is waiting (ARMv7, ARMv8)
    __asm__ __volatile__("yield" ::: "memory");

#elif (defined(__powerpc__) || defined(__powerpc64__))

    // PowerPC: Lightweight instruction for thread yielding
    __asm__ __volatile__("or 27,27,27");

#elif (defined(__riscv) && __riscv_xlen == 64)

    // RISC-V: Recommended NOP for reducing contention
    __asm__ __volatile__(".insn i 0x0F, 0, x0, x0, 0x010");

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

    // MSVC x86/x64: SSE2 pause instruction
    _mm_pause();

#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))

    // MSVC ARM/ARM64: Yield instruction
    __yield();

#endif
}

} // namespace os
} // namespace vx