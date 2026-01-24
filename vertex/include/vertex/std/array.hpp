#pragma once

#include "vertex/std/_priv/pointer_iterator.hpp"
#include "vertex/std/memory.hpp"

namespace vx {

template <typename T, size_t N>
class array
{
public:

    VX_STATIC_ASSERT_MSG(N > 0, "Empty array not allowed");

    //=========================================================================
    // member types
    //=========================================================================

    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using iterator               = _priv::pointer_iterator<T*>;
    using const_iterator         = _priv::pointer_iterator<const T*>;
    using reverse_iterator       = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        return m_array[0];
    }

    const T& front() const noexcept
    {
        return m_array[0];
    }

    T& back() noexcept
    {
        return m_array[N - 1];
    }

    const T& back() const noexcept
    {
        return m_array[N - 1];
    }

    T* data() noexcept
    {
        return m_array;
    }

    const T* data() const noexcept
    {
        return m_array;
    }

    T& operator[](size_type i) noexcept
    {
        return m_array[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        return m_array[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(m_array);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_array);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(m_array + m_buffer.size);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_array + m_buffer.size);
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
    // operations
    //=========================================================================

    void fill(const T& value)
    {
        mem::fill_range(m_array, N);
    }

    void swap(array& other) noexcept
    {
        std::swap(m_array, other.m_array);
    }

    //=========================================================================
    // size
    //=========================================================================

    static constexpr bool empty() noexcept
    {
        return false;
    }

    static constexpr size_type size() noexcept
    {
        return N;
    }

    static constexpr size_type data_size() noexcept
    {
        return size() * sizeof(T);
    }

    static constexpr size_type max_size() noexcept
    {
        return mem::max_array_size<T>();
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const array& lhs, const array& rhs)
    {
        return mem::equal_range(lhs.m_array, rhs.m_array, size());
    }

    friend bool operator!=(const array& lhs, const array& rhs)
    {
        return !operator==(lhs, rhs);
    }

    friend bool operator<(const array& lhs, const array& rhs)
    {
        return mem::less_range(lhs.m_array, rhs.m_array, size());
    }

    friend bool operator>(const array& lhs, const array& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const array& lhs, const array& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const array& lhs, const array& rhs)
    {
        return !(lhs < rhs);
    }

private:

    T m_array[N];
};

} // namespace vx
