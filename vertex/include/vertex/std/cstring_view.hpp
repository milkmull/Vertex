#pragma once

#include "vertex/std/_tools/pointer_iterator.hpp"
#include "vertex/std/char_traits.hpp"
#include "vertex/std/string.hpp"
#include "vertex/std/string_view.hpp"

namespace vx {
namespace str {

template <typename T>
class basic_cstring_view
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

    using iterator = vx::_priv::pointer_iterator<basic_cstring_view, const value_type>;
    using const_iterator = vx::_priv::pointer_iterator<basic_cstring_view, const value_type>;
    using reverse_iterator = vx::_priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = vx::_priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto npos{ static_cast<size_type>(-1) };

    // Tag type used to opt in to unchecked construction from a pointer and a
    // size. The caller must guarantee that ptr[count] == T(). Prefer any of
    // the other constructors whenever possible; this one exists as an escape
    // hatch for buffers you know are null terminated but can't prove it
    // through the type system (e.g. a fixed-size C array you've null
    // terminated yourself).
    struct unsafe_t
    {};
    static constexpr unsafe_t unsafe{};

    //=========================================================================
    // constructors
    //=========================================================================

    constexpr basic_cstring_view() noexcept
        : m_data(empty_string()), m_size(0)
    {}

    constexpr basic_cstring_view(const basic_cstring_view&) noexcept = default;
    constexpr basic_cstring_view& operator=(const basic_cstring_view&) noexcept = default;

    // String literals are always null terminated, so this is always safe.
    template <size_type N>
    constexpr basic_cstring_view(const value_type (&str)[N]) noexcept
        : m_data(str), m_size(N - 1)
    {
        VX_ASSERT(str[N - 1] == T());
    }

    // Raw C strings are assumed to be null terminated, per convention.
    constexpr basic_cstring_view(const T* ptr) noexcept
        : m_data(ptr), m_size(traits_type::length(ptr))
    {}

    basic_cstring_view(nullptr_t) = delete;

    template <typename Allocator2>
    constexpr basic_cstring_view(const str::basic_string<T, Allocator2>& s) noexcept
        : m_data(s.c_str()), m_size(s.size())
    {}

    template <typename Traits2, typename Allocator2>
    constexpr basic_cstring_view(const std::basic_string<T, Traits2, Allocator2>& s) noexcept
        : m_data(s.c_str()), m_size(s.size())
    {}

    constexpr basic_cstring_view(unsafe_t, const T* ptr, const size_type count) noexcept
        : m_data(ptr), m_size(count)
    {
        VX_ASSERT(ptr);
        VX_ASSERT(ptr[count] == T());
    }

    //=========================================================================
    // conversion
    //=========================================================================

    constexpr operator basic_string_view<T>() const noexcept
    {
        return basic_string_view<T>(m_data, m_size);
    }

    template <typename Allocator2>
    operator str::basic_string<T, Allocator2>() const
    {
        return std::basic_string<T, Allocator2>(data(), size());
    }

    template <typename Traits2, typename Allocator2>
    operator std::basic_string<T, Traits2, Allocator2>() const
    {
        return std::basic_string<T, Traits2, Allocator2>(data(), size());
    }

#if VX_HAVE_STD_STRING_VIEW

    template <typename Traits2>
    operator std::basic_string_view<T, Traits2>() const noexcept
    {
        return std::basic_string_view<T, Traits2>(data(), size());
    }

#endif // VX_HAVE_STD_STRING_VIEW

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

    constexpr const_pointer c_str() const noexcept
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

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data);
    }
    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data + m_size);
    }
    const_iterator cend() const noexcept
    {
        return end();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
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

    constexpr bool empty() const noexcept
    {
        return m_size == 0;
    }
    constexpr size_type size() const noexcept
    {
        return m_size;
    }
    constexpr size_type length() const noexcept
    {
        return size();
    }
    constexpr size_type data_size() const noexcept
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

    constexpr void swap(basic_cstring_view& other) noexcept
    {
        const basic_cstring_view tmp{ other };
        other = *this;
        *this = tmp;
    }

    //=========================================================================
    // prefix removal
    //=========================================================================

    constexpr void remove_prefix(const size_type count) noexcept
    {
        VX_ASSERT(m_size >= count);
        m_data += count;
        m_size -= count;
    }

    //=========================================================================
    // string ops
    //=========================================================================

    constexpr size_type copy(T* const ptr, const size_type count, const size_type off = 0) const
    {
        return view(off).copy(ptr, count);
    }

    constexpr basic_cstring_view substr(const size_type off = 0) const
    {
        VX_ASSERT(off <= m_size);
        return basic_cstring_view(unsafe, m_data + off, m_size - off);
    }

    constexpr basic_string_view<T> view(const size_type off = 0, const size_type count = npos) const noexcept
    {
        return basic_string_view<T>(*this).substr(off, count);
    }

    //=========================================================================
    // compare
    //=========================================================================

    constexpr int compare(const basic_string_view<T> right) const noexcept
    {
        return basic_string_view<T>(*this).compare(right);
    }

    constexpr int compare(const T* const ptr) const noexcept
    {
        return compare(basic_string_view<T>(ptr));
    }

    //=========================================================================
    // find
    //=========================================================================

    constexpr size_type find(const basic_string_view<T> right, const size_type off = 0) const noexcept
    {
        return basic_string_view<T>(*this).find(right, off);
    }

    constexpr size_type find(const T c, const size_type off = 0) const noexcept
    {
        return basic_string_view<T>(*this).find(c, off);
    }

    constexpr size_type rfind(const basic_string_view<T> right, const size_type off = npos) const noexcept
    {
        return basic_string_view<T>(*this).rfind(right, off);
    }

    constexpr size_type rfind(const T c, const size_type off = npos) const noexcept
    {
        return basic_string_view<T>(*this).rfind(c, off);
    }

    constexpr size_type find_first_of(const basic_string_view<T> right, const size_type off = 0) const noexcept
    {
        return basic_string_view<T>(*this).find_first_of(right, off);
    }

    constexpr size_type find_last_of(const basic_string_view<T> right, const size_type off = npos) const noexcept
    {
        return basic_string_view<T>(*this).find_last_of(right, off);
    }

    constexpr size_type find_first_not_of(const basic_string_view<T> right, const size_type off = 0) const noexcept
    {
        return basic_string_view<T>(*this).find_first_not_of(right, off);
    }

    constexpr size_type find_last_not_of(const basic_string_view<T> right, const size_type off = npos) const noexcept
    {
        return basic_string_view<T>(*this).find_last_not_of(right, off);
    }

private:

    static constexpr const T* empty_string() noexcept
    {
        static constexpr T s[1] = { T() };
        return s;
    }

    const_pointer m_data;
    size_type m_size;
};

//=========================================================================
// comparison operators
//=========================================================================

template <typename T>
constexpr bool operator==(
    const basic_cstring_view<T> lhs,
    const basic_cstring_view<T> rhs) noexcept
{
    using traits_type = typename basic_cstring_view<T>::traits_type;
    return _char_traits_priv::traits_equal<traits_type>(lhs.data(), lhs.size(), rhs.data(), rhs.size());
}

template <typename T>
constexpr bool operator==(
    const basic_cstring_view<T> lhs,
    const type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    using traits_type = typename basic_cstring_view<T>::traits_type;
    return _char_traits_priv::traits_equal<traits_type>(lhs.data(), lhs.size(), rhs.data(), rhs.size());
}

template <typename T>
constexpr bool operator==(
    const type_traits::identity_t<basic_cstring_view<T>> lhs,
    const basic_cstring_view<T> rhs) noexcept
{
    using traits_type = typename basic_cstring_view<T>::traits_type;
    return _char_traits_priv::traits_equal<traits_type>(lhs.data(), lhs.size(), rhs.data(), rhs.size());
}

//=========================================================================

template <typename T>
constexpr bool operator!=(
    basic_cstring_view<T> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename T, int = 1>
constexpr bool operator!=(
    basic_cstring_view<T> lhs,
    type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename T, int = 2>
constexpr bool operator!=(
    type_traits::identity_t<basic_cstring_view<T>> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(lhs == rhs);
}

//=========================================================================


template <typename T>
constexpr bool operator<(
    basic_cstring_view<T> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <typename T, int = 1>
constexpr bool operator<(
    basic_cstring_view<T> lhs,
    type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template <typename T, int = 2>
constexpr bool operator<(
    type_traits::identity_t<basic_cstring_view<T>> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

//=========================================================================

template <typename T>
constexpr bool operator>(
    basic_cstring_view<T> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return rhs < lhs;
}

template <typename T, int = 1>
constexpr bool operator>(
    basic_cstring_view<T> lhs,
    type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    return rhs < lhs;
}

template <typename T, int = 2>
constexpr bool operator>(
    type_traits::identity_t<basic_cstring_view<T>> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return rhs < lhs;
}

//=========================================================================

template <typename T>
constexpr bool operator<=(
    basic_cstring_view<T> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(rhs < lhs);
}

template <typename T, int = 1>
constexpr bool operator<=(
    basic_cstring_view<T> lhs,
    type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    return !(rhs < lhs);
}

template <typename T, int = 2>
constexpr bool operator<=(
    type_traits::identity_t<basic_cstring_view<T>> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(rhs < lhs);
}

//=========================================================================

template <typename T>
constexpr bool operator>=(
    basic_cstring_view<T> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(lhs < rhs);
}

template <typename T, int = 1>
constexpr bool operator>=(
    basic_cstring_view<T> lhs,
    type_traits::identity_t<basic_cstring_view<T>> rhs) noexcept
{
    return !(lhs < rhs);
}

template <typename T, int = 2>
constexpr bool operator>=(
    type_traits::identity_t<basic_cstring_view<T>> lhs,
    basic_cstring_view<T> rhs) noexcept
{
    return !(lhs < rhs);
}

//=========================================================================
// stream operators
//=========================================================================

template <typename T, typename Traits2>
std::basic_ostream<T, Traits2>& operator<<(
    std::basic_ostream<T, Traits2>& oss,
    const basic_cstring_view<T> s)
{
    std::string os(s.data(), s.size());
    oss << os;
    return oss;
}

} // namespace str

//=========================================================================

using cstring_view = str::basic_cstring_view<char>;
using wcstring_view = str::basic_cstring_view<wchar_t>;
#if VX_HAVE_STD_CHAR8_T
using u8cstring_view = str::basic_cstring_view<char8_t>;
#endif // VX_HAVE_STD_CHAR8_T
using u16cstring_view = str::basic_cstring_view<char16_t>;
using u32cstring_view = str::basic_cstring_view<char32_t>;

} // namespace vx

//=========================================================================
// hashing
//=========================================================================

namespace vx {

template <typename T>
struct hash;

template <typename T>
struct hash<str::basic_cstring_view<T>>
{
    size_t operator()(const vx::str::basic_cstring_view<T> s) const noexcept
    {
        using traits = typename vx::str::basic_cstring_view<T>::traits_type;
        return traits::hash(s.data(), s.size());
    }
};

} // namespace vx

namespace std {

template <typename T>
struct hash<vx::str::basic_cstring_view<T>>
{
    size_t operator()(const vx::str::basic_cstring_view<T> s) const noexcept
    {
        return vx::hash<vx::str::basic_cstring_view<T>>{}(s);
    }
};

} // namespace std
