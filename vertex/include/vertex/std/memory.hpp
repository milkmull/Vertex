#pragma once

#include <cstring>

#include "vertex/config/assert.hpp"
#include "vertex/config/language_config.hpp"
#include "vertex/config/simd.hpp"
#include "vertex/config/type_traits.hpp"
#include "vertex/std/error.hpp"

//#define VX_ALLOCATE_FAIL_FAST

namespace vx {
namespace mem {

//=========================================================================
// enable crash when allocation fails
//=========================================================================

#if defined(VX_ALLOCATE_FAIL_FAST)
    #define VX_ALLOCATOR_FAILED(ret) VX_CRASH_WITH_MESSAGE("allocation failed");
#else
    #define VX_ALLOCATOR_FAILED(ret) return ::vx::err::return_error(err::out_of_memory, ret)
#endif

//=========================================================================
// memory management (unaligned)
//=========================================================================

VX_ALLOCATOR inline void* allocate(const size_t bytes)
{
    return ::malloc(bytes);
}

VX_ALLOCATOR inline void* allocate_zero(const size_t bytes)
{
    return ::calloc(1, bytes);
}

inline void deallocate(void* ptr, const size_t bytes) noexcept
{
    ::free(ptr);
}

inline void* reallocate(void* ptr, const size_t bytes)
{
    return ::realloc(ptr, bytes);
}

inline void* copy(void* dst, const void* src, const size_t bytes)
{
    return ::memcpy(dst, src, bytes);
}

inline void* move(void* dst, const void* src, const size_t bytes)
{
    return ::memmove(dst, src, bytes);
}

inline void* set(void* dst, const uint8_t value, const size_t bytes)
{
    return ::memset(dst, static_cast<int>(value), bytes);
}

inline bool compare(const void* a, const void* b, const size_t bytes)
{
    return ::memcmp(a, b, bytes) == 0;
}

//=========================================================================
// memory management (aligned) internal
//=========================================================================

namespace _priv {

constexpr bool is_pow_2(const size_t x)
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

constexpr size_t alignment_padding_size(const size_t alignment)
{
    return aligned_header_size + alignment - 1;
}

template <size_t alignment>
inline void adjust_aligned_pointer(void*& out_ptr, size_t& bytes) noexcept
{
    constexpr size_t padding = _priv::alignment_padding_size(alignment);
    bytes += padding;

    const uintptr_t* const ptr = static_cast<uintptr_t*>(out_ptr);
    const uintptr_t block_addr = *(ptr - 1);

#if VX_DEBUG

    // check sentinel
    const uintptr_t sentinel = *(ptr - 2);
    VX_ASSERT(sentinel == _priv::aligned_allocation_sentinel);

#endif

    const uintptr_t back_shift = reinterpret_cast<uintptr_t>(ptr) - block_addr;
    VX_VERIFY(back_shift >= aligned_header_size && back_shift <= padding, "invalid argument");
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
    VX_ASSERT(sentinel == _priv::aligned_allocation_sentinel);

#endif

    const uintptr_t back_shift = reinterpret_cast<uintptr_t>(ptr) - block_addr;
    VX_VERIFY(back_shift >= aligned_header_size && back_shift <= padding, "invalid argument");
    out_ptr = reinterpret_cast<void*>(block_addr);
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

template <size_t alignment>
VX_ALLOCATOR void* allocate_aligned(const size_t bytes) noexcept
{
    VX_STATIC_ASSERT(_priv::is_pow_2(alignment), "alignment must be power of 2");

    constexpr size_t padding = _priv::alignment_padding_size(alignment);
    const size_t block_size = bytes + padding;
    VX_UNLIKELY_COLD_PATH(block_size <= bytes,
    {
        return err::return_error(err::size_error, nullptr);
    });

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(::malloc(block_size));
    VX_UNLIKELY_COLD_PATH(block_ptr == 0,
    {
        VX_ALLOCATOR_FAILED(nullptr);
    });

    void* const ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

VX_ALLOCATOR inline void* allocate_aligned(const size_t bytes, const size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    const size_t padding = _priv::alignment_padding_size(alignment);
    const size_t block_size = bytes + padding;
    VX_UNLIKELY_COLD_PATH(block_size <= bytes,
    {
        return err::return_error(err::size_error, nullptr);
    });

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(::malloc(block_size));
    VX_UNLIKELY_COLD_PATH(block_ptr == 0,
    {
        VX_ALLOCATOR_FAILED(nullptr);
    });

    void* const ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

template <size_t alignment>
void* reallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT(_priv::is_pow_2(alignment), "alignment must be power of 2");

    if (!ptr)
    {
        return allocate_aligned<alignment>(bytes);
    }

    constexpr size_t padding = _priv::alignment_padding_size(alignment);
    size_t block_size = bytes;
    _priv::adjust_aligned_pointer<alignment>(ptr, block_size);
    VX_UNLIKELY_COLD_PATH(block_size <= bytes,
    {
        return err::return_error(err::size_error, nullptr);
    });

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(6308);
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(::realloc(ptr, block_size));
    VX_DISABLE_MSVC_WARNING_POP();
    VX_UNLIKELY_COLD_PATH(block_ptr == 0,
    {
        VX_ALLOCATOR_FAILED(nullptr);
    });

    ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

inline void* reallocate_aligned(void* ptr, size_t bytes, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    if (!ptr)
    {
        return allocate_aligned(bytes, alignment);
    }

    const size_t padding = _priv::alignment_padding_size(alignment);
    size_t block_size = bytes;
    _priv::adjust_aligned_pointer(ptr, block_size, padding);
    VX_UNLIKELY_COLD_PATH(block_size <= bytes,
    {
        return err::return_error(err::size_error, nullptr);
    });

    VX_DISABLE_MSVC_WARNING_PUSH();
    VX_DISABLE_MSVC_WARNING(6308);
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(::realloc(ptr, block_size));
    VX_DISABLE_MSVC_WARNING_POP();
    VX_UNLIKELY_COLD_PATH(block_ptr == 0,
    {
        VX_ALLOCATOR_FAILED(nullptr);
    });

    ptr = reinterpret_cast<void*>((block_ptr + padding) & ~(alignment - 1));
    *(static_cast<uintptr_t*>(ptr) - 1) = block_ptr;

#if VX_DEBUG
    *(static_cast<uintptr_t*>(ptr) - 2) = _priv::aligned_allocation_sentinel;
#endif

    return ptr;
}

template <size_t alignment>
inline void deallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT(_priv::is_pow_2(alignment), "alignment must be power of 2");

    if (ptr)
    {
        _priv::adjust_aligned_pointer<alignment>(ptr, bytes);
        ::free(ptr);
    }
}

inline void deallocate_aligned(void* ptr, size_t bytes, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    if (ptr)
    {
        const size_t padding = _priv::alignment_padding_size(alignment);
        _priv::adjust_aligned_pointer(ptr, bytes, padding);
        ::free(ptr);
    }
}

//=========================================================================
// construct
//=========================================================================

template <typename T, typename... Args>
inline void construct_in_place(T* ptr, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
{
    ::new (const_cast<void*>(static_cast<const volatile void*>(ptr))) T(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
VX_ALLOCATOR inline T* construct(Args&&... args)
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    void* raw_ptr = allocate_aligned(sizeof(T), alignof(T));
    VX_UNLIKELY_COLD_PATH(!raw_ptr,
    {
        return nullptr;
    });

    T* ptr = static_cast<T*>(raw_ptr);
    construct_in_place(ptr, std::forward<Args>(args)...);
    return ptr;
}

//=========================================================================
// destroy
//=========================================================================

template <typename T>
inline void destroy_in_place(T* ptr)
{
    VX_IF_CONSTEXPR(!std::is_trivially_destructible<T>::value)
    {
        ptr->~T();
    }
}

template <typename T>
inline void destroy(T* ptr)
{
    destroy_in_place(ptr);
    deallocate_aligned(ptr, alignof(T));
}

template <typename T>
inline void destroy_safe(T* ptr)
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

namespace _priv {

template <typename T>
constexpr size_t get_max_count() noexcept
{
    return SIZE_MAX / sizeof(T);
}

template <typename T>
constexpr bool range_will_overflow(const size_t count)
{
    constexpr bool overflow_possible = sizeof(T) > 1;
    VX_IF_CONSTEXPR(overflow_possible)
    {
        return count > get_max_count<T>();
    }
    else
    {
        return false;
    }
}

} // namespace _priv

template <typename T, size_t N>
constexpr size_t array_size(const T (&)[N])
{
    return N;
}

template <typename T>
inline void construct_range(T* ptr, size_t count)
{
    VX_STATIC_ASSERT(std::is_default_constructible<T>::value, "Type must be default constructible");
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    for (; 0 < count; --count)
    {
        construct_in_place(ptr);
        ++ptr;
    }
}

template <typename T, typename U>
inline void construct_range(T* ptr, size_t count, const U& value)
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    VX_IF_CONSTEXPR((type_traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        set(ptr, value, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(ptr, value);
            ++ptr;
        }
    }
}

template <typename T>
inline void destroy_range(T* ptr, size_t count) noexcept
{
    for (; 0 < count; --count)
    {
        destroy_in_place(ptr);
        ++ptr;
    }
}

template <typename T>
inline void copy_range(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR(type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(dst, *src);
            ++src;
            ++dst;
        }
    }
}

template <typename T>
inline void move_range(T* dst, const T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR(type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(dst, std::move(*src));
            ++src;
            ++dst;
        }
    }
}

template <typename T>
inline void move_range_unchecked(T* dst, const T* src, size_t count)
{
    // this version favors memcpy to memmove

    VX_IF_CONSTEXPR(type_traits::memmove_is_safe<T*>::value)
    {
        copy(dst, src, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(dst, std::move(*src));
            ++src;
            ++dst;
        }
    }
}

template <typename T>
inline void move_range_back(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR((type_traits::_priv::iter_copy_cat<T*, T*>::is_bitcopy_assignable))
    {
        move(dst - count, src - count, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = std::move(*src);
            --src;
            --dst;
        }
    }
}

template <typename T, typename IT1, typename IT2>
inline void construct_from_range(T* dst, IT1 first, IT2 last)
{
    for (; first != last; ++first)
    {
        construct_in_place(dst, *first);
        ++dst;
    }
}

template <typename T>
inline bool compare_range(const T* a, const T* b, size_t count)
{
    VX_IF_CONSTEXPR((type_traits::is_bitwise_comparable<T, T>::value))
    {
        return compare(a, b, count * sizeof(T));
    }
    else
    {
        for (; 0 < count; --count)
        {
            if (!(*a == *b))
            {
                return false;
            }

            ++a;
            ++b;
        }

        return true;
    }
}

template <typename T>
VX_ALLOCATOR inline T* construct_array(const size_t count)
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    if (count == 0)
    {
        return nullptr;
    }

    if (_priv::range_will_overflow<T>())
    {
        VX_ERR(err::size_error);
        return nullptr;
    }

    const size_t size = sizeof(T) * count;
    void* ptr = allocate_aligned(size, alignof(T));
    if (!ptr)
    {
        return nullptr;
    }

    construct_range(static_cast<T*>(ptr), count);
    return static_cast<T*>(ptr);
}

template <typename T>
VX_ALLOCATOR inline T* construct_array(const size_t count, const T& value)
{
    //VX_STATIC_ASSERT((std::is_nothrow_constructible<T, Args...>::value), "Type must be nothrow constructible");

    if (count == 0)
    {
        return nullptr;
    }

    if (_priv::range_will_overflow<T>())
    {
        VX_ERR(err::size_error);
        return nullptr;
    }

    const size_t size = sizeof(T) * count;
    void* ptr = allocate_aligned(size, alignof(T));
    if (!ptr)
    {
        return nullptr;
    }

    construct_range(static_cast<T*>(ptr), count, value);
    return static_cast<T*>(ptr);
}

template <typename T>
inline void destroy_array(const T* ptr, const size_t count)
{
    destroy_range(ptr, count);
    deallocate_aligned(ptr, alignof(T));
}

template <typename T>
inline void destroy_array_safe(const T* ptr, const size_t count)
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

template <typename T, size_t Alignment = alignof(T)>
class default_allocator
{
public:

    //VX_STATIC_ASSERT(Alignment >= alignof(T), "Alignment must be at alignof(T)");
    VX_STATIC_ASSERT(mem::_priv::is_pow_2(Alignment), "Alignment must be power of 2");
    static constexpr size_t alignment = Alignment;

    VX_ALLOCATOR static T* allocate(const size_t count) noexcept
    {
        VX_UNLIKELY_COLD_PATH(count == 0,
        {
            return nullptr;
        });

        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR(alignment == 1)
        {
            return static_cast<T*>(mem::allocate(bytes));
        }
        else
        {
            return static_cast<T*>(mem::allocate_aligned<alignment>(bytes));
        }
    }

    static T* reallocate(T* ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR(alignment == 1)
        {
            return static_cast<T*>(mem::reallocate(ptr, bytes));
        }
        else
        {
            return static_cast<T*>(mem::reallocate_aligned<alignment>(ptr, bytes));
        }
    }

    static void deallocate(T* ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR(alignment == 1)
        {
            mem::deallocate(ptr, bytes);
        }
        else
        {
            mem::deallocate_aligned<alignment>(ptr, bytes);
        }
    }
};

} // namespace vx
