#pragma once

#include <span>

#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/char_traits.hpp"

namespace vx {

enum : size_t
{
    dynamic_extent = std::numeric_limits<size_t>::max()
};

namespace _priv {

template <typename T, size_t Extent>
struct extent_data
{
    constexpr extent_data() noexcept = default;

    constexpr explicit extent_data(const T* ptr, const size_t) noexcept
        : data(ptr)
    {}

    const T* data = nullptr;
    static constexpr size_t size = Extent;
};

template <typename T>
struct extent_data<T, dynamic_extent>
{
    constexpr extent_data() noexcept = default;

    constexpr explicit extent_data(const T* ptr, const size_t count) noexcept
        : data(ptr), size(count)
    {}

    const T* data = nullptr;
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
    using pointer = const T*;
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

public:

    //=========================================================================
    // constructors
    //=========================================================================

    VX_FN_REQUIRES(Extent == 0 || Extent == dynamic_extent)
    constexpr span() noexcept = default;

    VX_FN_REQUIRES(Extent == dynamic_extent)
    constexpr span(const T* ptr, size_type count) noexcept
        : m_data(ptr, count)
    {}

    VX_FN_REQUIRES(Extent != dynamic_extent && Extent != 0)
    constexpr span(const T* ptr, size_type count) noexcept
        : m_data(ptr, count)
    {
        VX_ASSERT(count == Extent);
    }

    template <size_t Count, VX_REQUIRES(Extent == dynamic_extent)>
    constexpr span(const T (&arr)[Count]) noexcept
        : m_data(arr, Count)
    {}

    template <size_t Count, VX_REQUIRES(Extent == Count)>
    constexpr span(const T (&arr)[Count]) noexcept
        : m_data(arr, Count)
    {}

    template <typename U, VX_REQUIRES(std::is_convertible<const U*, const T*>::value)>
    constexpr span(const span<U, Extent>& other) noexcept
        : m_data(other.data(), other.size())
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

    constexpr const_pointer data() const noexcept
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

    constexpr size_type size() const noexcept
    {
        return m_data.size;
    }

    constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    size_type data_size() const noexcept
    {
        return size() * sizeof(value_type);
    }

    //=========================================================================
    // sub-range
    //=========================================================================

    template <size_type Count>
    constexpr span<T, Count> first() const noexcept
    {
        VX_ASSERT(Count <= size());
        return span<T, Count>(m_data.data, Count);
    }

    constexpr span<T> first(const size_type count) const noexcept
    {
        VX_ASSERT(count <= size());
        return span<T>(m_data.data, count);
    }

    template <size_type Count>
    constexpr span<T, Count> last() const noexcept
    {
        VX_ASSERT(Count <= size());
        return span<T, Count>(m_data.data + (size() - Count), Count);
    }

    constexpr span<T> last(const size_type count) const noexcept
    {
        VX_ASSERT(count <= size());
        return span<T>(m_data.data + (size() - count), count);
    }

    template <size_type Off, size_type Count = dynamic_extent>
    constexpr auto subspan() const noexcept
    {
        VX_STATIC_ASSERT_MSG(Off <= Extent || Extent == dynamic_extent, "Off out of range");
        VX_ASSERT(Off <= size());

        using return_span_t = span<T, (Count != dynamic_extent ? Count : (Extent != dynamic_extent ? Extent - Off : dynamic_extent))>;
        const size_type new_size = (Count != dynamic_extent ? Count : size() - Off);
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
