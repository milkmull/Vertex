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
// helpers
//=========================================================================

template <typename T>
constexpr VX_NO_DISCARD typename std::remove_reference<T>::type&& move(T&& a) noexcept
{
    return std::move(a);
}

template <typename T>
constexpr void swap(T& a, T& b) noexcept
{
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

//=========================================================================
// memory management (unaligned)
//=========================================================================

VX_ALLOCATOR inline void* allocate(const size_t bytes) noexcept
{
    return ::malloc(bytes);
}

inline void deallocate(void* ptr, VX_MAYBE_UNUSED const size_t bytes) noexcept
{
    ::free(ptr);
}

inline void* reallocate(void* ptr, const size_t bytes) noexcept
{
    return ::realloc(ptr, bytes);
}

inline constexpr void* copy(void* dst, const void* src, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        char* d = static_cast<char*>(dst);
        const char* s = static_cast<const char*>(src);

        for (size_t i = 0; i < bytes; ++i)
        {
            d[i] = s[i];
        }

        return dst;
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_memcpy)
        return __builtin_memcpy(dst, src, bytes);
#else
        return ::memcpy(dst, src, bytes);
#endif
    }
}

inline constexpr void* move(void* dst, const void* src, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        char* d = static_cast<char*>(dst);
        const char* s = static_cast<const char*>(src);

        // Handle overlapping memory regions safely during compile-time evaluation
        if (d > s && d < s + bytes)
        {
            for (size_t i = bytes; i > 0; --i)
            {
                d[i - 1] = s[i - 1];
            }
        }
        else
        {
            for (size_t i = 0; i < bytes; ++i)
            {
                d[i] = s[i];
            }
        }

        return dst;
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_memmove)
        return __builtin_memmove(dst, src, bytes);
#else
        return ::memmove(dst, src, bytes);
#endif
    }
}

inline constexpr void* set(void* dst, const int value, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        char* d = static_cast<char*>(dst);
        const char byte_val = static_cast<char>(value);

        for (size_t i = 0; i < bytes; ++i)
        {
            d[i] = byte_val;
        }

        return dst;
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_memset)
        return __builtin_memset(dst, static_cast<int>(value), bytes);
#else
        return ::memset(dst, static_cast<int>(value), bytes);
#endif
    }
}

inline constexpr int compare(const void* a, const void* b, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        const char* ca = static_cast<const char*>(a);
        const char* cb = static_cast<const char*>(b);

        for (size_t i = 0; i < bytes; ++i)
        {
            if (ca[i] != cb[i])
            {
                return (static_cast<unsigned char>(ca[i]) < static_cast<unsigned char>(cb[i])) ? -1 : 1;
            }
        }

        return 0;
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_memcmp)
        return __builtin_memcmp(a, b, bytes);
#else
        return ::memcmp(a, b, bytes);
#endif
    }
}

inline constexpr const void* find(const void* src, const int value, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        const char* s = static_cast<const char*>(src);
        const char byte_val = static_cast<char>(value);

        for (size_t i = 0; i < bytes; ++i)
        {
            if (s[i] == byte_val)
            {
                return s + i;
            }
        }

        return nullptr;
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_memchr)
        return __builtin_memchr(src, value, bytes);
#else
        return ::memchr(src, value, bytes);
#endif
    }
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
VX_STATIC_ASSERT_MSG(ideal_align >= 2 * sizeof(uintptr_t), "ideal_align must be at least twice the pointer size to ensure vector register alignment");
// Ensure ideal_align is a power of two (required by aligned allocators and hardware alignment rules)
VX_STATIC_ASSERT_MSG(_priv::is_pow_2(ideal_align), "ideal_align must be a power of two");

constexpr size_t alignment_padding_size(const size_t alignment) noexcept
{
    return aligned_header_size + alignment - 1;
}

template <size_t alignment>
void adjust_aligned_pointer(void*& out_ptr, size_t& bytes) noexcept
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

//=========================================================================

template <size_t alignment>
VX_ALLOCATOR void* allocate_aligned(const size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_priv::is_pow_2(alignment), "alignment must be power of 2");

    constexpr size_t padding = _priv::alignment_padding_size(alignment);
    const size_t block_size = bytes + padding;
    VX_UNLIKELY_COLD_PATH(block_size <= bytes,
        {
            return err::return_error(err::size_error, nullptr);
        });

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(allocate(block_size));
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

    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(allocate(block_size));
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

//=========================================================================

template <size_t alignment>
void* reallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_priv::is_pow_2(alignment), "alignment must be power of 2");

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
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(reallocate(ptr, block_size));
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
    const uintptr_t block_ptr = reinterpret_cast<uintptr_t>(reallocate(ptr, block_size));
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

//=========================================================================

template <size_t alignment>
void deallocate_aligned(void* ptr, size_t bytes) noexcept
{
    VX_STATIC_ASSERT_MSG(_priv::is_pow_2(alignment), "alignment must be power of 2");

    if (ptr)
    {
        _priv::adjust_aligned_pointer<alignment>(ptr, bytes);
        deallocate(ptr, bytes);
    }
}

inline void deallocate_aligned(void* ptr, size_t bytes, size_t alignment) noexcept
{
    VX_ASSERT(_priv::is_pow_2(alignment));

    if (ptr)
    {
        const size_t padding = _priv::alignment_padding_size(alignment);
        _priv::adjust_aligned_pointer(ptr, bytes, padding);
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
VX_ALLOCATOR T* construct(Args&&... args)
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

//=========================================================================
// array base
//=========================================================================

template <typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) noexcept
{
    return N;
}

template <typename T>
constexpr size_t max_array_size() noexcept
{
    return std::numeric_limits<size_t>::max() / sizeof(T);
}

namespace _priv {

template <typename T>
constexpr bool range_will_overflow(const size_t count) noexcept
{
    constexpr bool overflow_possible = sizeof(T) > 1;
    VX_IF_CONSTEXPR (overflow_possible)
    {
        return count > max_array_size<T>();
    }
    else
    {
        return false;
    }
}

template <typename T>
bool is_all_bits_zero(const T& x) noexcept
{
    // checks if scalar type has all bits set to zero
    VX_STATIC_ASSERT(std::is_scalar<T>::value && !std::is_member_pointer<T>::value);

    VX_IF_CONSTEXPR ((std::is_same<T, nullptr_t>::value))
    {
        return true;
    }
    else
    {
        constexpr T zero{};
        return compare(&x, &zero, sizeof(T)) == 0;
    }
}

} // namespace _priv

//=========================================================================

template <typename T>
T* construct_range(T* ptr, size_t count)
{
    //VX_STATIC_ASSERT_MSG(std::is_default_constructible<T>::value, "Type must be default constructible");

    const T* last = ptr + count;
    while (ptr != last)
    {
        construct_in_place(*ptr);
        ++ptr;
    }

    return ptr;
}

template <typename T>
T* construct_range_maybe_trivial(T* ptr, size_t count)
{
    const T* last = ptr + count;
    while (ptr != last)
    {
        construct_in_place_maybe_trivial(*ptr);
        ++ptr;
    }

    return ptr;
}

template <typename T>
T* destroy_range(T* ptr, size_t count) noexcept
{
    const T* last = ptr + count;
    while (ptr != last)
    {
        destroy_in_place(ptr);
        ++ptr;
    }

    return ptr;
}

//=========================================================================

template <typename T, typename U>
constexpr T* fill_range(T* ptr, size_t count, const U& value)
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        // in a constant expression, just do the simple loop
        for (; 0 < count; --count)
        {
            *ptr = value;
            ++ptr;
        }
        return ptr;
    
    }

    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        set(ptr, value, count * sizeof(T));
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_priv::is_all_bits_zero(value))
            {
                set(ptr, 0, count * sizeof(T));
                return ptr + count;
            }
        }

        for (; 0 < count; --count)
        {
            *ptr = value;
            ++ptr;
        }

        return ptr;
    }
}

template <typename T, typename U>
T* fill_uninitialized_range(T* ptr, size_t count, const U& value)
{
    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<T*, U>::value))
    {
        // can optimize with memset
        set(ptr, value, count * sizeof(T));
        return ptr + count;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<T*, U>::value))
        {
            if (_priv::is_all_bits_zero(value))
            {
                set(ptr, 0, count * sizeof(T));
                return ptr + count;
            }
        }

        for (; 0 < count; --count)
        {
            construct_in_place(*ptr, value);
            ++ptr;
        }

        return ptr;
    }
}

template <typename IT, typename U, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
IT fill_range(IT first, IT last, const U& value)
{
    using T = typename type_traits::value_type<IT>::type;

    VX_IF_CONSTEXPR ((type_traits::is_fill_memset_safe<IT, U>::value))
    {
        // can optimize with memset
        const size_t count = static_cast<size_t>(std::distance(first, last));
        set(first, value, count * sizeof(T));
        return last;
    }
    else
    {
        VX_IF_CONSTEXPR ((type_traits::is_fill_zero_memset_safe<IT, U>::value))
        {
            if (_priv::is_all_bits_zero(value))
            {
                const size_t count = static_cast<size_t>(std::distance(first, last));
                set(first, 0, count * sizeof(T));
                return last;
            }
        }

        for (; first != last; ++first)
        {
            *first = value;
        }

        return last;
    }
}

//=========================================================================

template <typename T>
T* copy_range(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = *src;
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename T>
T* copy_uninitialized_range(T* dst, const T* src, size_t count)
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(*dst, *src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_range(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;
    //using U = typename type_traits::value_type<IT2>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        move(dst, first, count * sizeof(T));
        return dst + count;
    }
    else
    {
        //VX_STATIC_ASSERT_MSG((std::is_assignable<T, U>::value), "T must be assignable to U");

        for (; first != last; ++first)
        {
            *dst = *first;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 copy_uninitialized_range(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;
    //using U = typename type_traits::value_type<IT2>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(last - first);
        move(dst, first, count * sizeof(T));
        return dst + count;
    }
    else
    {
        //VX_STATIC_ASSERT_MSG((std::is_assignable<T, U>::value), "T must be assignable to U");

        for (; first != last; ++first)
        {
            construct_in_place(*dst, *first);
            ++dst;
        }

        return dst;
    }
}

//=========================================================================

template <typename T>
T* move_range(T* dst, T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            *dst = move(*src);
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename T>
T* move_uninitialized_range(T* dst, T* src, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(dst, src, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; 0 < count; --count)
        {
            construct_in_place(*dst, move(*src));
            ++src;
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 move_range(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first, last));
        move(dst, first, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            *dst = move(*first);
            ++dst;
        }

        return dst;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 move_uninitialized_range(IT1 dst, IT2 first, IT2 last)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(last - first);
        move(dst, first, count * sizeof(T));
        return dst + count;
    }
    else
    {
        for (; first != last; ++first)
        {
            construct_in_place(*dst, move(*first));
            ++dst;
        }

        return dst;
    }
}

//=========================================================================

template <typename T>
T* shift_forward_range(T* first, T* last, const size_t count) noexcept
{
    if (first == last || count == 0)
    {
        return first + count;
    }

    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(first + count, first, num_elements * sizeof(T));
        return first + count;
    }
    else
    {
        // Must iterate backwards from the end to avoid overwriting elements
        T* src = last;
        T* dst = last + count;
        for (size_t i = 0; i < num_elements; ++i)
        {
            --src;
            --dst;
            *dst = mem::move(*src);
        }
        return first + count;
    }
}

template <typename T>
T* shift_backward_range(T* first, T* last, const size_t count) noexcept
{
    if (first == last || count == 0)
    {
        return first - count;
    }

    const size_t num_elements = static_cast<size_t>(last - first);

    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        move(first - count, first, num_elements * sizeof(T));
        return first - count;
    }
    else
    {
        // Must iterate forwards from the beginning to avoid overwriting elements
        T* src = first;
        T* dst = first - count;
        for (size_t i = 0; i < num_elements; ++i)
        {
            *dst = mem::move(*src);
            ++src;
            ++dst;
        }
        return first - count;
    }
}

//=========================================================================

template <typename T>
T* swap_range(T* a, T* b, size_t count) noexcept
{
    VX_IF_CONSTEXPR (type_traits::memmove_is_safe<T*>::value)
    {
        // For trivially copyable/movable types, swap using a small local block stack buffer
        // to minimize loop overhead and leverage vectorization if possible.
        constexpr size_t buffer_bytes = 64;
        constexpr size_t elements_per_block = buffer_bytes / sizeof(T) > 0 ? buffer_bytes / sizeof(T) : 1;

        T temp_buffer[elements_per_block];

        while (count > 0)
        {
            const size_t current_chunk = std::min(count, elements_per_block);
            const size_t current_bytes = current_chunk * sizeof(T);

            move(temp_buffer, a, current_bytes);
            move(a, b, current_bytes);
            move(b, temp_buffer, current_bytes);

            a += current_chunk;
            b += current_chunk;
            count -= current_chunk;
        }
        return a;
    }
    else
    {
        for (; 0 < count; --count)
        {
            swap(*a, *b);
            ++a;
            ++b;
        }
        return a;
    }
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
IT1 swap_range(IT1 first1, IT1 last1, IT2 first2)
{
    using T = typename type_traits::value_type<IT1>::type;

    VX_IF_CONSTEXPR ((type_traits::memmove_is_safe<IT1, IT2>::value))
    {
        const size_t count = static_cast<size_t>(std::distance(first1, last1));
        swap_range(std::addressof(*first1), std::addressof(*first2), count);
        return first1 + count;
    }
    else
    {
        for (; first1 != last1; ++first1, ++first2)
        {
            swap(*first1, *first2);
        }
        return first1;
    }
}

//=========================================================================

template <typename T>
int compare_range(const T* a, const T* b, size_t count)
{
    return compare(a, b, count * sizeof(T));
}

template <typename T>
int compare_range(const T* a, size_t count_a, const T* b, size_t count_b)
{
    size_t min_count = std::min(count_a, count_b);

    VX_IF_CONSTEXPR ((type_traits::is_bitwise_comparable<T, T>::value))
    {
        const int res = compare_range(a, b, min_count);
        if (res != 0)
        {
            return res;
        }
    }
    else
    {
        for (; 0 < min_count; --min_count)
        {
            if (*a != *b)
            {
                return (*a < *b) ? -1 : 1;
            }

            ++a;
            ++b;
        }
    }

    if (count_a < count_b)
    {
        return -1;
    }

    if (count_a > count_b)
    {
        return 1;
    }

    return 0;
}

template <typename IT1, typename IT2, VX_REQUIRES((type_traits::is_iterator<IT1>::value && type_traits::is_iterator<IT2>::value))>
int compare_range(IT1 first1, IT1 last1, IT2 first2, IT2 last2)
{
    const size_t count1 = static_cast<size_t>(std::distance(first1, last1));
    const size_t count2 = static_cast<size_t>(std::distance(first2, last2));
    size_t min_count = std::min(count1, count2);

    for (; 0 < min_count; --min_count)
    {
        if (*first1 != *first2)
        {
            return (*first1 < *first2) ? -1 : 1;
        }

        ++first1;
        ++first2;
    }

    if (count1 < count2)
    {
        return -1;
    }

    if (count1 > count2)
    {
        return 1;
    }

    return 0;
}

//=========================================================================

template <typename T>
VX_ALLOCATOR T* construct_array(const size_t count)
{
    if (count == 0)
    {
        return nullptr;
    }

    if (_priv::range_will_overflow<T>(count))
    {
        VX_ERR(err::size_error);
        return nullptr;
    }

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

    if (!raw_ptr)
    {
        return nullptr;
    }

    T* ptr = static_cast<T*>(raw_ptr);
    construct_range(static_cast<T*>(ptr), count);
    return static_cast<T*>(ptr);
}

template <typename T>
VX_ALLOCATOR T* construct_array(const size_t count, const T& value)
{
    if (count == 0)
    {
        return nullptr;
    }

    if (_priv::range_will_overflow<T>(count))
    {
        VX_ERR(err::size_error);
        return nullptr;
    }

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

    if (!raw_ptr)
    {
        return nullptr;
    }

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

//=========================================================================
// allocator
//=========================================================================

enum class alignment_policy
{
    at_least,
    exact
};

template <typename T, size_t Alignment = alignof(T), alignment_policy Policy = alignment_policy::at_least>
class default_allocator
{
public:

    VX_STATIC_ASSERT_MSG(Alignment >= alignof(T), "Alignment must be at alignof(T)");
    VX_STATIC_ASSERT_MSG(_priv::is_pow_2(Alignment), "Alignment must be power of 2");

    static constexpr size_t alignment = Alignment;
    static constexpr alignment_policy policy = Policy;

    VX_ALLOCATOR static T* allocate(const size_t count) noexcept
    {
        VX_UNLIKELY_COLD_PATH(count == 0,
            {
                return nullptr;
            });

        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            return static_cast<T*>(mem::allocate(bytes));
        }
        else
        {
            return static_cast<T*>(allocate_aligned<alignment>(bytes));
        }
    }

    static T* reallocate(T* ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            return static_cast<T*>(mem::reallocate(ptr, bytes));
        }
        else
        {
            return static_cast<T*>(reallocate_aligned<alignment>(ptr, bytes));
        }
    }

    static void deallocate(T* ptr, const size_t count) noexcept
    {
        const size_t bytes = count * sizeof(T);

        VX_IF_CONSTEXPR (policy == alignment_policy::at_least && alignment <= max_align)
        {
            mem::deallocate(ptr, bytes);
        }
        else
        {
            deallocate_aligned<alignment>(ptr, bytes);
        }
    }
};

template <typename T>
using aligned_allocator = default_allocator<T, ideal_align, alignment_policy::exact>;

} // namespace mem
} // namespace vx
