#pragma once

#include <cstdio>
#include <string.h>
#include <wchar.h>

#include "vertex/config/type_traits.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace str {

//=========================================================================
// compare
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr int compare(const char_t* a, const char_t* b) noexcept
{
    while (*a && (*a == *b))
    {
        ++a;
        ++b;
    }
    return (*a < *b) ? -1 : (*a > *b);
}

constexpr int compare(const char* a, const char* b) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strcmp)
    return __builtin_strcmp(a, b);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare<char>(a, b);
    }
    else
    {
        return ::strcmp(a, b);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr int compare(const char8_t* a, const char8_t* b) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strcmp)
    return __builtin_u8strcmp(a, b);
    #else
    return compare<char8_t>(a, b);
    #endif
}
#endif

constexpr int compare(const wchar_t* a, const wchar_t* b) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcscmp)
    return __builtin_wcscmp(a, b);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare<wchar_t>(a, b);
    }
    else
    {
        return ::wcscmp(a, b);
    }
#endif
}

//=========================================================================
// compare
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr int compare_n(const char_t* a, const char_t* b, size_t n) noexcept
{
    for (; 0 < n; --n, ++a, ++b)
    {
        if (*a != *b)
        {
            return *a < *b ? -1 : +1;
        }
    }
    return 0;
}

constexpr int compare_n(const char* a, const char* b, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strncmp)
    return __builtin_strncmp(a, b, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare_n<char>(a, b, n);
    }
    else
    {
        return ::strncmp(a, b, n);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr int compare_n(const char8_t* a, const char8_t* b, size_t n) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strncmp)
    return __builtin_u8strncmp(a, b, n);
    #else
    return compare_n<char8_t>(a, b, n);
    #endif
}
#endif

constexpr int compare_n(const wchar_t* a, const wchar_t* b, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcsncmp)
    return __builtin_wcsncmp(a, b, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare_n<wchar_t>(a, b, n);
    }
    else
    {
        return ::wcsncmp(a, b, n);
    }
#endif
}

//=========================================================================
// copy
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr char_t* copy(char_t* dst, const char_t* src) noexcept
{
    char_t* out = dst;
    while ((*dst++ = *src++) != char_t()) {}
    return out;
}

constexpr char* copy(char* dst, const char* src) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strcpy)
    return __builtin_strcpy(dst, src);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy<char>(dst, src);
    }
    else
    {
        return ::strcpy(dst, src);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr char8_t* copy(char8_t* dst, const char8_t* src) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strcpy)
    return __builtin_u8strcpy(dst, src);
    #else
    return copy<char8_t>(dst, src);
    #endif
}
#endif

constexpr wchar_t* copy(wchar_t* dst, const wchar_t* src) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcscpy)
    return __builtin_wcscpy(dst, src);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy<wchar_t>(dst, src);
    }
    else
    {
        return ::wcscpy(dst, src);
    }
#endif
}

//=========================================================================
// copy_n (copy up to n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr char_t* copy_n(char_t* dst, const char_t* src, size_t n) noexcept
{
    char_t* out = dst;

    size_t i = 0;
    for (; i < n && src[i] != char_t(); ++i)
    {
        dst[i] = src[i];
    }
    // If src is shorter than n, pad the rest with null chars
    for (; i < n; ++i)
    {
        dst[i] = char_t();
    }

    return out;
}

constexpr char* copy_n(char* dst, const char* src, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strncpy)
    return __builtin_strncpy(dst, src, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy_n<char>(dst, src, n);
    }
    else
    {
        return ::strncpy(dst, src, n);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr char8_t* copy_n(char8_t* dst, const char8_t* src, size_t n) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strncpy)
    return __builtin_u8strncpy(dst, src, n);
    #else
    return copy_n<char8_t>(dst, src, n);
    #endif
}
#endif

constexpr wchar_t* copy_n(wchar_t* dst, const wchar_t* src, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcsncpy)
    return __builtin_wcsncpy(dst, src, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy_n<wchar_t>(dst, src, n);
    }
    else
    {
        return ::wcsncpy(dst, src, n);
    }
#endif
}

//=========================================================================
// length
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr size_t length(const char_t* src) noexcept
{
    size_t count = 0;
    while (*src != char_t())
    {
        ++count;
        ++src;
    }

    return count;
}

constexpr size_t length(const char* s) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strlen)
    return __builtin_strlen(s);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return length<char>(s);
    }
    else
    {
        return ::strlen(s);
    }
#endif
}

#if defined(__cpp_char8_t)

constexpr size_t length(const char8_t* s) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strlen)
    return __builtin_u8strlen(s);
    #else
    return length<char8_t>(s);
    #endif
}

#endif // defined(__cpp_char8_t)

constexpr size_t length(const wchar_t* s) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcslen)

    return __builtin_wcslen(s);

#else

    if (VX_IS_CONSTANT_EVALUATED())
    {
        return length<wchar_t>(s);
    }
    else
    {
        return ::wcslen(s);
    }

#endif
}

//=========================================================================
// concat
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr char_t* concat(char_t* dst, const char_t* src) noexcept
{
    char_t* out = dst;

    // advance dst to end of existing string
    while (*dst != char_t())
    {
        ++dst;
    }

    // copy src including terminating null
    while ((*dst++ = *src++) != char_t())
    {
    }

    return out;
}

constexpr char* concat(char* dst, const char* src) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strcat)
    return __builtin_strcat(dst, src);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat<char>(dst, src);
    }
    else
    {
        return ::strcat(dst, src);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr char8_t* concat(char8_t* dst, const char8_t* src) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strcat)
    return __builtin_u8strcat(dst, src);
    #else
    return concat<char8_t>(dst, src);
    #endif
}
#endif

constexpr wchar_t* concat(wchar_t* dst, const wchar_t* src) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcscat)
    return __builtin_wcscat(dst, src);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat<wchar_t>(dst, src);
    }
    else
    {
        return ::wcscat(dst, src);
    }
#endif
}

//=========================================================================
// concat_n (concatenate up to n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr char_t* concat_n(char_t* dst, const char_t* src, size_t n) noexcept
{
    char_t* out = dst;

    while (*dst != char_t())
    {
        ++dst;
    }

    size_t i = 0;
    for (; i < n && src[i] != char_t(); ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = char_t(); // null-terminate

    return out;
}

constexpr char* concat_n(char* dst, const char* src, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strncat)
    return __builtin_strncat(dst, src, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat_n<char>(dst, src, n);
    }
    else
    {
        return ::strncat(dst, src, n);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr char8_t* concat_n(char8_t* dst, const char8_t* src, size_t n) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strncat)
    return __builtin_u8strncat(dst, src, n);
    #else
    return concat_n<char8_t>(dst, src, n);
    #endif
}
#endif

constexpr wchar_t* concat_n(wchar_t* dst, const wchar_t* src, size_t n) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcsncat)
    return __builtin_wcsncat(dst, src, n);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat_n<wchar_t>(dst, src, n);
    }
    else
    {
        return ::wcsncat(dst, src, n);
    }
#endif
}

//=========================================================================
// find (first character)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr const char_t* find(const char_t* s, char_t c) noexcept
{
    while (*s)
    {
        if (*s == c)
        {
            return s;
        }

        ++s;
    }

    return (c == char_t()) ? s : nullptr;
}

constexpr const char* find(const char* s, char c) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strchr)
    return __builtin_strchr(s, c);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<char>(s, c);
    }
    else
    {
        return ::strchr(s, c);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr const char8_t* find(const char8_t* s, char8_t c) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strchr)
    return __builtin_u8strchr(s, c);
    #else
    return find<char8_t>(s, c);
    #endif
}
#endif

constexpr const wchar_t* find(const wchar_t* s, wchar_t c) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcschr)
    return __builtin_wcschr(s, c);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<wchar_t>(s, c);
    }
    else
    {
        return ::wcschr(s, c);
    }
#endif
}

//=========================================================================
// find (substring)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr const char_t* find(const char_t* haystack, const char_t* needle) noexcept
{
    if (*needle == char_t())
    {
        return haystack;
    }

    for (; *haystack; ++haystack)
    {
        const char_t* h = haystack;
        const char_t* n = needle;

        while (*h && *n && (*h == *n))
        {
            ++h;
            ++n;
        }

        if (*n == char_t())
        {
            return haystack;
        }
    }

    return nullptr;
}

constexpr const char* find(const char* s, const char* sub) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strstr)
    return __builtin_strstr(s, sub);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<char>(s, sub);
    }
    else
    {
        return ::strstr(s, sub);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr const char8_t* find(const char8_t* s, const char8_t* sub) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strstr)
    return __builtin_u8strstr(s, sub);
    #else
    return find<char8_t>(s, sub);
    #endif
}
#endif

constexpr const wchar_t* find(const wchar_t* s, const wchar_t* sub) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcsstr)
    return __builtin_wcsstr(s, sub);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<wchar_t>(s, sub);
    }
    else
    {
        return ::wcsstr(s, sub);
    }
#endif
}

//=========================================================================
// rfind (last character)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr const char_t* rfind(const char_t* s, char_t c) noexcept
{
    const char_t* last = nullptr;

    while (*s)
    {
        if (*s == c)
        {
            last = s;
        }
        ++s;
    }

    return (c == char_t()) ? s : last;
}

constexpr const char* rfind(const char* s, char c) noexcept
{
#if VX_HAS_BUILTIN(__builtin_strrchr)
    return __builtin_strrchr(s, c);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rfind<char>(s, c);
    }
    else
    {
        return ::strrchr(s, c);
    }
#endif
}

#if defined(__cpp_char8_t)
constexpr const char8_t* rfind(const char8_t* s, char8_t c) noexcept
{
    #if VX_HAS_BUILTIN(__builtin_u8strrchr)
    return __builtin_u8strrchr(s, c);
    #else
    return rfind<char8_t>(s, c);
    #endif
}
#endif

constexpr const wchar_t* rfind(const wchar_t* s, wchar_t c) noexcept
{
#if VX_HAS_BUILTIN(__builtin_wcsrchr)
    return __builtin_wcsrchr(s, c);
#else
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return rfind<wchar_t>(s, c);
    }
    else
    {
        return ::wcsrchr(s, c);
    }
#endif
}

//=========================================================================
// rfind (substring)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
static constexpr const char_t* rfind(const char_t* haystack, const char_t* needle) noexcept
{
    if (*needle == char_t())
    {
        // empty needle matches at end of haystack
        const char_t* end = haystack;
        while (*end)
        {
            ++end;
        }
        return end;
    }

    const char_t* result = nullptr;
    while (true)
    {
        const char_t* p = find(haystack, needle);
        if (p == nullptr)
        {
            break;
        }

        result = p;
        haystack = p + 1;
    }

    return result;
}

namespace _priv {

//=========================================================================
// character traits
//=========================================================================

template <typename char_t, typename int_t>
struct char_traits
{
    using char_type = char_t;
    using int_type = int_t;

    static void copy(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...)
        mem::copy(dst, src, count * sizeof(char_type));
    }

    static void move(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...), allowing overlap
        mem::move(dst, src, count * sizeof(char_type));
    }

    // For compare/length/find/assign, we can't uniformly call CRT functions (or their builtin versions).
    // 8-bit: memcmp/strlen/memchr/memset; 16-bit: wmemcmp/wcslen/wmemchr/wmemset; 32-bit: nonexistent

    static constexpr int compare(const char_type* first1, const char_type* first2, size_t count) noexcept
    {
        // compare [first1, first1 + count) with [first2, ...)

        for (; 0 < count; --count, ++first1, ++first2)
        {
            if (*first1 != *first2)
            {
                return *first1 < *first2 ? -1 : +1;
            }
        }

        return 0;
    }

    static constexpr size_t length(const char_type* src) noexcept
    {
        // find length of null-terminated sequence

        size_t count = 0;
        while (*src != char_type())
        {
            ++count;
            ++src;
        }

        return count;
    }

    static constexpr const char_type* find(const char_type* src, size_t count, const char_type& c) noexcept
    {
        // look for c in [src, src + count)
        for (; 0 < count; --count, ++src)
        {
            if (*src == c)
            {
                return src;
            }
        }

        return nullptr;
    }

    static char_type* assign(char_type* const src, size_t count, const char_type c) noexcept
    {
        // assign count * c to [src, ...)
        for (char_type* it = src; count > 0; --count, ++it)
        {
            *it = c;
        }

        return src;
    }

    static void assign(char_type& lhs, const char_type& rhs) noexcept
    {
        lhs = rhs;
    }

    static constexpr bool eq(const char_type lhs, const char_type rhs) noexcept
    {
        return lhs == rhs;
    }

    static constexpr bool lt(const char_type lhs, const char_type rhs) noexcept
    {
        return lhs < rhs;
    }

    static constexpr char_type to_char_type(const int_type _Meta) noexcept
    {
        return static_cast<char_type>(_Meta);
    }

    static constexpr int_type to_int_type(const char_type c) noexcept
    {
        return static_cast<int_type>(c);
    }

    static constexpr bool eq_int_type(const int_type lhs, const int_type rhs) noexcept
    {
        return lhs == rhs;
    }

    static constexpr int_type not_eof(const int_type _Meta) noexcept
    {
        return _Meta != eof() ? _Meta : !eof();
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(EOF);
    }
};

template <typename char_t, typename int_t>
struct narrow_char_traits
{
    // Implement char_traits for narrow character types char and char8_t

private:

    using primary_char_traits = char_traits<char_t, int_t>;

public:

    using char_type = char_t;
    using int_type = int_t;

    static void copy(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...)
        primary_char_traits::copy(dst, src, count);
    }

    static void move(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...), allowing overlap
        primary_char_traits::move(dst, src, count);
    }

    static int compare(const char_type* const first1, const char_type* const first2, const size_t count) noexcept
    {
        // compare [first1, first1 + count) with [first2, ...)
        return mem::compare(first1, first2, count);
    }

    static constexpr size_t length(const char_type* const first) noexcept
    {
        // find length of null-terminated string
        if (VX_IS_CONSTANT_EVALUATED())
        {
#if defined(__cpp_char8_t)

            VX_IF_CONSTEXPR((std::is_same<char_type, char8_t>::value))
            {
    #if VX_HAS_BUILTIN(__builtin_u8strlen)
                return __builtin_u8strlen(first);
    #else
                return primary_char_traits::length(first);
    #endif
            }
            else
#else
            {
    #if VX_HAS_BUILTIN(__builtin_strlen)
                return __builtin_strlen(s);
    #else
                return primary_char_traits::length(first);
    #endif
            }
#endif // defined(__cpp_char8_t)
        }
        else
        {
            return str::length(reinterpret_cast<const char*>(first));
        }
    }

    static const char_type* find(const char_type* const first, const size_t count, const char_type& c) noexcept
    {
        // look for c in [first, first + count)
        if (VX_IS_CONSTANT_EVALUATED())
        {
#if defined(__cpp_char8_t)

            VX_IF_CONSTEXPR((std::is_same<char_type, char8_t>::value))
            {
    #if VX_HAS_BUILTIN(__builtin_u8memchr)
                return __builtin_u8memchr(first, c, count);
    #else
                return primary_char_traits::find(first, c, count);
    #endif
            }
            else
#else
            {
    #if VX_HAS_BUILTIN(__builtin_char_memchr)
                return __builtin_char_memchr(s);
    #else
                return primary_char_traits::find(first, c, count);
    #endif
            }
#endif // defined(__cpp_char8_t)
        }
        else
        {
            return static_cast<const char_t>(str::find(reinterpret_cast<const char*>(first), c, count));
        }
    }

    static char_type* assign(char_type* const first, size_t count, const char_type c) noexcept
    {
        // assign count * c to [first, ...)
        return static_cast<char_type*>(mem::set(first, c, count));
    }

    static void assign(char_type& lhs, const char_type& rhs) noexcept
    {
        lhs = rhs;
    }

    static constexpr bool eq(const char_type lhs, const char_type rhs) noexcept
    {
        return lhs == rhs;
    }

    static constexpr bool lt(const char_type lhs, const char_type rhs) noexcept
    {
        return static_cast<unsigned char>(lhs) < static_cast<unsigned char>(rhs);
    }

    static constexpr char_type to_char_type(const int_type _Meta) noexcept
    {
        return static_cast<char_type>(_Meta);
    }

    static constexpr int_type to_int_type(const char_type c) noexcept
    {
        return static_cast<unsigned char>(c);
    }

    static constexpr bool eq_int_type(const int_type lhs, const int_type rhs) noexcept
    {
        return lhs == rhs;
    }

    static constexpr int_type not_eof(const int_type _Meta) noexcept
    {
        return _Meta != eof() ? _Meta : !eof();
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(EOF);
    }
};

//template <class char_type>
//struct wide_char_traits : private char_traits<char_type, unsigned short>
//{
//    // char_traits for the char16_t-likes: char16_t, wchar_t, unsigned short
//
//private:
//
//    using primary_char_traits = char_traits<char_type, unsigned short>;
//
//public:
//
//    using char_type = char_type;
//    using int_type = unsigned short;
//
//    using primary_char_traits::copy;
//    using primary_char_traits::move;
//
//    static int compare(const char_type* const first1, const char_type* const first2, const size_t count) noexcept
//    {
//        // compare [first1, first1 + count) with [first2, ...)
//        return ::wmemcmp(
//            reinterpret_cast<const wchar_t*>(first1),
//            reinterpret_cast<const wchar_t*>(first2),
//            count);
//    }
//
//    static size_t length(const char_type* src) noexcept
//    {
//        // find length of null-terminated sequence
//        return ::wcslen(reinterpret_cast<const wchar_t*>(src));
//    }
//
//    static const char_type* find(const char_type* src, const size_t count, const char_type& c) noexcept
//    {
//        // look for c in [src, src + count)
//        return reinterpret_cast<const char_type*>(::wmemchr(reinterpret_cast<const wchar_t*>(src), c, count));
//    }
//
//    static char_type* assign(char_type* const src, size_t count, const char_type c) noexcept
//    {
//        // assign count * c to [src, ...)
//        return reinterpret_cast<char_type*>(::wmemset(reinterpret_cast<wchar_t*>(src), c, count));
//    }
//
//    static void assign(char_type& lhs, const char_type& rhs) noexcept
//    {
//        lhs = rhs;
//    }
//
//    static constexpr bool eq(const char_type lhs, const char_type rhs) noexcept
//    {
//        return lhs == rhs;
//    }
//
//    static constexpr bool lt(const char_type lhs, const char_type rhs) noexcept
//    {
//        return lhs < rhs;
//    }
//
//    static constexpr char_type to_char_type(const int_type _Meta) noexcept
//    {
//        return _Meta;
//    }
//
//    static constexpr int_type to_int_type(const char_type c) noexcept
//    {
//        return c;
//    }
//
//    static constexpr bool eq_int_type(const int_type lhs, const int_type rhs) noexcept
//    {
//        return lhs == rhs;
//    }
//
//    static constexpr int_type not_eof(const int_type _Meta) noexcept
//    {
//        return _Meta != eof() ? _Meta : static_cast<int_type>(!eof());
//    }
//
//    static constexpr int_type eof() noexcept
//    {
//        return WEOF;
//    }
//};

} // namespace _priv

} // namespace str
} // namespace vx
