#pragma once

#include <cstring>

#include "vertex/config/assert.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/_memory/memory_base.hpp"
#include "vertex/std/error.hpp"

namespace vx {
namespace mem {

//=========================================================================
// memory management (aligned) internal
//=========================================================================

namespace _mem_priv {

constexpr bool is_pow_2(const size_t x) noexcept
{
    return x != 0 && (x & (x - 1)) == 0;
}

enum : size_t
{
#if defined(__MINGW32__) && !defined(__MINGW64__)

    // Hardcoded max alignment for 32-bit MinGW due to compiler bug
    max_align = 16,

#else

    max_align = alignof(max_align_t),

#endif

//=========================================================================

// Ideal alignment for vectorized loads/stores

#if defined(VX_SIMD_X86) || defined(VX_SIMD_X86_AMD)

    // x86/x64 CPUs with SSE, AVX, AVX2 typically require 16 or 32-byte alignment.
    // AVX and AVX2 use 256-bit registers, so 32 bytes is ideal.
    ideal_align = 32,

#elif defined(VX_SIMD_ARM_NEON)

    // ARM NEON uses 128-bit (16-byte) SIMD registers.
    // 16-byte alignment ensures efficient vector loads/stores.
    ideal_align = 16,

#elif defined(VX_SIMD_PPC)

    // PowerPC SIMD variants (VMX, VSX, QPX) generally require 128-bit alignment.
    // Use 16 bytes to cover all these variants safely.
    ideal_align = 16,

#else

    // Fallback: use the platform's max alignment as ideal alignment.
    ideal_align = max_align,

#endif

//=========================================================================

#if (INTPTR_MAX == INT64_MAX)

    aligned_allocation_sentinel = 0xFAFAFAFAFAFAFAFAULL,

#else

    aligned_allocation_sentinel = 0xFAFAFAFAUL,

#endif

//=========================================================================

#if VX_DEBUG

    aligned_header_size = 2 * sizeof(uintptr_t),

#else

    aligned_header_size = sizeof(uintptr_t),

#endif
};

//=========================================================================

// Ensure the ideal alignment is at least twice the pointer size (to match or exceed vector register alignment)
VX_STATIC_ASSERT_MSG(ideal_align >= 2 * sizeof(uintptr_t), "ideal_align must be at least twice the pointer size to ensure vector register alignment");
// Ensure ideal_align is a power of two (required by aligned allocators and hardware alignment rules)
VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(ideal_align), "ideal_align must be a power of two");

constexpr size_t alignment_padding_size(const size_t alignment) noexcept
{
    return aligned_header_size + alignment - 1;
}

template <size_t alignment>
void adjust_aligned_pointer(void*& out_ptr, size_t& bytes) noexcept
{
    constexpr size_t padding = _mem_priv::alignment_padding_size(alignment);
    bytes += padding;

    const uintptr_t* const ptr = static_cast<uintptr_t*>(out_ptr);
    const uintptr_t block_addr = *(ptr - 1);

#if VX_DEBUG

    // check sentinel
    const uintptr_t sentinel = *(ptr - 2);
    VX_ASSERT(sentinel == _mem_priv::aligned_allocation_sentinel);

#endif

    const uintptr_t back_shift = reinterpret_cast<uintptr_t>(ptr) - block_addr;
    VX_VERIFY_MSG(back_shift >= aligned_header_size && back_shift <= padding, "invalid argument");
    out_ptr = reinterpret_cast<void*>(block_addr);
}

inline void adjust_aligned_pointer(void*& out_ptr, size_t& bytes, const size_t padding) noexcept
{
    bytes += padding;

    const uintptr_t* const ptr = static_cast<uintptr_t*>(out_ptr);
    const uintptr_t block_addr = *(ptr - 1);

#if VX_DEBUG

    // check sentinel
    const uintptr_t sentinel = *(ptr - 2);
    VX_ASSERT(sentinel == _mem_priv::aligned_allocation_sentinel);

#endif

    const uintptr_t back_shift = reinterpret_cast<uintptr_t>(ptr) - block_addr;
    VX_VERIFY_MSG(back_shift >= aligned_header_size && back_shift <= padding, "invalid argument");
    out_ptr = reinterpret_cast<void*>(block_addr);
}

} // namespace _mem_priv

//=========================================================================
// memory management (aligned)
//=========================================================================

enum : size_t
{
    max_align = _mem_priv::max_align,
    ideal_align = _mem_priv::ideal_align
};

//=========================================================================

template <size_t alignment>
VX_ALLOCATOR VX_NO_DISCARD void* allocate_aligned(const size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(alignment), "alignment must be power of 2");

    constexpr size_t padding = _mem_priv::alignment_padding_size(alignment);
    const size_t block_size = bytes + padding;
    VX_RET_IF_UL(block_size <= bytes, nullptr);

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(allocate(block_size));
    VX_VERIFY_MSG(block_ptr != 0, "invalid argument");

    void* const ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _mem_priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

VX_ALLOCATOR inline VX_NO_DISCARD void* allocate_aligned(const size_t bytes, const size_t alignment) noexcept
{
    VX_ASSERT(_mem_priv::is_pow_2(alignment));

    const size_t padding = _mem_priv::alignment_padding_size(alignment);
    const size_t block_size = bytes + padding;
    VX_VERIFY_MSG(block_size <= bytes, "size_t overflow");

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(allocate(block_size));
    VX_VERIFY_MSG(block_ptr != 0, "invalid argument");

    void* const ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _mem_priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

//=========================================================================

template <size_t alignment>
VX_NO_DISCARD void* reallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(alignment), "alignment must be power of 2");

    if (!ptr)
    {
        return allocate_aligned<alignment>(bytes);
    }

    constexpr size_t padding = _mem_priv::alignment_padding_size(alignment);
    size_t block_size = bytes;
    _mem_priv::adjust_aligned_pointer<alignment>(ptr, block_size);
    VX_VERIFY_MSG(block_size <= bytes, "size_t overflow");

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(6308);
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(reallocate(ptr, block_size));
    VX_DISABLE_MSVC_WARNING_POP();
    VX_VERIFY_MSG(block_ptr != 0, "invalid argument");

    ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _mem_priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

inline VX_NO_DISCARD void* reallocate_aligned(void* ptr, size_t bytes, size_t alignment) noexcept
{
    VX_ASSERT(_mem_priv::is_pow_2(alignment));

    if (!ptr)
    {
        return allocate_aligned(bytes, alignment);
    }

    const size_t padding = _mem_priv::alignment_padding_size(alignment);
    size_t block_size = bytes;
    _mem_priv::adjust_aligned_pointer(ptr, block_size, padding);
    VX_VERIFY_MSG(block_size <= bytes, "size_t overflow");

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(6308);
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(reallocate(ptr, block_size));
    VX_DISABLE_MSVC_WARNING_POP();
    VX_VERIFY_MSG(block_ptr != 0, "invalid argument");

    ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _mem_priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

//=========================================================================

template <size_t alignment>
void deallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_mem_priv::is_pow_2(alignment), "alignment must be power of 2");

    if (ptr)
    {
        _mem_priv::adjust_aligned_pointer<alignment>(ptr, bytes);
        deallocate(ptr, bytes);
    }
}

inline void deallocate_aligned(void* ptr, size_t bytes, size_t alignment) noexcept
{
    VX_ASSERT(_mem_priv::is_pow_2(alignment));

    if (ptr)
    {
        const size_t padding = _mem_priv::alignment_padding_size(alignment);
        _mem_priv::adjust_aligned_pointer(ptr, bytes, padding);
        deallocate(ptr, bytes);
    }
}

//=========================================================================
// construct
//=========================================================================

template <typename T, typename... Args>
void construct_in_place(T& obj, Args&&... args)
{
    VX_IF_CONSTEXPR (sizeof...(Args) == 0 && std::is_trivially_default_constructible<T>::value)
    {
        ::new (static_cast<void*>(std::addressof(obj))) T;
    }
    else
    {
        ::new (static_cast<void*>(std::addressof(obj))) T(std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
void construct_in_place(T* ptr, Args&&... args)
{
    VX_IF_CONSTEXPR (sizeof...(Args) == 0 && std::is_trivially_default_constructible<T>::value)
    {
        ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T;
    }
    else
    {
        ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T(std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
void construct_in_place_maybe_trivial(T& obj, Args&&... args)
{
    VX_IF_CONSTEXPR (sizeof...(Args) == 0 && std::is_trivially_default_constructible<T>::value)
    {
        ::new (static_cast<void*>(std::addressof(obj))) T;
    }
    else
    {
        construct_in_place(obj, std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
void construct_in_place_maybe_trivial(T* ptr, Args&&... args)
{
    VX_IF_CONSTEXPR (sizeof...(Args) == 0 && std::is_trivially_default_constructible<T>::value)
    {
        ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T;
    }
    else
    {
        construct_in_place(ptr, std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
VX_ALLOCATOR VX_NO_DISCARD T* construct(Args&&... args)
{
    void* raw_ptr = nullptr;

    VX_IF_CONSTEXPR (alignof(T) <= max_align)
    {
        raw_ptr = allocate(sizeof(T));
    }
    else
    {
        raw_ptr = allocate_aligned<alignof(T)>(sizeof(T));
    }

    if (!raw_ptr)
    {
        return nullptr;
    }

    T* ptr = static_cast<T*>(raw_ptr);
    construct_in_place(*ptr, std::forward<Args>(args)...);
    return ptr;
}

//=========================================================================
// destroy
//=========================================================================

template <typename T>
void destroy_in_place(T* ptr)
{
    VX_IF_CONSTEXPR (!std::is_trivially_destructible<T>::value)
    {
        ptr->~T();
    }
}

template <typename T>
void destroy(T* ptr)
{
    VX_ASSERT(ptr != nullptr);
    destroy_in_place(ptr);

    VX_IF_CONSTEXPR (alignof(T) <= max_align)
    {
        deallocate(ptr, sizeof(T));
    }
    else
    {
        deallocate_aligned<alignof(T)>(ptr, sizeof(T));
    }
}

template <typename T>
void destroy_safe(T*& ptr)
{
    if (ptr)
    {
        destroy(ptr);
        ptr = nullptr;
    }
}

namespace _mem_priv {

template <typename T>
constexpr bool range_will_overflow(const size_t count) noexcept
{
    constexpr bool overflow_possible = sizeof(T) > 1;
    VX_IF_CONSTEXPR (overflow_possible)
    {
        return count > mem::max_array_size<T>();
    }
    else
    {
        return false;
    }
}

} // namespace _mem_priv

//=========================================================================

template <typename T>
VX_ALLOCATOR VX_NO_DISCARD T* construct_array(const size_t count)
{
    VX_RET_IF_UL(count == 0, nullptr);
    VX_VERIFY_MSG(!_mem_priv::range_will_overflow<T>(count), "array size too big");

    const size_t size = sizeof(T) * count;
    void* raw_ptr;

    VX_IF_CONSTEXPR (alignof(T) <= mem::max_align)
    {
        raw_ptr = mem::allocate(size);
    }
    else
    {
        raw_ptr = mem::allocate_aligned<alignof(T)>(size);
    }

    VX_RET_IF_UL(!raw_ptr, nullptr);

    T* ptr = static_cast<T*>(raw_ptr);
    construct_range(static_cast<T*>(ptr), count);
    return static_cast<T*>(ptr);
}

template <typename T>
VX_ALLOCATOR VX_NO_DISCARD T* construct_array(const size_t count, const T& value)
{
    VX_RET_IF_UL(count == 0, nullptr);
    VX_VERIFY_MSG(!_mem_priv::range_will_overflow<T>(count), "array size too big");

    const size_t size = sizeof(T) * count;
    void* raw_ptr;

    VX_IF_CONSTEXPR (alignof(T) <= mem::max_align)
    {
        raw_ptr = mem::allocate(size);
    }
    else
    {
        raw_ptr = mem::allocate_aligned<alignof(T)>(size);
    }

    VX_RET_IF_UL(!raw_ptr, nullptr);

    T* ptr = static_cast<T*>(raw_ptr);
    fill_uninitialized_range(ptr, count, value);
    return ptr;
}

template <typename T>
void destroy_array(T* ptr, const size_t count)
{
    destroy_range(ptr, count);
    const size_t size = sizeof(T) * count;

    VX_IF_CONSTEXPR (alignof(T) <= mem::max_align)
    {
        mem::deallocate(ptr, size);
    }
    else
    {
        mem::deallocate_aligned<alignof(T)>(ptr, size);
    }
}

template <typename T>
void destroy_array_safe(T*& ptr, const size_t count)
{
    if (ptr && count)
    {
        destroy_array(ptr, count);
        ptr = nullptr;
    }
}

} // namespace mem
} // namespace vx
