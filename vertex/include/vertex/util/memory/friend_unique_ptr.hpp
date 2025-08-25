#pragma once

#include <cstddef>
#include <utility>

namespace vx {
namespace mem {

template <typename T, typename U = T>
inline T exchange(T& obj, U&& new_value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}

template <typename T>
class friend_unique_ptr
{
public:

    using element_type = T;

    friend_unique_ptr() noexcept = default;
    ~friend_unique_ptr() { if (m_ptr) { delete m_ptr; } }

    explicit friend_unique_ptr(T* ptr) noexcept : m_ptr(ptr) {}

    friend_unique_ptr(const friend_unique_ptr&) = delete;
    friend_unique_ptr& operator=(const friend_unique_ptr&) = delete;

    friend_unique_ptr(friend_unique_ptr&& other) noexcept : m_ptr(exchange(other.m_ptr, nullptr)) {}

    friend_unique_ptr& operator=(friend_unique_ptr&& other) noexcept
    {
        reset(other.release());
        return *this;
    }

public:

    explicit operator bool() const noexcept { return m_ptr != nullptr; }

    T* get() const noexcept { return m_ptr; }
    T* operator->() const noexcept { return m_ptr; }
    T& operator*() const noexcept { return *m_ptr; }

    T* release() noexcept { return exchange(m_ptr, nullptr); }

    void reset(T* ptr = nullptr) noexcept
    {
        if (m_ptr != ptr)
        {
            delete m_ptr;
            m_ptr = ptr;
        }
    }

public:

    friend bool operator==(const friend_unique_ptr& p, std::nullptr_t) noexcept { return p.get() == nullptr; }
    friend bool operator!=(const friend_unique_ptr& p, std::nullptr_t) noexcept { return p.get() != nullptr; }

    friend bool operator==(std::nullptr_t, const friend_unique_ptr& p) noexcept { return p.get() == nullptr; }
    friend bool operator!=(std::nullptr_t, const friend_unique_ptr& p) noexcept { return p.get() != nullptr; }

public:

    friend bool operator==(const friend_unique_ptr& p, T* ptr) noexcept { return p.get() == ptr; }
    friend bool operator!=(const friend_unique_ptr& p, T* ptr) noexcept { return p.get() != ptr; }

    friend bool operator==(T* ptr, const friend_unique_ptr& p) noexcept { return p.get() == ptr; }
    friend bool operator!=(T* ptr, const friend_unique_ptr& p) noexcept { return p.get() != ptr; }

private:

    T* m_ptr = nullptr;
};

}
}