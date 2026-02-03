#pragma once

#include "vertex/config/language_config.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

#define _size_check() \
    VX_STATIC_ASSERT_MSG(sizeof(T) <= size, "size too small"); \
    VX_STATIC_ASSERT_MSG(alignof(T) <= alignment, "alignment too small")

template <size_t Size, size_t Alignment>
class aligned_storage
{
    VX_STATIC_ASSERT_MSG(Size > 0, "size too small");
    VX_STATIC_ASSERT_MSG(mem::_priv::is_pow_2(Alignment), "alignment must be power of 2");
    VX_STATIC_ASSERT_MSG(Alignment > 0, "alignment size too small");

public:

    static constexpr size_t size = Size;
    static constexpr size_t alignment = Alignment;

private:

    alignas(alignment) unsigned char m_storage[size];

public:

    aligned_storage() noexcept = default;
    ~aligned_storage() noexcept = default;

    aligned_storage(const aligned_storage&) = delete;
    aligned_storage(aligned_storage&&) noexcept = delete;

    aligned_storage& operator=(const aligned_storage&) = delete;
    aligned_storage& operator=(aligned_storage&&) noexcept = delete;

    template <typename T>
    explicit aligned_storage(T&& value) noexcept
    {
        _size_check();
        construct<T>(std::move(value));
    }

    template <typename T, typename... Args>
    void construct(Args&&... args)
    {
        _size_check();
        mem::construct_in_place<T>(ptr<T>(), std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy() noexcept
    {
        _size_check();
        mem::destroy_in_place(ptr<T>());
    }

    template <typename T>
    T* ptr() noexcept
    {
        _size_check();
        return reinterpret_cast<T*>(m_storage);
    }

    template <typename T>
    const T* ptr() const noexcept
    {
        _size_check();
        return reinterpret_cast<const T*>(m_storage);
    }

    template <typename T>
    T& get() noexcept
    {
        _size_check();
        return *ptr<T>();
    }

    template <typename T>
    const T& get() const noexcept
    {
        _size_check();
        return *ptr<T>();
    }
};

#undef _size_check

} // namespace vx
