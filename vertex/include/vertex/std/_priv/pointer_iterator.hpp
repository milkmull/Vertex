#pragma once

#include <iterator>

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace _priv {

template <typename Owner, typename T>
class pointer_iterator
{
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename std::remove_pointer<T>::type;
    using difference_type = ptrdiff_t;
    using pointer = T;
    using reference = typename std::add_lvalue_reference<value_type>::type;

    pointer_iterator() = default;

    explicit pointer_iterator(pointer p) noexcept
        : m_ptr(p)
    {}

    template <typename, typename>
    friend class pointer_iterator;

    template <typename U, VX_REQUIRES((std::is_convertible<U, T>::value))>
    pointer_iterator(const pointer_iterator<Owner, U>& other) noexcept
        : m_ptr(other.m_ptr)
    {}

    reference operator*() const noexcept
    {
        return *m_ptr;
    }

    pointer operator->() const noexcept
    {
        return m_ptr;
    }

    pointer_iterator& operator++() noexcept
    {
        ++m_ptr;
        return *this;
    }

    pointer_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    pointer_iterator& operator--() noexcept
    {
        --m_ptr;
        return *this;
    }

    pointer_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    pointer_iterator& operator+=(difference_type n) noexcept
    {
        m_ptr += n;
        return *this;
    }

    pointer_iterator& operator-=(difference_type n) noexcept
    {
        m_ptr -= n;
        return *this;
    }

    friend pointer_iterator operator+(pointer_iterator it, difference_type n) noexcept
    {
        return pointer_iterator(it.m_ptr + n);
    }

    friend pointer_iterator operator-(pointer_iterator it, difference_type n) noexcept
    {
        return pointer_iterator(it.m_ptr - n);
    }

    friend difference_type operator-(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr - b.m_ptr;
    }

    reference operator[](difference_type n) const noexcept
    {
        return *(m_ptr + n);
    }

    friend pointer_iterator operator+(difference_type n, pointer_iterator it) noexcept
    {
        return pointer_iterator(it.m_ptr + n);
    }

    friend bool operator==(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr == b.m_ptr;
    }

    friend bool operator!=(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr != b.m_ptr;
    }

    friend bool operator<(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr < b.m_ptr;
    }

    friend bool operator>(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr > b.m_ptr;
    }

    friend bool operator<=(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr <= b.m_ptr;
    }

    friend bool operator>=(pointer_iterator a, pointer_iterator b) noexcept
    {
        return a.m_ptr >= b.m_ptr;
    }

    pointer ptr() const noexcept
    {
        return m_ptr;
    }

    explicit operator bool() const noexcept
    {
        return m_ptr != nullptr;
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

    explicit reverse_pointer_iterator(IT it) noexcept
        : m_it(it)
    {}

    template <typename>
    friend class reverse_pointer_iterator;

    template <typename IT2, VX_REQUIRES((std::is_convertible<IT2, IT>::value))>
    reverse_pointer_iterator(const reverse_pointer_iterator<IT2>& other) noexcept
        : m_it(other.m_it)
    {}

    reference operator*() const noexcept
    {
        IT tmp = m_it;
        return *--tmp;
    }

    pointer operator->() const noexcept
    {
        return std::addressof(operator*());
    }

    reverse_pointer_iterator& operator++() noexcept
    {
        --m_it;
        return *this;
    }

    reverse_pointer_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        --m_it;
        return tmp;
    }

    reverse_pointer_iterator& operator--() noexcept
    {
        ++m_it;
        return *this;
    }

    reverse_pointer_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        ++m_it;
        return tmp;
    }

    reverse_pointer_iterator& operator+=(difference_type n) noexcept
    {
        m_it -= n;
        return *this;
    }

    reverse_pointer_iterator& operator-=(difference_type n) noexcept
    {
        m_it += n;
        return *this;
    }

    reverse_pointer_iterator operator+(difference_type n) const noexcept
    {
        return reverse_pointer_iterator(m_it - n);
    }

    reverse_pointer_iterator operator-(difference_type n) const noexcept
    {
        return reverse_pointer_iterator(m_it + n);
    }

    friend reverse_pointer_iterator operator+(difference_type n, reverse_pointer_iterator it) noexcept
    {
        return reverse_pointer_iterator(it.m_it - n);
    }

    friend difference_type operator-(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return b.m_it - a.m_it;
    }

    reference operator[](difference_type n) const noexcept
    {
        return *(*this + n);
    }

    friend bool operator<(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return b.m_it < a.m_it;
    }

    friend bool operator>(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return b.m_it > a.m_it;
    }

    friend bool operator<=(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return b.m_it <= a.m_it;
    }

    friend bool operator>=(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return b.m_it >= a.m_it;
    }

    friend bool operator==(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return a.m_it == b.m_it;
    }

    friend bool operator!=(reverse_pointer_iterator a, reverse_pointer_iterator b) noexcept
    {
        return a.m_it != b.m_it;
    }

    pointer ptr() const noexcept
    {
        return m_it.ptr();
    }

    explicit operator bool() const noexcept
    {
        return static_cast<bool>(m_it);
    }

private:

    IT m_it;
};

template <typename>
struct is_pointer_iterator : std::false_type
{
};

template <typename Owner, typename T>
struct is_pointer_iterator<pointer_iterator<Owner, T>> : std::true_type
{
};

template <typename IT>
struct is_pointer_iterator<reverse_pointer_iterator<IT>> : std::true_type
{
};

} // namespace _priv
} // namespace vx
