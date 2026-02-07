#pragma once

#include <algorithm>

#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/char_traits.hpp"

namespace vx {

template <typename T>
class basic_string_view
{
    VX_STATIC_ASSERT_MSG((type_traits::is_char<T>::value), "T must be character type");

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = str::char_traits<T>;
    using value_type = T;
    using pointer = const T*;
    using const_pointer = const T*;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = _priv::pointer_iterator<basic_string_view, pointer>;
    using const_iterator = _priv::pointer_iterator<basic_string_view, const_pointer>;
    using reverse_iterator = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

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
        : m_data(ptr), m_size(traits_type::length(ptr))
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
        return m_data[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_data + m_size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data + m_size);
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_size == 0;
    }

    size_type size() const noexcept
    {
        return m_size;
    }

    size_type data_size() const noexcept
    {
        return size() * sizeof(value_type);
    }

    static constexpr size_type max_size() noexcept
    {
        return mem::max_array_size<T>();
    }

    //=========================================================================
    // memory
    //=========================================================================

    constexpr void swap(basic_string_view& other) noexcept
    {
        const basic_string_view tmp{ other };
        other = *this;
        *this = tmp;
    }

    //=========================================================================
    // prefix/suffix
    //=========================================================================

    constexpr void remove_prefix(const size_type count) noexcept
    {
        VX_ASSERT(m_size >= count);
        m_data += count;
        m_size -= count;
    }

    constexpr void remove_suffix(const size_type count) noexcept
    {
        VX_ASSERT(m_size >= count);
        m_size -= count;
    }

    //=========================================================================
    // string ops
    //=========================================================================

private:

    constexpr size_type clamp_suffix_size(const size_type off, const size_type size) const noexcept
    {
        return std::min(size, m_size - off);
    }

    bool check_offset(const size_type off) const
    {
        return (off <= m_size);
    }

public:

    constexpr size_type copy(T* const ptr, size_type count, const size_type off = 0) const
    {
        if (!check_offset(off))
        {
            return 0;
        }

        count = clamp_suffix_size(off, count);
        traits_type::copy(ptr, m_data + off, count);
        return count;
    }

    constexpr basic_string_view substr(const size_type off = 0, size_type count = npos) const
    {
        if (!check_offset(off))
        {
            return basic_string_view();
        }

        count = clamp_suffix_size(off, count);
        return basic_string_view(m_data + off, count);
    }

    //=========================================================================
    // compare
    //=========================================================================

    constexpr int compare(const basic_string_view right) const noexcept
    {
        return str::_priv::traits_compare<traits_type>(m_data, m_size, right.m_data, right.m_size);
    }

    constexpr int compare(const size_type off, const size_type nx, const basic_string_view right) const
    {
        return substr(off, nx).compare(right);
    }

    constexpr int compare(const size_type off, const size_type nx, const basic_string_view right, const size_type roff, const size_type count) const
    {
        return substr(off, nx).compare(right.substr(roff, count));
    }

    constexpr int compare(const T* const ptr) const noexcept
    {
        return compare(basic_string_view(ptr));
    }

    constexpr int compare(const size_type off, const size_type nx, const T* const ptr) const
    {
        return substr(off, nx).compare(basic_string_view(ptr));
    }

    constexpr int compare(const size_type off, const size_type nx, const T* const ptr, const size_type count) const
    {
        return substr(off, nx).compare(basic_string_view(ptr, count));
    }

    friend constexpr bool operator==(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return str::_priv::traits_equal<traits_type>(lhs.m_data, lhs.m_size, rhs.m_data, rhs.m_size);
    }

    friend constexpr bool operator!=(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend constexpr bool operator>(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    template <typename T, typename traits_type>
    friend constexpr bool operator<=(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    template <typename T, typename traits_type>
    friend constexpr bool operator>=(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    //=========================================================================
    // find
    //=========================================================================

    constexpr size_type find(const basic_string_view right, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find(const T c, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_find<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find(const T* const ptr, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type rfind(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_rfind<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type rfind(const T c, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_rfind_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type rfind(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_rfind<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type rfind(const T* const ptr, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_rfind<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_first_of(const basic_string_view right, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_first_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_first_of(const T c, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_first_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_find_first_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_first_of(const T* const ptr, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_first_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_last_of(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_find_last_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_last_of(const T c, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_rfind_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_last_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_find_last_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_last_of(const T* const ptr, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_find_last_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_first_not_of(const basic_string_view right, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_first_not_of(const T c, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_not_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_first_not_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_first_not_of(const T* const ptr, const size_type off = 0) const noexcept
    {
        return str::_priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_last_not_of(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_last_not_of(const T c, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_rfind_not_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_last_not_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return str::_priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_last_not_of(const T* const ptr, const size_type off = npos) const noexcept
    {
        return str::_priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

private:

    const_pointer m_data;
    size_type m_size;
};

//=========================================================================

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
#if defined(__cpp_lib_char8_t)
using u8string_view = basic_string_view<char8_t>;
#endif // defined(__cpp_lib_char8_t)
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

} // namespace vx
