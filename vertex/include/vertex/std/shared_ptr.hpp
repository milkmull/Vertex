#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/os/atomic.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/aligned_storage.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

#if !defined(VX_STD_DISABLE_WEAK_PTR)
    #define VX_STD_DISABLE_WEAK_PTR 0
#endif

#define VX_STD_WEAK_PTR_ENABLED (!VX_STD_DISABLE_WEAK_PTR)

namespace vx {

#if VX_STD_WEAK_PTR_ENABLED

template <typename T>
class weak_ptr;

#endif // VX_STD_WEAK_PTR_ENABLED

template <typename T>
class shared_ptr;

template <typename T, typename... Args>
inline expected<shared_ptr<T>, error> make_shared(Args&&... args) noexcept;

namespace _shared_ptr_priv {

//=========================================================================
// control block base
//=========================================================================

class sp_counter_base
{
public:

    sp_counter_base() noexcept
        : m_strong(1)
#if VX_STD_WEAK_PTR_ENABLED
        , m_weak(1)
#endif // VX_STD_WEAK_PTR_ENABLED
    {
    }

    sp_counter_base(const sp_counter_base&) = delete;
    sp_counter_base& operator=(const sp_counter_base&) = delete;

    virtual ~sp_counter_base() = default;

    void inc_strong() noexcept
    {
        m_strong.fetch_add(1, os::memory_order_relaxed);
    }

#if VX_STD_WEAK_PTR_ENABLED

    // Attempts to add a strong reference, but only if the object is
    // still alive (strong count != 0). Used by weak_ptr::lock(), which
    // must not resurrect an already-destroyed object.
    bool inc_strong_if_nonzero() noexcept
    {
        size_t count = m_strong.load(os::memory_order_relaxed);

        while (count != 0)
        {
            if (m_strong.compare_exchange_weak(
                    count, count + 1,
                    os::memory_order_relaxed,
                    os::memory_order_relaxed))
            {
                return true;
            }
        }

        return false;
    }

#endif // VX_STD_WEAK_PTR_ENABLED

    void dec_strong() noexcept
    {
        if (m_strong.fetch_sub(1, os::memory_order_acq_rel) == 1)
        {
            destroy_object();
#if VX_STD_WEAK_PTR_ENABLED
            dec_weak();
#else
            // No weak count to release: the control block's storage
            // can be freed immediately.
            destroy_this();
#endif // VX_STD_WEAK_PTR_ENABLED
        }
    }

#if VX_STD_WEAK_PTR_ENABLED

    void inc_weak() noexcept
    {
        m_weak.fetch_add(1, os::memory_order_relaxed);
    }

    void dec_weak() noexcept
    {
        if (m_weak.fetch_sub(1, os::memory_order_acq_rel) == 1)
        {
            destroy_this();
        }
    }

#endif // VX_STD_WEAK_PTR_ENABLED

    size_t use_count() const noexcept
    {
        return m_strong.load(os::memory_order_relaxed);
    }

protected:

    // Destroys the managed object. Called exactly once, when the strong
    // count reaches 0. Must NOT free the control block's own storage.
    virtual void destroy_object() noexcept = 0;

    // Frees the control block's own storage. Called exactly once: right
    // after destroy_object() if weak_ptr support is disabled, or once
    // the weak count also reaches 0 if it's enabled.
    virtual void destroy_this() noexcept = 0;

private:

    os::atomic<size_t> m_strong;
#if VX_STD_WEAK_PTR_ENABLED
    os::atomic<size_t> m_weak;
#endif // VX_STD_WEAK_PTR_ENABLED
};

//=========================================================================
// control block: separately allocated object + deleter
//=========================================================================

// Used by shared_ptr(ptr) / shared_ptr(ptr, deleter): the object was
// allocated separately (or isn't owned via mem::construct at all), so
// this block just remembers a pointer + deleter and never touches the
// object's storage itself.
template <typename T, typename Deleter>
class sp_counter_ptr final : public sp_counter_base
{
public:

    sp_counter_ptr(T* ptr, const Deleter& deleter) noexcept
        : m_storage(one_then_variadic_args_tag{}, deleter, ptr)
    {
    }

    sp_counter_ptr(T* ptr, Deleter&& deleter) noexcept
        : m_storage(one_then_variadic_args_tag{}, std::move(deleter), ptr)
    {
    }

protected:

    void destroy_object() noexcept override
    {
        T* ptr = m_storage.second;
        if (ptr)
        {
            m_storage.first()(ptr);
            m_storage.second = nullptr;
        }
    }

    void destroy_this() noexcept override
    {
        mem::destroy(this);
    }

private:

    compressed_pair<Deleter, T*> m_storage;
};

//=========================================================================
// control block: object co-allocated with the control block
//=========================================================================

// Used by make_shared<T>(...): the object is stored directly inside the
// control block, so there is a single allocation for both the
// ref-counts and the object.
template <typename T>
class sp_counter_obj final : public sp_counter_base
{
public:

    template <typename... Args>
    explicit sp_counter_obj(Args&&... args)
    {
        m_storage.template construct<T>(std::forward<Args>(args)...);
    }

    T* get() noexcept
    {
        return m_storage.template ptr<T>();
    }

protected:

    void destroy_object() noexcept override
    {
        m_storage.template destroy<T>();
    }

    void destroy_this() noexcept override
    {
        mem::destroy(this);
    }

private:

    aligned_storage<sizeof(T), alignof(T)> m_storage;
};

} // namespace _shared_ptr_priv

//=========================================================================
// shared_ptr
//=========================================================================

template <typename T>
class shared_ptr
{
public:

    using element_type = T;

private:

    T* m_ptr;
    _shared_ptr_priv::sp_counter_base* m_ctrl;

    template <typename U>
    friend class shared_ptr;

#if VX_STD_WEAK_PTR_ENABLED

    template <typename U>
    friend class weak_ptr;

#endif // VX_STD_WEAK_PTR_ENABLED

    template <typename U, typename... Args>
    friend expected<shared_ptr<U>, error> make_shared(Args&&... args) noexcept;

    // Used internally by make_shared / weak_ptr::lock() / the pointer
    // casts below, where the control block already exists and its
    // strong count has already been accounted for by the caller.
    shared_ptr(_shared_ptr_priv::sp_counter_base* ctrl, T* ptr) noexcept
        : m_ptr(ptr), m_ctrl(ctrl)
    {
    }

public:

    //---------------------------------------------------------------
    // construction
    //---------------------------------------------------------------

    constexpr shared_ptr() noexcept
        : m_ptr(nullptr), m_ctrl(nullptr)
    {
    }

    constexpr shared_ptr(std::nullptr_t) noexcept
        : shared_ptr()
    {
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    explicit shared_ptr(U* ptr) noexcept
        : m_ptr(ptr), m_ctrl(nullptr)
    {
        init(ptr, default_delete<U>());
    }

    template <
        typename U,
        typename Deleter,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    shared_ptr(U* ptr, Deleter deleter) noexcept
        : m_ptr(ptr), m_ctrl(nullptr)
    {
        init(ptr, std::move(deleter));
    }

    template <typename U>
    shared_ptr(const shared_ptr<U>& other, T* aliased_ptr) noexcept
        : m_ptr(aliased_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_strong();
        }
    }

    shared_ptr(const shared_ptr& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_strong();
        }
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    shared_ptr(const shared_ptr<U>& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        if (m_ctrl)
        {
            m_ctrl->inc_strong();
        }
    }

    shared_ptr(shared_ptr&& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        other.m_ptr = nullptr;
        other.m_ctrl = nullptr;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    shared_ptr(shared_ptr<U>&& other) noexcept
        : m_ptr(other.m_ptr), m_ctrl(other.m_ctrl)
    {
        other.m_ptr = nullptr;
        other.m_ctrl = nullptr;
    }

    ~shared_ptr()
    {
        reset();
    }

    //---------------------------------------------------------------
    // assignment
    //---------------------------------------------------------------

    shared_ptr& operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    shared_ptr& operator=(const shared_ptr& other) noexcept
    {
        shared_ptr(other).swap(*this);
        return *this;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    shared_ptr& operator=(const shared_ptr<U>& other) noexcept
    {
        shared_ptr(other).swap(*this);
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept
    {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    shared_ptr& operator=(shared_ptr<U>&& other) noexcept
    {
        shared_ptr(std::move(other)).swap(*this);
        return *this;
    }

    //---------------------------------------------------------------
    // modifiers
    //---------------------------------------------------------------

    void reset() noexcept
    {
        if (m_ctrl)
        {
            m_ctrl->dec_strong();
        }
        m_ptr = nullptr;
        m_ctrl = nullptr;
    }

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    void reset(U* ptr) noexcept
    {
        shared_ptr(ptr).swap(*this);
    }

    template <
        typename U,
        typename Deleter,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    void reset(U* ptr, Deleter deleter) noexcept
    {
        shared_ptr(ptr, std::move(deleter)).swap(*this);
    }

    void swap(shared_ptr& other) noexcept
    {
        vx::swap(m_ptr, other.m_ptr);
        vx::swap(m_ctrl, other.m_ctrl);
    }

    //---------------------------------------------------------------
    // observers
    //---------------------------------------------------------------

    T* get() const noexcept
    {
        return m_ptr;
    }

    T& operator*() const noexcept
    {
        VX_ASSERT(m_ptr != nullptr);
        return *m_ptr;
    }

    T* operator->() const noexcept
    {
        VX_ASSERT(m_ptr != nullptr);
        return m_ptr;
    }

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
    }

    size_t use_count() const noexcept
    {
        return m_ctrl ? m_ctrl->use_count() : 0;
    }

    bool unique() const noexcept
    {
        return use_count() == 1;
    }

private:

    template <typename U, typename Deleter>
    void init(U* ptr, Deleter deleter) noexcept
    {
        if (!ptr)
        {
            return;
        }

        m_ctrl = mem::construct<_shared_ptr_priv::sp_counter_ptr<U, Deleter>>(ptr, std::move(deleter));
        VX_VERIFY_CODE(m_ctrl != nullptr, err::out_of_memory);
    }
};

//=========================================================================
// make_shared
//=========================================================================

template <typename T, typename... Args>
inline expected<shared_ptr<T>, error> make_shared(Args&&... args) noexcept
{
    using ctrl_type = _shared_ptr_priv::sp_counter_obj<T>;

    ctrl_type* ctrl = mem::construct<ctrl_type>(std::forward<Args>(args)...);
    VX_RET_UNEXPECTED_ERR_IF(!ctrl, err::out_of_memory);

    return shared_ptr<T>(ctrl, ctrl->get());
}

//=========================================================================
// casts
//=========================================================================

template <typename T, typename U>
inline shared_ptr<T> static_pointer_cast(const shared_ptr<U>& other) noexcept
{
    return shared_ptr<T>(other, static_cast<T*>(other.get()));
}

template <typename T, typename U>
inline shared_ptr<T> const_pointer_cast(const shared_ptr<U>& other) noexcept
{
    return shared_ptr<T>(other, const_cast<T*>(other.get()));
}

//=========================================================================
// comparisons
//=========================================================================

template <typename T, typename U>
inline bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept
{
    return lhs.get() == rhs.get();
}

template <typename T, typename U>
inline bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) noexcept
{
    return lhs.get() != rhs.get();
}

template <typename T>
inline bool operator==(const shared_ptr<T>& lhs, std::nullptr_t) noexcept
{
    return !lhs;
}

template <typename T>
inline bool operator==(std::nullptr_t, const shared_ptr<T>& rhs) noexcept
{
    return !rhs;
}

template <typename T>
inline bool operator!=(const shared_ptr<T>& lhs, std::nullptr_t) noexcept
{
    return static_cast<bool>(lhs);
}

template <typename T>
inline bool operator!=(std::nullptr_t, const shared_ptr<T>& rhs) noexcept
{
    return static_cast<bool>(rhs);
}

//=========================================================================
// non-member swap
//=========================================================================

template <typename T>
inline void swap(shared_ptr<T>& lhs, shared_ptr<T>& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace vx
