#pragma once

#include <iterator>

#include "vertex/std/_tools/pointer_iterator.hpp"

namespace vx {

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

//==============================================================================
// move iterator
//==============================================================================

template <typename IT>
class move_iterator
{
public:

    using iterator_type = IT;
    using iterator_category = typename std::iterator_traits<IT>::iterator_category;
    using value_type = typename std::iterator_traits<IT>::value_type;
    using difference_type = typename std::iterator_traits<IT>::difference_type;
    using pointer = IT;
    using reference = value_type&&;

    move_iterator() = default;

    explicit move_iterator(IT it) noexcept
        : m_it(it)
    {}

    template <typename IT2>
    move_iterator(const move_iterator<IT2>& other) noexcept
        : m_it(other.base())
    {}

    iterator_type base() const noexcept
    {
        return m_it;
    }

    reference operator*() const
    {
        return static_cast<reference>(*m_it);
    }

    pointer operator->() const noexcept
    {
        return m_it;
    }

    reference operator[](difference_type n) const
    {
        return static_cast<reference>(m_it[n]);
    }

    move_iterator& operator++() noexcept
    {
        ++m_it;
        return *this;
    }

    move_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++m_it;
        return tmp;
    }

    move_iterator& operator--() noexcept
    {
        --m_it;
        return *this;
    }

    move_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        --m_it;
        return tmp;
    }

    move_iterator operator+(difference_type n) const noexcept
    {
        return move_iterator(m_it + n);
    }

    move_iterator& operator+=(difference_type n) noexcept
    {
        m_it += n;
        return *this;
    }

    move_iterator operator-(difference_type n) const noexcept
    {
        return move_iterator(m_it - n);
    }

    move_iterator& operator-=(difference_type n) noexcept
    {
        m_it -= n;
        return *this;
    }

private:

    IT m_it;
};

//==============================================================================

template <typename IT>
constexpr move_iterator<IT> operator+(
    typename move_iterator<IT>::difference_type n,
    const move_iterator<IT>& it) noexcept
{
    return it + n;
}

template <typename IT1, typename IT2>
constexpr auto operator-(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
    -> decltype(a.base() - b.base())
{
    return a.base() - b.base();
}

template <typename IT1, typename IT2>
constexpr bool operator==(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return a.base() == b.base();
}

template <typename IT1, typename IT2>
constexpr bool operator!=(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return !(a == b);
}

template <typename IT1, typename IT2>
constexpr bool operator<(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return a.base() < b.base();
}

template <typename IT1, typename IT2>
constexpr bool operator>(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return b < a;
}

template <typename IT1, typename IT2>
constexpr bool operator<=(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return !(b < a);
}

template <typename IT1, typename IT2>
constexpr bool operator>=(const move_iterator<IT1>& a, const move_iterator<IT2>& b) noexcept
{
    return !(a < b);
}

//==============================================================================

template <typename IT>
constexpr move_iterator<IT> make_move_iterator(IT it) noexcept
{
    return move_iterator<IT>(it);
}

//==============================================================================
// helpers
//==============================================================================

namespace _priv {

template <typename IT1, typename IT2>
constexpr bool assert_valid_iterator_range(const IT1& first, const IT2& last)
{
    VX_IF_CONSTEXPR (
        (std::is_pointer<IT1>::value && std::is_pointer<IT2>::value) ||
        (is_pointer_iterator<IT1>::value && is_pointer_iterator<IT2>::value))
    {
        // transposed pointer range
        return (first <= last);
    }
    else
    {
        return true;
    }
}

//==============================================================================

#define VX_PRIV_ASSERT_VALID_ITER_RANGE(first, last) \
    VX_ASSERT(::vx::_priv::assert_valid_iterator_range((first), (last)))

} // namespace _priv

} // namespace vx
