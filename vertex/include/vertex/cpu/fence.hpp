#pragma once

#include "vertex/config/attributes.hpp"
#include "vertex/config/cxx_features.hpp"
#include "vertex/config/intrinsics.hpp"
#include "vertex/cpu/memory_order.hpp"

namespace vx {
namespace cpu {
//namespace _fence_priv {
//
////=============================================================================
//// helpers
////=============================================================================
//
//#if defined(VX_ATOMIC_BACKEND_GCC_SYNC)
//
//// Stops the compiler reordering memory accesses across this point,
//// emits no instruction.
//VX_FORCE_INLINE void fence_compiler_barrier() noexcept
//{
//    __asm__ __volatile__("" ::: "memory");
//}
//
//#endif
//
//#if defined(VX_ATOMIC_BACKEND_MSVC)
//
//// Full hardware barrier (also a compiler barrier).
//VX_FORCE_INLINE void fence_msvc_full_barrier() noexcept
//{
//    #if defined(VX_ATOMIC_ARCH_X86_64)
//
//    __faststorefence();
//
//    #elif defined(VX_ATOMIC_ARCH_ARM)
//
//    // 0xB = ISH (inner shareable, full). _ARM_BARRIER_ISH / _ARM64_BARRIER_ISH
//    // have this value, spelled numerically so we don't depend on those macros.
//    __dmb(0xB);
//
//    #else
//
//    // x86-32 and anything else: a locked read-modify-write on a local is a full
//    // barrier on x86 and works without SSE2 (unlike mfence).
//    volatile long guard = 0;
//    (void)_InterlockedOr(&guard, 0);
//
//    #endif
//}
//
//#endif
//
//} // namespace _fence_priv

//=============================================================================
// thread_fence
//=============================================================================

template <memory_order Order>
VX_FORCE_INLINE void thread_fence() noexcept
{
    VX_IF_CONSTEXPR (Order == memory_order::relaxed)
    {
        // no ordering requested
    }
    else
    {
#if defined(VX_ATOMIC_BACKEND_GCC_ATOMIC)

        __atomic_thread_fence(to_underlying(promote_consume(Order)));

#elif defined(VX_ATOMIC_BACKEND_GCC_SYNC)

        // Only a full barrier is available, so every non-relaxed order pays for it.
        __sync_synchronize();

#elif defined(VX_ATOMIC_BACKEND_MSVC)

    #if defined(VX_ATOMIC_ARCH_X86)

        // x86 is TSO: acquire, release and acq_rel fences only need to stop the
        // compiler. Only seq_cst needs the store-load barrier.
        _ReadWriteBarrier();

        VX_IF_CONSTEXPR (Order == memory_order::seq_cst)
        {
            _fence_priv::fence_msvc_full_barrier();
            _ReadWriteBarrier();
        }

    #elif defined(VX_ATOMIC_ARCH_ARM64)

        // Acquire only needs load->load/store ordering (dmb ishld). Anything
        // with release semantics needs store ordering too (dmb ish).
        // 0x9 = ISHLD, 0xB = ISH.
        VX_IF_CONSTEXPR (has_release(Order))
        {
            __dmb(0xB);
        }
        else
        {
            __dmb(0x9);
        }

    #else

        // ARM32 has no load-only barrier, and unknown MSVC targets get the safe choice
        _fence_priv::fence_msvc_full_barrier();

    #endif

#else

    #error "vx::cpu::thread_fence: this backend is not implemented yet"

#endif
    }
}

//=============================================================================
// signal_fence
//=============================================================================

// Orders memory accesses between a thread and a signal handler running on that
// same thread. No instruction is needed, only a compiler barrier.

template <memory_order Order>
VX_FORCE_INLINE void signal_fence() noexcept
{
    VX_IF_CONSTEXPR (Order == memory_order::relaxed)
    {
        // no ordering requested
    }
    else
    {
#if defined(VX_ATOMIC_BACKEND_GCC_ATOMIC)

        __atomic_signal_fence(to_underlying(promote_consume(Order)));

#elif defined(VX_ATOMIC_BACKEND_GCC_SYNC)

        _fence_priv::fence_compiler_barrier();

#elif defined(VX_ATOMIC_BACKEND_MSVC)

        _ReadWriteBarrier();

#else

    #error "vx::cpu::signal_fence: this backend is not implemented yet"

#endif
    }
}

} // namespace cpu
} // namespace vx
