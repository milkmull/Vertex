#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

template <typename T, size_t Size = sizeof(T), size_t Align = alignof(T)>
class opaque_ptr
{
    VX_STATIC_ASSERT_MSG(sizeof(T) <= Size, "Buffer size too small!");
    VX_STATIC_ASSERT_MSG(alignof(T) <= Align, "Alignment too small!");

public:

    using element_type = T;
    static constexpr size_t size = Size;
    static constexpr size_t alignment = Align;

private:

    alignas(align) unsigned char m_storage[size];

    void reset() noexcept
    {
        mem::destroy_in_place(get());
    }

public:

    template <typename... Args>
    opaque_ptr(Args&&... args)
    {
        mem::construct_in_place(get(), std::forward<Args>(args)...);
    }

    ~opaque_ptr()
    {
        reset();
    }

    opaque_ptr(const opaque_ptr&) = delete;
    opaque_ptr& operator=(const opaque_ptr&) = delete;

    opaque_ptr(opaque_ptr&& other) noexcept
    {
        mem::construct_in_place(get(), std::move(*other.get()));
    }

    opaque_ptr& operator=(opaque_ptr&& other) noexcept
    {
        if (this != &other)
        {
            VX_IF_CONSTEXPR (std::is_move_assignable<T>::value)
            {
                *get() = std::move(*other.get());
            }
            else
            {
                reset();
                mem::construct_in_place(std::move(*other.get()));
            }
        }
        return *this;
    }

    T* get() noexcept
    {
        return reinterpret_cast<T*>(&m_storage);
    }

    const T* get() const noexcept
    {
        return reinterpret_cast<const T*>(&m_storage);
    }

    T& operator*() noexcept
    {
        return *get();
    }
    const T& operator*() const noexcept
    {
        return *get();
    }

    T* operator->() noexcept
    {
        return get();
    }
    const T* operator->() const noexcept
    {
        return get();
    }

    constexpr explicit operator bool() const noexcept
    {
        return true;
    }
};

template <typename T, typename... Args>
inline opaque_ptr<T> make_opaque(Args&&... args) noexcept
{
    return opaque_ptr<T>(std::forward<Args>(args)...);
}

} // namespace vx
