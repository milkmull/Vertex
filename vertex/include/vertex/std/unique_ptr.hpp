#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/_tools/compressed_pair.hpp"
#include "vertex/std/expected.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

//=========================================================================
// default deleters
//=========================================================================

template <typename T>
struct default_delete
{
    constexpr default_delete() noexcept = default;

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U*, T*>::value)>
    default_delete(const default_delete<U>&) noexcept
    {}

    void operator()(T* ptr) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(T) > 0, "can't delete pointer to incomplete type");
        if (ptr)
        {
            mem::destroy(ptr);
        }
    }
};

template <typename T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;

    template <
        typename U,
        VX_REQUIRES(std::is_convertible<U (*)[], T (*)[]>::value)>
    default_delete(const default_delete<U[]>&) noexcept
    {}

    void operator()(T* ptr, size_t count) const noexcept
    {
        VX_STATIC_ASSERT_MSG(sizeof(T) > 0, "can't delete pointer to incomplete type");
        if (ptr && count)
        {
            mem::destroy_array(ptr, count);
        }
    }
};

//=========================================================================
// unique_ptr (single object)
//=========================================================================

template <typename T, typename Deleter = default_delete<T>>
class unique_ptr
{
public:

    using element_type = T;
    using deleter_type = Deleter;

private:

    using storage_type = _priv::compressed_pair<Deleter, T*>;
    storage_type m_storage;

public:

    constexpr unique_ptr() noexcept
        : m_storage(_priv::zero_then_variadic_args_tag{}, nullptr)
    {
    }

    explicit unique_ptr(T* ptr) noexcept
        : m_storage(_priv::zero_then_variadic_args_tag{}, ptr)
    {
    }

    unique_ptr(T* ptr, const Deleter& deleter) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, deleter, ptr)
    {
    }

    unique_ptr(T* ptr, Deleter&& deleter) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, std::move(deleter), ptr)
    {
    }

    ~unique_ptr()
    {
        reset();
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other) noexcept
        : m_storage(
              _priv::one_then_variadic_args_tag{},
              std::move(other.get_deleter()),
              other.m_storage.second)
    {
        other.m_storage.second = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
        if (this != &other)
        {
            reset(other.m_storage.second);
            get_deleter() = std::move(other.get_deleter());
            other.m_storage.second = nullptr;
        }
        return *this;
    }

    template <
        typename U,
        typename E,
        VX_REQUIRES(std::is_convertible<U*, T*>::value && std::is_convertible<E, Deleter>::value)>
        unique_ptr(unique_ptr<U, E>&& other) noexcept
        : m_storage(
              _priv::one_then_variadic_args_tag{},
              std::forward<E>(other.get_deleter()),
              other.release())
    {
    }

    void reset(T* new_ptr = nullptr) noexcept
    {
        T* old_ptr = m_storage.second;
        m_storage.second = new_ptr;
        if (old_ptr)
        {
            get_deleter()(old_ptr);
        }
    }

    T* release() noexcept
    {
        T* out = m_storage.second;
        m_storage.second = nullptr;
        return out;
    }

    void swap(unique_ptr& other) noexcept
    {
        vx::swap(get_deleter(), other.get_deleter());
        vx::swap(m_storage.second, other.m_storage.second);
    }

    T* get() noexcept
    {
        return m_storage.second;
    }

    const T* get() const noexcept
    {
        return m_storage.second;
    }

    Deleter& get_deleter() noexcept
    {
        return m_storage.first();
    }

    const Deleter& get_deleter() const noexcept
    {
        return m_storage.first();
    }

    T& operator*() noexcept
    {
        return *m_storage.second;
    }

    const T& operator*() const noexcept
    {
        return *m_storage.second;
    }

    T* operator->() noexcept
    {
        return m_storage.second;
    }

    const T* operator->() const noexcept
    {
        return m_storage.second;
    }

    explicit operator bool() const noexcept
    {
        return m_storage.second != nullptr;
    }
};

//=========================================================================
// unique_ptr (array)
//=========================================================================

namespace _unique_ptr_priv {

template <typename T>
struct unique_array_data
{
    T* ptr;
    size_t size;

    unique_array_data() noexcept
        : ptr(nullptr), size(0)
    {
    }

    unique_array_data(T* p, size_t s) noexcept
        : ptr(p), size(s)
    {
    }
};

} // namespace _unique_ptr_priv

template <typename T, typename Deleter>
class unique_ptr<T[], Deleter>
{
public:

    using element_type = T;
    using deleter_type = Deleter;

private:

    using data_type = _unique_ptr_priv::unique_array_data<T>;
    using storage_type = _priv::compressed_pair<Deleter, data_type>;
    storage_type m_storage;

public:

    constexpr unique_ptr() noexcept
        : m_storage(_priv::zero_then_variadic_args_tag{}, nullptr, size_t(0))
    {
    }

    explicit unique_ptr(T* ptr, size_t size) noexcept
        : m_storage(_priv::zero_then_variadic_args_tag{}, ptr, size)
    {
    }

    unique_ptr(T* ptr, size_t size, const Deleter& deleter) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, deleter, ptr, size)
    {
    }

    unique_ptr(T* ptr, size_t size, Deleter&& deleter) noexcept
        : m_storage(_priv::one_then_variadic_args_tag{}, std::move(deleter), ptr, size)
    {
    }

    ~unique_ptr()
    {
        reset();
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    unique_ptr(unique_ptr&& other) noexcept
        : m_storage(
              _priv::one_then_variadic_args_tag{},
              std::move(other.get_deleter()),
              other.m_storage.second.ptr,
              other.m_storage.second.size)
    {
        other.m_storage.second.ptr = nullptr;
        other.m_storage.second.size = 0;
    }

    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
        if (this != &other)
        {
            reset(other.m_storage.second.ptr, other.m_storage.second.size);
            get_deleter() = std::move(other.get_deleter());
            other.m_storage.second.ptr = nullptr;
            other.m_storage.second.size = 0;
        }
        return *this;
    }

    void reset(T* new_ptr = nullptr, size_t new_size = 0) noexcept
    {
        T* old_ptr = m_storage.second.ptr;
        size_t old_size = m_storage.second.size;
        m_storage.second.ptr = new_ptr;
        m_storage.second.size = new_size;
        if (old_ptr)
        {
            get_deleter()(old_ptr, old_size);
        }
    }

    T* release() noexcept
    {
        T* out = m_storage.second.ptr;
        m_storage.second.ptr = nullptr;
        m_storage.second.size = 0;
        return out;
    }

    void swap(unique_ptr& other) noexcept
    {
        vx::swap(get_deleter(), other.get_deleter());
        vx::swap(m_storage.second.ptr, other.m_storage.second.ptr);
        vx::swap(m_storage.second.size, other.m_storage.second.size);
    }

    T* get() noexcept
    {
        return m_storage.second.ptr;
    }

    const T* get() const noexcept
    {
        return m_storage.second.ptr;
    }

    size_t size() const noexcept
    {
        return m_storage.second.size;
    }

    Deleter& get_deleter() noexcept
    {
        return m_storage.first();
    }

    const Deleter& get_deleter() const noexcept
    {
        return m_storage.first();
    }

    T& operator[](size_t i) noexcept
    {
        VX_ASSERT(i < m_storage.second.size);
        return m_storage.second.ptr[i];
    }

    const T& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < m_storage.second.size);
        return m_storage.second.ptr[i];
    }

    explicit operator bool() const noexcept
    {
        return m_storage.second.ptr != nullptr;
    }
};

template <typename T, typename... Args>
inline expected<unique_ptr<T>, error> make_unique(Args&&... args) noexcept
{
    T* p = mem::construct<T>(std::forward<Args>(args)...);
    VX_RET_UNEXPECTED_ERR_IF(!p, err::out_of_memory);
    return unique_ptr<T>(p);
}

} // namespace vx
