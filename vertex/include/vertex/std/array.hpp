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

    constexpr T& front() noexcept
    {
        return _m_array[0];
    }

    constexpr const T& front() const noexcept
    {
        return _m_array[0];
    }

    constexpr T& back() noexcept
    {
        return _m_array[N - 1];
    }

    constexpr const T& back() const noexcept
    {
        return _m_array[N - 1];
    }

    constexpr T* data() noexcept
    {
        return _m_array;
    }

    constexpr const T* data() const noexcept
    {
        return _m_array;
    }

    constexpr T& operator[](size_type i) noexcept
    {
        return _m_array[i];
    }

    constexpr const T& operator[](size_type i) const noexcept
    {
        return _m_array[i];
    }

    //=========================================================================
    // iterators
    //=========================================================================

    constexpr iterator begin() noexcept
    {
        return iterator(_m_array);
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator(_m_array);
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr iterator end() noexcept
    {
        return iterator(_m_array + N);
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator(_m_array + N);
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    //=========================================================================
    // operations
    //=========================================================================

    constexpr void fill(const T& value)
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_type i = 0; i < N; ++i)
            {
                _m_array[i] = value;
            }
            return;
        }

        mem::fill_range(_m_array, N, value);
    }

    constexpr void swap(array& other) noexcept
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

    static constexpr size_type size_bytes() noexcept
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

    constexpr friend bool operator==(const array& lhs, const array& rhs)
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_type i = 0; i < N; ++i)
            {
                if (lhs._m_array[i] != rhs._m_array[i])
                {
                    return false;
                }
            }
            return true;
        }

        return mem::compare_range(lhs._m_array, rhs._m_array, size()) == 0;
    }

    constexpr friend bool operator!=(const array& lhs, const array& rhs)
    {
        return !operator==(lhs, rhs);
    }

    constexpr friend bool operator<(const array& lhs, const array& rhs)
    {
        if (VX_IS_CONSTANT_EVALUATED())
        {
            for (size_type i = 0; i < N; ++i)
            {
                if (lhs._m_array[i] != rhs._m_array[i])
                {
                    return lhs._m_array[i] < rhs._m_array[i];
                }
            }
            return false;
        }

        return mem::compare_range(lhs._m_array, rhs._m_array, size()) < 0;
    }

    constexpr friend bool operator>(const array& lhs, const array& rhs)
    {
        return rhs < lhs;
    }

    constexpr friend bool operator<=(const array& lhs, const array& rhs)
    {
        return !(rhs < lhs);
    }

    constexpr friend bool operator>=(const array& lhs, const array& rhs)
    {
        return !(lhs < rhs);
    }

    // this must be public to support aggregate initialization
    // https://en.cppreference.com/w/cpp/language/aggregate_initialization.html
    T _m_array[N];
};

} // namespace vx
