#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/config/os.hpp"
#include "vertex/os/thread.hpp"
#include "vertex/std/opaque_storage.hpp"

namespace vx {
namespace os {

//=============================================================================
// Mutex
//=============================================================================

struct mutex_impl_data;

class mutex
{
public:

    VX_API mutex() noexcept;
    VX_API ~mutex() noexcept;

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    VX_API bool lock() noexcept;
    VX_API bool try_lock() noexcept;
    VX_API void unlock() noexcept;

private:

#if defined(VX_HAVE_PTHREADS)

    // POSIX pthread_mutex_t is an opaque type whose size varies by platform and libc.
    // We use a conservative fixed buffer (64 bytes) to safely hold it.
    // Alignment uses the platform max alignment.
    static constexpr size_t storage_size = 64;
    static constexpr size_t storage_alignment = mem::max_align;

#elif defined(VX_OS_WINDOWS)

    // On Windows, a mutex consists of:
    //   - SRWLOCK: a wrapper around void*
    //   - DWORD  : a 32-bit unsigned integer holding the owning thread ID
    using example_storage = type_traits::composite_storage<void*, unsigned long>;
    static constexpr size_t storage_size = sizeof(example_storage);
    static constexpr size_t storage_alignment = alignof(example_storage);

#else

    // Unsupported or stub platform: no real storage needed.
    static constexpr size_t storage_size = 1;
    static constexpr size_t storage_alignment = 1;

#endif

    using storage_t = opaque_storage<storage_size, storage_alignment>;
    storage_t m_storage;
};

//=============================================================================
// Recursive Mutex
//=============================================================================

struct recursive_mutex_impl_data;

class recursive_mutex
{
public:

    VX_API recursive_mutex() noexcept;
    VX_API ~recursive_mutex() noexcept;

    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

    VX_API bool lock() noexcept;
    VX_API bool try_lock() noexcept;
    VX_API void unlock() noexcept;

private:

#if defined(VX_HAVE_PTHREAD_MUTEX_RECURSIVE)

    // POSIX we want to use the native recursive mutex if available
    static constexpr size_t storage_size = 64;
    static constexpr size_t storage_alignment = mem::max_align;

#else

    // Roll our own recursive mutex with size_t for count
    using example_storage = type_traits::composite_storage<mutex, size_t>;
    static constexpr size_t storage_size = sizeof(example_storage);
    static constexpr size_t storage_alignment = alignof(example_storage);

#endif

    using storage_t = opaque_storage<storage_size, storage_alignment>;
    storage_t m_storage;
};

//=============================================================================
// Lock Guard
//=============================================================================

template <typename mutex_t>
class lock_guard
{
public:

    using mutex_type = mutex_t;

    explicit lock_guard(mutex_type& mutex) noexcept
        : m_mutex(mutex)
    {
        m_mutex.lock();
    }
    ~lock_guard() noexcept
    {
        m_mutex.unlock();
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:

    mutex_type& m_mutex;
};

} // namespace os
} // namespace vx
