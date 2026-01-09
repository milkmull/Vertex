#pragma once

#include <iterator>

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace _priv {

template <typename T>
class pointer_iterator
{
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename std::remove_pointer<T>::type;
    using difference_type = ptrdiff_t;
    using pointer = T;
    using reference = typename std::add_lvalue_reference<value_type>::type;

    pointer_iterator() = default;

    explicit pointer_iterator(pointer p)
        : m_ptr(p)
    {}

    template <typename>
    friend class pointer_iterator;

    template <typename U, VX_REQUIRES((std::is_convertible<U, T>::value))>
    pointer_iterator(const pointer_iterator<U>& other)
        : m_ptr(other.m_ptr)
    {}

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    pointer_iterator& operator++()
    {
        ++m_ptr;
        return *this;
    }

    pointer_iterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    pointer_iterator& operator--()
    {
        --m_ptr;
        return *this;
    }

    pointer_iterator operator--(int)
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    pointer_iterator& operator+=(difference_type n)
    {
        m_ptr += n;
        return *this;
    }

    pointer_iterator& operator-=(difference_type n)
    {
        m_ptr -= n;
        return *this;
    }

    friend pointer_iterator operator+(pointer_iterator it, difference_type n)
    {
        return pointer_iterator(it.m_ptr + n);
    }

    friend pointer_iterator operator-(pointer_iterator it, difference_type n)
    {
        return pointer_iterator(it.m_ptr - n);
    }

    friend difference_type operator-(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr - b.m_ptr;
    }

    friend bool operator==(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr == b.m_ptr;
    }

    friend bool operator!=(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr != b.m_ptr;
    }

    friend bool operator<(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr < b.m_ptr;
    }

    friend bool operator>(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr > b.m_ptr;
    }

    friend bool operator<=(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr <= b.m_ptr;
    }

    friend bool operator>=(pointer_iterator a, pointer_iterator b)
    {
        return a.m_ptr >= b.m_ptr;
    }

    pointer ptr() const
    {
        return m_ptr;
    }

private:

    pointer m_ptr = nullptr;
};

template <typename IT>
class reverse_pointer_iterator
{
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename IT::value_type;
    using difference_type = typename IT::difference_type;
    using pointer = typename IT::pointer;
    using reference = typename IT::reference;

    reverse_pointer_iterator() = default;

    explicit reverse_pointer_iterator(IT it)
        : m_it(it)
    {}

    template <typename>
    friend class reverse_pointer_iterator;

    template <typename IT2, VX_REQUIRES((std::is_convertible<IT2, IT>::value))>
    reverse_pointer_iterator(const reverse_pointer_iterator<IT2>& other)
        : m_it(other.m_it)
    {}

    reference operator*() const
    {
        IT tmp = m_it;
        return *--tmp;
    }

    pointer operator->() const
    {
        return std::addressof(operator*());
    }

    reverse_pointer_iterator& operator++()
    {
        --m_it;
        return *this;
    }

    reverse_pointer_iterator operator++(int)
    {
        auto tmp = *this;
        --m_it;
        return tmp;
    }

    reverse_pointer_iterator& operator--()
    {
        ++m_it;
        return *this;
    }

    reverse_pointer_iterator operator--(int)
    {
        auto tmp = *this;
        ++m_it;
        return tmp;
    }

    friend bool operator==(reverse_pointer_iterator a, reverse_pointer_iterator b)
    {
        return a.m_it == b.m_it;
    }

    friend bool operator!=(reverse_pointer_iterator a, reverse_pointer_iterator b)
    {
        return a.m_it != b.m_it;
    }

    pointer ptr() const
    {
        return m_it.ptr();
    }

private:

    IT m_it;
};

} // namespace _priv
} // namespace vx
