#pragma once

#include <limits>

#include "vertex/std/char_traits.hpp"
#include "vertex/std/iterator.hpp"
#include "vertex/std/utf.hpp"

namespace vx {
namespace str {

//==============================================================================
// length
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t length(const C* s) noexcept
{
    return _char_traits_priv::length(s);
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
    return _char_traits_priv::traits_compare<traits>(a, a_size, b, b_size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr int compare(const C* a, const C* b) noexcept
{
    return compare(a, length(a), b, length(b));
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

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_lower_ascii_unchecked(C c) noexcept
{
    return static_cast<C>(c | 0x20);
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

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr C to_upper_ascii_unchecked(C c) noexcept
{
    return static_cast<C>(c & ~0x20);
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
    size_t n = vx::min(a_size, b_size);

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
    return _char_traits_priv::traits_find_ch(haystack, hay_size, start, c);
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
    return _char_traits_priv::traits_find<traits>(haystack, hay_size, start, needle, needle_size);
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
    return _char_traits_priv::traits_rfind_ch<traits>(haystack, hay_size, start, c);
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
    return _char_traits_priv::traits_rfind(haystack, hay_size, start, needle, needle_size);
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
    return _char_traits_priv::traits_find_first_of<traits>(haystack, hay_size, start, needle, needle_size);
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
    return _char_traits_priv::traits_find_last_of<traits>(haystack, hay_size, start, needle, needle_size);
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
    return _char_traits_priv::traits_find_first_not_of<traits>(haystack, hay_size, start, needle, needle_size);
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
    return _char_traits_priv::traits_find_last_not_of<traits>(haystack, hay_size, start, needle, needle_size);
}

//==============================================================================
// count
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const size_t size, const C c) noexcept
{
    using traits = char_traits<C>;
    return _char_traits_priv::traits_count<traits>(s, size, c);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr size_t count(const C* s, const C c) noexcept
{
    using traits = char_traits<C>;
    return _char_traits_priv::traits_count<traits>(s, length(s), c);
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
    _char_traits_priv::traits_replace<traits>(s, size, old_val, new_val);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void replace(
    C* s,
    const C old_val,
    const C new_val) noexcept
{
    replace(s, length(s), old_val, new_val);
}

//==============================================================================
// reverse
//==============================================================================

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void reverse(C* s, const size_t size) noexcept
{
    using traits = char_traits<C>;
    _char_traits_priv::traits_reverse<traits>(s, size);
}

template <typename C, VX_REQUIRES(type_traits::is_char<C>::value)>
constexpr void reverse(C* s) noexcept
{
    reverse(s, length(s));
}

//==============================================================================
// string_cast_size
//==============================================================================
// Number of to_char_t units string_cast would write for [src, src + src_size).
// Does not include a terminator. Invalid sequences count as one unit (the
// replacement), matching what the cast functions emit.
//==============================================================================

/**
 * @brief Counts the units needed to convert an iterator range.
 *
 * This is the implementation; the pointer + size overload funnels here.
 * Requires a multipass (forward or better) iterator, since the range is
 * walked once here and again by the conversion.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @return Number of to_char_t units the conversion produces, excluding any terminator.
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char_iter<IT>::value)>
size_t string_cast_size(IT first, IT last) noexcept
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    using from_char_t = typename type_traits::value_type<IT>::type;
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    size_t total = 0;

    while (first != last)
    {
        utf::code_point c{};
        first = decoder::decode(first, last, c);
        total += utf::is_valid_codepoint(c) ? encoder::width(c) : 1;
    }

    return total;
}

/**
 * @brief Counts the units needed to convert a range to another encoding.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range (may be null if src_size is 0).
 * @param src_size Number of characters in the input range.
 * @return Number of to_char_t units the conversion produces, excluding any terminator.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
size_t string_cast_size(const from_char_t* src, size_t src_size) noexcept
{
    return string_cast_size<to_char_t>(src, src + src_size);
}

/**
 * @brief Counts the units needed to convert an iterator range, assuming it is pure ASCII.
 *
 * This is the implementation; the pointer + size overload funnels here.
 * Precondition: every unit in the range is < 0x80. Conversion is then 1:1, so
 * the result is simply the range length. If the precondition is violated, the
 * result is still the range length and is consistent with
 * string_cast_assume_ascii, but not with the general string_cast.
 *
 * @tparam to_char_t Destination character type (unused, kept for symmetry).
 * @tparam IT Input iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @return std::distance(first, last).
 */
template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char_iter<IT>::value)>
constexpr size_t string_cast_size_assume_ascii(IT first, IT last) noexcept
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

#if VX_DEBUG

    using utype = typename utf::utf_traits<typename type_traits::value_type<IT>::type>::utype;
    for (IT it = first; it != last; ++it)
    {
        VX_ASSERT(is_ascii(*it));
    }

#endif // VX_DEBUG

    return static_cast<size_t>(vx::distance(first, last));
}

/**
 * @brief Counts the units needed to convert a range, assuming it is pure ASCII.
 *
 * Precondition: every unit in the range is < 0x80. See the iterator overload.
 *
 * @tparam to_char_t Destination character type (unused, kept for symmetry).
 * @tparam from_char_t Source character type.
 * @param src Pointer to the beginning of the input range (may be null if src_size is 0).
 * @param src_size Number of characters in the input range.
 * @return src_size.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
constexpr size_t string_cast_size_assume_ascii(const from_char_t* src, size_t src_size) noexcept
{
    return string_cast_size_assume_ascii<to_char_t>(src, src + src_size);
}

//==============================================================================
// char_cast
//==============================================================================
// Converts one encoded character (one code point). A different job from the
// string_cast family, so it stays its own small primitive.
//==============================================================================

/**
 * @brief Converts a single encoded character from one encoding to another.
 *
 * Precondition: first != last. Writes between 1 and encoder::max_width()
 * units to out; the caller must guarantee that much space. An invalid or
 * truncated sequence writes one replacement unit and still consumes at least
 * one source unit, so loops calling this always make progress.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param first Reference to the current input pointer. Advanced past the consumed units.
 * @param last Pointer past the end of the input range.
 * @param out Output pointer.
 * @param replacement Replacement unit for invalid sequences.
 * @return Pointer past the last written output unit.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* char_cast(
    const from_char_t*& first,
    const from_char_t* last,
    to_char_t* out,
    to_char_t replacement = to_char_t('?')) noexcept
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    utf::code_point c{};
    first = decoder::decode(first, last, c);

    return encoder::encode(c, out, replacement);
}

/**
 * @brief Converts a single ASCII character to another encoding.
 *
 * Precondition: first != last and *first < 0x80. Consumes exactly one source
 * unit and writes exactly one output unit, with no decode or validation.
 *
 * @tparam to_char_t Destination character type.
 * @tparam from_char_t Source character type.
 * @param first Reference to the current input pointer. Advanced by one.
 * @param out Output pointer; must have room for one unit.
 * @return out + 1.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
to_char_t* char_cast_assume_ascii(const from_char_t*& first, to_char_t* out) noexcept
{
    using utype = typename utf::utf_traits<from_char_t>::utype;
    VX_ASSERT(is_ascii(*first));

    // go through utype first so a signed char never sign-extends
    *out = static_cast<to_char_t>(static_cast<utype>(*first));
    ++first;
    return out + 1;
}

//==============================================================================
// string_cast (core #1): bounded, contiguous
//==============================================================================
// Every higher-level string_cast form reduces to this (ptr + size in, ptr + capacity out).
// Never writes past dst_size and never writes a partial code point. Does not
// write a terminator; the caller owns that (size with string_cast_size, allocate +1).
//==============================================================================

/**
 * @brief Result of a bounded conversion.
 */
template <typename from_char_t, typename to_char_t>
struct string_cast_result
{
    const from_char_t* src_end; ///< First unconsumed source unit.
    to_char_t* dst_end;         ///< One past the last written output unit.
    bool complete;              ///< True if the whole source was consumed.

    explicit operator bool() const noexcept
    {
        return complete;
    }
};

/**
 * @brief Converts [src, src + src_size) into a buffer of dst_size units.
 *
 * Invalid sequences become one `replacement` unit. Stops early, with
 * complete == false, if the next code point's encoding doesn't fit.
 *
 * @param src Source pointer (may be null if src_size is 0).
 * @param src_size Number of source units.
 * @param dst Destination pointer (may be null if dst_size is 0).
 * @param dst_size Destination capacity in to_char_t units.
 * @param replacement Unit written for invalid sequences.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
string_cast_result<from_char_t, to_char_t> string_cast(
    const from_char_t* src,
    size_t src_size,
    to_char_t* dst,
    size_t dst_size,
    to_char_t replacement = to_char_t('?')) noexcept
{
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    const from_char_t* first = src;
    const from_char_t* const last = src + src_size;
    to_char_t* out = dst;
    to_char_t* const out_end = dst + dst_size;

    to_char_t scratch[encoder::max_width()];

    while (first != last)
    {
        const size_t remaining = static_cast<size_t>(out_end - out);

        if (remaining >= encoder::max_width())
        {
            // worst case fits: encode straight into dst
            utf::code_point c{};
            first = decoder::decode(first, last, c);
            out = encoder::encode(c, out, replacement);
            continue;
        }

        // tail: measure through scratch so a partial code point is never written
        const from_char_t* next = first;
        utf::code_point c{};
        next = decoder::decode(next, last, c);

        const size_t n = static_cast<size_t>(encoder::encode(c, scratch, replacement) - scratch);
        if (n > remaining)
        {
            break;
        }

        for (size_t i = 0; i < n; ++i)
        {
            out[i] = scratch[i];
        }

        out += n;
        first = next;
    }

    return { first, out, first == last };
}

/**
 * @brief Converts a range assuming it is pure ASCII (every unit < 0x80).
 *
 * Conversion is 1:1, so this is a straight widening/narrowing copy with no
 * decode, validation, or replacement. Writes min(src_size, dst_size) units.
 *
 * Contract violations are memory-safe: the copy is still bounded by dst_size,
 * and non-ASCII units are copied as their truncated value (wrong output, never
 * an overrun). The result matches string_cast_size_assume_ascii. A debug build
 * asserts on violations.
 *
 * @param src Source pointer (may be null if src_size is 0).
 * @param src_size Number of source units.
 * @param dst Destination pointer (may be null if dst_size is 0).
 * @param dst_size Destination capacity in to_char_t units.
 */
template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
string_cast_result<from_char_t, to_char_t> string_cast_assume_ascii(
    const from_char_t* src,
    size_t src_size,
    to_char_t* dst,
    size_t dst_size) noexcept
{
    using utype = typename utf::utf_traits<from_char_t>::utype;
    const size_t n = src_size < dst_size ? src_size : dst_size;

    for (size_t i = 0; i < n; ++i)
    {
        const utype u = static_cast<utype>(src[i]);
        VX_ASSERT(is_ascii(u));
        dst[i] = static_cast<to_char_t>(u);
    }

    return { src + n, dst + n, n == src_size };
}

//==============================================================================
// string_cast (core #2): unbounded, iterator based
//==============================================================================
// Converts an entire [first, last) range, writing through an output iterator
// until the source is exhausted. No bound is placed on the output side - safe
// for a growing container via back_inserter, NOT safe for a fixed-capacity
// buffer. Use the bounded core above for a fixed-size destination.
//
// IT2 is constrained to be a non-character type so that a call like
// string_cast<char>(it, it, '?') resolves to the string-returning overload
// (replacement) rather than treating '?' as an output iterator.
//==============================================================================

/**
 * @brief Converts a character range from one encoding to another.
 *
 * @tparam to_char_t Destination character type.
 * @tparam IT1 Input iterator type.
 * @tparam IT2 Output iterator type.
 * @param first Iterator to the beginning of the input range.
 * @param last Iterator to the end of the input range.
 * @param out Output iterator to write converted characters.
 * @param replacement Replacement character to use for invalid sequences.
 * @return Output iterator after the last written character.
 */
template <typename to_char_t, typename IT1, typename IT2, VX_REQUIRES(type_traits::is_char_iter<IT1>::value && !type_traits::is_char<IT2>::value)>
IT2 string_cast(IT1 first, IT1 last, IT2 out, to_char_t replacement = to_char_t('?'))
{
    VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last);

    using from_char_t = typename type_traits::value_type<IT1>::type;
    using encoder = utf::utf_traits<to_char_t>;
    using decoder = utf::utf_traits<from_char_t>;

    while (first != last)
    {
        utf::code_point c{};
        first = decoder::decode(first, last, c);
        out = encoder::encode(c, out, replacement);
    }

    return out;
}

} // namespace str
} // namespace vx
