#pragma once

#include <sstream>

#include "vertex/system/error.hpp"
#include "utf.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// To/From String
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string to_string(T value)
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

inline bool contains(const str_arg_t& s, const char val)
{
    return s.find(val) != str_arg_t::npos;
}

inline bool contains(const wstr_arg_t& s, const wchar_t val)
{
    return s.find(val) != wstr_arg_t::npos;
}

inline bool contains(const str_arg_t& s, const str_arg_t& val)
{
    return s.find(val) != str_arg_t::npos;
}

inline bool contains(const wstr_arg_t& s, const wstr_arg_t& val)
{
    return s.find(val) != wstr_arg_t::npos;
}

///////////////////////////////////////////////////////////////////////////////
// count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const str_arg_t& s, const char val)
{
    size_t count = 0;

    for (const char c : s)
    {
        if (c == val)
        {
            ++count;
        }
    }

    return count;
}

inline size_t count(const wstr_arg_t& s, const wchar_t val)
{
    size_t count = 0;

    for (const wchar_t c : s)
    {
        if (c == val)
        {
            ++count;
        }
    }

    return count;
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

inline size_t count(const wstr_arg_t& s, const wstr_arg_t& val)
{
    size_t count = 0;
    size_t i = 0;
    const size_t step = val.size();

    while ((i = s.find(val, i)) != wstr_arg_t::npos)
    {
        ++count;
        i += step;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// starts_with
///////////////////////////////////////////////////////////////////////////////

inline bool starts_with(const str_arg_t& s, const char prefix)
{
    return !s.empty() && s[0] == prefix;
}

inline bool starts_with(const wstr_arg_t& s, const wchar_t prefix)
{
    return !s.empty() && s[0] == prefix;
}

inline bool starts_with(const str_arg_t& s, const str_arg_t& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

inline bool starts_with(const wstr_arg_t& s, const wstr_arg_t& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// ends_with
///////////////////////////////////////////////////////////////////////////////

inline bool ends_with(const str_arg_t& s, const char suffix)
{
    return s.rfind(suffix) == (s.size() - 1);
}

inline bool ends_with(const wstr_arg_t& s, const wchar_t suffix)
{
    return s.rfind(suffix) == (s.size() - 1);
}

inline bool ends_with(const str_arg_t& s, const str_arg_t& suffix)
{
    return s.rfind(suffix) == (s.size() - suffix.size());
}

inline bool ends_with(const wstr_arg_t& s, const wstr_arg_t& suffix)
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

inline bool is_alpha(const wchar_t c)
{
    return std::isalpha(c);
}

inline bool is_alpha(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_alpha(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_alpha(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_alpha(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// numeric
///////////////////////////////////////////////////////////////////////////////

inline bool is_digit(const char c)
{
    return std::isdigit(c);
}

inline bool is_digit(const wchar_t c)
{
    return std::isdigit(c);
}

inline bool is_numeric(const str_arg_t& s, const bool allow_sign)
{
    const size_t size = s.size();

    if (size == 0)
    {
        return false;
    }

    size_t start = 0;
    if (allow_sign && (s[0] == '+' || s[0] == '-'))
    {
        ++start;

        if (start >= size)
        {
            return false;
        }
    }

    for (size_t i = start; i < size; ++i)
    {
        if (!is_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

inline bool is_numeric(const wstr_arg_t& s, const bool allow_sign)
{
    const size_t size = s.size();

    if (size == 0)
    {
        return false;
    }

    size_t start = 0;
    if (allow_sign && (s[0] == L'+' || s[0] == L'-'))
    {
        ++start;

        if (start >= size)
        {
            return false;
        }
    }

    for (size_t i = start; i < size; ++i)
    {
        if (!is_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// alnum
///////////////////////////////////////////////////////////////////////////////

inline bool is_alnum(const char c)
{
    return std::isalnum(c);
}

inline bool is_alnum(const wchar_t c)
{
    return std::isalnum(c);
}

inline bool is_alnum(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_alnum(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_alnum(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_alnum(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// ascii
///////////////////////////////////////////////////////////////////////////////

inline bool is_ascii(const char c)
{
    return 0x00 <= c && c < 0x80;
}

inline bool is_ascii(const wchar_t c)
{
    return 0x00 <= c && c < 0x80;
}

inline bool is_ascii(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_ascii(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_ascii(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_ascii(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// space
///////////////////////////////////////////////////////////////////////////////

inline bool is_space(const char c)
{
    return std::isspace(c);
}

inline bool is_space(const wchar_t c)
{
    return std::isspace(c);
}

inline bool is_space(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_space(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_space(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_space(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// hex
///////////////////////////////////////////////////////////////////////////////

inline bool is_hex_digit(const char c)
{
    return std::isxdigit(c);
}

inline bool is_hex_digit(const wchar_t c)
{
    return std::isxdigit(c);
}

inline bool is_hex(const str_arg_t& s)
{
    const size_t size = s.size();

    if (size == 0)
    {
        return false;
    }

    // Check for optional '0x' or '0X' prefix
    size_t start = 0;
    if (size > 2 && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X'))
    {
        start = 2;

        if (start >= size)
        {
            return false;
        }
    }

    // Validate the remaining characters are hex digits
    for (size_t i = start; i < size; ++i)
    {
        if (!is_hex_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

inline bool is_hex(const wstr_arg_t& s)
{
    const size_t size = s.size();

    if (size == 0)
    {
        return false;
    }

    // Check for optional '0x' or '0X' prefix
    size_t start = 0;
    if (size > 2 && (s[0] == L'0') && (s[1] == L'x' || s[1] == L'X'))
    {
        start = 2;

        if (start >= size)
        {
            return false;
        }
    }

    // Validate the remaining characters are hex digits
    for (size_t i = start; i < size; ++i)
    {
        if (!is_hex_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

inline std::string to_hex_string(const void* data, size_t size)
{
    static constexpr char hex[] = "0123456789ABCDEF";

    std::string res;
    res.reserve(2 * size);

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);

    for (int i = 0; i < size; ++i)
    {
        const char x1 = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        const char x2 = hex[(bytes[i] >> 0) & 0xF]; // Low nibble

        res.push_back(x1);
        res.push_back(x2);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// lower
///////////////////////////////////////////////////////////////////////////////

inline bool is_lower(const char c)
{
    return std::islower(c);
}

inline bool is_lower(const wchar_t c)
{
    return std::islower(c);
}

inline bool is_lower(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_lower(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_lower(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_lower(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// to_lower
///////////////////////////////////////////////////////////////////////////////

inline char to_lower(const char c)
{
    return static_cast<char>(std::tolower(c));
}

inline wchar_t to_lower(const wchar_t c)
{
    return static_cast<wchar_t>(std::tolower(c));
}

inline std::string to_lower(const str_arg_t& s)
{
    std::string res;
    res.reserve(s.size());

    for (const char c : s)
    {
        res.push_back(to_lower(c));
    }

    return res;
}

inline std::wstring to_lower(const wstr_arg_t& s)
{
    std::wstring res;
    res.reserve(s.size());

    for (const wchar_t c : s)
    {
        res.push_back(to_lower(c));
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// upper
///////////////////////////////////////////////////////////////////////////////

inline bool is_upper(const char c)
{
    return std::isupper(c);
}

inline bool is_upper(const wchar_t c)
{
    return std::isupper(c);
}

inline bool is_upper(const str_arg_t& s)
{
    for (const char c : s)
    {
        if (!is_upper(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_upper(const wstr_arg_t& s)
{
    for (const wchar_t c : s)
    {
        if (!is_upper(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// to_upper
///////////////////////////////////////////////////////////////////////////////

inline char to_upper(const char c)
{
    return static_cast<char>(std::toupper(c));
}

inline wchar_t to_upper(const wchar_t c)
{
    return static_cast<wchar_t>(std::toupper(c));
}

inline std::string to_upper(const str_arg_t& s)
{
    std::string res;
    res.reserve(s.size());

    for (const char c : s)
    {
        res.push_back(to_upper(c));
    }

    return res;
}

inline std::wstring to_upper(const wstr_arg_t& s)
{
    std::wstring res;
    res.reserve(s.size());

    for (const wchar_t c : s)
    {
        res.push_back(to_upper(c));
    }

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

inline std::wstring title(const wstr_arg_t& s)
{
    std::wstring res;
    res.reserve(s.size());

    bool new_word = true;
    for (const wchar_t c : s)
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

inline bool case_insensitive_compare(const wstr_arg_t& s1, const wstr_arg_t& s2)
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

inline std::wstring strip(const wstr_arg_t& s, const wstr_arg_t& characters)
{
    std::wstring res;

    const size_t start = s.find_first_not_of(characters);
    if (start != wstr_arg_t::npos)
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

inline std::wstring lstrip(const wstr_arg_t& s, const wstr_arg_t& characters)
{
    std::wstring res;

    const size_t start = s.find_first_not_of(characters);
    if (start != wstr_arg_t::npos)
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

inline std::wstring rstrip(const wstr_arg_t& s, const wstr_arg_t& characters)
{
    std::wstring res;

    const size_t end = s.find_last_not_of(characters);
    if (end != wstr_arg_t::npos)
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

inline std::wstring trim_prefix(const wstr_arg_t& s, const wstr_arg_t& prefix)
{
    return std::wstring(starts_with(s, prefix) ? s.substr(prefix.size()) : s);
}

///////////////////////////////////////////////////////////////////////////////
// trim_suffix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_suffix(const str_arg_t& s, const str_arg_t& suffix)
{
    return std::string(ends_with(s, suffix) ? s.substr(0, s.size() - suffix.size()) : s);
}

inline std::wstring trim_suffix(const wstr_arg_t& s, const wstr_arg_t& suffix)
{
    return std::wstring(ends_with(s, suffix) ? s.substr(0, s.size() - suffix.size()) : s);
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

inline std::wstring remove(const wstr_arg_t& s, const wchar_t val)
{
    std::wstring res;
    res.reserve(s.size() - count(s, val));

    for (const wchar_t c : s)
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

inline std::wstring remove(const wstr_arg_t& s, const wstr_arg_t& val)
{
    std::wstring res;

    size_t start = 0;
    size_t end = 0;
    const size_t step = val.size();

    const size_t size = s.size() - count(s, val) * step;
    res.reserve(size);

    while (end == wstr_arg_t::npos)
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
    std::string res;
    res.reserve(s.size());

    for (const char c : s)
    {
        res.push_back((c == old_val) ? new_val : c);
    }

    return res;
}

inline std::wstring replace(
    const wstr_arg_t& s,
    const wchar_t old_val,
    const wchar_t new_val
)
{
    std::wstring res;
    res.reserve(s.size());

    for (const wchar_t c : s)
    {
        res.push_back((c == old_val) ? new_val : c);
    }

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

inline std::wstring replace(
    const wstr_arg_t& s,
    const wstr_arg_t& old_val,
    const wstr_arg_t& new_val
)
{
    std::wstring res;

    size_t start = 0;
    size_t end = 0;
    const size_t step = old_val.size();

    const size_t size = s.size() + count(s, old_val) * (new_val.size() - step);
    res.reserve(size);

    while (true)
    {
        end = s.find(old_val, start);
        res.append(s.substr(start, end - start));

        if (end == wstr_arg_t::npos)
        {
            break;
        }

        res.append(new_val);
        start = end + step;
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// join
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join(IT first, IT last, const str_arg_t& delimiter)
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

template <typename IT>
inline std::wstring wjoin(IT first, IT last, const wstr_arg_t& delimiter)
{
    std::wostringstream oss;

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

template <typename IT>
inline std::wstring wjoin_path(IT first, IT last)
{
    return wjoin(first, last, L"\\");
}

inline std::string join_path(const str_arg_t& s1, const str_arg_t& s2)
{
    const str_arg_t pair[2] = { s1, s2 };
    return join_path(std::begin(pair), std::end(pair));
}

inline std::wstring wjoin_path(const wstr_arg_t& s1, const wstr_arg_t& s2)
{
    const wstr_arg_t pair[2] = { s1, s2 };
    return wjoin_path(std::begin(pair), std::end(pair));
}

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

inline std::vector<std::wstring> split(const wstr_arg_t& s, const wchar_t delimiter)
{
    std::vector<std::wstring> res;
    res.reserve(count(s, delimiter) + 1);

    size_t start = 0;
    size_t end = 0;

    while (end != wstr_arg_t::npos)
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

inline std::vector<std::wstring> split(const wstr_arg_t& s, const wstr_arg_t& delimiter)
{
    std::vector<std::wstring> res;
    res.reserve(count(s, delimiter) + 1);

    size_t start = 0;
    size_t end = 0;

    while (end != wstr_arg_t::npos)
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

inline std::vector<std::wstring> split_any(const wstr_arg_t& s, const wstr_arg_t& characters)
{
    std::vector<std::wstring> res;

    size_t start = 0;
    size_t end = 0;

    while (end != wstr_arg_t::npos)
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

inline std::vector<std::wstring> split_words(const wstr_arg_t& s)
{
    std::vector<std::wstring> res;

    size_t start = 0;
    size_t end = 0;

    constexpr const wchar_t* chars = L" \t\n\v\f\r";

    while ((start = s.find_first_not_of(chars, end)) != wstr_arg_t::npos)
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

inline std::vector<std::wstring> split_lines(const wstr_arg_t& s)
{
    return split(s, L'\n');
}

///////////////////////////////////////////////////////////////////////////////
// split_path
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split_path(const str_arg_t& s)
{
    return split_any(s, "/\\");
}

inline std::vector<std::wstring> split_path(const wstr_arg_t& s)
{
    return split_any(s, L"/\\");
}

///////////////////////////////////////////////////////////////////////////////
// repeat
///////////////////////////////////////////////////////////////////////////////

inline std::string repeat(const char s, size_t n)
{
    return std::string(n, s);
}

inline std::wstring repeat(const wchar_t s, size_t n)
{
    return std::wstring(n, s);
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

inline std::wstring repeat(const wstr_arg_t& s, size_t n)
{
    std::wstring res;
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

inline std::wstring reverse(const wstr_arg_t& s)
{
    return std::wstring(s.rbegin(), s.rend());
}

///////////////////////////////////////////////////////////////////////////////
// wstring
///////////////////////////////////////////////////////////////////////////////

inline std::wstring string_to_wstring(const std::string& s)
{
    const size_t count = utf8::count(s.begin(), s.end());
    std::wstring res(count, 0);
    utf8::to_wide(s.begin(), s.end(), res.begin());
    return res;
}

inline std::string wstring_to_string(const std::wstring& ws)
{
    std::string res;
    utf8::from_wide(ws.begin(), ws.end(), std::back_inserter(res));
    return res;
}

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

inline int32_t to_int32(const std::string& s, size_t* count, int base)
{
    int32_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stol(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

inline int64_t to_int64(const std::string& s, size_t* count, int base)
{
    int64_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoll(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

inline uint32_t to_uint32(const std::string& s, size_t* count, int base)
{
    uint32_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoul(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

inline uint64_t to_uint64(const std::string& s, size_t* count, int base)
{
    uint64_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = std::stoull(s, &i, base);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

inline float to_float(const std::string& s, size_t* count)
{
    float value = 0.0f;
    size_t i = 0;

    try
    {
        value = std::stof(s, &i);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

inline double to_double(const std::string& s, size_t* count)
{
    double value = 0.0;
    size_t i = 0;

    try
    {
        value = std::stod(s, &i);
    }
    catch (const std::invalid_argument&)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        error::set_error(error::error_code::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

}
}