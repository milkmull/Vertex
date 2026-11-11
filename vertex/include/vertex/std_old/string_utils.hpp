#pragma once

#include "vertex/std/string.hpp"
#include "vertex/std/char_utils.hpp"

namespace vx {
namespace str {

//==============================================================================
// compare
//==============================================================================

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

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_upper(const S& s) noexcept
{
    return is_upper(s.data(), s.size());
}

//==============================================================================
// is_lower
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_lower(const S& s) noexcept
{
    return is_lower(s.data(), s.size());
}

//==============================================================================
// is_alpha
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_alpha(const S& s) noexcept
{
    return is_alpha(s.data(), s.size());
}

//==============================================================================
// is_numeric
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_numeric(const S& s) noexcept
{
    return is_numeric(s.data(), s.size());
}

//==============================================================================
// is_alnum
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_alnum(const S& s) noexcept
{
    return is_alnum(s.data(), s.size());
}

//==============================================================================
// ascii
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_ascii(const S& s) noexcept
{
    return is_ascii(s.data(), s.size());
}

//==============================================================================
// space
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_space(const S& s) noexcept
{
    return is_space(s.data(), s.size());
}

//==============================================================================
// ctrl
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_ctrl(const S& s) noexcept
{
    return is_ctrl(s.data(), s.size());
}

//==============================================================================
// hex
//==============================================================================

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr bool is_hex(const S& s, const bool allow_prefix) noexcept
{
    return is_hex(s.data(), s.size(), allow_prefix);
}

//==============================================================================
// to_lower
//==============================================================================

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void to_lower(S& s) noexcept
{
    to_lower(s.data(), s.size());
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

template <typename S, VX_REQUIRES(is_mutable_string_like<S>::value)>
constexpr void to_upper(S& s) noexcept
{
    to_upper(s.data(), s.size());
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

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr size_t find(
    const S& haystack,
    const typename S::value_type c,
    const size_t start = 0) noexcept
{
    return _char_traits_priv::traits_find_ch(haystack.data(), haystack.size(), start, c);
}

//==============================================================================

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

template <typename S, VX_REQUIRES(is_string_like<S>::value)>
constexpr size_t rfind(
    const S& haystack,
    const typename S::value_type c,
    const size_t start = std::numeric_limits<size_t>::max()) noexcept
{
    return rfind(haystack.data(), haystack.size(), c, start);
}

//==============================================================================

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
    const auto it = _char_traits_priv::traits_remove<traits>(s.data(), s.size(), c);
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

        for (;;)
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

//==============================================================================
// string_cast (string-returning overloads)
//==============================================================================
// Every contiguous input reduces to (ptr, size) and goes through
// string_cast_size + string_cast (core #1), so there is one allocation and no
// growth. Non-contiguous iterators use the unbounded back_inserter path.
//==============================================================================

namespace _string_cast_priv {

template <typename to_char_t, typename from_char_t>
basic_string<to_char_t> cast_to_string(const from_char_t* src, size_t n, to_char_t replacement)
{
    const size_t out_size = string_cast_size<to_char_t>(src, n);

    basic_string<to_char_t> res;
    res.resize(out_size);

    string_cast<to_char_t>(src, n, res.data(), out_size, replacement);
    return res;
}

template <typename to_char_t, typename from_char_t>
basic_string<to_char_t> cast_to_string_assume_ascii(const from_char_t* src, size_t n)
{
    basic_string<to_char_t> res;
    res.resize(string_cast_size_assume_ascii<to_char_t>(src, n));

    string_cast_assume_ascii<to_char_t>(src, n, res.data(), res.size());
    return res;
}

// size argument: any integer type that is not itself a character type, so that
// string_cast<char>(ptr, 5) means "size 5" and string_cast<char>(ptr, '?') means
// "null-terminated, replacement '?'"
template <typename T>
using is_size_arg = std::bool_constant<std::is_integral<T>::value && !type_traits::is_char<T>::value>;

} // namespace _string_cast_priv

//-- pointer + size ------------------------------------------------------------

template <typename to_char_t, typename from_char_t, typename size_t_like, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value&& _string_cast_priv::is_size_arg<size_t_like>::value)>
auto string_cast(const from_char_t* src, size_t_like size, to_char_t replacement = to_char_t('?'))
{
    return _string_cast_priv::cast_to_string<to_char_t>(src, static_cast<size_t>(size), replacement);
}

template <typename to_char_t, typename from_char_t, typename size_t_like, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value&& _string_cast_priv::is_size_arg<size_t_like>::value)>
auto string_cast_assume_ascii(const from_char_t* src, size_t_like size)
{
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(src, static_cast<size_t>(size));
}

//-- pointer range -------------------------------------------------------------

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast(const from_char_t* first, const from_char_t* last, to_char_t replacement = to_char_t('?'))
{
    VX_ITERATOR_VALID_RANGE(first, last);
    return _string_cast_priv::cast_to_string<to_char_t>(first, static_cast<size_t>(last - first), replacement);
}

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast_assume_ascii(const from_char_t* first, const from_char_t* last)
{
    VX_ITERATOR_VALID_RANGE(first, last);
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(first, static_cast<size_t>(last - first));
}

//-- null-terminated -----------------------------------------------------------

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast(const from_char_t* cstr, to_char_t replacement = to_char_t('?'))
{
    using traits = str::char_traits<from_char_t>;
    return _string_cast_priv::cast_to_string<to_char_t>(cstr, cstr ? traits::length(cstr) : 0, replacement);
}

template <typename to_char_t, typename from_char_t, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& type_traits::is_char<from_char_t>::value)>
auto string_cast_assume_ascii(const from_char_t* cstr)
{
    using traits = str::char_traits<from_char_t>;
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(cstr, cstr ? traits::length(cstr) : 0);
}

//-- string-like (anything contiguous with data() and size()) -----------------

template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& is_string_like<S>::value)>
auto string_cast(const S& s, to_char_t replacement = to_char_t('?'))
{
    return _string_cast_priv::cast_to_string<to_char_t>(s.data(), s.size(), replacement);
}

template <typename to_char_t, typename S, VX_REQUIRES(type_traits::is_char<to_char_t>::value&& is_string_like<S>::value)>
auto string_cast_assume_ascii(const S& s)
{
    return _string_cast_priv::cast_to_string_assume_ascii<to_char_t>(s.data(), s.size());
}

//-- generic iterators (slow path, not contiguous) ----------------------------

template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value && !std::is_pointer<IT>::value)>
auto string_cast(IT first, IT last, to_char_t replacement = to_char_t('?'))
{
    basic_string<to_char_t> res;
    res.reserve(string_cast_size<to_char_t>(first, last)); // exact: one allocation, no regrowth
    string_cast<to_char_t>(first, last, back_inserter(res), replacement);
    return res;
}

template <typename to_char_t, typename IT, VX_REQUIRES(type_traits::is_char_iter<IT>::value && !std::is_pointer<IT>::value)>
auto string_cast_assume_ascii(IT first, IT last)
{
    using utype = typename utf::utf_traits<typename type_traits::value_type<IT>::type>::utype;

    basic_string<to_char_t> res;
    res.reserve(static_cast<size_t>(vx::distance(first, last))); // exact for 1:1

    for (; first != last; ++first)
    {
        const utype u = static_cast<utype>(*first);
        VX_ASSERT(u < 0x80);
        res.push_back(static_cast<to_char_t>(u));
    }
    return res;
}

} // namespace str
} // namespace vx
