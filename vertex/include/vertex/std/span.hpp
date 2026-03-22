#pragma once

#include <initializer_list>
//#include <span>

#include "vertex/config/type_traits.hpp"
#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/array.hpp"

namespace vx {

enum : size_t
{
    dynamic_extent = std::numeric_limits<size_t>::max()
};

namespace _priv {

template <typename T, size_t Extent>
struct extent_data
{
    using pointer = T*;
    constexpr extent_data() noexcept = default;

    constexpr explicit extent_data(pointer ptr, const size_t) noexcept
        : data(ptr)
    {}

    pointer data = nullptr;
    static constexpr size_t size = Extent;
};

template <typename T>
struct extent_data<T, dynamic_extent>
{
    using pointer = T*;
    constexpr extent_data() noexcept = default;

    constexpr explicit extent_data(pointer ptr, const size_t count) noexcept
        : data(ptr), size(count)
    {}

    pointer data = nullptr;
    size_t size = 0;
};

} // namespace _priv

template <typename T, size_t Extent = dynamic_extent>
class span
{
public:

    //=========================================================================
    // member types
    //=========================================================================

    using value_type = typename std::remove_cv<T>::type;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = const T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using iterator = vx::_priv::pointer_iterator<span, const value_type>;
    using const_iterator = vx::_priv::pointer_iterator<span, const value_type>;
    using reverse_iterator = vx::_priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = vx::_priv::reverse_pointer_iterator<const_iterator>;

    static constexpr auto extent = Extent;

private:

    using data_type = _priv::extent_data<T, Extent>;
    data_type m_data;

    template <typename IT, typename = void>
    struct is_compatible_iterator : std::false_type
    {};


    template <typename IT>
    struct is_compatible_iterator<
        IT,
        typename std::enable_if<type_traits::is_iterator<IT>::value>::type> : std::bool_constant<std::is_convertible<typename std::remove_reference<typename type_traits::iter_reference<IT>::type>::type (*)[], T (*)[]>::value>
    {};

    template <typename IT, typename S>
    struct is_compatible_sentinel
    {
    private:

        static constexpr bool iter_ok = is_compatible_iterator<IT>::value;

        template <typename X = IT, typename Y = S>
        static auto test(int) -> decltype(

            // 1. Comparable (sentinel_for requirement)
            static_cast<bool>(std::declval<X>() == std::declval<Y>()),

            // 2. If sized sentinel: both subtractions must be valid
            static_cast<typename std::iterator_traits<X>::difference_type>(
                std::declval<Y>() - std::declval<X>()),
            static_cast<typename std::iterator_traits<X>::difference_type>(
                std::declval<X>() - std::declval<Y>()),

            std::true_type{});

        template <typename, typename>
        static std::false_type test(...);

    public:

        static constexpr bool value = iter_ok && decltype(test<IT, S>(0))::value;
    };

public:

    //=========================================================================
    // constructors
    //=========================================================================

    VX_FN_REQUIRES(extent == 0 || extent == dynamic_extent)
    constexpr span() noexcept
    {
    }

    constexpr span(const span&) noexcept = default;

    template <typename U, VX_REQUIRES(std::is_same<U, T>::value)>
    constexpr span(U* ptr, size_type count) noexcept
        : m_data(ptr, count)
    {}

    template <typename IT, VX_REQUIRES(extent != dynamic_extent && is_compatible_iterator<IT>::value)>
    constexpr explicit span(IT first, size_type count) noexcept
        : m_data(std::addressof(*first), count)
    {}

    template <typename IT, VX_REQUIRES(extent == dynamic_extent && is_compatible_iterator<IT>::value)>
    constexpr span(IT first, size_type count) noexcept
        : m_data(std::addressof(*first), count)
    {}

    template <typename IT, typename S, VX_REQUIRES(extent == dynamic_extent && is_compatible_sentinel<IT, S>::value)>
    constexpr span(IT first, S last) noexcept
        : m_data(std::addressof(*first), static_cast<size_type>(last - first))
    {}

    template <typename IT, typename S, VX_REQUIRES(extent != dynamic_extent && is_compatible_sentinel<IT, S>::value)>
    constexpr explicit span(IT first, S last) noexcept
        : m_data(std::addressof(*first), static_cast<size_type>(last - first))
    {}

    template <size_t N = extent, VX_REQUIRES((extent == dynamic_extent || extent == N))>
    constexpr span(T (&arr)[N]) noexcept
        : m_data(arr, N)
    {}

    template <typename U, size_t N, VX_REQUIRES((extent == dynamic_extent || extent == N) && std::is_convertible<U (*)[], T (*)[]>::value)>
    constexpr span(array<U, N>& arr) noexcept
        : m_data(arr.data(), N)
    {}
    
    template <typename U, size_t N, VX_REQUIRES((extent == dynamic_extent || extent == N) && std::is_convertible<const U (*)[], T (*)[]>::value)>
    constexpr span(const array<U, N>& arr) noexcept
        : m_data(arr.data(), N)
    {}
    
    VX_FN_REQUIRES(extent != dynamic_extent)
    constexpr span(std::initializer_list<T> init) noexcept
        : m_data(init.begin(), init.size())
    {}
    
    template <typename U, size_t E, VX_REQUIRES((extent == dynamic_extent || E == dynamic_extent || extent == E) && std::is_convertible<U (*)[], T (*)[]>::value)>
    constexpr span(const span<U, E>& other) noexcept
        : m_data(other.data(), E)
    {}

    //=========================================================================
    // element access
    //=========================================================================

    constexpr const_reference front() const noexcept
    {
        VX_ASSERT(size() > 0);
        return m_data.data[0];
    }

    constexpr const_reference back() const noexcept
    {
        const size_type count = size();
        VX_ASSERT(count > 0);
        return m_data.data[count - 1];
    }

    constexpr pointer data() const noexcept
    {
        return m_data.data;
    }

    constexpr const_reference operator[](const size_type i) const noexcept
    {
        VX_ASSERT(i < size());
        return m_data.data[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_data.data);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_data.data);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_data.data + m_data.size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_data.data + m_data.size);
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

    constexpr size_type size() const noexcept
    {
        return m_data.size;
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type size_bytes() const noexcept
    {
        return size() * sizeof(value_type);
    }

    //=========================================================================
    // sub-range
    //=========================================================================

    template <size_type N>
    constexpr span<T, N> first() const noexcept
    {
        VX_ASSERT(N <= size());
        return span<T, N>(m_data.data, N);
    }

    constexpr span<T> first(const size_type count) const noexcept
    {
        VX_ASSERT(count <= size());
        return span<T>(m_data.data, count);
    }

    template <size_type N>
    constexpr span<T, N> last() const noexcept
    {
        VX_ASSERT(N <= size());
        return span<T, N>(m_data.data + (size() - N), N);
    }

    constexpr span<T> last(const size_type count) const noexcept
    {
        VX_ASSERT(count <= size());
        return span<T>(m_data.data + (size() - count), count);
    }

    template <size_type Off, size_type N = dynamic_extent>
    constexpr auto subspan() const noexcept
    {
        VX_STATIC_ASSERT_MSG(Off <= Extent || Extent == dynamic_extent, "Off out of range");
        VX_ASSERT(Off <= size());

        using return_span_t = span<T, (N != dynamic_extent ? N : (Extent != dynamic_extent ? Extent - Off : dynamic_extent))>;
        size_type new_size = (N != dynamic_extent ? N : size() - Off);
        VX_ASSERT(Off + new_size <= size());

        return return_span_t(m_data.data + Off, new_size);
    }

    constexpr span<T> subspan(size_type off, size_type count = dynamic_extent) const noexcept
    {
        VX_ASSERT(off <= size());

        const size_type r = (count == dynamic_extent ? (size() - off) : count);
        VX_ASSERT(off + r <= size());

        return span<T>(m_data.data + off, r);
    }
};

} // namespace vx
