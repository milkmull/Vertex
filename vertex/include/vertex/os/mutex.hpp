#pragma once

#include <mutex>

#include "vertex/config/os.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/os/thread.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Mutex
///////////////////////////////////////////////////////////////////////////////

class mutex
{
public:

    mutex() = default;

    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

public:

    bool lock() noexcept { m_mutex.lock(); return true; }
    bool try_lock() noexcept { return m_mutex.try_lock(); }

    void unlock() noexcept
    {
        // avoid windows complaining about unlock without lock
        VX_DISABLE_MSVC_WARNING_PUSH();
        VX_DISABLE_MSVC_WARNING(26110);
        m_mutex.unlock();
        VX_DISABLE_CLANG_WARNING_POP();
    }

private:

    std::mutex m_mutex;
};

///////////////////////////////////////////////////////////////////////////////
// Recursive Mutex
///////////////////////////////////////////////////////////////////////////////

class recursive_mutex
{
public:

    recursive_mutex() noexcept = default;

    recursive_mutex(const recursive_mutex&) = delete;
    recursive_mutex& operator=(const recursive_mutex&) = delete;

public:

    bool lock() noexcept { m_mutex.lock(); return true; }
    bool try_lock() noexcept { return m_mutex.try_lock(); }

    void unlock() noexcept
    {
        // avoid windows complaining about unlock without lock
        VX_DISABLE_MSVC_WARNING_PUSH();
        VX_DISABLE_MSVC_WARNING(26110);
        m_mutex.unlock();
        VX_DISABLE_CLANG_WARNING_POP();
    }

private:

    std::recursive_mutex m_mutex;
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
