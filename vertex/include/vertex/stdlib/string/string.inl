#pragma once

#include <sstream>
#include <vector>

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
// Contains
///////////////////////////////////////////////////////////////////////////////

inline bool contains(const std::string& s, const char c)
{
    return s.find(c) != std::string::npos;
}

inline bool contains(const std::string& s1, const std::string& s2)
{
    return s1.find(s2) != std::string::npos;
}

///////////////////////////////////////////////////////////////////////////////
// Count
///////////////////////////////////////////////////////////////////////////////

inline size_t count(const std::string& s, const char c)
{
    return std::count(s.begin(), s.end(), c);
}

inline size_t count(const std::string& s1, const std::string& s2)
{
    size_t count = 0;
    size_t i = 0;

    while ((i = s1.find(s2, i)) != std::string::npos)
    {
        ++count;
        i += s2.size();
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// Starts/Ends With
///////////////////////////////////////////////////////////////////////////////

inline bool starts_with(const std::string& s, const char value)
{
    return s.find(value) == 0;
}

inline bool starts_with(const std::string& s, const std::string& value)
{
    return s.find(value) == 0;
}

inline bool ends_with(const std::string& s, const char value)
{
    return s.find(value) == (s.size() - 1);
}

inline bool ends_with(const std::string& s, const std::string& value)
{
    return s.find(value) == (s.size() - value.size() - 1);
}

///////////////////////////////////////////////////////////////////////////////
// Boolean Checks
///////////////////////////////////////////////////////////////////////////////

inline bool is_alpha(const char c)
{
    return std::isalpha(c);
}

inline bool is_alpha(const std::string& s)
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

inline bool is_digit(const char c)
{
    return std::isdigit(c);
}

inline bool is_numeric(const std::string& s)
{
    for (const char c : s)
    {
        if (!is_digit(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_alnum(const char c)
{
    return std::isalnum(c);
}

inline bool is_alnum(const std::string& s)
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

inline bool is_ascii(const char c)
{
    return 0x00 <= c && c < 0x80;
}

inline bool is_ascii(const std::string& s)
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

inline bool is_space(const char c)
{
    return std::isspace(c);
}

inline bool is_space(const std::string& s)
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

inline bool is_printable(const char c)
{
    return std::isprint(c);
}

inline bool is_printable(const std::string& s)
{
    for (const char c : s)
    {
        if (!is_printable(c))
        {
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hex
///////////////////////////////////////////////////////////////////////////////

inline bool is_hex_digit(const char c)
{
    return std::isxdigit(c);
}

inline bool is_hex(const std::string& s)
{
    if (s.empty() || s.size() < 3)
    {
        return false;
    }

    // Check for optional '0x' or '0X' prefix
    size_t start = 0;
    if (s.size() > 2 && (s[0] == '0') && (s[1] == 'x' || s[1] == 'X'))
    {
        start = 2;
    }

    if (start >= s.size())
    {
        return false;
    }

    for (size_t i = start; i < s.size(); ++i)
    {
        if (!is_hex_digit(s[i]))
        {
            return false;
        }
    }

    return true;
}

inline std::string to_hex_string(const uint8_t* data, size_t size)
{
    static const char hex[] = "0123456789ABCDEF";

    std::string result;
    result.reserve(2 * size);

    for (int i = 0; i < size; ++i)
    {
        const char x1 = hex[(data[i] >> 4) & 0xF]; // High nibble
        const char x2 = hex[(data[i] >> 0) & 0xF]; // Low nibble

        result.push_back(x1);
        result.push_back(x2);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Upper/Lower
///////////////////////////////////////////////////////////////////////////////

inline bool is_lower(const char c)
{
    return std::islower(c);
}

inline bool is_lower(const std::string& s)
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

inline bool is_upper(const char c)
{
    return std::isupper(c);
}

inline bool is_upper(const std::string& s)
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

inline char to_lower(const char c)
{
    return static_cast<char>(std::tolower(c));
}

inline std::string to_lower(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    for (const char c : s)
    {
        result.push_back(to_lower(c));
    }

    return result;
}

inline char to_upper(const char c)
{
    return static_cast<char>(std::toupper(c));
}

inline std::string to_upper(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    for (const char c : s)
    {
        result.push_back(to_upper(c));
    }

    return result;
}

inline std::string title(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    bool new_word = true;
    for (const char c : s)
    {
        if (is_alpha(c))
        {
            result.push_back(new_word ? to_upper(c) : to_lower(c));
            new_word = false;
        }
        else
        {
            result.push_back(c);
            new_word = true;
        }
    }

    return result;
}

inline bool case_insensitive_compare(const std::string& s1, const std::string& s2)
{
    return to_lower(s1) == to_lower(s2);
}

///////////////////////////////////////////////////////////////////////////////
// Strip
///////////////////////////////////////////////////////////////////////////////

inline std::string strip(const std::string& s)
{
    return strip(s, std::string(" \t\n\v\f\r"));
}

inline std::string strip(const std::string& s, const std::string& characters)
{
    std::string result;
    size_t start = 0;
    size_t end = 0;

    start = s.find_first_not_of(characters);
    if (start != std::string::npos)
    {
        end = s.find_last_not_of(characters);
        result = s.substr(start, end - start + 1);
    }

    return result;
}

inline std::string lstrip(const std::string& s)
{
    return lstrip(s, std::string(" \t\n\v\f\r"));
}

inline std::string lstrip(const std::string& s, const std::string& characters)
{
    std::string result;

    size_t start = s.find_first_not_of(characters);
    if (start != std::string::npos)
    {
        result = s.substr(start);
    }

    return result;
}

inline std::string rstrip(const std::string& s)
{
    return rstrip(s, std::string(" \t\n\v\f\r"));
}

inline std::string rstrip(const std::string& s, const std::string& characters)
{
    std::string result;

    size_t end = s.find_last_not_of(characters);
    if (end != std::string::npos)
    {
        result = s.substr(0, end + 1);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Trim Perfix/Suffix
///////////////////////////////////////////////////////////////////////////////

inline std::string trim_prefix(const std::string& s, const std::string& prefix)
{
    return starts_with(s, prefix) ? s.substr(0, prefix.size()) : s;
}

inline std::string trim_suffix(const std::string& s, const std::string& suffix)
{
    return ends_with(s, suffix) ? s.substr(s.size() - suffix.size()) : s;
}

///////////////////////////////////////////////////////////////////////////////
// Replace
///////////////////////////////////////////////////////////////////////////////

inline std::string replace(const std::string& s, const std::string old_val, const std::string& new_val)
{
    std::string result = s;
    size_t i = 0;

    while ((i = result.find(old_val, i)) != std::string::npos)
    {
        result.replace(i, old_val.size(), new_val);
        i += new_val.size();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Join
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline std::string join(IT first, IT last)
{
    return join(first, last, std::string());
}

template <typename IT>
inline std::string join(IT first, IT last, const std::string& delimiter)
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
inline std::string join_path(IT first, IT last)
{
    return join(first, last, "\\");
}

inline std::string join_path(const std::string& s1, const std::string& s2)
{
    const char* pair[2] = { s1.c_str(), s2.c_str() };
    return join(std::begin(pair), std::end(pair), "\\");
}

///////////////////////////////////////////////////////////////////////////////
// Split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos)
    {
        end = s.find(delimiter, start);
        result.push_back(s.substr(start, end - start));
        start = end + delimiter.size();
    }

    return result;
}

inline std::vector<std::string> split_any(const std::string& s, const std::string& characters)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while (end != std::string::npos)
    {
        end = s.find_first_of(characters, start);
        result.push_back(s.substr(start, end - start));
        start = end + 1;
    }

    return result;
}

inline std::vector<std::string> split_words(const std::string& s)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((start = s.find_first_not_of(" \t\n\v\f\r", end)) != std::string::npos)
    {
        end = s.find_first_of(" \t\n\v\f\r", start);
        result.push_back(s.substr(start, end - start));
    }

    return result;
}

inline std::vector<std::string> split_lines(const std::string& s)
{
    std::vector<std::string> lines;
    std::string line;
    std::istringstream iss(s);

    while (std::getline(iss, line))
    {
        lines.push_back(line);
    }

    return lines;
}

inline std::vector<std::string> split_path(const std::string& s)
{
    return split_any(s, "/\\");
}

///////////////////////////////////////////////////////////////////////////////
// Repeat
///////////////////////////////////////////////////////////////////////////////

inline std::string repeat(const char c, size_t n)
{
    return std::string(n, c);
}

inline std::string repeat(const std::string& s, size_t n)
{
    std::string result;
    result.reserve(result.size() * n);

    for (size_t i = 0; i < n; ++i)
    {
        result.append(s);
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Reverse
///////////////////////////////////////////////////////////////////////////////

inline std::string reverse(const std::string& s)
{
    std::string result(s);
    std::reverse(result.begin(), result.end());
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// wstring
///////////////////////////////////////////////////////////////////////////////

inline std::wstring string_to_wstring(const std::string& s)
{
    const size_t count = utf8::count(s.begin(), s.end());
    std::wstring wstr(count, 0);
    utf8::to_wide(s.begin(), s.end(), wstr.begin());
    return wstr;
}

inline std::string wstring_to_string(const std::wstring& ws)
{
    std::vector<typename std::string::value_type> str_data;
    utf8::from_wide(ws.begin(), ws.end(), std::back_inserter(str_data));
    return std::string(str_data.begin(), str_data.end());
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