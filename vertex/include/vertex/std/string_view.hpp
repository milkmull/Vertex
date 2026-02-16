#pragma once

#include <algorithm>

#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/char_traits.hpp"

namespace vx {
namespace str {

template <typename T>
class basic_string_view
{
    VX_STATIC_ASSERT_MSG((type_traits::is_char<T>::value), "T must be character type");

public:

    //=========================================================================
    // member types
    //=========================================================================

    using traits_type = char_traits<T>;
    using value_type = T;
    using pointer = const T*;
    using const_pointer = const T*;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = vx::_priv::pointer_iterator<basic_string_view, const value_type>;
    using const_iterator = vx::_priv::pointer_iterator<basic_string_view, const value_type>;
    using reverse_iterator = vx::_priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = vx::_priv::reverse_pointer_iterator<const_iterator>;

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

    template <typename Traits2>
    constexpr basic_string_view(const std::basic_string_view<T, Traits2> sv) noexcept
        : m_data(sv.data()), m_size(sv.size())
    {}

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

    bool full() const noexcept
    {
        return m_size == max_size();
    }

    size_type size() const noexcept
    {
        return m_size;
    }

    size_type length() const noexcept
    {
        return size();
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

public:

    constexpr size_type copy(T* const ptr, size_type count, const size_type off = 0) const
    {
        if (!_priv::check_offset(m_size, off))
        {
            return 0;
        }

        count = _priv::clamp_suffix_size(m_size, off, count);
        traits_type::copy(ptr, m_data + off, count);
        return count;
    }

    constexpr basic_string_view substr(const size_type off = 0, size_type count = npos) const
    {
        if (!_priv::check_offset(m_size, off))
        {
            return basic_string_view();
        }

        count = _priv::clamp_suffix_size(m_size, off, count);
        return basic_string_view(m_data + off, count);
    }

    //=========================================================================
    // compare
    //=========================================================================

    constexpr int compare(const basic_string_view right) const noexcept
    {
        return _priv::traits_compare<traits_type>(m_data, m_size, right.m_data, right.m_size);
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
        return _priv::traits_equal<traits_type>(lhs.m_data, lhs.m_size, rhs.m_data, rhs.m_size);
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

    friend constexpr bool operator<=(
        const basic_string_view lhs,
        const basic_string_view rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

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
        return _priv::traits_find<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find(const T c, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_find<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find(const T* const ptr, const size_type off = 0) const noexcept
    {
        return _priv::traits_find<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type rfind(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return _priv::traits_rfind<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type rfind(const T c, const size_type off = npos) const noexcept
    {
        return _priv::traits_rfind_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type rfind(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_rfind<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type rfind(const T* const ptr, const size_type off = npos) const noexcept
    {
        return _priv::traits_rfind<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_first_of(const basic_string_view right, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_first_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_first_of(const T c, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_first_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_find_first_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_first_of(const T* const ptr, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_first_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_last_of(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return _priv::traits_find_last_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_last_of(const T c, const size_type off = npos) const noexcept
    {
        return _priv::traits_rfind_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_last_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_find_last_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_last_of(const T* const ptr, const size_type off = npos) const noexcept
    {
        return _priv::traits_find_last_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_first_not_of(const basic_string_view right, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_first_not_of(const T c, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_not_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_first_not_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_first_not_of(const T* const ptr, const size_type off = 0) const noexcept
    {
        return _priv::traits_find_first_not_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    constexpr size_type find_last_not_of(const basic_string_view right, const size_type off = npos) const noexcept
    {
        return _priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, right.m_data, right.m_size);
    }

    constexpr size_type find_last_not_of(const T c, const size_type off = npos) const noexcept
    {
        return _priv::traits_rfind_not_ch<traits_type>(m_data, m_size, off, c);
    }

    constexpr size_type find_last_not_of(const T* const ptr, const size_type off, const size_type count) const noexcept
    {
        return _priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, ptr, count);
    }

    constexpr size_type find_last_not_of(const T* const ptr, const size_type off = npos) const noexcept
    {
        return _priv::traits_find_last_not_of<traits_type>(m_data, m_size, off, ptr, traits_type::length(ptr));
    }

    //=========================================================================

    template <typename Traits2, typename Alloc>
    friend std::basic_ostream<T, Traits2>& operator<<(
        std::basic_ostream<T, Traits2>& oss,
        const basic_string_view s)
    {
        std::string os(s.data(), s.size());
        oss << os;
        return oss;
    }

private:

    const_pointer m_data;
    size_type m_size;
};

} // namespace str

//=========================================================================

using string_view = str::basic_string_view<char>;
using wstring_view = str::basic_string_view<wchar_t>;
#if defined(__cpp_lib_char8_t)
using u8string_view = str::basic_string_view<char8_t>;
#endif // defined(__cpp_lib_char8_t)
using u16string_view = str::basic_string_view<char16_t>;
using u32string_view = str::basic_string_view<char32_t>;

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <typename T>
struct hash;

template <typename T>
struct hash<str::basic_string_view<T>>
{
    size_t operator()(const vx::str::basic_string_view<T> s) const noexcept
    {
        using traits = typename vx::str::basic_string_view<T>::traits_type;
        return traits::hash(s.data(), s.size());
    }
};

} // namespace vx

namespace std {

template <typename T>
struct hash<vx::str::basic_string_view<T>>
{
    size_t operator()(const vx::str::basic_string_view<T> s) const noexcept
    {
        return vx::hash<vx::str::basic_string_view<T>>{}(s);
    }
};

} // namespace std
