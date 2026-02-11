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

    using iterator               = _priv::pointer_iterator<array, T>;
    using const_iterator         = _priv::pointer_iterator<array, const T>;
    using reverse_iterator       = _priv::reverse_pointer_iterator<iterator>;
    using const_reverse_iterator = _priv::reverse_pointer_iterator<const_iterator>;

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        return _m_array[0];
    }

    const T& front() const noexcept
    {
        return _m_array[0];
    }

    T& back() noexcept
    {
        return _m_array[N - 1];
    }

    const T& back() const noexcept
    {
        return _m_array[N - 1];
    }

    T* data() noexcept
    {
        return _m_array;
    }

    const T* data() const noexcept
    {
        return _m_array;
    }

    T& operator[](size_type i) noexcept
    {
        return _m_array[i];
    }

    const T& operator[](size_type i) const noexcept
    {
        return _m_array[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    iterator begin() noexcept
    {
        return iterator(_m_array);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(_m_array);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    iterator end() noexcept
    {
        return iterator(_m_array + N);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(_m_array + N);
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
        mem::fill_range(_m_array, N, value);
    }

    void swap(array& other) noexcept
    {
        std::swap(_m_array, other._m_array);
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
        return mem::equal_range(lhs._m_array, rhs._m_array, size());
    }

    friend bool operator!=(const array& lhs, const array& rhs)
    {
        return !operator==(lhs, rhs);
    }

    friend bool operator<(const array& lhs, const array& rhs)
    {
        return mem::less_range(lhs._m_array, rhs._m_array, size());
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

    // this must be public to support aggregate initialization
    // https://en.cppreference.com/w/cpp/language/aggregate_initialization.html
    T _m_array[N];
};

} // namespace vx
