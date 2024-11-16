#pragma once

#include <sstream>
#include <algorithm>

#include "vertex/util/encode/utf.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// To/From String
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename T>
inline T from_string(const std::string& s)
{
    T value{};
    std::istringstream(s) >> value;
    return value;
}

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

constexpr bool contains(const str_arg_t& s, const char val)
{
    return s.find(val) != str_arg_t::npos;
}

constexpr bool contains(const str_arg_t& s, const str_arg_t& val)
{
    return s.find(val) != str_arg_t::npos;
}

///////////////////////////////////////////////////////////////////////////////
// count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const str_arg_t& s, const char val)
{
    return std::count(s.begin(), s.end(), val);
}

inline size_t count(const str_arg_t& s, const str_arg_t& val)
{
    size_t count = 0;
    size_t i = 0;
    const size_t step = val.size();

    while ((i = s.find(val, i)) != str_arg_t::npos)
    {
        ++count;
        i += step;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// starts_with
///////////////////////////////////////////////////////////////////////////////

constexpr bool starts_with(const str_arg_t& s, const char prefix)
{
    return !s.empty() && s[0] == prefix;
}

constexpr bool starts_with(const str_arg_t& s, const str_arg_t& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// ends_with
///////////////////////////////////////////////////////////////////////////////

constexpr bool ends_with(const str_arg_t& s, const char suffix)
{
    return s.rfind(suffix) == (s.size() - 1);
}

constexpr bool ends_with(const str_arg_t& s, const str_arg_t& suffix)
{
    return s.rfind(suffix) == (s.size() - suffix.size());
}

///////////////////////////////////////////////////////////////////////////////
// alpha
///////////////////////////////////////////////////////////////////////////////

inline bool is_alpha(const char c)
{
    return std::isalpha(c);
}

inline bool is_alpha(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_alpha));
}

///////////////////////////////////////////////////////////////////////////////
// numeric
///////////////////////////////////////////////////////////////////////////////

inline bool is_digit(const char c)
{
    return std::isdigit(c);
}

inline bool is_numeric(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_digit));
}

///////////////////////////////////////////////////////////////////////////////
// alnum
///////////////////////////////////////////////////////////////////////////////

inline bool is_alnum(const char c)
{
    return std::isalnum(c);
}

inline bool is_alnum(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_alnum));
}

///////////////////////////////////////////////////////////////////////////////
// ascii
///////////////////////////////////////////////////////////////////////////////

inline bool is_ascii(const char c)
{
    return 0x00 <= c && c < 0x80;
}

inline bool is_ascii(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_ascii));
}

///////////////////////////////////////////////////////////////////////////////
// space
///////////////////////////////////////////////////////////////////////////////

inline bool is_space(const char c)
{
    return std::isspace(c);
}

inline bool is_space(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_space));
}

///////////////////////////////////////////////////////////////////////////////
// hex
///////////////////////////////////////////////////////////////////////////////

inline bool is_hex_digit(const char c)
{
    return std::isxdigit(c);
}

inline bool is_hex(const str_arg_t& s, const bool allow_prefix)
{
    const size_t size = s.size();

    if (size == 0)
    {
        return false;
    }

    size_t start = 0;

    // Check for optional '0x' or '0X' prefix
    if (allow_prefix && size > 2 && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X'))
    {
        start = 2;

        if (start >= size)
        {
            return false;
        }
    }

    return std::all_of(s.begin() + start, s.end(), static_cast<bool(*)(const char)>(is_hex_digit));
}

///////////////////////////////////////////////////////////////////////////////
// lower
///////////////////////////////////////////////////////////////////////////////

inline bool is_lower(const char c)
{
    return std::islower(c);
}

inline bool is_lower(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_lower));
}

///////////////////////////////////////////////////////////////////////////////
// to_lower
///////////////////////////////////////////////////////////////////////////////

inline char to_lower(const char c)
{
    return static_cast<char>(std::tolower(c));
}

inline std::string to_lower(const str_arg_t& s)
{
    std::string res(s);
    std::transform(res.begin(), res.end(), res.begin(), static_cast<char(*)(const char)>(to_lower));
    return res;
}

///////////////////////////////////////////////////////////////////////////////
// upper
///////////////////////////////////////////////////////////////////////////////

inline bool is_upper(const char c)
{
    return std::isupper(c);
}

inline bool is_upper(const str_arg_t& s)
{
    return std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_upper));
}

///////////////////////////////////////////////////////////////////////////////
// to_upper
///////////////////////////////////////////////////////////////////////////////

inline char to_upper(const char c)
{
    return static_cast<char>(std::toupper(c));
}

inline std::string to_upper(const str_arg_t& s)
{
    std::string res(s);
    std::transform(res.begin(), res.end(), res.begin(), static_cast<char(*)(const char)>(to_upper));
    return res;
}

///////////////////////////////////////////////////////////////////////////////
// title
///////////////////////////////////////////////////////////////////////////////

inline std::string title(const str_arg_t& s)
{
    std::string res;
    res.reserve(s.size());

    bool new_word = true;
    for (const char c : s)
    {
        if (is_alpha(c))
        {
            res.push_back(new_word ? to_upper(c) : to_lower(c));
            new_word = false;
        }
        else
        {
            res.push_back(c);
            new_word = true;
        }
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// case_insensitive_compare
///////////////////////////////////////////////////////////////////////////////

inline bool case_insensitive_compare(const str_arg_t& s1, const str_arg_t& s2)
{
    return to_lower(s1) == to_lower(s2);
}

///////////////////////////////////////////////////////////////////////////////
// strip
///////////////////////////////////////////////////////////////////////////////

inline std::string strip(const str_arg_t& s, const str_arg_t& characters)
{
    std::string res;

    const size_t start = s.find_first_not_of(characters);
    if (start != str_arg_t::npos)
    {
        const size_t end = s.find_last_not_of(characters);
        res = s.substr(start, end - start + 1);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// lstrip
///////////////////////////////////////////////////////////////////////////////

inline std::string lstrip(const str_arg_t& s, const str_arg_t& characters)
{
    std::string res;

    const size_t start = s.find_first_not_of(characters);
    if (start != str_arg_t::npos)
    {
        res = s.substr(start);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// rstrip
///////////////////////////////////////////////////////////////////////////////

inline std::string rstrip(const str_arg_t& s, const str_arg_t& characters)
{
    std::string res;

    const size_t end = s.find_last_not_of(characters);
    if (end != str_arg_t::npos)
    {
        res = s.substr(0, end + 1);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// trim_prefix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_prefix(const str_arg_t& s, const str_arg_t& prefix)
{
    return std::string(starts_with(s, prefix) ? s.substr(prefix.size()) : s);
}

///////////////////////////////////////////////////////////////////////////////
// trim_suffix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_suffix(const str_arg_t& s, const str_arg_t& suffix)
{
    return std::string(ends_with(s, suffix) ? s.substr(0, s.size() - suffix.size()) : s);
}

///////////////////////////////////////////////////////////////////////////////
// remove
///////////////////////////////////////////////////////////////////////////////

inline std::string remove(const str_arg_t& s, const char val)
{
    std::string res;
    res.reserve(s.size() - count(s, val));

    for (const char c : s)
    {
        if (c != val)
        {
            res.push_back(c);
        }
    }

    return res;
}

inline std::string remove(const str_arg_t& s, const str_arg_t& val)
{
    std::string res;

    size_t start = 0;
    size_t end = 0;
    const size_t step = val.size();

    const size_t size = s.size() - count(s, val) * step;
    res.reserve(size);

    while (end == str_arg_t::npos)
    {
        end = s.find(val, start);
        res.append(s.substr(start, end - start));
        start = end + step;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// replace
///////////////////////////////////////////////////////////////////////////////

inline std::string replace(
    const str_arg_t& s,
    const char old_val,
    const char new_val
)
{
    std::string res(s);
    std::replace(res.begin(), res.end(), old_val, new_val);
    return res;
}

inline std::string replace(
    const str_arg_t& s,
    const str_arg_t& old_val,
    const str_arg_t& new_val
)
{
    std::string res;

    size_t start = 0;
    size_t end = 0;
    const size_t step = old_val.size();

    const size_t size = s.size() + count(s, old_val) * (new_val.size() - step);
    res.reserve(size);

    while (true)
    {
        end = s.find(old_val, start);
        res.append(s.substr(start, end - start));

        if (end == str_arg_t::npos)
        {
            break;
        }

        res.append(new_val);
        start = end + step;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// concat
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string concat(IT first, IT last)
{
    return join(first, last, str_arg_t());
}

///////////////////////////////////////////////////////////////////////////////
// join
///////////////////////////////////////////////////////////////////////////////

template <typename IT, typename Delim>
inline std::string join(IT first, IT last, const Delim& delimiter)
{
    std::ostringstream oss;

    for (IT it = first; it != last; ++it)
    {
        if (it != first)
        {
            oss << delimiter;
        }

        oss << *it;
    }

    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
// join_path
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join_path(IT first, IT last)
{
    return join(first, last, "\\");
}

inline std::string join_path(const char* s1, const char* s2)
{
    return join_path(str_arg_t(s1), str_arg_t(s2));
}

inline std::string join_path(const char* s1, const std::string& s2)
{
    return join_path(str_arg_t(s1), str_arg_t(s2));
}

inline std::string join_path(const std::string& s1, const char* s2)
{
    return join_path(str_arg_t(s1), str_arg_t(s2));
}

inline std::string join_path(const std::string& s1, const std::string& s2)
{
    return join_path(str_arg_t(s1), str_arg_t(s2));
}

#if (VX_CPP_STANDARD == 17)

inline std::string join_path(const str_arg_t& s1, const str_arg_t& s2)
{
    const str_arg_t pair[2] = { s1, s2 };
    return join(std::begin(pair), std::end(pair), "\\");
}

#endif

///////////////////////////////////////////////////////////////////////////////
// split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split(const str_arg_t& s, const char delimiter)
{
    std::vector<std::string> res;
    res.reserve(count(s, delimiter) + 1);

    size_t start = 0;
    size_t end = 0;

    while (end != str_arg_t::npos)
    {
        end = s.find(delimiter, start);
        res.emplace_back(s.substr(start, end - start));
        start = end + 1;
    }

    return res;
}

inline std::vector<std::string> split(const str_arg_t& s, const str_arg_t& delimiter)
{
    std::vector<std::string> res;
    res.reserve(count(s, delimiter) + 1);

    size_t start = 0;
    size_t end = 0;

    while (end != str_arg_t::npos)
    {
        end = s.find(delimiter, start);
        res.emplace_back(s.substr(start, end - start));
        start = end + delimiter.size();
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// split_any
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_any(const str_arg_t& s, const str_arg_t& characters)
{
    std::vector<std::string> res;

    size_t start = 0;
    size_t end = 0;

    while (end != str_arg_t::npos)
    {
        end = s.find_first_of(characters, start);
        res.emplace_back(s.substr(start, end - start));
        start = end + 1;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// split_words
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_words(const str_arg_t& s)
{
    std::vector<std::string> res;

    size_t start = 0;
    size_t end = 0;

    constexpr const char* chars = " \t\n\v\f\r";

    while ((start = s.find_first_not_of(chars, end)) != str_arg_t::npos)
    {
        end = s.find_first_of(chars, start);
        res.emplace_back(s.substr(start, end - start));
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// split_lines
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_lines(const str_arg_t& s)
{
    return split(s, '\n');
}

///////////////////////////////////////////////////////////////////////////////
// split_path
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_path(const str_arg_t& s)
{
    return split_any(s, "/\\");
}

///////////////////////////////////////////////////////////////////////////////
// repeat
///////////////////////////////////////////////////////////////////////////////

inline std::string repeat(const char s, size_t n)
{
    return std::string(n, s);
}

inline std::string repeat(const str_arg_t& s, size_t n)
{
    std::string res;
    res.reserve(s.size() * n);

    for (size_t i = 0; i < n; ++i)
    {
        res.append(s);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// reverse
///////////////////////////////////////////////////////////////////////////////

inline std::string reverse(const str_arg_t& s)
{
    return std::string(s.rbegin(), s.rend());
}

///////////////////////////////////////////////////////////////////////////////
// wstring
///////////////////////////////////////////////////////////////////////////////

inline std::wstring string_to_wstring(const str_arg_t& s)
{
    const size_t count = utf8::count(s.begin(), s.end());
    std::wstring res(count, 0);
    utf8::to_wide(s.begin(), s.end(), res.begin());
    return res;
}

inline std::string wstring_to_string(const wstr_arg_t& ws)
{
    std::string res;
    utf8::from_wide(ws.begin(), ws.end(), std::back_inserter(res));
    return res;
}

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT, typename Num>
inline IT parse_digits(IT first, IT last, Num& value, size_t* count)
{
    const IT start = first;
    value = static_cast<Num>(0);

    while (first != last && is_digit(*first))
    {
        value = (value * 10) + static_cast<Num>(*first - '0');
        ++first;
    }

    if (count)
    {
        *count = static_cast<size_t>(std::distance(first, start));
    }

    return first;
}

} // namespace str
} // namespace vx