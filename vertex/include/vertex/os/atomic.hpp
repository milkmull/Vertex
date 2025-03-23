#pragma once

#include <atomic>

#include "vertex/system/compiler.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Spin Lock
///////////////////////////////////////////////////////////////////////////////

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

public:

    VX_API void lock() noexcept;
    VX_API void unlock() noexcept;
    bool is_locked() const noexcept { return m_lock == 1; }

private:

    lock_type m_lock = 0;
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