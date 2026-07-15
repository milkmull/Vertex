#pragma once

#include <limits>

#include "vertex/std/string.hpp"

namespace vx {
namespace str {

//==============================================================================
// length
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t length(const C* s) noexcept
{
    return _priv::length(s);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr size_t length(const S& s) noexcept
{
    return s.length();
}

//==============================================================================
// compare
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int compare(const C c1, const C c2) noexcept
{
    return (c1 < c2) ? -1 : (c1 > c2);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int compare(const C* a, const size_t a_size, const C* b, const size_t b_size) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_compare<traits>(a, a_size, b, b_size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int compare(const C* a, const C* b) noexcept
{
    return compare(a, length(a), b, length(b));
}

template <typename C, typename S, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr int compare(const C* a, const S& b) noexcept
{
    return compare(a, length(a), b.data(), b.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr int compare(const S& a, const C* b) noexcept
{
    return compare(a.data(), a.size(), b, length(b));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr int compare(const S1& a, const S2& b) noexcept
{
    return compare(a.data(), a.size(), b.data(), b.size());
}

//==============================================================================
// is_upper
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_upper(const C c) noexcept
{
    return ('A' <= c) && (c <= 'Z');
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_upper(const C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_upper(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_upper(const C* s) noexcept
{
    return is_upper(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_upper(const S& s) noexcept
{
    return is_upper(s.data(), s.size());
}

//==============================================================================
// is_lower
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_lower(const C c) noexcept
{
    return ('a' <= c) && (c <= 'z');
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_lower(const C* s, const size_t size) noexcept
{
    VX_ASSERT(s);
    for (size_t i = 0; i < size; ++i)
    {
        if (!is_lower(s[i]))
        {
            return false;
        }
    }
    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_lower(const C* s) noexcept
{
    return is_lower(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_lower(const S& s) noexcept
{
    return is_lower(s.data(), s.size());
}

//==============================================================================
// is_alpha
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alpha(const C c) noexcept
{
    return is_upper(c) || is_lower(c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alpha(const C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_alpha(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alpha(const C* s) noexcept
{
    return is_alpha(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_alpha(const S& s) noexcept
{
    return is_alpha(s.data(), s.size());
}

//==============================================================================
// is_numeric
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_digit(const C c) noexcept
{
    return ('0' <= c) && (c <= '9');
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_numeric(const C* s, size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_digit(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_numeric(const C* s) noexcept
{
    return is_numeric(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_numeric(const S& s) noexcept
{
    return is_numeric(s.data(), s.size());
}

//==============================================================================
// is_alnum
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alnum(const C c) noexcept
{
    return is_alpha(c) || is_digit(c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alnum(const C* s, size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_alnum(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_alnum(const C* s) noexcept
{
    return is_alnum(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_alnum(const S& s) noexcept
{
    return is_alnum(s.data(), s.size());
}

//==============================================================================
// ascii
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ascii(const C c) noexcept
{
    return static_cast<unsigned char>(c) < 0x80;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ascii(const C* s, size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_ascii(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ascii(const C* s) noexcept
{
    return is_ascii(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_ascii(const S& s) noexcept
{
    return is_ascii(s.data(), s.size());
}

//==============================================================================
// space
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_space(const C c) noexcept
{
    return (c == ' ') || (c == '\t') || (c == '\r') || (c == '\n') || (c == '\f') || (c == '\v');
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_space(const C* s, size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_space(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_space(const C* s) noexcept
{
    return is_space(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_space(const S& s) noexcept
{
    return is_space(s.data(), s.size());
}

//==============================================================================
// ctrl
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ctrl(const C c) noexcept
{
    const unsigned char uc = static_cast<unsigned char>(c);
    return (uc <= '\x1f') || (uc == '\x7f');
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ctrl(const C* s, size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        if (!is_ctrl(s[i]))
        {
            return false;
        }
    }

    return size != 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_ctrl(const C* s) noexcept
{
    return is_ctrl(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_ctrl(const S& s) noexcept
{
    return is_ctrl(s.data(), s.size());
}

//==============================================================================
// hex
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_hex_digit(const C c) noexcept
{
    return (('A' <= c) && (c <= 'F')) || (('a' <= c) && (c <= 'f')) || is_digit(c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_hex(const C* s, const size_t size, const bool allow_prefix) noexcept
{
    VX_ASSERT(s);

    if (size == 0)
    {
        return false;
    }

    size_t i = 0;

    // Check for optional '0x' or '0X' prefix
    if (allow_prefix && size > 2 && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X'))
    {
        i = 2;

        if (i >= size)
        {
            return false;
        }
    }

    for (; i < size; ++i)
    {
        if (!is_hex_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool is_hex(const C* s, const bool allow_prefix) noexcept
{
    return is_hex(s, length(s), allow_prefix);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_hex(const S& s, const bool allow_prefix) noexcept
{
    return is_hex(s.data(), s.size(), allow_prefix);
}

//==============================================================================
// to_lower
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_lower(const C c) noexcept
{
    return ('A' <= c) && (c <= 'Z') ? ('a' + (c - 'A')) : c;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_lower(C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        s[i] = to_lower(s[i]);
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_lower(C* s) noexcept
{
    to_lower(s, length(s));
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void to_lower(S& s) noexcept
{
    to_lower(s.data(), s.size());
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_lower_ascii_unchecked(C c) noexcept
{
    return static_cast<C>(c | 0x20);
}

//==============================================================================
// to_lower_copy
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_lower_copy(const C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    basic_string<C> res(size, '\0');

    for (size_t i = 0; i < size; ++i)
    {
        res[i] = to_lower(s[i]);
    }

    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_lower_copy(const C* s) noexcept
{
    return to_lower_copy(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> to_lower_copy(const S& s) noexcept
{
    return to_lower_copy(s.data(), s.size());
}

//==============================================================================
// to_upper
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_upper(const C c) noexcept
{
    return ('a' <= c) && (c <= 'z') ? ('A' + (c - 'a')) : c;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_upper(C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    for (size_t i = 0; i < size; ++i)
    {
        s[i] = to_upper(s[i]);
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_upper(C* s) noexcept
{
    return to_upper(s, length(s));
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void to_upper(S& s) noexcept
{
    to_upper(s.data(), s.size());
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_upper_ascii_unchecked(C c) noexcept
{
    return static_cast<C>(c & ~0x20);
}

//==============================================================================
// to_upper_copy
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_upper_copy(const C* s, const size_t size) noexcept
{
    VX_ASSERT(s);

    basic_string<C> res(size, '\0');

    for (size_t i = 0; i < size; ++i)
    {
        res[i] = to_upper(s[i]);
    }

    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_upper_copy(const C* s) noexcept
{
    return to_upper_copy(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> to_upper_copy(const S& s) noexcept
{
    return to_upper_copy(s.data(), s.size());
}

//==============================================================================
// case_compare
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int case_compare(const C c1, const C c2) noexcept
{
    return compare(to_lower(c1), to_lower(c2));
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int case_compare_ascii_unchecked(const C c1, const C c2) noexcept
{
    return compare(to_lower_ascii_unchecked(c1), to_lower_ascii_unchecked(c2));
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int case_compare(const C* a, const size_t a_size, const C* b, const size_t b_size) noexcept
{
    size_t n = std::min(a_size, b_size);

    for (; 0 < n; --n, ++a, ++b)
    {
        const C ac = to_lower(*a);
        const C bc = to_lower(*b);

        if (ac != bc)
        {
            return (ac < bc) ? -1 : +1;
        }
    }

    if (a_size < b_size)
    {
        return -1;
    }

    if (a_size > b_size)
    {
        return 1;
    }

    return 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int case_compare(const C* a, const C* b) noexcept
{
    return case_compare(a, length(a), b, length(b));
}

template <typename C, typename S, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr int case_compare(const C* a, const S& b) noexcept
{
    return case_compare(a, length(a), b.data(), b.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr int case_compare(const S& a, const C* b) noexcept
{
    return case_compare(a, length(a), b.data(), b.size());
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr int case_compare(const S1& a, const S2& b) noexcept
{
    return case_compare(a.data(), a.size(), b.data(), b.size());
}

//==============================================================================
// find
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find(
    const C* haystack,
    const size_t hay_size,
    const C c,
    const size_t start = 0) noexcept
{
    return _priv::traits_find_ch(haystack, hay_size, start, c);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr size_t find(
    const S& haystack,
    const typename S::value_type c,
    const size_t start = 0) noexcept
{
    return _priv::traits_find_ch(haystack.data(), haystack.size(), start, c);
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = 0) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_find<traits>(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find(
    const C* haystack,
    const S& needle,
    const size_t start = 0) noexcept
{
    return find(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find(
    const S& haystack,
    const C* needle,
    const size_t start = 0) noexcept
{
    return find(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t find(
    const S1& haystack,
    const S2& needle,
    const size_t start = 0) noexcept
{
    return find(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// rfind
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t rfind(
    const C* haystack,
    const size_t hay_size,
    const C c,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_rfind_ch<traits>(haystack, hay_size, start, c);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr size_t rfind(
    const S& haystack,
    const typename S::value_type c,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return rfind(haystack.data(), haystack.size(), c, start);
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t rfind(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_rfind(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t rfind(
    const C* haystack,
    const S& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return rfind(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t rfind(
    const S& haystack,
    const C* needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return rfind(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t rfind(
    const S1& haystack,
    const S2& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return rfind(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// find_first_of
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find_first_of(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = 0) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_find_first_of<traits>(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_first_of(
    const C* haystack,
    const S& needle,
    const size_t start = 0) noexcept
{
    return find_first_of(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_first_of(
    const S& haystack,
    const C* needle,
    const size_t start = 0) noexcept
{
    return find_first_of(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t find_first_of(
    const S1& haystack,
    const S2& needle,
    const size_t start = 0) noexcept
{
    return find_first_of(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// find_last_of
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find_last_of(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_find_last_of<traits>(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_last_of(
    const C* haystack,
    const S& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_of(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_last_of(
    const S& haystack,
    const C* needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_of(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t find_last_of(
    const S1& haystack,
    const S2& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_of(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// find_first_not_of
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find_first_not_of(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = 0) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_find_first_not_of<traits>(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_first_not_of(
    const C* haystack,
    const S& needle,
    const size_t start = 0) noexcept
{
    return find_first_not_of(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_first_not_of(
    const S& haystack,
    const C* needle,
    const size_t start = 0) noexcept
{
    return find_first_not_of(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t find_first_not_of(
    const S1& haystack,
    const S2& needle,
    const size_t start = 0) noexcept
{
    return find_first_not_of(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// find_last_not_of
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t find_last_not_of(
    const C* haystack,
    const size_t hay_size,
    const C* needle,
    const size_t needle_size,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_find_last_not_of<traits>(haystack, hay_size, start, needle, needle_size);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_last_not_of(
    const C* haystack,
    const S& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_not_of(
        haystack, length(haystack),
        needle.data(), needle.size(),
        start);
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t find_last_not_of(
    const S& haystack,
    const C* needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_not_of(
        haystack.data(), haystack.size(),
        needle, length(needle),
        start);
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t find_last_not_of(
    const S1& haystack,
    const S2& needle,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return find_last_not_of(
        haystack.data(), haystack.size(),
        needle.data(), needle.size(),
        start);
}

//==============================================================================
// count
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const size_t size, const C c) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_count<traits>(s, size, c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const C c) noexcept
{
    using traits = char_traits<C>;
    return _priv::traits_count<traits>(s, length(s), c);
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const size_t size, const C* val, const size_t val_size) noexcept
{
    if (val_size == 0)
    {
        return size + 1;
    }

    constexpr size_t npos = std::numeric_limits<size_t>::max();
    size_t count = 0;
    size_t off = find(s, size, val, val_size);

    while (off != npos)
    {
        ++count;
        off += val_size;
        off = find(s, size, val, val_size, off);
    }

    return count;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const C* val) noexcept
{
    return count(s, length(s), val, length(val));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t count(const C* s, const S& val) noexcept
{
    return count(s, length(s), val.data(), val.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr size_t count(const S& s, const C* val) noexcept
{
    return count(s.data(), s.size(), val, length(val));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr size_t count(const S1& s, const S2& val) noexcept
{
    return count(s.data(), s.size(), val.data(), val.size());
}

//==============================================================================
// starts_with
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool starts_with(const C* s, const size_t size, const C prefix) noexcept
{
    return (size != 0) && (s[0] == prefix);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool starts_with(const C* s, const C prefix) noexcept
{
    return starts_with(s, length(s), prefix);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool starts_with(const C* s, const size_t size, const C* prefix, const size_t prefix_size) noexcept
{
    return (size >= prefix_size) && compare(s, prefix_size, prefix, prefix_size) == 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool starts_with(const C* s, const C* prefix) noexcept
{
    return starts_with(s, length(s), prefix, length(prefix));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr bool starts_with(const C* s, const S& prefix) noexcept
{
    return starts_with(s, length(s), prefix.data(), prefix.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr bool starts_with(const S& s, const C* prefix) noexcept
{
    return starts_with(s.data(), s.size(), prefix, length(prefix));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr bool starts_with(const S1& s, const S2& prefix) noexcept
{
    return starts_with(s.data(), s.size(), prefix.data(), prefix.size());
}

//==============================================================================
// ends_with
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool ends_with(const C* s, const size_t size, const C suffix) noexcept
{
    return (size != 0) && (s[size - 1] == suffix);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool ends_with(const C* s, const C suffix) noexcept
{
    return ends_with(s, length(s), suffix);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool ends_with(const C* s, const size_t size, const C* suffix, const size_t suffix_size) noexcept
{
    return (size >= suffix_size) && compare(s + size - suffix_size, suffix_size, suffix, suffix_size) == 0;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr bool ends_with(const C* s, const C* suffix) noexcept
{
    return ends_with(s, length(s), suffix, length(suffix));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr bool ends_with(const C* s, const S& suffix) noexcept
{
    return ends_with(s, length(s), suffix.data(), suffix.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr bool ends_with(const S& s, const C* suffix) noexcept
{
    return ends_with(s.data(), s.size(), suffix, length(suffix));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr bool ends_with(const S1& s, const S2& suffix) noexcept
{
    return ends_with(s.data(), s.size(), suffix.data(), suffix.size());
}

//==============================================================================
// to_title
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_title(C* s, const size_t size) noexcept
{
    bool new_word = true;

    for (size_t i = 0; i < size; ++i)
    {
        const C c = s[i];

        if (is_alpha(c))
        {
            s[i] = new_word ? to_upper(c) : to_lower(c);
            new_word = false;
        }
        else
        {
            new_word = true;
        }
    }
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void to_title(C* s) noexcept
{
    return to_title(s, length(s));
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void to_title(S& s) noexcept
{
    return to_title(s.data(), s.size());
}

//==============================================================================
// to_title_copy
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_title_copy(const C* s, const size_t size) noexcept
{
    basic_string<C> res(size, C());

    bool new_word = true;

    for (size_t i = 0; i < size; ++i)
    {
        const C c = s[i];

        if (is_alpha(c))
        {
            res[i] = (new_word ? to_upper(c) : to_lower(c));
            new_word = false;
        }
        else
        {
            res[i] = c;
            new_word = true;
        }
    }

    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> to_title_copy(const C* s) noexcept
{
    return to_title_copy(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> to_title_copy(const S& s) noexcept
{
    return to_title_copy(s.data(), s.size());
}

//==============================================================================
// strip
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> strip(
    const C* s,
    const size_t size) noexcept
{
    size_t start = 0;
    size_t end = size;
    while (start < end && is_space(s[start]))
    {
        ++start;
    }
    while (end > start && is_space(s[end - 1]))
    {
        --end;
    }
    return basic_string_view<C>(s + start, end - start);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> strip(const C* s) noexcept
{
    return strip(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string_view<typename S::value_type> strip(const S& s) noexcept
{
    return strip(s.data(), s.size());
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> strip(
    const C* s,
    const size_t size,
    const C* chars,
    const size_t char_size) noexcept
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    const size_t start = find_first_not_of(s, size, chars, char_size);

    if (start != npos)
    {
        const size_t end = find_last_not_of(s, size, chars, char_size);
        return basic_string_view<C>(s + start, end - start + 1);
    }

    return basic_string_view<C>();
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> strip(const C* s, const C* chars) noexcept
{
    return strip(s, length(s), chars, length(chars));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> strip(const C* s, const S& chars) noexcept
{
    return strip(s, length(s), chars.data(), chars.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> strip(const S& s, const C* chars) noexcept
{
    return strip(s.data(), s.size(), chars, length(chars));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string_view<typename S1::value_type> strip(const S1& s, const S2& chars) noexcept
{
    return strip(s.data(), s.size(), chars.data(), chars.size());
}

//==============================================================================
// lstrip
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> lstrip(
    const C* s,
    const size_t size) noexcept
{
    size_t start = 0;
    while (start < size && is_space(s[start]))
    {
        ++start;
    }
    return basic_string_view<C>(s + start, size - start);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> lstrip(const C* s) noexcept
{
    return lstrip(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string_view<typename S::value_type> lstrip(const S& s) noexcept
{
    return lstrip(s.data(), s.size());
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> lstrip(
    const C* s,
    const size_t size,
    const C* chars,
    const size_t char_size) noexcept
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    const size_t start = find_first_not_of(s, size, chars, char_size);

    if (start != npos)
    {
        return basic_string_view<C>(s + start, size - start);
    }

    return basic_string_view<C>();
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> lstrip(const C* s, const C* chars) noexcept
{
    return lstrip(s, length(s), chars, length(chars));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> lstrip(const C* s, const S& chars) noexcept
{
    return lstrip(s, length(s), chars.data(), chars.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> lstrip(const S& s, const C* chars) noexcept
{
    return lstrip(s.data(), s.size(), chars, length(chars));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string_view<typename S1::value_type> lstrip(const S1& s, const S2& chars) noexcept
{
    return lstrip(s.data(), s.size(), chars.data(), chars.size());
}

//==============================================================================
// rstrip
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> rstrip(
    const C* s,
    const size_t size) noexcept
{
    size_t end = size;
    while (end > 0 && is_space(s[end - 1]))
    {
        --end;
    }
    return basic_string_view<C>(s, end);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> rstrip(const C* s) noexcept
{
    return rstrip(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string_view<typename S::value_type> rstrip(const S& s) noexcept
{
    return rstrip(s.data(), s.size());
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> rstrip(
    const C* s,
    const size_t size,
    const C* chars,
    const size_t char_size) noexcept
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    const size_t end = find_last_not_of(s, size, chars, char_size);

    if (end != npos)
    {
        return basic_string_view<C>(s, end + 1);
    }

    return basic_string_view<C>();
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> rstrip(const C* s, const C* chars) noexcept
{
    return rstrip(s, length(s), chars, length(chars));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> rstrip(const C* s, const S& chars) noexcept
{
    return rstrip(s, length(s), chars.data(), chars.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> rstrip(const S& s, const C* chars) noexcept
{
    return rstrip(s.data(), s.size(), chars, length(chars));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string_view<typename S1::value_type> rstrip(const S1& s, const S2& chars) noexcept
{
    return rstrip(s.data(), s.size(), chars.data(), chars.size());
}

//==============================================================================
// trim_prefix
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> trim_prefix(
    const C* s,
    const size_t size,
    const C* prefix,
    const size_t prefix_size) noexcept
{
    if (starts_with(s, size, prefix, prefix_size))
    {
        return basic_string_view<C>(s + prefix_size, size - prefix_size);
    }

    return basic_string_view<C>(s, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> trim_prefix(const C* s, const C* prefix) noexcept
{
    return trim_prefix(s, length(s), prefix, length(prefix));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> trim_prefix(const C* s, const S& prefix) noexcept
{
    return trim_prefix(s, length(s), prefix.data(), prefix.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> trim_prefix(const S& s, const C* prefix) noexcept
{
    return trim_prefix(s.data(), s.size(), prefix, length(prefix));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string_view<typename S1::value_type> trim_prefix(const S1& s, const S2& prefix) noexcept
{
    return trim_prefix(s.data(), s.size(), prefix.data(), prefix.size());
}

//==============================================================================
// trim_suffix
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> trim_suffix(
    const C* s,
    const size_t size,
    const C* suffix,
    const size_t suffix_size) noexcept
{
    if (ends_with(s, size, suffix, suffix_size))
    {
        return basic_string_view<C>(s, size - suffix_size);
    }

    return basic_string_view<C>(s, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string_view<C> trim_suffix(const C* s, const C* suffix) noexcept
{
    return trim_suffix(s, length(s), suffix, length(suffix));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> trim_suffix(const C* s, const S& suffix) noexcept
{
    return trim_suffix(s, length(s), suffix.data(), suffix.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string_view<C> trim_suffix(const S& s, const C* suffix) noexcept
{
    return trim_suffix(s.data(), s.size(), suffix, length(suffix));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string_view<typename S1::value_type> trim_suffix(const S1& s, const S2& suffix) noexcept
{
    return trim_suffix(s.data(), s.size(), suffix.data(), suffix.size());
}

//==============================================================================
// remove
//==============================================================================

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void remove(S& s, const typename S::value_type c) noexcept
{
    using traits = char_traits<typename S::value_type>;
    const auto it = _priv::traits_remove<traits>(s.data(), s.size(), c);
    const size_t off = it - s.data();
    const size_t count = s.size() - off;
    s.erase(off, count);
}

template <typename S, typename C, VX_REQUIRES(is_mutable_string_like<S>::value && is_string_of<S, C>::value)>
constexpr void remove(S& s, const C* val, const size_t val_size) noexcept
{
    if (val_size == 0)
    {
        return;
    }

    constexpr size_t npos = std::numeric_limits<size_t>::max();
    size_t i = find(s.data(), s.size(), val, val_size);

    while (i != npos)
    {
        s.erase(i, val_size);
        i = find(s.data(), s.size(), val, val_size, i + val_size);
    }
}

template <typename S, typename C, VX_REQUIRES(is_mutable_string_like<S>::value && is_string_of<S, C>::value)>
constexpr void remove(S& s, const C* val) noexcept
{
    remove(s, val, length(val));
}

template <typename S1, typename S2, VX_REQUIRES(is_mutable_string_like<S1>::value && is_string_compatible<S1, S2>::value)>
constexpr void remove(S1& s, const S2& val) noexcept
{
    remove(s, val.data(), val.size());
}

//==============================================================================
// remove_copy
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> remove_copy(
    const C* s,
    const size_t size,
    const C val) noexcept
{
    basic_string<C> res(s, size);
    remove(res, val);
    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> remove_copy(
    const C* s,
    const C val) noexcept
{
    return remove_copy(s, length(s), val);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> remove_copy(
    const S& s,
    const typename S::value_type val) noexcept
{
    return remove_copy(s.data(), s.size(), val);
}

//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> remove_copy(
    const C* s,
    const size_t size,
    const C* val,
    const size_t val_size) noexcept
{
    basic_string<C> res(s, size);
    remove(res, val, val_size);
    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> remove_copy(const C* s, const C* val) noexcept
{
    return remove_copy(s, length(s), val, length(val));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string<C> remove_copy(const C* s, const S& val) noexcept
{
    return remove_copy(s, length(s), val.data(), val.size());
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string<C> remove_copy(const S& s, const C* val) noexcept
{
    return remove_copy(s.data(), s.size(), val, length(val));
}

template <typename S1, typename S2, VX_REQUIRES(is_string_compatible<S1, S2>::value)>
constexpr basic_string<typename S1::value_type> remove_copy(const S1& s, const S2& val) noexcept
{
    return remove_copy(s.data(), s.size(), val.data(), val.size());
}

//==============================================================================
// replace (char)
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void replace(
    C* s,
    const size_t size,
    const C old_val,
    const C new_val) noexcept
{
    using traits = char_traits<C>;
    _priv::traits_replace<traits>(s, size, old_val, new_val);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void replace(
    C* s,
    const C old_val,
    const C new_val) noexcept
{
    replace(s, length(s), old_val, new_val);
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void replace(
    S& s,
    const typename S::value_type old_val,
    const typename S::value_type new_val) noexcept
{
    replace(s.data(), s.size(), old_val, new_val);
}

//==============================================================================
// replace_copy (char)
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const size_t size,
    const C old_val,
    const C new_val) noexcept
{
    basic_string<C> res(s, size);
    replace(res.data(), res.size(), old_val, new_val);
    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const C old_val,
    const C new_val) noexcept
{
    return replace_copy(s, length(s), old_val, new_val);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> replace_copy(
    const S& s,
    const typename S::value_type old_val,
    const typename S::value_type new_val) noexcept
{
    return replace_copy(s.data(), s.size(), old_val, new_val);
}

//==============================================================================
// replace_copy (string)
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const size_t size,
    const C* old_val,
    const size_t old_val_size,
    const C* new_val,
    const size_t new_val_size) noexcept
{
    constexpr size_t npos = std::numeric_limits<size_t>::max();
    basic_string<C> res;

    if (old_val_size == 0)
    {
        res.reserve((size + 1) * new_val_size);
        res.append(new_val);

        for (size_t i = 0; i < size; ++i)
        {
            res.push_back(s[i]);
            res.append(new_val);
        }
    }
    else
    {
        size_t start = 0;
        size_t end = 0;
        const size_t step = (old_val_size == 0) ? 1 : old_val_size;

        while (true)
        {
            end = find(s, size, old_val, old_val_size, start);
            res.append(s + start, end == npos ? size - start : end - start);

            if (end == npos)
            {
                break;
            }

            res.append(new_val, new_val_size);
            start = end + step;
        }
    }

    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const C* old_val,
    const C* new_val) noexcept
{
    return replace_copy(s, length(s), old_val, length(old_val), new_val, length(new_val));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string<C> replace_copy(
    const S& s,
    const C* old_val,
    const C* new_val) noexcept
{
    return replace_copy(s.data(), s.size(), old_val, length(old_val), new_val, length(new_val));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const S& old_val,
    const C* new_val) noexcept
{
    return replace_copy(s, length(s), old_val.data(), old_val.size(), new_val, length(new_val));
}

template <typename S, typename C, VX_REQUIRES(is_string_of<S, C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const C* old_val,
    const S& new_val) noexcept
{
    return replace_copy(s, length(s), old_val, length(old_val), new_val.data(), new_val.size());
}

template <typename S1, typename S2, typename C, VX_REQUIRES(is_string_compatible<S1, S2>::value&& is_string_of<S1, C>::value)>
constexpr basic_string<C> replace_copy(
    const S1& s,
    const S2& old_val,
    const C* new_val) noexcept
{
    return replace_copy(s.data(), s.size(), old_val.data(), old_val.size(), new_val, length(new_val));
}

template <typename S1, typename S2, typename C, VX_REQUIRES(is_string_compatible<S1, S2>::value&& is_string_of<S1, C>::value)>
constexpr basic_string<C> replace_copy(
    const S1& s,
    const C* old_val,
    const S2* new_val) noexcept
{
    return replace_copy(s.data(), s.size(), old_val, length(old_val), new_val.data(), new_val.size());
}

template <typename S1, typename S2, typename C, VX_REQUIRES(is_string_compatible<S1, S2>::value&& is_string_of<S1, C>::value)>
constexpr basic_string<C> replace_copy(
    const C* s,
    const S1& old_val,
    const S2& new_val) noexcept
{
    return replace_copy(s, length(s), old_val.data(), old_val.size(), new_val.data(), new_val.size());
}

template <typename S1, typename S2, typename S3, VX_REQUIRES(is_string_compatible<S1, S2>::value&& is_string_compatible<S2, S3>::value)>
constexpr basic_string<typename S1::value_type> replace_copy(
    const S1& s,
    const S2& old_val,
    const S3& new_val) noexcept
{
    return replace_copy(s.data(), s.size(), old_val.data(), old_val.size(), new_val.data(), new_val.size());
}

//==============================================================================
// replace (string)
//==============================================================================

template <typename S, typename C, VX_REQUIRES(is_mutable_string_like<S>::value && is_string_of<S, C>::value)>
constexpr void replace(
    S& s,
    const C* old_val,
    const C* new_val) noexcept
{
    s = replace_copy(s.data(), s.size(), old_val, length(old_val), new_val, length(new_val));
}

template <typename S1, typename S2, typename C, VX_REQUIRES(is_mutable_string_like<S1>::value && is_string_compatible<S1, S2>::value&& is_string_of<S1, C>::value)>
constexpr void replace(
    S1& s,
    const S2& old_val,
    const C* new_val) noexcept
{
    s = replace_copy(s.data(), s.size(), old_val.data(), old_val.size(), new_val, length(new_val));
}

template <typename S1, typename S2, typename C, VX_REQUIRES(is_mutable_string_like<S1>::value && is_string_compatible<S1, S2>::value&& is_string_of<S1, C>::value)>
constexpr void replace(
    S1& s,
    const C* old_val,
    const S2* new_val) noexcept
{
    s = replace_copy(s.data(), s.size(), old_val, length(old_val), new_val.data(), new_val.size());
}

template <typename S1, typename S2, typename S3, VX_REQUIRES(is_mutable_string_like<S1>::value && is_string_compatible<S1, S2>::value&& is_string_compatible<S2, S3>::value)>
constexpr void replace(
    S1& s,
    const S2& old_val,
    const S3& new_val) noexcept
{
    s = replace_copy(s.data(), s.size(), old_val.data(), old_val.size(), new_val.data(), new_val.size());
}

//==============================================================================
// repeat
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> repeat(const C c, const size_t n) noexcept
{
    return basic_string<C>(n, c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> repeat(const C* s, const size_t size, const size_t n) noexcept
{
    basic_string<C> res;
    res.reserve(size * n);

    for (size_t i = 0; i < n; ++i)
    {
        res.append(s, size);
    }

    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> repeat(const C* s, const size_t n) noexcept
{
    return repeat(s, length(s), n);
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> repeat(const S& s, const size_t n) noexcept
{
    return repeat(s.data(), s.size(), n);
}

//==============================================================================
// reverse
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void reverse(C* s, const size_t size) noexcept
{
    using traits = char_traits<C>;
    _priv::traits_reverse<traits>(s, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void reverse(C* s) noexcept
{
    reverse(s, length(s));
}

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void reverse(S& s) noexcept
{
    reverse(s.data(), s.size());
}

//==============================================================================
// reverse_copy
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> reverse_copy(const C* s, const size_t size) noexcept
{
    basic_string<C> res(s, size);
    reverse(res.data(), size);
    return res;
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr basic_string<C> reverse_copy(const C* s) noexcept
{
    return reverse_copy(s, length(s));
}

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr basic_string<typename S::value_type> reverse_copy(const S& s) noexcept
{
    return reverse_copy(s.data(), s.size());
}

} // namespace str
} // namespace vx
