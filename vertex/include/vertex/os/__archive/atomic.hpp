#pragma once

#include "vertex/system/compiler.hpp"

// https://github.com/libsdl-org/SDL/blob/b8381b3a2ea9873c6baedb8586da8bb5a2775d24/CMakeLists.txt#L580
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic#L464

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#   include <intrin.h>
#   define HAVE_MSC_ATOMICS
#endif

#if !defined(HAVE_GCC_ATOMICS) && defined(VX_PLATFORM_MACOS)
#   include <libkern/OSAtomic.h>
#endif

#if defined(__clang__) && (VX_HAS_BUILTIN(__atomic_load_n) || defined(HAVE_GCC_ATOMICS))
#   define HAVE_ATOMIC_LOAD_N
#elif defined(__GNCU__) && (__GNCU__ >= 5)
#   define HAVE_ATOMIC_LOAD_N
#endif

#if !defined(HAVE_MSC_ATOMICS) && \
    !defined(HAVE_GCC_ATOMICS) && \
    !defined(SDL_PLATFORM_MACOS) && \
    !defined(SDL_PLATFORM_SOLARIS) && \
    !defined(HAVE_WATCOM_ATOMICS)

#   define EMULATE_CAS

#endif

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// barrier
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/ops_msvc_common.hpp#L28

#if defined(_MSC_VER)

#if defined(__INTEL_COMPILER)

#define VX_ATOMIC_BARRIER() __memory_barrier()

#elif defined(__clang__) && VX_HAS_BUILTIN(__atomic_signal_fence)

#define VX_ATOMIC_BARRIER() __atomic_signal_fence(__ATOMIC_SEQ_CST)

#elif defined(_MSC_VER) && !defined(_WIN32_WCE)

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
#define VX_ATOMIC_BARRIER() _ReadWriteBarrier()

#endif

#endif // _MSC_VER

#if !defined(VX_ATOMIC_BARRIER)
#   define VX_ATOMIC_BARRIER()
#endif

///////////////////////////////////////////////////////////////////////////////
// Spinlock
///////////////////////////////////////////////////////////////////////////////

// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic#L1001

class spinlock
{
public:

#if defined(__GNUC__) || defined(__clang__)
    using lock_type = int32_t;
#elif defined(_MSC_VER)
    using lock_type = long;
#else
    using lock_type = int32_t;
#endif

    spinlock() noexcept = default;
    ~spinlock() = default;

    spinlock(const spinlock&) = delete;
    spinlock& operator=(const spinlock&) = delete;

private:

    void yield() noexcept
    {
#   if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))

        // x86/x86-64: Reduces power consumption while spinning (SSE2, "rep nop")
        __asm__ __volatile__("pause\n");

#   elif (defined(__arm__) && defined(__ARM_ARCH) && __ARM_ARCH >= 7) || defined(__aarch64__)

        // ARM/ARM64: Hint to the processor that the thread is waiting (ARMv7, ARMv8)
        __asm__ __volatile__("yield" ::: "memory");

#   elif (defined(__powerpc__) || defined(__powerpc64__))

        // PowerPC: Lightweight instruction for thread yielding
        __asm__ __volatile__("or 27,27,27");

#   elif (defined(__riscv) && __riscv_xlen == 64)

        // RISC-V: Recommended NOP for reducing contention
        __asm__ __volatile__(".insn i 0x0F, 0, x0, x0, 0x010");

#   elif defined(_MSC_VER) && ((defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)) || (defined(_M_X64) && !defined(_M_ARM64EC)))

        // MSVC x86/x64: SSE2 pause instruction
        _mm_pause();

#   elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64))

        // MSVC ARM/ARM64: Yield instruction
        __yield();

#   endif
    }

    // https://github.com/gcc-mirror/gcc/blob/master/libitm/local_atomic
    // https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic
    // https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/src/atomic/SDL_atomic.c

    bool try_lock() noexcept
    {
#   if defined(HAVE_GCC_ATOMICS) || defined(HAVE_GCC_SYNC_LOCK_TEST_AND_SET)

        // GCC built-in function for atomic test-and-set.
        // Returns the old value of 'lock' before setting it to 1.
        return __sync_lock_test_and_set(&m_lock, 1) == 0;

#   elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64))

        // MSVC atomic exchange operation for ARM platforms.
        // Sets 'lock' to 1 and returns the previous value.
        return _InterlockedExchange_acq(m_lock, 1) == 0;

#   elif defined(_MSC_VER) && ((defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)) || (defined(_M_X64) && !defined(_M_ARM64EC)))

        // MSVC atomic exchange operation for x86/x64 platforms.
        // Performs an atomic exchange of 'lock' with 1.
        static_assert(sizeof(m_lock) == sizeof(long));
        return _InterlockedExchange(reinterpret_cast<volatile long*>(&m_lock), 1) == 0;

#   elif defined(__GNUC__) && defined(__arm__) &&                \
        (defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__) ||  \
         defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__) ||  \
         defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5TE__) || \
         defined(__ARM_ARCH_5TEJ__))

        int result;

        // ARMv5 and earlier atomic swap using 'swp' instruction.
        // Swaps the value at address 'lock' with 1.
        __asm__ __volatile__(
            "swp %0, %1, [%2]\n"
            : "=&r"(result)         // Output: 'result' will contain the old value of 'lock'.
            : "r"(1),               // Input: The value to set 'lock' to (1).
            "r"(&m_lock)            // Input: Address of 'lock'.
            : "memory");            // Clobbers: Ensures memory ordering.

        return result == 0;

#   elif defined(__GNUC__) && defined(__arm__)

        int result;

        // ARMv6 and above: Load-Exclusive / Store-Exclusive for atomic operations.
        // This sequence attempts to atomically set 'lock' to 1 if it is 0.
        __asm__ __volatile__(
            "ldrex %0, [%2]\n"          // Load-Exclusive: Load 'lock' value into 'result'.
            "teq   %0, #0\n"            // Test if 'result' is zero (unlocked).
            "strexeq %0, %1, [%2]"      // Conditionally store 1 to 'lock' if 'result' was zero.
            : "=&r"(result)             // Output: 'result' will be 0 if the store was successful.
            : "r"(1),                   // Input: The value to set 'lock' to (1).
            "r"(&m_lock)                // Input: Address of 'lock'.
            : "cc", "memory");          // Clobbers: Flags register and memory ordering.

        return result == 0;

#   elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

        int result;

        // x86/x86_64 atomic exchange using 'xchg' instruction.
        // Atomically swaps 'lock' with the value 1 and stores previous value in 'result'.
        __asm__ __volatile__(
            "lock ; xchgl %0, (%1)\n"
            : "=r"(result)              // Output: 'result' receives the old value of 'lock'.
            : "r"(&m_lock), "0"(1)      // Inputs: Address of 'lock' and the value 1.
            : "cc", "memory");          // Clobbers: Flags register and memory ordering.

        return result == 0;

#   elif defined(VX_PLATFORM_MACOS) || defined(VX_PLATFORM_IOS)

        // Apple platforms (may use PowerPC or Intel). Prefers GCC atomics if available.
        return OSAtomicCompareAndSwap32Barrier(0, 1, m_lock);

#   else

        // Fallback implementation using mutex if nothing else is available.

        static os::mutex s_mtx;
        s_mtx.lock();

        if (lock == 0)
        {
            m_lock = 1;
            s_mtx.unlock();
            return true;
        }
        else
        {
            s_mtx.unlock();
            return false;
        }

#   endif
    }

public:

    // https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic#L464

    void lock() noexcept
    {
        int backoff = 1;
        constexpr int max_backoff = 64;

        while (!try_lock())
        {
            while (m_lock != 0) // Volatile read
            {
                for (int i = backoff; i != 0; --i)
                {
                    yield();
                }

                backoff = backoff < max_backoff ? backoff << 1 : max_backoff;
            }
        }
    }

    void unlock() noexcept
    {
#   if defined(HAVE_ATOMIC_LOAD_N)

        __atomic_store_n(reinterpret_cast<volatile unsigned __int##_Width*>(_Ptr), static_cast<unsigned __int##_Width>(_Desired), 3)

#   elif defined(HAVE_GCC_ATOMICS) || defined(HAVE_GCC_SYNC_LOCK_TEST_AND_SET)

        __sync_lock_release(&m_lock);

#   elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))

        _ReadWriteBarrier();
        _InterlockedExchange_rel(reinterpret_cast<volatile long*>(&lock), 0);
    }

#   elif defined(_MSC_VER)

        _ReadWriteBarrier();
        _InterlockedExchange(&m_lock, 0);

#   else

        m_lock = 0;

#   endif
    }

    bool is_locked() const noexcept { return m_lock == 1; }

private:

    volatile lock_type m_lock = 0;
};

namespace __detail {

class atomic_impl;

} // namespace __detail

template <typename T>
class atomic
{
public:

    using type = T;

    T load() const noexcept
    {

    }

    void store(T value) noexcept
    {

    }

private:

    friend __detail::atomic_impl;

    T m_value;
};

}
}