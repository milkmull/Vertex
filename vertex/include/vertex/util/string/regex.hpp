#pragma once

#include <regex>

#include "vertex/system/error.hpp"

namespace vx {
namespace re {

/// @brief Regex match results for iterators.
template <typename IT>
using regex_match_results = std::match_results<IT>;

/// @brief Regex match results for const char*.
using regex_cmatch = regex_match_results<const char*>;
/// @brief Regex match results for std::string::const_iterator.
using regex_smatch = regex_match_results<typename std::string::const_iterator>;

/// @brief Regex iterator for matching all occurrences.
template <typename IT>
using regex_iterator = std::regex_iterator<IT>;
/// @brief Regex iterator for const char*.
using regex_citerator = regex_iterator<const char*>;
/// @brief Regex iterator for std::string::const_iterator.
using regex_siterator = regex_iterator<typename std::string::const_iterator>;

/**
 * @brief A lightweight wrapper around std::basic_regex with enhanced error handling.
 *
 * Provides methods for matching, searching, replacing, and iterating over regular expression results.
 * Wraps std::basic_regex and integrates with the vx::err system to report regex compilation errors.
 *
 * @tparam char_t Character type, typically `char` or `wchar_t`.
 */
template <typename char_t>
class basic_regex
{
private:

    std::basic_regex<char_t> m_re;
    bool m_valid;

public:

    /**
     * @brief Constructs a regex object from a null-terminated pattern string.
     * @param pattern The C-style pattern string.
     */
    inline basic_regex(const char_t* pattern) noexcept
    {
        try
        {
            m_re = std::basic_regex<char_t>(pattern);
            m_valid = true;
        }
        catch (const std::regex_error& e)
        {
            m_valid = false;
            err::set(err::invalid_argument, e.what());
        }
    }

    /**
     * @brief Constructs a regex object from a std::basic_string pattern.
     * @param pattern The regex pattern.
     */
    inline basic_regex(const std::basic_string<char_t>& pattern) noexcept
    {
        try
        {
            m_re = std::basic_regex<char_t>(pattern);
            m_valid = true;
        }
        catch (const std::regex_error& e)
        {
            m_valid = false;
            err::set(err::invalid_argument, e.what());
        }
    }


    /**
     * @brief Checks whether the regex compiled successfully.
     * @return True if the regex is valid, false otherwise.
     */
    inline bool is_valid() const noexcept
    {
        return m_valid;
    }

    /**
     * @brief Returns the number of capture groups (excluding group 0).
     * @return The number of submatches in the regex.
     */
    inline size_t group_count() const
    {
        return m_valid ? m_re.mark_count() : 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Match
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Checks if the full range matches the pattern.
     * @param first Begin iterator.
     * @param last End iterator.
     * @param m Output match results.
     * @return True if the pattern fully matches the range.
     */
    template <typename IT>
    inline bool match(IT first, IT last, regex_match_results<IT>& m) const
    {
        return std::regex_match(first, last, m, m_re);
    }

    /// @brief Matches null-terminated string.
    inline bool match(const char_t* s, regex_match_results<const char_t*>& m) const
    {
        return std::regex_match(s, m, m_re);
    }

    /// @brief Matches std::basic_string input.
    inline bool match(
        const std::basic_string<char_t>& s,
        regex_match_results<typename std::basic_string<char_t>::const_iterator>& m
    ) const
    {
        return std::regex_match(s, m, m_re);
    }

    bool match(
        const std::basic_string<char_t>&&,
        regex_match_results<typename std::basic_string<char_t>::const_iterator>&
    ) const = delete;

    ///////////////////////////////////////////////////////////////////////////////
    // Search
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Searches for the first occurrence of the pattern in the range.
     * @param first Begin iterator.
     * @param last End iterator.
     * @param m Output match results.
     * @return True if a match is found.
     */
    template <typename IT>
    inline bool search(IT first, IT last, regex_match_results<IT>& m) const
    {
        return std::regex_search(first, last, m, m_re);
    }

    /// @brief Searches null-terminated string.
    inline bool search(const char_t* s, regex_match_results<const char_t*>& m) const
    {
        return std::regex_search(s, m, m_re);
    }

    /// @brief Searches std::basic_string input.
    inline bool search(
        const std::basic_string<char_t>& s,
        regex_match_results<typename std::basic_string<char_t>::const_iterator>& m
    ) const
    {
        return std::regex_search(s, m, m_re);
    }

    bool search(
        const std::basic_string<char_t>&&,
        regex_match_results<typename std::basic_string<char_t>::const_iterator>&
    ) const = delete;

    ///////////////////////////////////////////////////////////////////////////////
    // Search All
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Finds all non-overlapping matches in the range.
     * @param first Begin iterator.
     * @param last End iterator.
     * @return A vector of match results.
     */
    template <typename IT>
    inline std::vector<regex_match_results<IT>> search_all(IT first, IT last) const
    {
        std::vector<regex_match_results<IT>> matches;
        regex_match_results<IT> m;

        IT it = first;
        while (search(it, last, m))
        {
            matches.push_back(m);
            it = m[0].second;
        }

        return matches;
    }

    /// @brief Overload for null-terminated string.
    inline auto search_all(const char_t* s) const
    {
        return search_all(s, s + std::char_traits<char_t>::length(s));
    }

    /// @brief Overload for std::basic_string input.
    inline auto search_all(const std::basic_string<char_t>& s) const
    {
        return search_all(s.begin(), s.end());
    }

    auto search_all(const std::basic_string<char_t>&&) const = delete;

    ///////////////////////////////////////////////////////////////////////////////
    // Replace
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Performs regex replacement on a character range.
     * @param first Begin iterator.
     * @param last End iterator.
     * @param fmt Replacement format string.
     * @return A new string with replacements.
     */
    template <typename IT>
    inline std::basic_string<char_t> replace(IT first, IT last, const char_t* fmt) const
    {
        std::basic_string<char_t> out;
        std::regex_replace(out, first, last, m_re, fmt);
        return out;
    }

    /// @brief Overload using std::basic_string format.
    template <typename IT>
    inline std::basic_string<char_t> replace(IT first, IT last, const std::basic_string<char_t>& fmt) const
    {
        std::basic_string<char_t> out;
        std::regex_replace(out, first, last, m_re, fmt);
        return out;
    }

    /// @brief Replaces using a null-terminated input string.
    inline std::basic_string<char_t> replace(const char_t* s, const char_t* fmt) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    /// @brief Overload using std::basic_string format.
    inline std::basic_string<char_t> replace(
        const char_t* s,
        const std::basic_string<char_t>& fmt
    ) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    /// @brief Replaces in a std::basic_string input using C-style format.
    inline std::basic_string<char_t> replace(
        const std::basic_string<char_t>& s,
        const char_t* fmt
    ) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    /// @brief Replaces in a std::basic_string input using std::basic_string format.
    inline std::basic_string<char_t> replace(
        const std::basic_string<char_t>& s,
        const std::basic_string<char_t>& fmt
    ) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Iterator
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Returns an iterator to the first match.
     * @param first Begin iterator.
     * @param last End iterator.
     */
    template <typename IT>
    inline regex_iterator<IT> begin(IT first, IT last) const
    {
        return regex_iterator<IT>(first, last, m_re);
    }

    /// @brief Begin iterator for null-terminated string.
    inline regex_iterator<const char_t*> begin(const char_t* s) const
    {
        return begin(s, std::char_traits<char_t>::length(s));
    }

    /// @brief Begin iterator for std::basic_string.
    inline regex_iterator<typename std::basic_string<char_t>::const_iterator>
        begin(const std::basic_string<char_t>& s) const
    {
        return begin(s.begin(), s.end());
    }

    regex_iterator<typename std::basic_string<char_t>::const_iterator>
        begin(const std::basic_string<char_t>&&) const = delete;

    /**
     * @brief End sentinel for regex iteration.
     */
    template <typename IT>
    inline regex_iterator<IT> end(regex_iterator<IT>) const
    {
        return regex_iterator<IT>();
    }

    /// @brief End sentinel for null-terminated string.
    inline regex_iterator<const char_t*> end(const char_t*) const
    {
        return regex_iterator<const char*>();
    }

    /// @brief End sentinel for std::basic_string.
    inline regex_iterator<typename std::basic_string<char_t>::const_iterator>
        end(const std::basic_string<char_t>&) const
    {
        return regex_iterator<typename std::basic_string<char_t>::const_iterator>();
    }

    regex_iterator<typename std::basic_string<char_t>::const_iterator>
        end(const std::basic_string<char_t>&&) const = delete;

};

using regex = basic_regex<char>;

} // namespace re
} // namespace vx