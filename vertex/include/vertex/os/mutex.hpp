#pragma once

#include "vertex/system/compiler.hpp"
#include "vertex/os/thread.hpp"

namespace vx {
namespace os {

namespace __detail {

///////////////////////////////////////////////////////////////////////////////
// Predefines
///////////////////////////////////////////////////////////////////////////////

class mutex_impl;

} // namespace __detail

class recursive_mutex;

///////////////////////////////////////////////////////////////////////////////
// Mutex
///////////////////////////////////////////////////////////////////////////////

class mutex
{
    friend __detail::mutex_impl;
    friend recursive_mutex;

public:

    VX_API mutex() noexcept;

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

public:

    VX_API bool lock() noexcept;
    VX_API bool try_lock() noexcept;
    VX_API void unlock() noexcept;

private:

    ///////////////////////////////////////////////////////////////////////////////
    // Platform specific Data
    ///////////////////////////////////////////////////////////////////////////////

#if defined(VX_PLATFORM_WINDOWS)

    // https://github.com/microsoft/STL/blob/1f6e5b16ec02216665624c1e762f3732605cf2b4/stl/inc/__msvc_threads_core.hpp#L35
    // https://github.com/libsdl-org/SDL/blob/476e7e54cb24774919537d963140eb19cb66ac9d/src/thread/windows/SDL_sysmutex_c.h

    struct critical_section_t
    {
        void* lock = nullptr;
    };

#endif // VX_PLATFORM_WINDOWS

    struct impl_data
    {
        bool recursive = false;
        size_t count = 0;
        thread::id thread_id = 0;

#if defined(VX_PLATFORM_WINDOWS)

        critical_section_t critical_section;

#endif // VX_PLATFORM_WINDOWS
    };

private:

    impl_data m_impl_data;
};

///////////////////////////////////////////////////////////////////////////////
// Recursive Mutex
///////////////////////////////////////////////////////////////////////////////

class recursive_mutex
{
    friend __detail::mutex_impl;

public:

    VX_API recursive_mutex() noexcept;

    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

public:

    VX_API bool lock() noexcept;
    VX_API bool try_lock() noexcept;
    VX_API void unlock() noexcept;

private:

    mutex::impl_data m_impl_data;
};

///////////////////////////////////////////////////////////////////////////////
// Lock Guard
///////////////////////////////////////////////////////////////////////////////

template <typename mutex_t>
class lock_guard
{
public:

    using mutex_type = mutex_t;

    explicit lock_guard(mutex_type& mutex) noexcept : m_mutex(mutex) { m_mutex.lock(); }
    ~lock_guard() noexcept { m_mutex.unlock(); }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;

private:

    mutex_type& m_mutex;
};

} // namespace os
} // namespace vx
