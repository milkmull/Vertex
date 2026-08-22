#pragma once

#include <cstring>
#include <limits>

#include "vertex/config/language_config.hpp"

namespace vx {
namespace mem {

//=========================================================================
// array
//=========================================================================

template <typename T, size_t N>
constexpr size_t array_size(const T (&)[N]) noexcept
{
    return N;
}

template <typename T, size_t N>
constexpr size_t literal_size(const T (&)[N]) noexcept
{
    VX_STATIC_ASSERT(N > 0);
    return N - 1;
}

template <typename T>
constexpr size_t max_array_size() noexcept
{
    return std::numeric_limits<size_t>::max() / sizeof(T);
}

//=========================================================================
// memory management (unaligned)
//=========================================================================

inline VX_ALLOCATOR VX_NO_DISCARD void* allocate(const size_t bytes) noexcept
{
    return ::malloc(bytes);
}

inline void deallocate_raw(void* ptr) noexcept
{
    ::free(ptr);
}

inline void deallocate(void* ptr, VX_MAYBE_UNUSED const size_t bytes) noexcept
{
    ::free(ptr);
}

inline VX_NO_DISCARD void* reallocate(void* ptr, const size_t bytes) noexcept
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
        const auto d = const_cast<char*>(reinterpret_cast<const volatile char*>(dst));
        const auto s = const_cast<const char*>(reinterpret_cast<const volatile char*>(src));

#if VX_HAS_BUILTIN(__builtin_memcpy)
        return __builtin_memcpy(d, s, bytes);
#else
        return ::memcpy(d, s, bytes);
#endif
    }
}

inline constexpr void* move(void* dst, const void* src, const size_t bytes) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        char* d = static_cast<char*>(dst);
        const char* s = static_cast<const char*>(src);

        if (bytes == 0)
        {
            return d;
        }

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
        const auto d = const_cast<char*>(reinterpret_cast<const volatile char*>(dst));
        const auto s = const_cast<const char*>(reinterpret_cast<const volatile char*>(src));

#if VX_HAS_BUILTIN(__builtin_memmove)
        return __builtin_memmove(d, s, bytes);
#else
        return ::memmove(d, s, bytes);
#endif
    }
}

inline constexpr void* set(void* dst, const int value, const size_t bytes) noexcept
{
    const auto v = static_cast<char>(value);

    if (VX_IS_CONSTANT_EVALUATED())
    {
        char* d = static_cast<char*>(dst);

        for (size_t i = 0; i < bytes; ++i)
        {
            d[i] = v;
        }

        return dst;
    }
    else
    {
        const auto d = const_cast<char*>(reinterpret_cast<const volatile char*>(dst));

#if VX_HAS_BUILTIN(__builtin_memset)
        return __builtin_memset(d, static_cast<int>(v), bytes);
#else
        return ::memset(d, static_cast<int>(v), bytes);
#endif
    }
}

inline constexpr int compare(const void* a, const void* b, const size_t bytes) noexcept
{
    const auto ca = static_cast<const char*>(a);
    const auto cb = static_cast<const char*>(b);

    if (VX_IS_CONSTANT_EVALUATED())
    {
        for (size_t i = 0; i < bytes; ++i)
        {
            if (ca[i] != cb[i])
            {
                return (static_cast<unsigned char>(ca[i]) < static_cast<unsigned char>(cb[i])) ? -1 : 1;
            }
        }

        return 0;
    }

#if VX_HAS_BUILTIN(__builtin_memcmp)
    return __builtin_memcmp(ca, cb, bytes);
#else
    return ::memcmp(ca, cb, bytes);
#endif
}

inline constexpr const void* find(const void* src, const int value, const size_t bytes) noexcept
{
    const auto s = static_cast<const char*>(src);
    const auto v = static_cast<char>(value);

    if (VX_IS_CONSTANT_EVALUATED())
    {
        for (size_t i = 0; i < bytes; ++i)
        {
            if (s[i] == v)
            {
                return s + i;
            }
        }

        return nullptr;
    }

#if VX_HAS_BUILTIN(__builtin_memchr)
    return __builtin_memchr(s, v, bytes);
#else
    return ::memchr(s, v, bytes);
#endif
}

} // namespace mem
} // namespace vx
