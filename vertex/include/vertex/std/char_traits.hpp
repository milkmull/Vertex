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
constexpr int compare(const char_t* a, const char_t* b) noexcept
{
    while ((*a != char_t()) && (*a == *b))
    {
        ++a;
        ++b;
    }
    return (*a < *b) ? -1 : (*a > *b);
}

constexpr int compare(const char* a, const char* b) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare<char>(a, b);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strcmp)
        return __builtin_strcmp(a, b);
#else
        return ::strcmp(a, b);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare<wchar_t>(a, b);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcscmp)
        return __builtin_wcscmp(a, b);
#else
        return ::wcscmp(a, b);
#endif
    }
}

//=========================================================================
// compare
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr int compare_n(const char_t* a, const char_t* b, size_t n) noexcept
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare_n<char>(a, b, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strncmp)
        return __builtin_strncmp(a, b, n);
#else
        return ::strncmp(a, b, n);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return compare_n<wchar_t>(a, b, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcsncmp)
        return __builtin_wcsncmp(a, b, n);
#else
        return ::wcsncmp(a, b, n);
#endif
    }
}

//=========================================================================
// copy
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* copy(char_t* dst, const char_t* src) noexcept
{
    char_t* out = dst;
    while ((*dst++ = *src++) != char_t()) {}
    return out;
}

constexpr char* copy(char* dst, const char* src) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy<char>(dst, src);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strcpy)
        return __builtin_strcpy(dst, src);
#else
        return ::strcpy(dst, src);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy<wchar_t>(dst, src);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcscpy)
        return __builtin_wcscpy(dst, src);
#else
        return ::wcscpy(dst, src);
#endif
    }
}

//=========================================================================
// copy_n (copy up to n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* copy_n(char_t* dst, const char_t* src, size_t n) noexcept
{
    for (size_t i = 0; i != n; ++i)
    {
        dst[i] = src[i];
    }

    return dst;
}

constexpr char* copy_n(char* dst, const char* src, size_t n) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy_n<char>(dst, src, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strncpy)
        return __builtin_strncpy(dst, src, n);
#else
        return ::strncpy(dst, src, n);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy_n<wchar_t>(dst, src, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcsncpy)
        return __builtin_wcsncpy(dst, src, n);
#else
        return ::wcsncpy(dst, src, n);
#endif
    }
}

//=========================================================================
// assign
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* assign(char_t* s, char_t c) noexcept
{
    for (char_t* it = s; *it != char_t(); ++it)
    {
        *it = c;
    }

    return s;
}

//=========================================================================
// assign_n (assign a character to up to n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* assign_n(char_t* s, size_t n, char_t c) noexcept
{
    for (char_t* it = s; 0 < n; --n, ++it)
    {
        *it = c;
    }

    return s;
}

constexpr char* assign_n(char* s, size_t n, char c) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return assign_n<char>(s, n, c);
    }
    else
    {
        return static_cast<char*>(mem::set(s, c, n));
    }
}

#if defined(__cpp_char8_t)
constexpr char8_t* assign_n(char8_t* s, size_t n, char8_t c) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return assign_n<char8_t>(s, n, c);
    }
    else
    {
        return static_cast<char8_t*>(mem::set(s, c, n));
    }
}
#endif

constexpr wchar_t* assign_n(wchar_t* s, size_t n, wchar_t c) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return assign_n<wchar_t>(s, n, c);
    }
    else
    {
        return static_cast<wchar_t*>(::wmemset(s, c, n));
    }
}

//=========================================================================
// length
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr size_t length(const char_t* src) noexcept
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return length<char>(s);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strlen)
        return __builtin_strlen(s);
#else
        return ::strlen(s);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return length<wchar_t>(s);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcslen)
        return __builtin_wcslen(s);
#else
        return ::wcslen(s);
#endif
    }
}

//=========================================================================
// concat
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* concat(char_t* dst, const char_t* src) noexcept
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat<char>(dst, src);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strcat)
        return __builtin_strcat(dst, src);
#else
        return ::strcat(dst, src);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat<wchar_t>(dst, src);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcscat)
        return __builtin_wcscat(dst, src);
#else
        return ::wcscat(dst, src);
#endif
    }
}

//=========================================================================
// concat_n (concatenate up to n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr char_t* concat_n(char_t* dst, const char_t* src, size_t n) noexcept
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat_n<char>(dst, src, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strncat)
        return __builtin_strncat(dst, src, n);
#else
        return ::strncat(dst, src, n);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return concat_n<wchar_t>(dst, src, n);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcsncat)
        return __builtin_wcsncat(dst, src, n);
#else
        return ::wcsncat(dst, src, n);
#endif
    }
}

//=========================================================================
// find (first character)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr const char_t* find(const char_t* s, char_t c) noexcept
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<char>(s, c);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_strchr)
        return __builtin_strchr(s, c);
#else
        return ::strchr(s, c);
#endif
    }
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
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return find<wchar_t>(s, c);
    }
    else
    {
#if VX_HAS_BUILTIN(__builtin_wcschr)
        return __builtin_wcschr(s, c);
#else
        return ::wcschr(s, c);
#endif
    }
}

//=========================================================================
// find_n (first character within first n characters)
//=========================================================================

template <typename char_t, VX_REQUIRES(type_traits::is_char<char_t>::value)>
constexpr const char_t* find_n(const char_t* s, size_t n, char_t c) noexcept
{
    for (; 0 < n; --n, ++s)
    {
        if (*s == c)
        {
            return s;
        }
    }

    return nullptr;
}

namespace _priv {

//=========================================================================
// character traits
//=========================================================================

template <typename char_t, typename int_t, typename pos_t>
struct char_traits_base
{
    using char_type = char_t;
    using int_type = int_t;
    using off_type = std::streamoff;
    using pos_type = pos_t;
    using state_type = std::mbstate_t;

    static void copy(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...)
        str::copy_n(dst, src, count);
    }

    static void move(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...), allowing overlap
        mem::move(dst, src, count * sizeof(char_type));
    }

    // For compare/length/find/assign, we can't uniformly call CRT functions (or their builtin versions).
    // 8-bit: memcmp/strlen/memchr/memset; 16-bit: wmemcmp/wcslen/wmemchr/wmemset; 32-bit: nonexistent

    static constexpr int compare(const char_type* a, const char_type* b, size_t count) noexcept
    {
        // compare [a, a + count) with [b, ...)
        return str::compare_n(a, b, count);
    }

    static constexpr size_t length(const char_type* s) noexcept
    {
        // find length of null-terminated sequence
        return str::length(s);
    }

    static constexpr const char_type* find(const char_type* s, size_t count, const char_type& c) noexcept
    {
        // look for c in [s, s + count)
        return str::find_n(s, count, c);
    }

    static char_type* assign(char_type* const s, size_t count, const char_type c) noexcept
    {
        // assign count * c to [s, ...)
        return str::assign_n(s, count, c);
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

    static constexpr char_type to_char_type(const int_type i) noexcept
    {
        return static_cast<char_type>(i);
    }

    static constexpr int_type to_int_type(const char_type c) noexcept
    {
        return static_cast<int_type>(c);
    }

    static constexpr bool eq_int_type(const int_type lhs, const int_type rhs) noexcept
    {
        return lhs == rhs;
    }

    static constexpr int_type not_eof(const int_type i) noexcept
    {
        return i != eof() ? i : !eof();
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(EOF);
    }
};

} // namespace _priv

template <typename char_t>
struct char_traits : _priv::char_traits_base<char_t, long, std::fpos<std::mbstate_t>> {};

template <>
struct char_traits<char> : _priv::char_traits_base<char, int, std::streampos> {};

template <>
struct char_traits<wchar_t> : _priv::char_traits_base<wchar_t, wint_t, std::wstreampos> {};

#if defined(__cpp_char8_t)
template <>
struct char_traits<char8_t> : _priv::char_traits_base<char8_t, unsigned int, std::u8streampos> {};
#endif // defined(__cpp_char8_t)

template <>
struct char_traits<char16_t> : _priv::char_traits_base<char16_t, uint_least16_t, std::u16streampos> {};

template <>
struct char_traits<char32_t> : _priv::char_traits_base<char32_t, uint_least32_t, std::u32streampos> {};

//=========================================================================
// traits
//=========================================================================

namespace _priv {

template <typename Traits>
using traits_char_t = typename Traits::char_type;

template <typename Traits>
using traits_ptr_t = const typename Traits::char_type*;

#define VX_USE_VECTOR_ALGORITHMS 1

// signed char and other unsigned integral types are supported as an extension.
// Use of other arithmetic types and nullptr_t should be rejected.
template <typename T>
struct is_implementation_handled_char_like_type
{
    static constexpr bool value = std::is_arithmetic<T>::value || std::is_null_pointer<T>::value;
};

template <typename>
struct is_implementation_handled_char_traits : std::false_type {};

template <typename T>
struct is_implementation_handled_char_traits<char_traits<T>> : is_implementation_handled_char_like_type<T> {};

//=========================================================================

template <typename Traits>
constexpr bool traits_equal(
    const traits_ptr_t<Traits> left, const size_t left_size,
    const traits_ptr_t<Traits> right, const size_t right_size) noexcept
{
    // compare [left, left + left_size) to [right, right + right_size) for equality using Traits
    if (left_size != right_size)
    {
        return false;
    }

    if (left_size == 0)
    {
        return true;
    }

    return Traits::compare(left, right, left_size) == 0;
}

//=========================================================================

template <typename Traits>
constexpr int traits_compare(
    const traits_ptr_t<Traits> left, const size_t left_size,
    const traits_ptr_t<Traits> right, const size_t right_size) noexcept
{
    // compare [left, left + left_size) to [right, right + right_size) using Traits
    const int res = Traits::compare(left, right, std::min(left_size, right_size));

    if (res != 0)
    {
        return res;
    }

    if (left_size < right_size)
    {
        return -1;
    }

    if (left_size > right_size)
    {
        return 1;
    }

    return 0;
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find(
    const traits_ptr_t<Traits> haystack, const size_t hay_size,
    const size_t start,
    const traits_ptr_t<Traits> needle, const size_t needle_size) noexcept
{
    // search [haystack, haystack + hay_size) for [needle, needle + needle_size), at/after start
    if (needle_size > hay_size || start > hay_size - needle_size)
    {
        // xpos cannot exist, report failure
        // N4950 [string.view.find]/3 says:
        // 1. start <= xpos
        // 2. xpos + needle_size <= hay_size;
        // therefore:
        // 3. needle_size <= hay_size (by 2) (checked above)
        // 4. start + needle_size <= hay_size (substitute 1 into 2)
        // 5. start <= hay_size - needle_size (4, move needle_size to other side) (also checked above)
        return static_cast<size_t>(-1);
    }

    if (needle_size == 0)
    {
        // empty string always matches if xpos is possible
        return start;
    }

#if VX_USE_VECTOR_ALGORITHMS

    VX_IF_CONSTEXPR(is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto _End = haystack + hay_size;
            const auto _Ptr = _STD _Search_vectorized(haystack + start, _End, needle, needle_size);

            if (_Ptr != _End)
            {
                return static_cast<size_t>(_Ptr - haystack);
            }
            else
            {
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // VX_USE_VECTOR_ALGORITHMS

    const auto end = haystack + (hay_size - needle_size) + 1;

    for (auto it = haystack + start;; ++it)
    {
        it = Traits::find(it, static_cast<size_t>(end - it), *needle);

        if (!it)
        {
            // didn't find first character; report failure
            return static_cast<size_t>(-1);
        }

        if (Traits::compare(it, needle, needle_size) == 0)
        {
            // found match
            return static_cast<size_t>(it - haystack);
        }
    }
}

} // namespace _priv

} // namespace str
} // namespace vx
