#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

template <typename T>
class unique_ptr
{
public:

    using element_type = T;

private:

    T* m_ptr;

public:

    unique_ptr() noexcept
        : m_ptr(nullptr)
    {
    }

    explicit unique_ptr(T* ptr) noexcept
        : m_ptr(ptr)
    {
    }

    ~unique_ptr()
    {
        reset();
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other) noexcept
        : m_ptr(other.m_ptr)
    {
        other.m_ptr = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }

    void reset(T* new_ptr = nullptr) noexcept
    {
        mem::destroy_safe(m_ptr);
        m_ptr = new_ptr;
    }

    T* release() noexcept
    {
        T* out = m_ptr;
        m_ptr = nullptr;
        return out;
    }

    T* get() noexcept
    {
        return m_ptr;
    }

    const T* get() const noexcept
    {
        return m_ptr;
    }

    T& operator*() noexcept
    {
        return *m_ptr;
    }

    const T& operator*() const noexcept
    {
        return *m_ptr;
    }

    T* operator->() noexcept
    {
        return m_ptr;
    }

    const T* operator->() const noexcept
    {
        return m_ptr;
    }

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }
};

template <typename T, typename... Args>
inline unique_ptr<T> make_unique(Args&&... args) noexcept
{
    T* p = mem::construct<T>(std::forward<Args>(args)...);
    return unique_ptr<T>(p);
}

} // namespace vx
