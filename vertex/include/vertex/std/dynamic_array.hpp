#pragma once

#include "vertex/std/_priv/dynamic_array_base.hpp"

namespace vx {

template <typename T, typename Allocator = mem::default_allocator<T>>
class dynamic_array
{
private:

    using array_type = _priv::dynamic_array_base<T, Allocator>;
    array_type m_array;

public:

    //=========================================================================
    // member types
    //=========================================================================

    template <intmax_t N, intmax_t D = 1>
    using growth_rate_type = typename array_type::template growth_rate_type<N, D>;
    using default_growth_rate = typename array_type::default_growth_rate;

    using value_type = typename array_type::value_type;
    using allocator_type = typename array_type::allocator_type;
    using pointer = typename array_type::pointer;
    using const_pointer = typename array_type::const_pointer;
    using reference = typename array_type::reference;
    using const_reference = typename array_type::const_reference;
    using size_type = typename array_type::size_type;
    using difference_type = typename array_type::difference_type;

    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;
    using reverse_iterator = typename array_type::reverse_iterator;
    using const_reverse_iterator = typename array_type::const_reverse_iterator;

    //=========================================================================
    // constructors
    //=========================================================================

    dynamic_array() = default;
    ~dynamic_array() = default;

    dynamic_array(size_type count)
        : m_array(count)
    {}

    dynamic_array(const size_type count, const T& value)
        : m_array(count, value)
    {}

    dynamic_array(std::initializer_list<T> init)
        : m_array(init)
    {}

    dynamic_array(const dynamic_array& other)
        : m_array(other.m_array)
    {}

    dynamic_array(dynamic_array&& other) noexcept
        : m_array(std::move(other.m_array))
    {}

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    dynamic_array(IT first, IT last) noexcept
        : m_array(first, last)
    {}

    //=========================================================================
    // assignment operators
    //=========================================================================

    dynamic_array& operator=(const dynamic_array& other)
    {
        m_array = other.m_array;
        return *this;
    }

    dynamic_array& operator=(std::initializer_list<T> init)
    {
        m_array = init;
        return *this;
    }

    dynamic_array& operator=(dynamic_array&& other) noexcept
    {
        m_array = std::move(other.m_array);
        return *this;
    }

    //=========================================================================
    // assign
    //=========================================================================

    bool assign(const dynamic_array& other)
    {
        return m_array.assign(other.m_array);
    }

    bool assign(dynamic_array&& other) noexcept
    {
        m_array.assign(std::move(other.m_array));
        return true;
    }

    bool assign(std::initializer_list<T> init)
    {
        return m_array.assign(init);
    }

    template <typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    bool assign(IT first, IT last)
    {
        return m_array.assign(first, last);
    }

    //=========================================================================
    // element access
    //=========================================================================

    T& front() noexcept
    {
        return m_array.front();
    }

    const T& front() const noexcept
    {
        return m_array.front();
    }

    T& back() noexcept
    {
        return m_array.back();
    }

    const T& back() const noexcept
    {
        return m_array.back();
    }

    T* data() noexcept
    {
        return m_array.data();
    }

    const T* data() const noexcept
    {
        return m_array.data();
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
        return m_array.begin();
    }

    const_iterator begin() const noexcept
    {
        return m_array.begin();
    }

    const_iterator cbegin() const noexcept
    {
        return m_array.cbegin();
    }

    iterator end() noexcept
    {
        return m_array.end();
    }

    const_iterator end() const noexcept
    {
        return m_array.end();
    }

    const_iterator cend() const noexcept
    {
        return m_array.cend();
    }

    reverse_iterator rbegin() noexcept
    {
        return m_array.rbegin();
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return m_array.rbegin();
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return m_array.crbegin();
    }

    reverse_iterator rend() noexcept
    {
        return m_array.rend();
    }

    const_reverse_iterator rend() const noexcept
    {
        return m_array.rend();
    }

    const_reverse_iterator crend() const noexcept
    {
        return m_array.crend();
    }

    //=========================================================================
    // memory
    //=========================================================================

    void clear()
    {
        m_array.clear();
    }

    void clear_and_deallocate()
    {
        m_array.clear_and_deallocate();
    }

    bool shrink_to_fit()
    {
        return m_array.shrink_to_fit();
    }

    T* release() noexcept
    {
        return m_array.release();
    }

    bool acquire(T* ptr, size_type count) noexcept
    {
        return m_array.acquire(ptr, count);
    }

    void swap(dynamic_array& other) noexcept
    {
        m_array.swap(other.m_array);
    }

    //=========================================================================
    // size
    //=========================================================================

    bool empty() const noexcept
    {
        return m_array.empty();
    }

    size_type size() const noexcept
    {
        return m_array.size();
    }

    size_type data_size() const noexcept
    {
        return m_array.data_size();
    }

    static constexpr size_type max_size() noexcept
    {
        return array_type::max_size();
    }

    //=========================================================================
    // capacity
    //=========================================================================

    size_type capacity() const noexcept
    {
        return m_array.capacity();
    }

    //=========================================================================
    // reserve
    //=========================================================================

    bool reserve(size_type new_capacity)
    {
        return m_array.reserve(new_capacity);
    }

    //=========================================================================
    // resize
    //=========================================================================

    template <typename U>
    bool resize(const size_type count, const U& value)
    {
        return m_array.resize(count, value);
    }

    bool resize(const size_type count)
    {
        return m_array.resize(count);
    }

    //=========================================================================
    // insert
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, const T& value)
    {
        return m_array.insert(pos, value);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, T&& value) noexcept
    {
        return m_array.template insert<growth_rate>(pos, std::move(value));
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, size_type count, const T& value)
    {
        return m_array.template insert<growth_rate>(pos, count, value);
    }

    template <typename growth_rate = default_growth_rate>
    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        return m_array.template insert<growth_rate>(pos, init);
    }

    template <typename growth_rate = default_growth_rate, typename IT, VX_REQUIRES(type_traits::is_iterator<IT>::value)>
    iterator insert(const_iterator pos, IT first, IT last)
    {
        return m_array.template insert<growth_rate>(pos, first, last);
    }

    //=========================================================================
    // emplace
    //=========================================================================

    template <typename growth_rate = default_growth_rate, typename... Args>
    pointer emplace_back(Args&&... args)
    {
        return m_array.template emplace_back<growth_rate>(std::forward<Args>(args)...);
    }

    template <typename growth_rate = default_growth_rate, typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        return m_array.template emplace<growth_rate>(pos, std::forward<Args>(args)...);
    }

    //=========================================================================
    // push back
    //=========================================================================

    template <typename growth_rate = default_growth_rate>
    void push_back(const T& value)
    {
        return m_array.template push_back<growth_rate>(value);
    }

    template <typename growth_rate = default_growth_rate>
    void push_back(T&& value)
    {
        return m_array.template push_back<growth_rate>(std::move(value));
    }

    //=========================================================================
    // erase
    //=========================================================================

    iterator erase(const_iterator pos)
    {
        return m_array.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return m_array.erase(first, last);
    }

    void pop_back()
    {
        m_array.pop_back();
    }

    //=========================================================================
    // comparison
    //=========================================================================

    friend bool operator==(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return lhs.m_array == rhs.m_array;
    }

    friend bool operator!=(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return !operator==(lhs, rhs);
    }

    friend bool operator<(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return lhs.m_array < rhs.m_array;
    }

    friend bool operator>(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return !(rhs < lhs);
    }

    friend bool operator>=(const dynamic_array& lhs, const dynamic_array& rhs)
    {
        return !(lhs < rhs);
    }
};

} // namespace vx
