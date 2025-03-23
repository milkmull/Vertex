#include "vertex/os/atomic.hpp"
#include "vertex/os/mutex.hpp"
#include "vertex_impl/os/__platform/thread.hpp"

#include "vertex/system/platform.hpp"

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex_impl/os/__platform/windows/windows_tools.hpp"
#endif

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#   include <xmmintrin.h>
#endif

#if !defined(HAVE_GCC_ATOMICS) && defined(VX_PLATFORM_MACOS)
#   include <libkern/OSAtomic.h>
#endif

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// spinlock lock/unlock
///////////////////////////////////////////////////////////////////////////////

// https://github.com/gcc-mirror/gcc/blob/master/libitm/local_atomic
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic
// https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/src/atomic/SDL_atomic.c

static bool try_spinlock(typename spinlock::lock_type& lock) noexcept
{
#if defined(HAVE_GCC_ATOMICS) || defined(HAVE_GCC_SYNC_LOCK_TEST_AND_SET)

    // GCC built-in function for atomic test-and-set.
    // Returns the old value of 'lock' before setting it to 1.
    return __sync_lock_test_and_set(&lock, 1) == 0;

#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))

    // MSVC atomic exchange operation for ARM platforms.
    // Sets 'lock' to 1 and returns the previous value.
    return _InterlockedExchange_acq(lock, 1) == 0;

#elif defined(_MSC_VER)

    // MSVC atomic exchange operation for x86/x64 platforms.
    // Performs an atomic exchange of 'lock' with 1.
    static_assert(sizeof(lock) == sizeof(long));
    return InterlockedExchange(reinterpret_cast<volatile long*>(&lock), 1) == 0;

#elif defined(__GNUC__) && defined(__arm__) &&               \
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
        "r"(&lock)              // Input: Address of 'lock'.
        : "memory");            // Clobbers: Ensures memory ordering.

    return result == 0;

#elif defined(__GNUC__) && defined(__arm__)

    int result;

    // ARMv6 and above: Load-Exclusive / Store-Exclusive for atomic operations.
    // This sequence attempts to atomically set 'lock' to 1 if it is 0.
    __asm__ __volatile__(
        "ldrex %0, [%2]\n"          // Load-Exclusive: Load 'lock' value into 'result'.
        "teq   %0, #0\n"            // Test if 'result' is zero (unlocked).
        "strexeq %0, %1, [%2]"      // Conditionally store 1 to 'lock' if 'result' was zero.
        : "=&r"(result)             // Output: 'result' will be 0 if the store was successful.
        : "r"(1),                   // Input: The value to set 'lock' to (1).
        "r"(&lock)                  // Input: Address of 'lock'.
        : "cc", "memory");          // Clobbers: Flags register and memory ordering.

    return result == 0;

#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

    int result;

    // x86/x86_64 atomic exchange using 'xchg' instruction.
    // Atomically swaps 'lock' with the value 1 and stores previous value in 'result'.
    __asm__ __volatile__(
        "lock ; xchgl %0, (%1)\n"
        : "=r"(result)           // Output: 'result' receives the old value of 'lock'.
        : "r"(&lock), "0"(1)     // Inputs: Address of 'lock' and the value 1.
        : "cc", "memory");       // Clobbers: Flags register and memory ordering.

    return result == 0;

#elif defined(VX_PLATFORM_MACOS) || defined(VX_PLATFORM_IOS)

    // Apple platforms (may use PowerPC or Intel). Prefers GCC atomics if available.
    return OSAtomicCompareAndSwap32Barrier(0, 1, lock);

#else

    // Fallback implementation using mutex if nothing else is available.

    static os::mutex s_mtx;
    s_mtx.lock();

    if (lock == 0)
    {
        lock = 1;
        s_mtx.unlock();
        return true;
    }
    else
    {
        s_mtx.unlock();
        return false;
    }

#endif
}

VX_API void spinlock::lock() noexcept
{
    while (!try_spinlock(m_lock))
    {
        thread_yield();
    }
}

// https://github.com/libsdl-org/SDL/blob/6b13d69105721ac54cd7224b0493065c57776c89/src/atomic/SDL_spinlock.c#L179

VX_API void spinlock::unlock() noexcept
{
#if defined(HAVE_GCC_ATOMICS) || defined(HAVE_GCC_SYNC_LOCK_TEST_AND_SET)

    __sync_lock_release(&m_lock);

#elif defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64))

    _InterlockedExchange_rel(reinterpret_cast<volatile long*>(&lock), 0);
}

#elif defined(_MSC_VER)

    InterlockedExchange(&m_lock, 0);

#else

    m_lock = 0;

#endif
}

} // namespace os
} // namespace vx