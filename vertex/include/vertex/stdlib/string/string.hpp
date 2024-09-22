#pragma once

#include <sstream>
#include <vector>

#include "vertex/system/error.hpp"
#include "utf.hpp"

namespace vx {
namespace str {

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

inline std::string wstring_to_string(const std::wstring& wstr)
{
    std::vector<typename std::string::value_type> str_data;
    utf8::from_wide(wstr.begin(), wstr.end(), std::back_inserter(str_data));
    return std::string(str_data.begin(), str_data.end());
}

///////////////////////////////////////////////////////////////////////////////
// Starts/Ends With
///////////////////////////////////////////////////////////////////////////////

inline bool starts_with(const std::string& s, const std::string& value)
{
    return s.find(value) == 0;
}

inline bool ends_with(const std::string& s, const std::string& value)
{
    return s.find(value) == (s.size() - value.size());
}

///////////////////////////////////////////////////////////////////////////////
// Boolean Checks
///////////////////////////////////////////////////////////////////////////////

inline bool is_alpha(char c)
{
    return std::isalpha(c);
}

inline bool is_alpha(const std::string& s)
{
    for (char c : s)
    {
        if (!is_alpha(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_digit(char c)
{
    return std::isdigit(c);
}

inline bool is_numeric(const std::string& s)
{
    for (char c : s)
    {
        if (!is_digit(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_alnum(char c)
{
    return std::isalnum(c);
}

inline bool is_alnum(const std::string& s)
{
    for (char c : s)
    {
        if (!is_alnum(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_ascii(char c)
{
    return 0x00 <= c && c < 0x80;
}

inline bool is_ascii(const std::string& s)
{
    for (char c : s)
    {
        if (!is_ascii(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_space(char c)
{
    return std::isspace(c);
}

inline bool is_space(const std::string& s)
{
    for (char c : s)
    {
        if (!is_space(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_identifier(const std::string& s)
{
    if (s.empty())
    {
        return false;
    }

    if (is_digit(s[0]))
    {
        return false;
    }

    for (char c : s)
    {
        if (!(is_alnum(c) || c == '_'))
        {
            return false;
        }
    }

    return true;
}

inline bool is_lower(char c)
{
    return std::islower(c);
}

inline bool is_lower(const std::string& s)
{
    for (char c : s)
    {
        if (is_alpha(c) && !is_lower(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_upper(char c)
{
    return std::isupper(c);
}

inline bool is_upper(const std::string& s)
{
    for (char c : s)
    {
        if (is_alpha(c) && !is_upper(c))
        {
            return false;
        }
    }
    return true;
}

inline bool is_printable(char c)
{
    return std::isprint(c);
}

inline bool is_printable(const std::string& s)
{
    for (char c : s)
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

inline bool is_hex(const std::string& s)
{
    return !(s.size() % 2) && (s.find_first_not_of(" 0123456789abcdefABCDEF") == std::string::npos);
}

///////////////////////////////////////////////////////////////////////////////
// To Upper/Lower
///////////////////////////////////////////////////////////////////////////////

inline char to_upper(char c)
{
    return static_cast<char>(std::toupper(c));
}

inline std::string to_upper(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    for (char c : s)
    {
        result.push_back(to_upper(c));
    }

    return result;
}

inline char to_lower(char c)
{
    return static_cast<char>(std::tolower(c));
}

inline std::string to_lower(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    for (char c : s)
    {
        result.push_back(to_lower(c));
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Join
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline std::string join(const T& container, const std::string& delimiter = "")
{
    std::ostringstream oss;
    bool is_first = true;

    for (const auto& element : container)
    {
        if (is_first)
        {
            oss << element;
            is_first = false;
        }
        else
        {
            oss << delimiter << element;
        }
    }

    return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
// Strip
///////////////////////////////////////////////////////////////////////////////

inline std::string strip(const std::string& s, const std::string& characters = " \t\n\v\f\r")
{
    std::string result;

    size_t start = s.find_first_not_of(characters);
    if (start != std::string::npos)
    {
        size_t end = s.find_last_not_of(characters);
        result = s.substr(start, end - start + 1);
    }

    return result;
}

inline std::string lstrip(const std::string& s, const std::string& characters = " \t\n\v\f\r")
{
    std::string result;

    size_t start = s.find_first_not_of(characters);
    if (start != std::string::npos)
    {
        result = s.substr(start);
    }

    return result;
}

inline std::string rstrip(const std::string& s, const std::string& characters = " \t\n\v\f\r")
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
// Split
///////////////////////////////////////////////////////////////////////////////

inline std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = s.find(delimiter, start)) != std::string::npos)
    {
        result.push_back(s.substr(start, end - start));
        start = end + delimiter.size();
    }

    if (start < s.size())
    {
        result.push_back(s.substr(start));
    }

    return result;
}

inline std::vector<std::string> split_words(const std::string& s)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = s.find_first_of(" \t\n\v\f\r", start)) != std::string::npos)
    {
        result.push_back(s.substr(start, end - start));
        start = s.find_first_not_of(" \t\n\v\f\r", end + 1);
    }

    if (start < s.size())
    {
        result.push_back(s.substr(start));
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

///////////////////////////////////////////////////////////////////////////////
// Numeric Conversions
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
inline bool to_int(const std::string& s, T& value, int32_t base = 10)
{
    value = 0;

    if (base < 2 || base > 36)
    {
        error::set_error(error::error_code::INVALID_ARGUMENT, "Base must be between 2 and 36");
        return false;
    }

    if (s.empty())
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return false;
    }

    size_t start = 0;
    bool is_negative = false;

    // Check sign prefix
    if (s.size() > 1)
    {
        if (s[0] == '-')
        {
            start = 1;
            is_negative = true;
        }
        else if (s[0] == '+')
        {
            start = 1;
        }
    }

    // Check base prefix
    if (s.size() > 2 && s[start] == '0')
    {
        ++start;

        if (to_upper(s[start]) == 'B')
        {
            // Binary
            if (base != 2)
            {
                error::set_error(error::error_code::INVALID_ARGUMENT);
                return false;
            }

            ++start;
        }
        else if (to_upper(s[start]) == 'X')
        {
            // Hex
            if (base != 16)
            {
                error::set_error(error::error_code::INVALID_ARGUMENT);
                return false;
            }

            ++start;
        }
    }

    for (size_t i = start; i < s.size(); ++i)
    {
        T inc = 0;

        if (is_digit(s[i]))
        {
            inc = (s[i] - '0');
        }
        else if (is_alpha(s[i]))
        {
            inc = to_upper(s[i]) - 'A' + 10;
        }
        else
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return false;
        }

        // Check for overflow
        if (value > (std::numeric_limits<T>::max() - inc) / static_cast<T>(base))
        {
            error::set_error(error::error_code::OUT_OF_RANGE);
            return false;
        }

        value = value * static_cast<T>(base) + inc;
    }

    if (is_negative)
    {
        value *= -1;
    }

    return true;
}

}
}