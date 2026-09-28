#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/shared_ptr.hpp"

namespace vx {

//=========================================================================
// weak_ptr
//=========================================================================

#if VX_STD_WEAK_PTR_ENABLED

template <typename T>
class weak_ptr
{
public:

    using element_type = T;

private:

    T* m_ptr;
    _shared_ptr_priv::sp_counter_base* m_ctrl;

    template <typename U>
    friend class weak_ptr;

public:

    constexpr weak_ptr() noexcept
        : m_ptr(nullptr), m_ctrl(nullptr)
    {
    }

    weak_ptr(const weak_ptr& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_weak();
        }
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    weak_ptr(const weak_ptr<U>& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_weak();
        }
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    weak_ptr(const shared_ptr<U>& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_weak();
        }
    }

    weak_ptr(weak_ptr&& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        other.m_ptr = nullptr;
        other.m_ctrl = nullptr;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    weak_ptr(weak_ptr<U>&& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        other.m_ptr = nullptr;
        other.m_ctrl = nullptr;
    }

    ~weak_ptr()
    {
        reset();
    }

    weak_ptr& operator=(const weak_ptr& other) noexcept
    {
        weak_ptr(other).swap(*this);
        return *this;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    weak_ptr& operator=(const weak_ptr<U>& other) noexcept
    {
        weak_ptr(other).swap(*this);
        return *this;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    weak_ptr& operator=(const shared_ptr<U>& other) noexcept
    {
        weak_ptr(other).swap(*this);
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& other) noexcept
    {
        weak_ptr(std::move(other)).swap(*this);
        return *this;
    }

    void reset() noexcept
    {
        if (m_ctrl)
        {
            m_ctrl->dec_weak();
        }
        m_ptr = nullptr;
        m_ctrl = nullptr;
    }

    void swap(weak_ptr& other) noexcept
    {
        vx::swap(m_ptr, other.m_ptr);
        vx::swap(m_ctrl, other.m_ctrl);
    }

    size_t use_count() const noexcept
    {
        return m_ctrl ? m_ctrl->use_count() : 0;
    }

    bool expired() const noexcept
    {
        return use_count() == 0;
    }

    shared_ptr<T> lock() const noexcept
    {
        if (m_ctrl && m_ctrl->inc_strong_if_nonzero())
        {
            return shared_ptr<T>(m_ctrl, m_ptr);
        }
        return shared_ptr<T>();
    }
};

//=========================================================================
// non-member swap
//=========================================================================

template <typename T>
inline void swap(weak_ptr<T>& lhs, weak_ptr<T>& rhs) noexcept
{
    lhs.swap(rhs);
}

#endif // VX_STD_WEAK_PTR_ENABLED

} // namespace vx
