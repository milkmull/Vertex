#pragma once

#include <iterator>

#include "vertex/config/type_traits.hpp"

namespace vx {

//==============================================================================

template <typename Owner, typename T>
class pointer_iterator;

template <typename IT>
class reverse_pointer_iterator;

//==============================================================================

template <typename>
struct is_pointer_iterator : std::false_type
{};

template <typename Owner, typename T>
struct is_pointer_iterator<pointer_iterator<Owner, T>> : std::true_type
{};

template <typename IT>
struct is_pointer_iterator<reverse_pointer_iterator<IT>> : std::true_type
{};

//==============================================================================

template <typename>
struct is_forward_pointer_iterator : std::false_type
{};

template <typename Owner, typename T>
struct is_forward_pointer_iterator<pointer_iterator<Owner, T>> : std::true_type
{};

//==============================================================================

template <typename>
struct is_reverse_pointer_iterator : std::false_type
{};

template <typename IT>
struct is_reverse_pointer_iterator<reverse_pointer_iterator<IT>> : std::true_type
{};

//==============================================================================

template <typename IT1, typename IT2>
struct is_compatible_pointer_iterator : std::false_type
{};

template <typename Owner, typename T1, typename T2>
struct is_compatible_pointer_iterator<pointer_iterator<Owner, T1>, pointer_iterator<Owner, T2>> :
    std::is_convertible<typename std::remove_cv<T1>::type (*)[], typename std::remove_cv<T2>::type (*)[]>
{};

template <typename IT1, typename IT2>
struct is_compatible_pointer_iterator<reverse_pointer_iterator<IT1>, reverse_pointer_iterator<IT2>> : is_compatible_pointer_iterator<IT1, IT2>
{};

//==============================================================================

template <typename Owner, typename T>
class pointer_iterator
{
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

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

    reference operator[](difference_type n) const noexcept
    {
        return *(m_ptr + n);
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

//==============================================================================

template <typename Owner, typename T>
constexpr pointer_iterator<Owner, T> operator+(
    pointer_iterator<Owner, T> it,
    typename pointer_iterator<Owner, T>::difference_type n) noexcept
{
    return pointer_iterator<Owner, T>(it.ptr() + n);
}

template <typename Owner, typename T>
constexpr pointer_iterator<Owner, T> operator+(
    typename pointer_iterator<Owner, T>::difference_type n,
    pointer_iterator<Owner, T> it) noexcept
{
    return pointer_iterator<Owner, T>(it.ptr() + n);
}

template <typename Owner, typename T>
constexpr pointer_iterator<Owner, T> operator-(
    pointer_iterator<Owner, T> it,
    typename pointer_iterator<Owner, T>::difference_type n) noexcept
{
    return pointer_iterator<Owner, T>(it.ptr() - n);
}

//==============================================================================

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

    reference operator[](difference_type n) const noexcept
    {
        return *(*this + n);
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

//==============================================================================

template <typename IT>
constexpr reverse_pointer_iterator<IT> operator+(
    typename reverse_pointer_iterator<IT>::difference_type n,
    reverse_pointer_iterator<IT> it) noexcept
{
    return reverse_pointer_iterator<IT>(it.m_it - n);
}

template <typename IT>
constexpr reverse_pointer_iterator<IT> operator+(
    reverse_pointer_iterator<IT> it,
    typename reverse_pointer_iterator<IT>::difference_type n) noexcept
{
    return reverse_pointer_iterator<IT>(it.m_it - n);
}

template <typename IT>
constexpr reverse_pointer_iterator<IT> operator-(
    reverse_pointer_iterator<IT> it,
    typename reverse_pointer_iterator<IT>::difference_type n) noexcept
{
    return reverse_pointer_iterator<IT>(it.m_it + n);
}

//==============================================================================
// general operators
//==============================================================================

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr typename IT1::difference_type operator-(IT1 a, IT2 b) noexcept
{
    return static_cast<typename IT1::difference_type>(a.ptr() - b.ptr());
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator==(IT1 a, IT2 b) noexcept
{
    return a.ptr() == b.ptr();
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator!=(IT1 a, IT2 b) noexcept
{
    return a.ptr() != b.ptr();
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator<(IT1 a, IT2 b) noexcept
{
    return a.ptr() < b.ptr();
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator>(IT1 a, IT2 b) noexcept
{
    return a.ptr() > b.ptr();
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator<=(IT1 a, IT2 b) noexcept
{
    return a.ptr() <= b.ptr();
}

template <typename IT1, typename IT2, VX_REQUIRES(is_compatible_pointer_iterator<IT1, IT2>::value)>
constexpr bool operator>=(IT1 a, IT2 b) noexcept
{
    return a.ptr() >= b.ptr();
}

//==============================================================================
// back inserter
//==============================================================================

template <typename T>
class back_insert_iterator
{
public:

    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;

    using m_container_type = T;
    using difference_type = ptrdiff_t;

    constexpr explicit back_insert_iterator(T& c) noexcept
        : m_container(std::addressof(c))
    {}

    constexpr back_insert_iterator& operator=(const typename T::value_type& v)
    {
        m_container->push_back(v);
        return *this;
    }

    constexpr back_insert_iterator& operator=(typename T::value_type&& v)
    {
        m_container->push_back(std::move(v));
        return *this;
    }

    _NODISCARD constexpr back_insert_iterator& operator*() noexcept
    {
        return *this;
    }

    constexpr back_insert_iterator& operator++() noexcept
    {
        return *this;
    }

    constexpr back_insert_iterator operator++(int) noexcept
    {
        return *this;
    }

protected:

    T* m_container;
};

template <typename T>
VX_NO_DISCARD constexpr back_insert_iterator<T> back_inserter(T& c) noexcept
{
    return back_insert_iterator<T>(c);
}

} // namespace vx
