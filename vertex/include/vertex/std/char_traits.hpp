#pragma once

#include <cstdio>
#include <sstream>
#include <string.h>
#include <wchar.h>

#include "vertex/config/type_traits.hpp"
#include "vertex/std/_priv/simd_algorithms.hpp"
#include "vertex/std/crypto/fnv1a.hpp"
#include "vertex/std/memory.hpp"

namespace vx {
namespace str {

namespace _priv {

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
// compare_n
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
        VX_DISABLE_WARNING_PUSH();
        VX_DISABLE_WARNING("-Wdeprecated-declarations", 4996); // for strcpy
        return ::strcpy(dst, src);
        VX_DISABLE_WARNING_POP();
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
        VX_DISABLE_WARNING_PUSH();
        VX_DISABLE_WARNING("-Wdeprecated-declarations", 4996); // for strcpy
        return ::wcscpy(dst, src);
        VX_DISABLE_WARNING_POP();
#endif
    }
}

//=========================================================================
// copy_n
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
        return static_cast<char*>(mem::copy(dst, src, n));
    }
}

#if defined(__cpp_char8_t)
constexpr char8_t* copy_n(char8_t* dst, const char8_t* src, size_t n) noexcept
{
    if (VX_IS_CONSTANT_EVALUATED())
    {
        return copy_n<char8_t>(dst, src, n);
    }
    else
    {
        return static_cast<char8_t*>(mem::copy(dst, src, n));
    }
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
#if VX_HAS_BUILTIN(__builtin_wmemcpy)
        return __builtin_wmemcpy(dst, src, n);
#else
        return ::wmemcpy(dst, src, n);
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
// assign_n
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
// find_n
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

//=========================================================================
// character Traits
//=========================================================================

template <typename char_t, typename int_t, typename pos_t>
struct char_traits_base
{
    using char_type = char_t;
    using int_type = int_t;
    using off_type = std::streamoff;
    using pos_type = pos_t;
    using state_type = std::mbstate_t;

    static char_type* copy(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...)
        return _priv::copy_n(dst, src, count);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    static char_type* copy_range(char_type* dst, IT first, IT last) noexcept
    {
        for (; first != last; ++first)
        {
            _priv::assign(*dst, *first);
            ++dst;
        }

        return dst;
    }

    static char_type* move(char_type* const dst, const char_type* const src, const size_t count) noexcept
    {
        // copy [src, src + count) to [dst, ...), allowing overlap
        mem::move(dst, src, count * sizeof(char_type));
        return dst;
    }

    // For compare/length/find/assign, we can't uniformly call CRT functions (or their builtin versions).
    // 8-bit: memcmp/strlen/memchr/memset; 16-bit: wmemcmp/wcslen/wmemchr/wmemset; 32-bit: nonexistent

    static constexpr int compare(const char_type* a, const char_type* b, size_t count) noexcept
    {
        // compare [a, a + count) with [b, ...)
        return _priv::compare_n(a, b, count);
    }

    static constexpr size_t length(const char_type* s) noexcept
    {
        // find length of null-terminated sequence
        return _priv::length(s);
    }

    static constexpr const char_type* find(const char_type* s, size_t count, const char_type& c) noexcept
    {
        // look for c in [s, s + count)
        return _priv::find_n(s, count, c);
    }

    static char_type* assign(char_type* const s, size_t count, const char_type c) noexcept
    {
        // assign count * c to [s, ...)
        return _priv::assign_n(s, count, c);
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

    static constexpr size_t hash(const char_type* const s, const size_t count) noexcept
    {
        fnv1a fnv;
        fnv.update(reinterpret_cast<const unsigned char*>(s), count * sizeof(char_type));
        return fnv.result();
    }
};

} // namespace _priv

//=========================================================================
// Traits
//=========================================================================

template <typename char_t>
struct char_traits : _priv::char_traits_base<char_t, long, std::fpos<std::mbstate_t>>
{
};

template <>
struct char_traits<char> : _priv::char_traits_base<char, int, std::streampos>
{
};

template <>
struct char_traits<wchar_t> : _priv::char_traits_base<wchar_t, short, std::wstreampos>
{
};

#if defined(__cpp_char8_t)
template <>
struct char_traits<char8_t> : _priv::char_traits_base<char8_t, int, std::u8streampos>
{
};
#endif // defined(__cpp_char8_t)

template <>
struct char_traits<char16_t> : _priv::char_traits_base<char16_t, int_least16_t, std::u16streampos>
{
};

template <>
struct char_traits<char32_t> : _priv::char_traits_base<char32_t, int_least32_t, std::u32streampos>
{
};

//=========================================================================

namespace _priv {

template <typename Traits>
using traits_char_t = typename Traits::char_type;

template <typename Traits>
using traits_ptr_t = const typename Traits::char_type*;

// signed char and other unsigned integral types are supported as an extension.
// Use of other arithmetic types and nullptr_t should be rejected.
template <typename T>
struct is_implementation_handled_char_like_type
{
    static constexpr bool value = std::is_arithmetic<T>::value || std::is_null_pointer<T>::value;
};

template <typename>
struct is_implementation_handled_char_traits : std::false_type
{
};

template <typename T>
struct is_implementation_handled_char_traits<char_traits<T>> : is_implementation_handled_char_like_type<T>
{
};

//=========================================================================

template <typename Traits>
constexpr bool traits_equal(
    traits_ptr_t<Traits> left,
    const size_t left_size,
    traits_ptr_t<Traits> right,
    const size_t right_size) noexcept
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
    traits_ptr_t<Traits> left,
    const size_t left_size,
    traits_ptr_t<Traits> right,
    const size_t right_size) noexcept
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
constexpr traits_char_t<Traits>* traits_remove(traits_char_t<Traits>* s, const size_t size, const traits_char_t<Traits> c) noexcept
{
#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = s + size;
            return _simd::remove_simd(s, end, c);
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    const auto end = s + size;
    auto next = s;

    while (++s != end)
    {
        if (!Traits::eq(*s, c))
        {
            Traits::assign(*next, *s);
            ++next;
        }
    }

    return next;
}

//=========================================================================

template <typename Traits>
constexpr void traits_reverse(traits_char_t<Traits>* s, const size_t size) noexcept
{
    if (size == 0)
    {
        return;
    }

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            _simd::reverse_simd<sizeof(traits_char_t<Traits>)>(s, s + size);
            return;
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    auto last = s + size;

    for (; s != last && s != --last; ++s)
    {
        std::swap(*s, *last);
    }
}

//=========================================================================

template <typename Traits>
constexpr void traits_replace(
    traits_char_t<Traits>* s,
    const size_t size,
    const traits_char_t<Traits> old_val,
    const traits_char_t<Traits> new_val) noexcept
{
    if (size == 0)
    {
        return;
    }

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value && _simd::replace_is_safe<traits_char_t<Traits>>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = s + size;
            _simd::replace_simd(s, end, old_val, new_val);
            return;
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    const auto last = s + size;

    for (; s != last; ++s)
    {
        if (*s == old_val)
        {
            *s = new_val;
        }
    }
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_count(traits_ptr_t<Traits> s, const size_t size, const traits_char_t<Traits> c) noexcept
{
    if (size == 0)
    {
        return 0;
    }

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = s + size;
            return _simd::count_simd(s, end, c);
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    size_t n = 0;

    for (size_t i = 0; i < size; ++i, ++s)
    {
        if (Traits::eq(*s, c))
        {
            ++n;
        }
    }

    return n;
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find(
    traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start,
    traits_ptr_t<Traits> needle,
    const size_t needle_size) noexcept
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

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = haystack + hay_size;
            const auto ptr = _simd::search_simd(haystack + start, end, needle, needle_size);

            if (ptr != end)
            {
                return static_cast<size_t>(ptr - haystack);
            }
            else
            {
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

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

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_ch(traits_ptr_t<Traits> haystack, const size_t hay_size, const size_t start_at, const traits_char_t<Traits> c) noexcept
{
    // search [haystack, haystack + hay_size) for c, at/after start_at
    if (start_at >= hay_size)
    {
        // (npos) no room for match
        return static_cast<size_t>(-1);
    }

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = haystack + hay_size;
            const auto ptr = _simd::find_simd(haystack + start_at, end, c);

            if (ptr != end)
            {
                return static_cast<size_t>(ptr - haystack);
            }
            else
            {
                // (npos) no match
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    const auto found_at = Traits::find(haystack + start_at, hay_size - start_at, c);
    if (found_at)
    {
        return static_cast<size_t>(found_at - haystack);
    }

    // (npos) no match
    return static_cast<size_t>(-1);
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_rfind(traits_ptr_t<Traits> haystack, const size_t hay_size, const size_t start_at, traits_ptr_t<Traits> needle, const size_t needle_size) noexcept
{
    // search [haystack, haystack + hay_size) for [needle, needle + needle_size) beginning before start_at
    if (needle_size == 0)
    {
        // empty string always matches
        return std::min(start_at, hay_size);
    }

    if (needle_size > hay_size)
    {
        // no room for match
        return static_cast<size_t>(-1);
    }

    const size_t actual_start_at = std::min(start_at, hay_size - needle_size);

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            // find_end_simd takes into account the needle length when locating the search start.
            // As a potentially earlier start position can be specified, we need to take it into account,
            // and pick between the maximum possible start position and the specified one,
            // and then add needle_size, so that it is subtracted back in find_end_simd.
            const auto end = haystack + actual_start_at + needle_size;
            const auto ptr = _simd::find_end_simd(haystack, end, needle, needle_size);

            if (ptr != end)
            {
                return static_cast<size_t>(ptr - haystack);
            }
            else
            {
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    for (auto match_try = haystack + actual_start_at;; --match_try)
    {
        if (Traits::eq(*match_try, *needle) && Traits::compare(match_try, needle, needle_size) == 0)
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }

        if (match_try == haystack)
        {
            // at beginning, no more chance for match
            return static_cast<size_t>(-1);
        }
    }
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_rfind_ch(traits_ptr_t<Traits> haystack, const size_t hay_size, const size_t start_at, const traits_char_t<Traits> c) noexcept
{
    // search [haystack, haystack + hay_size) for c before start_at

    if (hay_size == 0)
    {
        // no room for match
        return static_cast<size_t>(-1);
    }

    const size_t actual_start_at = std::min(start_at, hay_size - 1);

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto end = haystack + actual_start_at + 1;
            const auto ptr = _simd::find_last_simd(haystack, end, c);

            if (ptr != end)
            {
                return static_cast<size_t>(ptr - haystack);
            }
            else
            {
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    for (auto match_try = haystack + actual_start_at;; --match_try)
    {
        if (Traits::eq(*match_try, c))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }

        if (match_try == haystack)
        {
            // at beginning, no more chance for match
            return static_cast<size_t>(-1);
        }
    }
}

//=========================================================================

// string_bitmap for character types
template <typename char_t, bool = type_traits::is_char<char_t>::value>
class string_bitmap
{
public:

    constexpr bool mark(const char_t* first, const char_t* const last) noexcept
    {
        // mark this bitmap such that the characters in [first, last) are intended to match
        // returns whether all inputs can be placed in the bitmap
        for (; first != last; ++first)
        {
            m_matches[static_cast<unsigned char>(*first)] = true;
        }

        return true;
    }

    constexpr bool match(const char_t c) const noexcept
    { // test if c is in the bitmap
        // CodeQL [SM01954] This index is valid: we cast to unsigned char and the array has 256 elements.
        return m_matches[static_cast<unsigned char>(c)];
    }

private:

    bool m_matches[256] = {};
};

// string_bitmap for wchar_t/unsigned short/char16_t/char32_t/etc. types
template <typename char_t>
class string_bitmap<char_t, false>
{
public:

    VX_STATIC_ASSERT_MSG(std::is_unsigned<char_t>::value,
        "Standard char_traits is only provided for char, wchar_t, char8_t, char16_t, "
        "and char32_t. See N4988 [char.Traits].");

    constexpr bool mark(const char_t* first, const char_t* const last) noexcept
    {
        // mark this bitmap such that the characters in [first, last) are intended to match
        // returns whether all inputs can be placed in the bitmap
        for (; first != last; ++first)
        {
            const auto c = *first;
            if (c >= 256U)
            {
                return false;
            }

            m_matches[static_cast<unsigned char>(c)] = true;
        }

        return true;
    }

    constexpr bool match(const char_t c) const noexcept
    {
        // test if c is in the bitmap
        return c < 256U && m_matches[c];
    }

private:

    bool m_matches[256] = {};
};

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_first_of(
    traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    traits_ptr_t<Traits> needle,
    const size_t needle_size) noexcept
{
    // in [haystack, haystack + hay_size), look for one of [needle, needle + needle_size), at/after start_at

    if (needle_size == 0 || start_at >= hay_size)
    {
        // no match possible
        return static_cast<size_t>(-1);
    }

    const auto hay_start = haystack + start_at;
    const auto hay_end = haystack + hay_size;

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
#if _VX_USE_SIMD_ALGORITHMS

        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const size_t remaining_size = hay_size - start_at;
            if (remaining_size + needle_size >= _VX_SIMD_THRESHOLD_FIND_FIRST_OF)
            {
                size_t pos = _simd::find_first_of_pos_simd(hay_start, remaining_size, needle, needle_size);
                if (pos != static_cast<size_t>(-1))
                {
                    pos += start_at;
                }
                return pos;
            }
        }

#endif // _VX_USE_SIMD_ALGORITHMS

        string_bitmap<typename Traits::char_type> matches;

        if (matches.mark(needle, needle + needle_size))
        {
            for (auto match_try = hay_start; match_try < hay_end; ++match_try)
            {
                if (matches.match(*match_try))
                {
                    // found a match
                    return static_cast<size_t>(match_try - haystack);
                }
            }

            // no match
            return static_cast<size_t>(-1);
        }

        // couldn't put one of the characters into the bitmap, fall back to serial algorithm
    }

    for (auto match_try = hay_start; match_try < hay_end; ++match_try)
    {
        if (Traits::find(needle, needle_size, *match_try))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }
    }

    // no match
    return static_cast<size_t>(-1);
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_last_of(
    traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    traits_ptr_t<Traits> needle,
    const size_t needle_size) noexcept
{
    // in [haystack, haystack + hay_size), look for last of [needle, needle + needle_size), before start_at

    if (needle_size == 0 || hay_size == 0)
    {
        // not worth searching
        return static_cast<size_t>(-1);
    }

    const auto hay_start = std::min(start_at, hay_size - 1);

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        using char_t = typename Traits::char_type;

#if _VX_USE_SIMD_ALGORITHMS

        VX_IF_CONSTEXPR (sizeof(char_t) <= 2)
        {
            if (!VX_IS_CONSTANT_EVALUATED())
            {
                const size_t remaining_size = hay_start + 1;
                if (remaining_size + needle_size >= _VX_SIMD_THRESHOLD_FIND_FIRST_OF)
                {
                    // same threshold for first/last
                    return _simd::find_last_of_pos_simd(haystack, remaining_size, needle, needle_size);
                }
            }
        }

#endif // _VX_USE_SIMD_ALGORITHMS

        string_bitmap<char_t> matches;
        if (matches.mark(needle, needle + needle_size))
        {
            for (auto match_try = haystack + hay_start;; --match_try)
            {
                if (matches.match(*match_try))
                {
                    // found a match
                    return static_cast<size_t>(match_try - haystack);
                }

                if (match_try == haystack)
                {
                    // at beginning, no more chance for match
                    return static_cast<size_t>(-1);
                }
            }
        }

        // couldn't put one of the characters into the bitmap, fall back to serial algorithm
    }

    for (auto match_try = haystack + hay_start;; --match_try)
    {
        if (Traits::find(needle, needle_size, *match_try))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }

        if (match_try == haystack)
        {
            // at beginning, no more chance for match
            return static_cast<size_t>(-1);
        }
    }
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_first_not_of(
    traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    traits_ptr_t<Traits> needle,
    const size_t needle_size) noexcept
{
    // in [haystack, haystack + hay_size), look for none of [needle, needle + needle_size), at/after start_at

    if (start_at >= hay_size)
    {
        // no room for match
        return static_cast<size_t>(-1);
    }

    const auto hay_start = haystack + start_at;
    const auto hay_end = haystack + hay_size;

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        using char_t = typename Traits::char_type;

#if _VX_USE_SIMD_ALGORITHMS

        VX_IF_CONSTEXPR (sizeof(char_t) <= 2)
        {
            if (!VX_IS_CONSTANT_EVALUATED())
            {
                const size_t remaining_size = hay_size - start_at;
                if (remaining_size + needle_size >= _VX_SIMD_THRESHOLD_FIND_FIRST_OF)
                {
                    size_t pos = _simd::find_first_not_of_pos_simd(hay_start, remaining_size, needle, needle_size);
                    if (pos != static_cast<size_t>(-1))
                    {
                        pos += start_at;
                    }
                    return pos;
                }
            }
        }

#endif // _VX_USE_SIMD_ALGORITHMS

        string_bitmap<char_t> matches;
        if (matches.mark(needle, needle + needle_size))
        {
            for (auto match_try = hay_start; match_try < hay_end; ++match_try)
            {
                if (!matches.match(*match_try))
                {
                    // found a match
                    return static_cast<size_t>(match_try - haystack);
                }
            }
            // no match
            return static_cast<size_t>(-1);
        }

        // couldn't put one of the characters into the bitmap, fall back to the serial algorithm
    }

    for (auto match_try = hay_start; match_try < hay_end; ++match_try)
    {
        if (!Traits::find(needle, needle_size, *match_try))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }
    }

    // no match
    return static_cast<size_t>(-1);
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_not_ch(traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    const traits_char_t<Traits> c) noexcept
{
    // search [haystack, haystack + hay_size) for any value other than c, at/after start_at

    if (start_at >= hay_size)
    {
        // no room for match
        return static_cast<size_t>(-1);
    }

    const auto end = haystack + hay_size;

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            const auto result = _simd::find_not_ch_simd(haystack + start_at, end, c);
            if (result != end)
            {
                return static_cast<size_t>(result - haystack);
            }
            else
            {
                // no match
                return static_cast<size_t>(-1);
            }
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    for (auto match_try = haystack + start_at; match_try < end; ++match_try)
    {
        if (!Traits::eq(*match_try, c))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }
    }

    // no match
    return static_cast<size_t>(-1);
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_find_last_not_of(traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    traits_ptr_t<Traits> needle,
    const size_t needle_size) noexcept
{
    // in [haystack, haystack + hay_size), look for none of [needle, needle + needle_size), before start_at

    if (hay_size == 0)
    {
        // no match possible
        return static_cast<size_t>(-1);
    }

    const auto hay_start = std::min(start_at, hay_size - 1);

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        using char_t = typename Traits::char_type;

#if _VX_USE_SIMD_ALGORITHMS

        VX_IF_CONSTEXPR (sizeof(char_t) <= 2)
        {
            if (!VX_IS_CONSTANT_EVALUATED())
            {
                const size_t remaining_size = hay_start + 1;
                if (remaining_size + needle_size >= _VX_SIMD_THRESHOLD_FIND_FIRST_OF)
                {
                    // same threshold for first/last
                    return _simd::find_last_not_of_pos_simd(haystack, remaining_size, needle, needle_size);
                }
            }
        }

#endif // _VX_USE_SIMD_ALGORITHMS

        string_bitmap<char_t> m_matches;
        if (m_matches.mark(needle, needle + needle_size))
        {
            for (auto match_try = haystack + hay_start;; --match_try)
            {
                if (!m_matches.match(*match_try))
                {
                    // found a match
                    return static_cast<size_t>(match_try - haystack);
                }

                if (match_try == haystack)
                {
                    // at beginning, no more chance for match
                    return static_cast<size_t>(-1);
                }
            }
        }

        // couldn't put one of the characters into the bitmap, fall back to the serial algorithm
    }

    for (auto match_try = haystack + hay_start;; --match_try)
    {
        if (!Traits::find(needle, needle_size, *match_try))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }

        if (match_try == haystack)
        {
            // at beginning, no more chance for match
            return static_cast<size_t>(-1);
        }
    }
}

//=========================================================================

template <typename Traits>
constexpr size_t traits_rfind_not_ch(traits_ptr_t<Traits> haystack,
    const size_t hay_size,
    const size_t start_at,
    const traits_char_t<Traits> c) noexcept
{
    // search [haystack, haystack + hay_size) for any value other than c before start_at

    if (hay_size == 0)
    {
        // no room for match
        return static_cast<size_t>(-1);
    }

    const size_t actual_start_at = std::min(start_at, hay_size - 1);

#if _VX_USE_SIMD_ALGORITHMS

    VX_IF_CONSTEXPR (is_implementation_handled_char_traits<Traits>::value)
    {
        if (!VX_IS_CONSTANT_EVALUATED())
        {
            return _simd::find_last_not_ch_pos_simd(haystack, haystack + actual_start_at + 1, c);
        }
    }

#endif // _VX_USE_SIMD_ALGORITHMS

    for (auto match_try = haystack + actual_start_at;; --match_try)
    {
        if (!Traits::eq(*match_try, c))
        {
            // found a match
            return static_cast<size_t>(match_try - haystack);
        }

        if (match_try == haystack)
        {
            // at beginning, no more chance for match
            return static_cast<size_t>(-1);
        }
    }
}

//=========================================================================

template <typename T, typename U>
constexpr void copy_batch(T* const first1, const U* const first2, const size_t count) noexcept
{
    using traits_type = char_traits<T>;

    VX_IF_CONSTEXPR (std::is_volatile<U>::value)
    {
        for (size_t i = 0; i != count; ++i)
        {
            traits_type::assign(first1[i], T{ first2[i] });
        }
    }
    else
    {
        traits_type::copy(first1, first2, count);
    }
}

template <typename T, typename U>
constexpr void move_batch(T* const first1, const U* const first2, const size_t count) noexcept
{
    using traits_type = char_traits<T>;

    VX_IF_CONSTEXPR (std::is_volatile<U>::value)
    {
        bool loop_forward = true;

        for (const U* src = first2; src != first2 + count; ++src)
        {
            if (first1 == src)
            {
                loop_forward = false;
                break;
            }
        }

        if (loop_forward)
        {
            for (size_t i = 0; i != count; ++i)
            {
                traits_type::assign(first1[i], T{ first2[i] });
            }
        }
        else
        {
            for (size_t i = count; i != 0; --i)
            {
                traits_type::assign(first1[i - 1], T{ first2[i - 1] });
            }
        }
    }
    else
    {
        traits_type::move(first1, first2, count);
    }
}

//=========================================================================

constexpr size_t clamp_suffix_size(const size_t size, const size_t off, const size_t count) noexcept
{
    return std::min(count, size - off);
}

constexpr bool check_offset(const size_t size, const size_t off) noexcept
{
    return (off <= size);
}

} // namespace _priv

//=========================================================================

template <typename T>
class basic_string_view;

template <typename T, typename Allocator>
class basic_string;

template <size_t N, typename T>
class basic_static_string;

//=========================================================================

template <typename T>
struct is_string_like : std::false_type
{};

template <typename T>
struct is_string_like<basic_string_view<T>> : std::true_type
{};

template <typename T, typename Allocator>
struct is_string_like<basic_string<T, Allocator>> : std::true_type
{};

template <size_t N, typename T>
struct is_string_like<basic_static_string<N, T>> : std::true_type
{};

#if defined(__cpp_lib_string_view)

template <typename T, typename Traits>
struct is_string_like<std::basic_string_view<T, Traits>> : std::true_type
{};

#endif // defined(__cpp_lib_string_view)

template <typename T, typename Traits, typename Alloc>
struct is_string_like<std::basic_string<T, Traits, Alloc>> : std::true_type
{};

//=========================================================================

template <typename T>
struct is_mutable_string_like : std::false_type
{};

template <typename T, typename Allocator>
struct is_mutable_string_like<basic_string<T, Allocator>> : std::true_type
{};

template <size_t N, typename T>
struct is_mutable_string_like<basic_static_string<N, T>> : std::true_type
{};

template <typename T, typename Traits, typename Alloc>
struct is_mutable_string_like<std::basic_string<T, Traits, Alloc>> : std::true_type
{};

//=========================================================================

template <typename S, typename T, typename = void>
struct is_string_of : std::false_type
{};

template <typename S, typename T>
struct is_string_of<S, T, type_traits::void_t<typename S::value_type>> : std::bool_constant<is_string_like<S>::value && std::is_same<typename S::value_type, T>::value>
{};

//=========================================================================

template <typename S1, typename S2, typename = void>
struct is_string_compatible : std::false_type
{};

template <typename S1, typename S2>
struct is_string_compatible<S1, S2, type_traits::void_t<typename S1::value_type, typename S2::value_type>> : std::bool_constant<is_string_like<S1>::value && is_string_like<S2>::value && std::is_same<typename S1::value_type, typename S2::value_type>::value>
{};

//=========================================================================

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
S1 operator+(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().append(rhs.c_str())))
{
    return S1(lhs).append(rhs.c_str());
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator==(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) == 0;
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator!=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) != 0;
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator<(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) < 0;
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator>(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) > 0;
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator<=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) <= 0;
}

template <typename S1, typename S2, VX_REQUIRES((is_string_compatible<S1, S2>::value))>
bool operator>=(const S1& lhs, const S2& rhs) noexcept(noexcept(S1().compare(rhs.c_str())))
{
    return lhs.compare(rhs.c_str()) >= 0;
}

} // namespace str
} // namespace vx
