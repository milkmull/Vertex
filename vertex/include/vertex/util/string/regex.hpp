#pragma once

#include <regex>

#include "vertex/system/error.hpp"

namespace vx {
namespace re {

template <typename IT>
using regex_match_results = std::match_results<IT>;

using regex_cmatch = regex_match_results<const char*>;
using regex_smatch = regex_match_results<typename std::string::const_iterator>;

template <typename IT>
using regex_iterator = std::regex_iterator<IT>;

using regex_citerator = regex_iterator<const char*>;
using regex_siterator = regex_iterator<typename std::string::const_iterator>;

template <typename char_t>
class basic_regex
{
private:

    std::basic_regex<char_t> m_re;
    bool m_valid;

public:

    inline basic_regex(const char_t* pattern)
    {
        try
        {
            m_re = std::basic_regex<char_t>(pattern);
            m_valid = true;
        }
        catch (const std::regex_error& e)
        {
            m_valid = false;
            VX_ERR(err::code::INVALID_ARGUMENT) << e.what();
        }
    }

    inline basic_regex(const std::basic_string<char_t>& pattern)
    {
        try
        {
            m_re = std::basic_regex<char_t>(pattern);
            m_valid = true;
        }
        catch (const std::regex_error& e)
        {
            m_valid = false;
            VX_ERR(err::code::INVALID_ARGUMENT) << e.what();
        }
    }

    inline bool is_valid() const
    {
        return m_valid;
    }

    inline size_t group_count() const
    {
        return m_valid ? m_re.mark_count() : 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Match
    ///////////////////////////////////////////////////////////////////////////////

    template <typename IT>
    inline bool match(IT first, IT last, regex_match_results<IT>& m) const
    {
        return std::regex_match(first, last, m, m_re);
    }

    inline bool match(const char_t* s, regex_match_results<const char_t*>& m) const
    {
        return std::regex_match(s, m, m_re);
    }

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

    template <typename IT>
    inline bool search(IT first, IT last, regex_match_results<IT>& m) const
    {
        return std::regex_search(first, last, m, m_re);
    }

    inline bool search(const char_t* s, regex_match_results<const char_t*>& m) const
    {
        return std::regex_search(s, m, m_re);
    }

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

    inline std::vector<regex_match_results<const char_t*>> search_all(const char_t* s) const
    {
        return search_all(s, s + std::char_traits<char_t>::length(s));
    }

    inline std::vector<regex_match_results<typename std::basic_string<char_t>::const_iterator>>
        search_all(const std::basic_string<typename std::basic_string<char_t>::const_iterator>& s) const
    {
        return search_all(s.begin(), s.end());
    }

    std::vector<regex_match_results<typename std::basic_string<char_t>::const_iterator>>
        search_all(const std::basic_string<typename std::basic_string<char_t>::const_iterator>&&) const = delete;

    ///////////////////////////////////////////////////////////////////////////////
    // Replace
    ///////////////////////////////////////////////////////////////////////////////

    template <typename IT>
    inline std::basic_string<char_t> replace(IT first, IT last, const char_t* fmt) const
    {
        std::basic_string<char_t> out;
        std::regex_replace(out, first, last, m_re, fmt);
        return out;
    }

    template <typename IT>
    inline std::basic_string<char_t> replace(IT first, IT last, const std::basic_string<char_t>& fmt) const
    {
        std::basic_string<char_t> out;
        std::regex_replace(out, first, last, m_re, fmt);
        return out;
    }

    inline std::basic_string<char_t> replace(const char_t* s, const char_t* fmt) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    inline std::basic_string<char_t> replace(
        const char_t* s,
        const std::basic_string<char_t>& fmt
    ) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

    inline std::basic_string<char_t> replace(
        const std::basic_string<char_t>& s,
        const char_t* fmt
    ) const
    {
        return std::regex_replace(s, m_re, fmt);
    }

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

    template <typename IT>
    inline regex_iterator<IT> begin(IT first, IT last) const
    {
        return regex_iterator<IT>(first, last, m_re);
    }

    inline regex_iterator<const char_t*> begin(const char_t* s) const
    {
        return begin(s, std::char_traits<char_t>::length(s));
    }

    inline regex_iterator<typename std::basic_string<char_t>::const_iterator>
        begin(const std::basic_string<char_t>& s) const
    {
        return begin(s.begin(), s.end());
    }

    regex_iterator<typename std::basic_string<char_t>::const_iterator>
        begin(const std::basic_string<char_t>&&) const = delete;

    template <typename IT>
    inline regex_iterator<IT> end(regex_iterator<IT> it) const
    {
        return regex_iterator<IT>();
    }

    inline regex_iterator<const char_t*> end(const char_t* s) const
    {
        return regex_iterator<const char*>();
    }

    inline regex_iterator<typename std::basic_string<char_t>::const_iterator>
        end(const std::basic_string<char_t>& s) const
    {
        return regex_iterator<typename std::basic_string<char_t>::const_iterator>();
    }

    regex_iterator<typename std::basic_string<char_t>::const_iterator>
        end(const std::basic_string<char_t>&&) const = delete;

};

using regex = basic_regex<char>;

} // namespace re
} // namespace vx