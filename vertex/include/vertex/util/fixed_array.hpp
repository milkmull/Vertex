#pragma once

#include <memory>
#include <iterator>

namespace vx {

template <typename T>
class fixed_array
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // member types
    ///////////////////////////////////////////////////////////////////////////////

    using value_type = T;
    using allocator_type = std::allocator<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    fixed_array() noexcept : m_size(0), m_data(nullptr) {}

    explicit fixed_array(size_type count)
        : m_size(count), m_data(new T[count]) {}

    explicit fixed_array(size_type count, const T& value)
        : m_size(count), m_data(new T[count])
    {
        std::fill(m_data, m_data + count, value);
    }

    fixed_array(std::initializer_list<T> ilist)
        : m_size(ilist.size()), m_data(new T[m_size])
    {
        std::copy(ilist.begin(), ilist.end(), m_data);
    }

    template <typename IT>
    fixed_array(IT first, IT last)
        : m_size(static_cast<size_type>(std::distance(first, last))), m_data(new T[m_size])
    {
        std::copy(first, last, m_data);
    }

    fixed_array(const fixed_array& other)
        : m_size(other.m_size), m_data(new T[m_size])
    {
        std::copy(other.m_data, other.m_data + m_size, m_data);
    }

    fixed_array(fixed_array&& other) noexcept
        : m_size(other.m_size), m_data(other.m_data)
    {
        other.m_size = 0;
        other.m_data = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~fixed_array() { delete[] m_data; }

    ///////////////////////////////////////////////////////////////////////////////
    // assignment
    ///////////////////////////////////////////////////////////////////////////////

    fixed_array& operator=(const fixed_array& rhs)
    {
        if (this != &rhs)
        {
            if (m_size != rhs.m_size)
            {
                delete[] m_data;
                m_size = rhs.m_size;
                m_data = new T[m_size];
            }

            std::copy(rhs.m_data, rhs.m_data + m_size, m_data);
        }

        return *this;
    }

    fixed_array& operator=(fixed_array&& rhs) noexcept
    {
        if (this != &rhs)
        {
            delete[] m_data;
            m_size = rhs.m_size;
            m_data = rhs.m_data;
            rhs.m_size = 0;
            rhs.m_data = nullptr;
        }

        return *this;
    }

    fixed_array& operator=(std::initializer_list<T> ilist)
    {
        if (m_size != ilist.size())
        {
            delete[] m_data;
            m_size = ilist.size();
            m_data = new T[m_size];
        }

        std::copy(ilist.begin(), ilist.end(), m_data);
        return *this;
    }

    fixed_array& assign(size_type count, const T& value)
    {
        if (m_size != count)
        {
            delete[] m_data;
            m_size = count;
            m_data = new T[m_size];
        }

        std::fill(m_data, m_data + count, value);
        return *this;
    }

    template <typename IT>
    fixed_array& assign(IT first, IT last)
    {
        const size_type size = static_cast<size_type>(std::distance(first, last));

        if (m_size != size)
        {
            delete[] m_data;
            m_size = size;
            m_data = new T[m_size];
        }

        std::copy(first, last, m_data);
        return *this;
    }

    fixed_array& assign(std::initializer_list<T> ilist)
    {
        return operator=(ilist);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // element access
    ///////////////////////////////////////////////////////////////////////////////

    reference operator[](size_type index)
    {
        return m_data[index];
    }

    const_reference operator[](size_type index) const
    {
        return m_data[index];
    }

    reference at(size_type index)
    {
        return m_data[index];
    }

    const_reference at(size_type index) const
    {
        return m_data[index];
    }

    reference front()
    {
        return m_data[0];
    }

    const_reference front() const
    {
        return m_data[0];
    }

    reference back()
    {
        return m_data[m_size - 1];
    }

    const_reference back() const
    {
        return m_data[m_size - 1];
    }

    pointer data() noexcept { return m_data; }
    const_pointer data() const noexcept { return m_data; }

    ///////////////////////////////////////////////////////////////////////////////
    // capacity
    ///////////////////////////////////////////////////////////////////////////////

    size_type size() const noexcept { return m_size; }
    bool empty() const noexcept { return m_size == 0; }
    constexpr size_type max_size() const noexcept{ return std::numeric_limits<size_type>::max(); }

    ///////////////////////////////////////////////////////////////////////////////
    // iterators
    ///////////////////////////////////////////////////////////////////////////////

    iterator begin() noexcept { return m_data; }
    const_iterator begin() const noexcept { return m_data; }
    const_iterator cbegin() const noexcept { return m_data; }

    iterator end() noexcept { return m_data + m_size; }
    const_iterator end() const noexcept { return m_data + m_size; }
    const_iterator cend() const noexcept { return m_data + m_size; }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const fixed_array& rhs) const
    {
        return (m_size == rhs.m_size) && std::equal(m_data, m_data + m_size, rhs.m_data);
    }

    bool operator!=(const fixed_array& rhs) const
    {
        return (m_size != rhs.m_size) || !std::equal(m_data, m_data + m_size, rhs.m_data);
    }

private:

    size_type m_size = 0;
    T* m_data;
};

} // namespace vx
