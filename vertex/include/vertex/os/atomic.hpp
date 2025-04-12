#pragma once

#include "vertex/config/architecture.hpp"
#include "vertex/config/language_config.hpp"

#include "vertex/os/mutex.hpp"

//#define VX_OS_DUMMY

// https://github.com/libsdl-org/SDL/blob/b8381b3a2ea9873c6baedb8586da8bb5a2775d24/CMakeLists.txt#L580
// https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/atomic#L464

///////////////////////////////////////////////////////////////////////////////
// Configuration
///////////////////////////////////////////////////////////////////////////////

 // https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/platform.hpp

#if !defined(VX_OS_DUMMY)

#if defined(_MSC_VER) && (_MSC_VER >= 1400)

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/core_ops_windows.hpp
#   define __VX_ATOMIC_BACKEND_WINDOWS

#   include <intrin.h>

#   if defined(__INTEL_COMPILER)
#       define __VX_ATOMIC_COMPILER_BARRIER() __memory_barrier()
#   elif defined(__clang__) && VX_HAS_BUILTIN(__atomic_signal_fence)
#       define __VX_ATOMIC_COMPILER_BARRIER() __atomic_signal_fence(__ATOMIC_SEQ_CST)
#   elif defined(__VX_ATOMIC_BACKEND_WINDOWS)
#       define __VX_ATOMIC_COMPILER_BARRIER() _ReadWriteBarrier()
#   else
#       define __VX_ATOMIC_COMPILER_BARRIER()
#   endif

// IBM XL C++ Compiler has to be checked before GCC/Clang as it pretends to be one but does not support __atomic* intrinsics.
// It does support GCC inline assembler though.
#elif !(defined(__ibmxl__) || defined(__IBMCPP__)) && \
    ((defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 407)) || \
    (defined(BOOST_CLANG) && ((__clang_major__ * 100 + __clang_minor__) >= 302))) && \
    ( \
        (__GCC_ATOMIC_BOOL_LOCK_FREE == 2) || \
        (__GCC_ATOMIC_CHAR_LOCK_FREE == 2) || \
        (__GCC_ATOMIC_SHORT_LOCK_FREE == 2) || \
        (__GCC_ATOMIC_INT_LOCK_FREE == 2) || \
        (__GCC_ATOMIC_LONG_LOCK_FREE == 2) || \
        (__GCC_ATOMIC_LLONG_LOCK_FREE == 2) \
    )

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/core_ops_gcc_atomic.hpp
#   define __VX_ATOMIC_BACKEND_GCC_ATOMIC

#elif defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 401) && \
    ( \
        defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1) || \
        defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2) || \
        defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) || \
        defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8) || \
        defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) \
    )

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/core_ops_gcc_sync.hpp
#   define __VX_ATOMIC_BACKEND_GCC_SYNC

#else

// https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/core_operations_emulated.hpp
#   define __VX_ATOMIC_BACKEND_GENERIC

#endif

#else // VX_OS_DUMMY

#   define __VX_ATOMIC_BACKEND_GENERIC

#endif // VX_OS_DUMMY

#if defined(__VX_ATOMIC_BACKEND_GENERIC)
#   define __VX_ATOMIC_LOCK_GUARD(storage) lock_pool::scoped_lock<storage_alignment> lock(&storage)
#endif

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// storage traits
///////////////////////////////////////////////////////////////////////////////

template <size_t size>
struct storage_traits {};

template <>
struct storage_traits<1>
{
    using type = uint8_t;
    static constexpr size_t native_alignment = 1;
    static constexpr size_t alignment = 1;
};

template <>
struct storage_traits<2>
{
    using type = uint16_t;
    static constexpr size_t native_alignment = 2;
    static constexpr size_t alignment = 2;
};

template <>
struct storage_traits<4>
{
    using type = uint32_t;
    static constexpr size_t native_alignment = 4;
    static constexpr size_t alignment = 4;
};

template <>
struct storage_traits<8>
{
    using type = uint64_t;
    static constexpr size_t native_alignment = 8;
    static constexpr size_t alignment = 8;
};

///////////////////////////////////////////////////////////////////////////////
// lock pool
///////////////////////////////////////////////////////////////////////////////

#if defined(__VX_ATOMIC_BACKEND_GENERIC)

struct lock_pool
{
    static constexpr size_t s_size = 16;
    static mutex s_pool[s_size];

    // https://github.com/boostorg/atomic/blob/develop/include/boost/atomic/detail/pause.hpp

    static VX_FORCE_INLINE void pause() noexcept
    {
#if defined(_MSC_VER)

#   if defined(_M_AMD64) || defined(_M_IX86)

        _mm_pause();

#   elif defined(_M_ARM64) || defined(_M_ARM)

        __yield();

#   endif

#elif defined(__GNUC__)

#   if defined(__i386__) || defined(__x86_64__)

        __asm__ __volatile__("pause;" : : : "memory");

#   elif (defined(__ARM_ARCH) && __ARM_ARCH >= 8) || defined(__ARM_ARCH_8A__) || defined(__aarch64__)

        __asm__ __volatile__("yield;" : : : "memory");

#   elif defined(__riscv) && (__riscv_xlen == 64)

#       if defined(__riscv_zihintpause)

        __asm__ __volatile__("pause" : : : "memory");

#       else

        /* Encoding of the pause instruction */
        __asm__ __volatile__(".4byte 0x100000F");

#       endif

#   endif

#endif
    }

    static VX_FORCE_INLINE void spinlock(mutex& mutex) noexcept
    {
        for (int i = 0; i < 10; ++i)
        {
            if (VX_LIKELY(mutex.try_lock()))
            {
                return;
            }

            pause();
        }

        mutex.lock();
    }

    // https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/lock_pool.hpp#L72

    template <size_t alignment>
    static VX_FORCE_INLINE uintptr_t hash_ptr(const volatile void* void_ptr) noexcept
    {
        uintptr_t ptr = static_cast<uintptr_t>(void_ptr);
        uintptr_t h = ptr / alignment;

        // Since many malloc/new implementations return pointers with higher alignment
        // than indicated by Alignment, it makes sense to mix higher bits
        // into the lower ones. On 64-bit platforms, malloc typically aligns to 16 bytes,
        // on 32-bit - to 8 bytes.
        constexpr size_t malloc_alignment = (sizeof(void*) >= 8) ? 16 : 8;

        VX_IF_CONSTEXPR(alignment != malloc_alignment)
        {
            h ^= ptr / malloc_alignment;
        }

        return h;
    }

    template <size_t alignment>
    class scoped_lock
    {
    public:

        VX_FORCE_INLINE explicit scoped_lock(const volatile void* addr) noexcept
        {
            const uintptr_t h = hash_ptr<alignment>(addr);
            const size_t index = h & (s_size - 1);
            m_mutex = s_pool[index];
            spinlock(m_mutex);
        }

        ~scoped_lock() noexcept { m_mutex.unlock(); }

        scoped_lock(const scoped_lock&) = delete;
        scoped_lock& operator=(const scoped_lock&) = delete;

    private:

        mutex& m_mutex;
    };
};

#endif // __VX_ATOMIC_BACKEND_GENERIC

///////////////////////////////////////////////////////////////////////////////
// fence
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)

#   if defined(__INTEL_COMPILER)

#       define fence_before() __memory_barrier()

#   elif defined(__clang__) && VX_HAS_BUILTIN(__atomic_signal_fence)

#       define fence_before() __atomic_signal_fence(__ATOMIC_SEQ_CST)

#   elif defined(__VX_ATOMIC_BACKEND_WINDOWS)

#       define fence_before() _ReadWriteBarrier()

#   endif

#elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

#   define fence_before()

#elif defined(__VX_ATOMIC_BACKEND_GENERIC)

#   define fence_before() lock_guard(lock_pool::get_lock(&storage))

#endif

///////////////////////////////////////////////////////////////////////////////
// atomic base
///////////////////////////////////////////////////////////////////////////////

template <size_t size, bool _signed>
class atomic_base
{
private:

    using storage_type = typename storage_traits<size>::type;
    using signed_storage_type = typename std::make_signed<storage_type>::type;
    
    static constexpr size_t storage_size = size;
    static constexpr size_t storage_alignment = storage_traits<size>::alignment;
    static constexpr size_t is_signed = _signed;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // store
    ///////////////////////////////////////////////////////////////////////////////

#if defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

    // https://github.com/boostorg/atomic/blob/199906f4e1789d8d182d478842ea4df9543d5fea/include/boost/atomic/detail/core_ops_gcc_sync.hpp#L72
    using plain_stores_loads_are_atomic = std::integral_constant<bool, storage_size <= sizeof(void*)>::type;

    static VX_FORCE_INLINE void store(volatile storage_type& storage, storage_type v, std::true_type) noexcept
    {
        storage = v;
        __sync_synchronize();
    }

    static VX_FORCE_INLINE void store(volatile storage_type& storage, storage_type v, std::false_type) noexcept
    {
        exchange(storage, v);
    }

#endif

    static VX_FORCE_INLINE void store(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        exchange(storage, v);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        __atomic_store_n(&storage, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        store(storage, v, plain_stores_loads_are_atomic());

#   else

        __VX_ATOMIC_LOCK_GUARD(storage);
        const_cast<storage_type&>(storage) = v;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // load
    ///////////////////////////////////////////////////////////////////////////////

#if defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

    static VX_FORCE_INLINE storage_type load(const volatile storage_type& storage, std::true_type) noexcept
    {
        storage_type v = storage;
        fence_after();
        return v;
    }

    static VX_FORCE_INLINE storage_type load(const volatile storage_type& storage, std::false_type) noexcept
    {
        // Note: don't use fetch_add or other arithmetics here since storage_type may not be an arithmetic type.
        storage_type expected = storage_type();
        storage_type desired = expected;
        // We don't care if CAS succeeds or not. If it does, it will just write the same value there was before.
        return __sync_val_compare_and_swap(const_cast<volatile storage_type*>(&storage), expected, desired);
    }

#endif

    static VX_FORCE_INLINE storage_type load(const volatile storage_type& storage) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        return fetch_add(const_cast<volatile storage_type&>(storage), static_cast<storage_type>(0));

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_load_n(&storage, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return load(storage, v, plain_stores_loads_are_atomic());

#   else

        __VX_ATOMIC_LOCK_GUARD(storage);
        return const_cast<const storage_type&>(storage);

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // fetch add
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type fetch_add(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        __VX_ATOMIC_COMPILER_BARRIER();
        v = static_cast<storage_type>(_InterlockedExchangeAdd(static_cast<volatile long*>(&storage), static_cast<long>(v)));
        __VX_ATOMIC_COMPILER_BARRIER();
        return v;

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_fetch_add(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_fetch_and_add(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s += v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // fetch sub
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type fetch_sub(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        return fetch_add(storage, static_cast<storage_type>(-static_cast<signed_storage_type>(v)));

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_fetch_sub(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_fetch_and_sub(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s -= v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // exchange
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type exchange(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        __VX_ATOMIC_COMPILER_BARRIER();
        v = static_cast<storage_type>(_InterlockedExchange(static_cast<volatile long*>(&storage), static_cast<long>(v)));
        __VX_ATOMIC_COMPILER_BARRIER();
        return v;

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_exchange_n(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_lock_test_and_set(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s = v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // compare exchange strong
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type compare_exchange_strong(
        volatile storage_type& storage,
        storage_type& expected,
        storage_type desired
    ) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        storage_type previous = expected;
        __VX_ATOMIC_COMPILER_BARRIER();
        storage_type old = static_cast<storage_type>(_InterlockedCompareExchange(
            static_cast<volatile long*>(&storage), static_cast<long>(desired), static_cast<long>(previous))
        );
        expected = old;
        __VX_ATOMIC_COMPILER_BARRIER();
        return (previous == old);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_compare_exchange_n(&storage, &expected, desired, false __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        storage_type expected2 = expected;
        storage_type old = __sync_val_compare_and_swap(&storage, expected2, desired);

        if (old == expected2)
        {
            return true;
        }

        expected = old;
        return false;

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        const bool res = old == expected;
        if (res)
        {
            s = desired;
        }
        expected = old;
        return res;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // compare exchange weak
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type compare_exchange_weak(
        volatile storage_type& storage,
        storage_type& expected,
        storage_type desired
    ) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        return compare_exchange_strong(storage, expected, desired);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_compare_exchange_n(&storage, &expected, desired, true __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return compare_exchange_strong(storage, expected, desired);

#   else

        return compare_exchange_strong(storage, expected, desired);

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // fetch and
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type fetch_and(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        __VX_ATOMIC_COMPILER_BARRIER();
        v = static_cast<storage_type>(_InterlockedAnd(static_cast<volatile long*>(&storage), static_cast<long>(v)));
        __VX_ATOMIC_COMPILER_BARRIER();
        return v;

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_fetch_and(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_fetch_and_and(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s &= v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // fetch or
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type fetch_or(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        __VX_ATOMIC_COMPILER_BARRIER();
        v = static_cast<storage_type>(_InterlockedOr(static_cast<volatile long*>(&storage), static_cast<long>(v)));
        __VX_ATOMIC_COMPILER_BARRIER();
        return v;

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_fetch_or(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_fetch_and_or(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s |= v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // fetch xor
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE storage_type fetch_xor(volatile storage_type& storage, storage_type v) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        __VX_ATOMIC_COMPILER_BARRIER();
        v = static_cast<storage_type>(_InterlockedXor(static_cast<volatile long*>(&storage), static_cast<long>(v)));
        __VX_ATOMIC_COMPILER_BARRIER();
        return v;

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_fetch_xor(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return __sync_fetch_and_xor(&storage, v);

#   else

        storage_type& s = const_cast<storage_type&>(storage);
        __VX_ATOMIC_LOCK_GUARD(storage);
        storage_type old = s;
        s ^= v;
        return old;

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // test and set
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE bool test_and_set(volatile storage_type& storage) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        return !!compare_exchange_strong(storage, static_cast<storage_type>(1));

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        return __atomic_test_and_set(&storage, v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        return !!__sync_lock_test_and_set(&storage, 1);

#   else

        return !!exchange(storage, static_cast<storage_type>(1));

#   endif
    }

    ///////////////////////////////////////////////////////////////////////////////
    // clear
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE void clear(volatile storage_type& storage) noexcept
    {
#   if defined(__VX_ATOMIC_BACKEND_WINDOWS)

        store(storage, static_cast<storage_type>(0));

#   elif defined(__VX_ATOMIC_BACKEND_GCC_ATOMIC)

        __atomic_clear(const_cast<storage_type*>(&storage), v, __ATOMIC_SEQ_CST);

#   elif defined(__VX_ATOMIC_BACKEND_GCC_SYNC)

        __sync_lock_release(&storage);
        __sync_synchronize();

#   else

        store(storage, static_cast<storage_type>(0));

#   endif
    }
};

}
}