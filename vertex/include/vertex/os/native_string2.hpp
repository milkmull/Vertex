#pragma once

#include <string>

#include "vertex/config/os.hpp"
#include "vertex/util/string/string_cast.hpp"

namespace vx {
namespace os {

class native_string
{
public:

#if defined(VX_OS_WINDOWS)

    using value_type = wchar_t;

#else

    using value_type = char;

#endif // VX_OS_WINDOWS

    using string_type = std::basic_string<value_type>;

public:

    native_string() noexcept = default;
    ~native_string() = default;

    native_string(const native_string&) = default;
    native_string(native_string&&) noexcept = default;

    native_string& operator=(const native_string&) = default;
    native_string& operator=(native_string&&) noexcept = default;

public:

    native_string(string_type&& s) noexcept : m_string(std::move(s)) {}

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    native_string(const Src& src) : m_string(str::string_cast<value_type>(src)) {}

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    native_string(IT first, IT last)
    {
        str::string_cast<value_type>(first, last, std::back_inserter(m_string));
    }

    native_string& operator=(string_type&& s) noexcept
    {
        m_string = std::move(s);
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    native_string& operator=(const Src& rhs)
    {
        m_string = str::string_cast<value_type>(rhs);
        return *this;
    }

    native_string& assign(string_type&& s) noexcept
    {
        m_string = std::move(s);
        return *this;
    }

    template <typename Src, VX_REQUIRES(type_traits::is_string_like<Src>::value)>
    native_string& assign(const Src& rhs)
    {
        m_string = str::string_cast<value_type>(rhs);
        return *this;
    }

    template <typename IT, VX_REQUIRES(type_traits::is_char_iterator<IT>::value)>
    native_string& assign(IT first, IT last)
    {
        m_string = str::string_cast<value_type>(first, last);
        return *this;
    }

public:

    string_type& native() noexcept { return m_string; }
    const string_type& native() const noexcept { return m_string; }

    const value_type* c_str() const noexcept { return m_string.c_str(); }
    operator string_type() const { return m_string; }

private:

    string_type m_string;
};

} // namespace os
} // namespace vx