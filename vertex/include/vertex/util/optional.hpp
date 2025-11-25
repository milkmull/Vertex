#pragma once

#include <new>
#include <utility>
#include <type_traits>

#include "vertex/system/error.hpp"

namespace vx {

template <typename T>
class optional
{
public:

    //=============================================================================
    // constructors
    //=============================================================================

    optional() noexcept : m_engaged(false){}

    optional(const T& value)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
        : m_engaged(false)
    {
        emplace(value);
    }

    optional(T&& value)
        noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_engaged(false)
    {
        emplace(std::move(value));
    }

    optional(const optional& other)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
        : m_engaged(false)
    {
        if (other.m_engaged)
        {
            emplace(*other);
        }
    }

    optional(optional&& other)
        noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_engaged(false)
    {
        if (other.m_engaged)
        {
            emplace(std::move(*other));
            other.reset();
        }
    }

    optional& operator=(const optional& other)
        noexcept(
            std::is_nothrow_copy_assignable<T>::value &&
            std::is_nothrow_copy_constructible<T>::value
            )
    {
        if (this != &other)
        {
            if (other.m_engaged)
            {
                if (m_engaged)
                {
                    **this = *other;
                }
                else
                {
                    emplace(*other);
                }
            }
            else
            {
                reset();
            }
        }

        return *this;
    }

    optional& operator=(optional&& other)
        noexcept(
            std::is_nothrow_move_assignable<T>::value &&
            std::is_nothrow_move_constructible<T>::value
            )
    {
        if (this != &other)
        {
            if (other.m_engaged)
            {
                if (m_engaged)
                {
                    **this = std::move(*other);
                }
                else
                {
                    emplace(std::move(*other));
                }
                other.reset();
            }
            else
            {
                reset();
            }
        }

        return *this;
    }

    ~optional() { reset(); }

    //=============================================================================
    // edit
    //=============================================================================

    template <class... Args>
    void emplace(Args&&... args)
        noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
    {
        reset();
        new (&m_storage) T(std::forward<Args>(args)...);
        m_engaged = true;
    }

    void reset() noexcept
    {
        if (m_engaged)
        {
            reinterpret_cast<T*>(&m_storage)->~T();
            m_engaged = false;
        }
    }

    //=============================================================================
    // check
    //=============================================================================

    bool has_value() const noexcept { return m_engaged; }
    explicit operator bool() const noexcept { return m_engaged; }

    //=============================================================================
    // value
    //=============================================================================

    T& value()
    {
        if (!m_engaged) err::set(err::runtime_error, "bad optional access");
        return **this;
    }

    const T& value() const
    {
        if (!m_engaged) err::set(err::runtime_error, "bad optional access");
        return **this;
    }


    //=============================================================================
    // operators
    //=============================================================================

    T& operator*() noexcept
    {
        return *reinterpret_cast<T*>(&m_storage);
    }

    const T& operator*() const noexcept
    {
        return *reinterpret_cast<const T*>(&m_storage);
    }

    T* operator->() noexcept
    {
        return &**this;
    }

    const T* operator->() const noexcept
    {
        return &**this;
    }

    //=============================================================================

private:

    typename std::aligned_storage<sizeof(T), alignof(T)>::type m_storage;
    bool m_engaged;
};

} // namespace vx