#pragma once

#include <sstream>
#include <algorithm>

#include "vertex/system/validate.hpp"
#include "vertex/system/error.hpp"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// contains
///////////////////////////////////////////////////////////////////////////////

constexpr bool contains(const str_arg_t& s, const char val) noexcept
{
    return s.find(val) != str_arg_t::npos;
}

constexpr bool contains(const str_arg_t& s, const str_arg_t& val) noexcept
{
    return s.find(val) != str_arg_t::npos;
}

///////////////////////////////////////////////////////////////////////////////
// count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const str_arg_t& s, const char val)
{
    return static_cast<size_t>(
        std::count(s.begin(), s.end(), val)
    );
}

inline size_t count(const str_arg_t& s, const str_arg_t& val)
{
    if (val.empty())
    {
        return s.size() + 1;
    }

    const size_t step = val.size();
    size_t count = 0;
    size_t i = s.find(val);

    while (i != str_arg_t::npos)
    {
        ++count;
        i = s.find(val, i + step);
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// starts_with
///////////////////////////////////////////////////////////////////////////////

constexpr bool starts_with(const str_arg_t& s, const char prefix) noexcept
{
    return !s.empty() && (s[0] == prefix);
}

constexpr bool starts_with(const str_arg_t& s, const str_arg_t& prefix) noexcept
{
    return (s.size() >= prefix.size()) && (s.compare(0, prefix.size(), prefix) == 0);
}

///////////////////////////////////////////////////////////////////////////////
// ends_with
///////////////////////////////////////////////////////////////////////////////

constexpr bool ends_with(const str_arg_t& s, const char suffix) noexcept
{
    return !s.empty() && (s.rfind(suffix) == (s.size() - 1));
}

constexpr bool ends_with(const str_arg_t& s, const str_arg_t& suffix) noexcept
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
    return !s.empty() && std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_alpha));
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
    return !s.empty() && std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_digit));
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
    return !s.empty() && std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_alnum));
}

///////////////////////////////////////////////////////////////////////////////
// ascii
///////////////////////////////////////////////////////////////////////////////

inline bool is_ascii(const char c)
{
    return static_cast<unsigned char>(c) < 0x80;
}

inline bool is_ascii(const str_arg_t& s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_ascii));
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
    return !s.empty() && std::all_of(s.begin(), s.end(), static_cast<bool(*)(const char)>(is_space));
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
    std::string res;
    res.reserve(s.size());

    for (const char c : s)
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
    std::string res;
    res.reserve(s.size());

    for (const char c : s)
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

///////////////////////////////////////////////////////////////////////////////
// case_insensitive_compare
///////////////////////////////////////////////////////////////////////////////

inline bool case_insensitive_compare(const char c1, const char c2)
{
    return to_lower(c1) == to_lower(c2);
}

inline bool case_insensitive_compare(const str_arg_t& s1, const str_arg_t& s2)
{
    return (s1.size() == s2.size()) && std::equal(
        s1.begin(), s1.end(), s2.begin(),
        static_cast<bool(*)(const char, const char)>(case_insensitive_compare)
    );
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
    std::string res(s);
    res.erase(std::remove(res.begin(), res.end(), val), res.end());
    return res;
}

inline std::string remove(const str_arg_t& s, const str_arg_t& val)
{
    std::string res(s);

    if (val.empty())
    {
        return res;
    }

    const size_t step = val.size();
    size_t i = res.find(val);

    while (i != str_arg_t::npos)
    {
        res.erase(i, step);
        i = res.find(val, i + step);
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

    if (old_val.empty())
    {
        const size_t size = s.size();

        res.reserve((size + 1) * new_val.size());
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
        const size_t step = old_val.empty() ? 1 : old_val.size();

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
    VX_ITERATOR_VALID_RANGE(first, last);

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
// split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<str_arg_t> split(const str_arg_t& s, const char delimiter)
{
    std::vector<str_arg_t> res;

    if (s.empty())
    {
        return res;
    }

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

inline std::vector<str_arg_t> split(const str_arg_t& s, const str_arg_t& delimiter)
{
    std::vector<str_arg_t> res;

    if (s.empty())
    {
        return res;
    }

    if (delimiter.empty())
    {
        const size_t size = s.size();
        res.reserve(size);

        for (size_t i = 0; i < size; ++i)
        {
            res.emplace_back(s.substr(i, 1));
        }
    }
    else
    {
        const size_t step = delimiter.size();
        size_t start = 0;
        size_t end = 0;

        while (end != str_arg_t::npos)
        {
            end = s.find(delimiter, start);
            res.emplace_back(s.substr(start, end - start));
            start = end + step;
        }
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// split_words
///////////////////////////////////////////////////////////////////////////////

inline std::vector<str_arg_t> split_words(const str_arg_t& s)
{
    std::vector<str_arg_t> res;

    constexpr const char* chars = " \t\n\v\f\r";

    size_t start = s.find_first_not_of(chars);
    size_t end = 0;

    while (start != str_arg_t::npos)
    {
        end = s.find_first_of(chars, start);
        res.emplace_back(s.substr(start, end - start));
        start = s.find_first_not_of(chars, end);
    }

    return res;
}

///////////////////////////////////////////////////////////////////////////////
// split_lines
///////////////////////////////////////////////////////////////////////////////

inline std::vector<str_arg_t> split_lines(const str_arg_t& s)
{
    return split(s, '\n');
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
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT, typename T>
inline IT parse_digits(IT first, IT last, T& value, size_t* count)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    const IT start = first;
    value = static_cast<T>(0);

    while (first != last && is_digit(*first))
    {
        value = static_cast<T>(value * 10) + static_cast<T>(*first - '0');
        ++first;
    }

    if (count)
    {
        *count = static_cast<size_t>(std::distance(start, first));
    }

    return first;
}

inline std::string to_hex_string(const void* data, size_t size)
{
    static constexpr char hex[] = "0123456789ABCDEF";

    std::string res;
    res.reserve(2 * size);

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);

    for (size_t i = 0; i < size; ++i)
    {
        const char x1 = hex[(bytes[i] >> 4) & 0xF]; // High nibble
        const char x2 = hex[(bytes[i] >> 0) & 0xF]; // Low nibble

        res.push_back(x1);
        res.push_back(x2);
    }

    return res;
}

inline int32_t to_int32(const std::string& s, size_t* count, int base)
{
    int32_t value = 0;
    size_t i = 0;

    if (base < 2 || base > 36)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = static_cast<int32_t>(std::stol(s, &i, base));
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
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
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = static_cast<int64_t>(std::stoll(s, &i, base));
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
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
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = static_cast<uint32_t>(std::stoul(s, &i, base));
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
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
        VX_ERR(err::INVALID_ARGUMENT) << "base should be between 2 and 36";
    }

    try
    {
        value = static_cast<uint64_t>(std::stoull(s, &i, base));
    }
    catch (const std::invalid_argument&)
    {
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
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
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
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
        VX_ERR(err::INVALID_ARGUMENT) << "invalid character: " << s[i];
    }
    catch (const std::out_of_range&)
    {
        err::set(err::OUT_OF_RANGE);
    }

    if (count)
    {
        *count = i;
    }

    return value;
}

} // namespace str
} // namespace vx