#pragma once

#include <cstring>

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/config/type_traits.hpp"

namespace vx {
namespace mem {

//=========================================================================
// memory management (unaligned)
//=========================================================================

template <bool zero = false>
VX_FORCE_INLINE void* allocate(size_t size) noexcept
{
    if (size == 0)
    {
        return nullptr;
    }

    void* ptr;

    VX_IF_CONSTEXPR(zero)
    {
        ptr = std::calloc(1, size);
    }
    else
    {
        ptr = std::malloc(size);
    }

    return ptr;
}

VX_FORCE_INLINE void* allocate_zero(size_t size) noexcept
{
    return allocate<true>(size);
}

VX_FORCE_INLINE void deallocate(void* mem) noexcept
{
    std::free(mem);
}

VX_FORCE_INLINE void* reallocate(void* ptr, size_t size) noexcept
{
    if (!ptr)
    {
        return allocate(size);
    }

    return std::realloc(ptr, size);
}

VX_FORCE_INLINE void* copy(void* dst, const void* src, size_t size) noexcept
{
    return std::memcpy(dst, src, size);
}

VX_FORCE_INLINE void* move(void* dst, const void* src, size_t size) noexcept
{
    return std::memmove(dst, src, size);
}

VX_FORCE_INLINE void* set(void* dst, uint8_t value, size_t size) noexcept
{
    return std::memset(dst, static_cast<int>(value), size);
}

VX_FORCE_INLINE int compare(const void* a, const void* b, size_t size) noexcept
{
    return std::memcmp(a, b, size);
}

//=========================================================================
// memory management (aligned) internal
//=========================================================================

namespace _priv {

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
VX_STATIC_ASSERT(ideal_align >= 2 * sizeof(uintptr_t), "ideal_align must be at least twice the pointer size to ensure vector register alignment");
// Ensure ideal_align is a power of two (required by aligned allocators and hardware alignment rules)
VX_STATIC_ASSERT(_priv::is_pow_2(ideal_align), "ideal_align must be a power of two");

//=========================================================================

constexpr size_t alignment_padding_size(size_t alignment)
{
    return aligned_header_size + alignment - 1;
}

VX_FORCE_INLINE void* raw_to_aligned(void* raw_ptr, size_t alignment, size_t padding) noexcept
{
    const uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw_ptr);
    const uintptr_t aligned_addr = (raw_addr + padding) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned_addr);
}

VX_FORCE_INLINE void* aligned_to_raw(void* aligned_ptr, size_t padding) noexcept
{
    const uintptr_t aligned_addr = reinterpret_cast<uintptr_t>(aligned_ptr);

#if VX_DEBUG

    // check sentinel
    const uintptr_t sentinel = *(reinterpret_cast<const uintptr_t*>(aligned_addr) - 2);
    VX_ASSERT(sentinel == _priv::aligned_allocation_sentinel);

#endif

    const uintptr_t raw_addr = *(reinterpret_cast<const uintptr_t*>(aligned_addr) - 1);
    return reinterpret_cast<void*>(raw_addr);
}

VX_FORCE_INLINE void* setup_aligned_block(void* raw_ptr, size_t alignment, size_t padding) noexcept
{
    const uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw_ptr);
    const uintptr_t aligned_addr = (raw_addr + padding) & ~(alignment - 1);

    // Store raw pointer metadata
    *(reinterpret_cast<uintptr_t*>(aligned_addr) - 1) = raw_addr;

#if VX_DEBUG
    *(reinterpret_cast<uintptr_t*>(aligned_addr) - 2) = aligned_allocation_sentinel;
#endif

    return reinterpret_cast<void*>(aligned_addr);
}

} // namespace _priv

//=========================================================================
// memory management (aligned)
//=========================================================================

enum : size_t
{
    max_align = _priv::max_align,
    ideal_align = _priv::ideal_align
};

template <bool zero = false>
VX_FORCE_INLINE void* allocate_aligned(size_t size, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    if (size == 0)
    {
        return nullptr;
    }

    const size_t padding = _priv::alignment_padding_size(alignment);
    const size_t block_size = size + padding;
    if (block_size <= size)
    {
        // overflow
        return nullptr;
    }

    void* raw_ptr;

    VX_IF_CONSTEXPR(zero)
    {
        raw_ptr = std::calloc(1, block_size);
    }
    else
    {
        raw_ptr = std::malloc(block_size);
    }

    if (!raw_ptr)
    {
        return nullptr;
    }

    return _priv::setup_aligned_block(raw_ptr, alignment, padding);
}

VX_FORCE_INLINE void* reallocate_aligned(void* ptr, size_t size, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    if (!ptr)
    {
        return allocate_aligned(size, alignment);
    }

    const size_t padding = _priv::alignment_padding_size(alignment);
    const size_t block_size = size + padding;
    if (block_size <= size)
    {
        // overflow
        return nullptr;
    }

    void* raw_ptr = _priv::aligned_to_raw(ptr, padding);
    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(6308);
    raw_ptr = std::realloc(raw_ptr, block_size);
    VX_DISABLE_MSVC_WARNING_POP();
    if (!raw_ptr)
    {
        return nullptr;
    }

    return _priv::setup_aligned_block(raw_ptr, alignment, padding);
}

VX_FORCE_INLINE void deallocate_aligned(void* ptr, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));
    void* raw_ptr = _priv::aligned_to_raw(ptr, alignment);
    std::free(raw_ptr);
}

//=========================================================================
// construct
//=========================================================================

template <typename T, typename... Args>
VX_FORCE_INLINE T* construct_in_place(T* ptr, Args&&... args) noexcept
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    VX_IF_CONSTEXPR(type_traits::is_zero_constructible<T>::value)
    {
        // No user-visible construction needed
        return ptr;
    }
    else
    {
        return ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
VX_FORCE_INLINE T* construct(Args&&... args) noexcept
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    constexpr bool zero = type_traits::is_zero_constructible<T>::value;
    void* ptr = allocate_aligned<zero>(sizeof(T), alignof(T));
    if (!ptr)
    {
        return nullptr;
    }

    return construct_in_place(static_cast<T*>(ptr), std::forward<Args>(args)...);
}

//=========================================================================
// destroy
//=========================================================================

template <typename T>
VX_FORCE_INLINE void destroy_in_place(T* ptr) noexcept
{
    VX_IF_CONSTEXPR(!std::is_trivially_destructible<T>::value)
    {
        ptr->~T();
    }
}

template <typename T>
VX_FORCE_INLINE void destroy(T* ptr) noexcept
{
    destroy_in_place(ptr);
    deallocate_aligned(ptr, alignof(T));
}

template <typename T>
VX_FORCE_INLINE void destroy_safe(T* ptr) noexcept
{
    if (ptr)
    {
        destroy(ptr);
        ptr = nullptr;
    }
}

//=========================================================================
// array base
//=========================================================================

template <typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) noexcept
{
    return N;
}

template <typename T, typename... Args>
VX_FORCE_INLINE void construct_array_elements(T* ptr, size_t count, Args&&... args) noexcept
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    VX_IF_CONSTEXPR(sizeof...(Args) == 0 && type_traits::is_zero_constructible<T>::value)
    {
        // can optimize with memset
        set(ptr, 0, count * sizeof(T));
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            construct_in_place(ptr + i, std::forward<Args>(args)...);
        }
    }
}

template <typename T>
VX_FORCE_INLINE void destroy_array_elements(T* ptr, size_t count) noexcept
{
    VX_IF_CONSTEXPR(!std::is_trivially_destructible<T>::value)
    {
        for (size_t i = 0; i < count; ++i)
        {
            ptr[i].~T();
        }
    }
}

template <typename T>
VX_FORCE_INLINE void copy_array_elements(T* dst, const T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR(std::is_trivially_copyable<T>::value)
    {
        copy(dst, src, count * sizeof(T));
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            construct_in_place(dst + i, src[i]);
        }
    }
}

template <typename T, typename... Args>
VX_FORCE_INLINE T* construct_array(size_t count, Args&&... args) noexcept
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    if (count == 0)
    {
        return nullptr;
    }

    const size_t size = sizeof(T) * count;
    constexpr bool zero = type_traits::is_zero_constructible<T>::value;
    void* ptr = allocate_aligned<zero>(size, alignof(T));
    if (!ptr)
    {
        return nullptr;
    }

    VX_IF_CONSTEXPR(!zero)
    {
        construct_array_elements(static_cast<T*>(ptr), count, std::forward<Args>(args)...);
    }

    return static_cast<T*>(ptr);
}

template <typename T>
VX_FORCE_INLINE void destroy_array(T* ptr, size_t count) noexcept
{
    destroy_array_elements(ptr, count);
    deallocate_aligned(ptr, alignof(T));
}

template <typename T>
VX_FORCE_INLINE void destroy_array_safe(T* ptr, size_t count) noexcept
{
    if (ptr && count)
    {
        destroy_array(ptr, count);
        ptr = nullptr;
    }
}

} // namespace mem

//=========================================================================
// allocator
//=========================================================================

class default_allocator
{
public:

    VX_FORCE_INLINE static void* alloc(size_t size) noexcept
    {
        return mem::allocate(size);
    }

    VX_FORCE_INLINE static void* realloc(void* ptr, size_t size) noexcept
    {
        return mem::reallocate(ptr, size);
    }

    VX_FORCE_INLINE static void free(void* ptr) noexcept
    {
        mem::deallocate(ptr);
    }
};

template <size_t alignment>
class aligned_allocator
{
public:

    VX_FORCE_INLINE static void* alloc(size_t size) noexcept
    {
        return mem::allocate_aligned(size, alignment);
    }

    VX_FORCE_INLINE static void* realloc(void* ptr, size_t size) noexcept
    {
        return mem::reallocate_aligned(ptr, size, alignment);
    }

    VX_FORCE_INLINE static void free(void* ptr) noexcept
    {
        mem::deallocate_aligned(ptr, alignment);
    }
};

//=========================================================================

template <size_t alignment>
struct allocator_selector
{
    using type = aligned_allocator<alignment>;
};

template <>
struct allocator_selector<0>
{
    using type = default_allocator;
};

} // namespace vx
