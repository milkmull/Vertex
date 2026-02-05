#pragma once

#include "vertex/std/char_traits.hpp"

namespace vx {

template <typename char_t, typename Traits = str::char_traits<char_t>>
class basic_string_view
{
    VX_STATIC_ASSERT_MSG((std::is_same<char_t, typename Traits::char_type>::value),
        "Bad char_traits for basic_string_view; N4950 [string.view.template.general]/1 "
        "\"The program is ill-formed if traits::char_type is not the same type as charT.\"");

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = Traits;
    using value_type = char_t;
    using pointer = const char_t*;
    using const_pointer = const char_t*;
    using reference = const char_t&;
    using const_reference = const char_t&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr auto npos{ static_cast<size_type>(-1) };

    //=========================================================================
    // constructors
    //=========================================================================

    constexpr basic_string_view() noexcept
        : m_data(), m_size(0)
    {}

    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(const const_pointer ptr) noexcept
        : m_data(ptr), m_size(Traits::length(ptr))
    {}

    basic_string_view(nullptr_t) = delete;

    constexpr basic_string_view(const const_pointer ptr, const size_type count) noexcept
        : m_data(ptr), m_size(count)
    {
        VX_ASSERT(count == 0 || ptr);
    }

    //=========================================================================
    // element access
    //=========================================================================

    constexpr const_reference front() const noexcept
    {
        VX_ASSERT(m_size > 0);
        return m_data[0];
    }

    constexpr const_reference back() const noexcept
    {
        VX_ASSERT(m_size > 0);
        return m_data[m_size - 1];
    }

    constexpr const_pointer data() const noexcept
    {
        return m_data;
    }

    constexpr const_reference operator[](size_type i) const noexcept
    {
        VX_ASSERT(i < m_size);
        return m_array[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    //=========================================================================
    // memory
    //=========================================================================

private:

    const_pointer m_data;
    size_type m_size;
};

} // namespace vx
