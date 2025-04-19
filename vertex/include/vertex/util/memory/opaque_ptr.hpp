#pragma once

#include "vertex/config/language_config.hpp"

/*
 * opaque_ptr - A zero-heap, high-performance alternative to smart pointers for the PImpl idiom.
 *
 * Benefits over std::unique_ptr<T> (heap-based PImpl):
 * - **No Heap Allocations**: Stores the object directly inside a fixed-size stack buffer.
 * - **Better Cache Locality**: Avoids pointer indirection, leading to fewer cache misses and better CPU performance.
 * - **Faster Access**: Direct access to the object without an extra pointer lookup.
 * - **Move Semantics Support**: Efficient ownership transfer without heap allocations.
 * - **No Dynamic Memory Overhead**: Eliminates malloc/free calls, reducing fragmentation.
 *
 * Use when:
 * - The maximum object size is known at compile-time.
 * - High-performance, low-latency applications (e.g., game engines, real-time systems).
 * - Reducing memory fragmentation and improving CPU cache efficiency.
 *
 * Note:
 * - Object size must be carefully chosen to fit in the stack buffer.
 * - Copy semantics are intentionally disabled to avoid deep copies.
 */

namespace vx {
namespace mem {

template <typename T, size_t Size = sizeof(T), size_t Align = alignof(T)>
class opaque_ptr
{
public:

    using element_type = T;
    static constexpr size_t size = Size;
    static constexpr size_t alignment = Align;

private:

    alignas(Align) unsigned char m_storage[Size];

    T* get() noexcept { return reinterpret_cast<T*>(&m_storage); }
    const T* get() const noexcept { return reinterpret_cast<const T*>(&m_storage); }

public:

    template <typename... Args>
    opaque_ptr(Args&&... args)
    {
        static_assert(sizeof(T) <= Size, "Buffer size too small!");
        static_assert(alignof(T) <= Align, "Alignment too small!");
        new (get()) T(std::forward<Args>(args)...);
    }

    ~opaque_ptr() { get()->~T(); }

    opaque_ptr(const opaque_ptr&) = delete;
    opaque_ptr& operator=(const opaque_ptr&) = delete;

    opaque_ptr(opaque_ptr&& other) noexcept
    {
        new (get()) T(std::move(*other.get()));
    }

    opaque_ptr& operator=(opaque_ptr&& other) noexcept
    {
        if (this != &other)
        {
            get()->~T();
            new (get()) T(std::move(*other.get()));
        }
        return *this;
    }

    T* operator->() noexcept { return get(); }
    const T* operator->() const noexcept { return get(); }

    T& operator*() noexcept { return *get(); }
    const T& operator*() const noexcept { return *get(); }
};

}
}