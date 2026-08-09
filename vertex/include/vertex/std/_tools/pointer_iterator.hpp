#pragma once

#include <iterator>

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace _priv {

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
// pointer_iterator_base
//
// Holds every operation that does NOT depend on Owner. This means the
// compiler only has to instantiate one copy of this logic per T (really per
// cv-qualification of T), instead of once per (Owner, T) pair. pointer_iterator
// below is a thin, owner-tagged shell over this.
//==============================================================================

template <typename T>
class pointer_iterator_base
{
public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    pointer_iterator_base() = default;

    explicit pointer_iterator_base(pointer p) noexcept
        : m_ptr(p)
    {}

    reference operator*() const noexcept
    {
        return *m_ptr;
    }

    pointer operator->() const noexcept
    {
        return m_ptr;
    }

    pointer_iterator_base& operator++() noexcept
    {
        ++m_ptr;
        return *this;
    }

    pointer_iterator_base& operator--() noexcept
    {
        --m_ptr;
        return *this;
    }

    pointer_iterator_base& operator+=(difference_type n) noexcept
    {
        m_ptr += n;
        return *this;
    }

    pointer_iterator_base& operator-=(difference_type n) noexcept
    {
        m_ptr -= n;
        return *this;
    }

    // Kept here (not owner-tagged) purely so operator[] can use it internally.
    pointer_iterator_base operator+(difference_type n) const noexcept
    {
        return pointer_iterator_base(m_ptr + n);
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

protected:

    pointer m_ptr = nullptr;
};

//==============================================================================
// pointer_iterator
//
// Owner is a pure phantom tag: it never affects m_ptr, arithmetic, or
// dereferencing, only which iterators are allowed to compare/convert with
// each other. Everything that actually generates code lives in
// pointer_iterator_base<T> above and is shared across every Owner.
//==============================================================================

template <typename Owner, typename T>
class pointer_iterator : public pointer_iterator_base<T>
{
    using base = pointer_iterator_base<T>;

public:

    using typename base::difference_type;
    using typename base::iterator_category;
    using typename base::pointer;
    using typename base::reference;
    using typename base::value_type;

    pointer_iterator() = default;

    explicit pointer_iterator(pointer p) noexcept
        : base(p)
    {}

    template <typename U, VX_REQUIRES((std::is_convertible<U, T>::value))>
    pointer_iterator(const pointer_iterator<Owner, U>& other) noexcept
        : base(other.ptr())
    {}

    // Every operator below is a thin forwarder to pointer_iterator_base<T>'s
    // logic. We deliberately do NOT use "using base::operator++;" style
    // un-hiding here: declaring operator++(int) hides all base overloads of
    // operator++ (C++ name hiding), and pulling them back via a using-
    // declaration from a dependent template base is a spec-compliant but
    // MSVC-unfriendly pattern that has failed to un-hide operators in some
    // MSVC versions. Explicit forwarders sidestep that entirely and are
    // portable everywhere.

    pointer_iterator& operator++() noexcept
    {
        base::operator++();
        return *this;
    }

    pointer_iterator& operator--() noexcept
    {
        base::operator--();
        return *this;
    }

    pointer_iterator& operator+=(difference_type n) noexcept
    {
        base::operator+=(n);
        return *this;
    }

    pointer_iterator& operator-=(difference_type n) noexcept
    {
        base::operator-=(n);
        return *this;
    }

    pointer_iterator operator++(int) noexcept
    {
        pointer_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    pointer_iterator operator--(int) noexcept
    {
        pointer_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    // Hides base::operator+ so that it+n stays owner-tagged.
    pointer_iterator operator+(difference_type n) const noexcept
    {
        return pointer_iterator(this->ptr() + n);
    }

    pointer_iterator operator-(difference_type n) const noexcept
    {
        return pointer_iterator(this->ptr() - n);
    }
};

//==============================================================================

template <typename Owner, typename T>
constexpr pointer_iterator<Owner, T> operator+(
    typename pointer_iterator<Owner, T>::difference_type n,
    pointer_iterator<Owner, T> it) noexcept
{
    return it + n;
}

//==============================================================================
// reverse_pointer_iterator_base
//
// Same idea as pointer_iterator_base: every operation that only needs T (not
// the full owner-tagged IT) lives here, shared across every IT that wraps
// the same T.
//==============================================================================

template <typename T>
class reverse_pointer_iterator_base
{
public:

    using base_it = pointer_iterator_base<T>;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    reverse_pointer_iterator_base() = default;

    explicit reverse_pointer_iterator_base(base_it it) noexcept
        : m_it(it)
    {}

    reference operator*() const noexcept
    {
        base_it tmp = m_it;
        return *--tmp;
    }

    pointer operator->() const noexcept
    {
        return std::addressof(operator*());
    }

    reverse_pointer_iterator_base& operator++() noexcept
    {
        --m_it;
        return *this;
    }

    reverse_pointer_iterator_base& operator--() noexcept
    {
        ++m_it;
        return *this;
    }

    reverse_pointer_iterator_base& operator+=(difference_type n) noexcept
    {
        m_it -= n;
        return *this;
    }

    reverse_pointer_iterator_base& operator-=(difference_type n) noexcept
    {
        m_it += n;
        return *this;
    }

    reference operator[](difference_type n) const noexcept
    {
        base_it tmp(m_it.ptr() - n);
        return *--tmp;
    }

    pointer ptr() const noexcept
    {
        return m_it.ptr();
    }

    explicit operator bool() const noexcept
    {
        return static_cast<bool>(m_it);
    }

protected:

    base_it m_it;
};

//==============================================================================
// reverse_pointer_iterator
//
// IT still varies per (Owner, T), but since IT is now itself a thin wrapper
// over pointer_iterator_base<T>, this shell only needs to carry that IT type
// through the conversion constructor and postfix ops; the arithmetic lives
// in reverse_pointer_iterator_base<T> and is shared across every Owner.
//==============================================================================

template <typename IT>
class reverse_pointer_iterator : public reverse_pointer_iterator_base<typename IT::value_type>
{
    using base = reverse_pointer_iterator_base<typename IT::value_type>;

public:

    using typename base::difference_type;
    using typename base::iterator_category;
    using typename base::pointer;
    using typename base::reference;
    using typename base::value_type;
    using base_it = typename base::base_it;

    reverse_pointer_iterator() = default;

    explicit reverse_pointer_iterator(IT it) noexcept
        : base(base_it(it.ptr()))
    {}

    template <typename IT2, VX_REQUIRES((std::is_convertible<IT2, IT>::value))>
    reverse_pointer_iterator(const reverse_pointer_iterator<IT2>& other) noexcept
        : base(base_it(other.ptr()))
    {}

    // See the comment in pointer_iterator: explicit forwarders instead of
    // "using base::operator++/--;", both for MSVC portability and because it
    // makes the direction of each op unambiguous at the call site (a
    // reverse iterator's ++ must call the base's ++, not its --).

    reverse_pointer_iterator& operator++() noexcept
    {
        base::operator++();
        return *this;
    }

    reverse_pointer_iterator& operator--() noexcept
    {
        base::operator--();
        return *this;
    }

    reverse_pointer_iterator& operator+=(difference_type n) noexcept
    {
        base::operator+=(n);
        return *this;
    }

    reverse_pointer_iterator& operator-=(difference_type n) noexcept
    {
        base::operator-=(n);
        return *this;
    }

    reverse_pointer_iterator operator++(int) noexcept
    {
        reverse_pointer_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    reverse_pointer_iterator operator--(int) noexcept
    {
        reverse_pointer_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    reverse_pointer_iterator operator+(difference_type n) const noexcept
    {
        return reverse_pointer_iterator(IT(this->ptr() - n));
    }

    reverse_pointer_iterator operator-(difference_type n) const noexcept
    {
        return reverse_pointer_iterator(IT(this->ptr() + n));
    }
};

//==============================================================================

template <typename IT>
constexpr reverse_pointer_iterator<IT> operator+(
    typename reverse_pointer_iterator<IT>::difference_type n,
    reverse_pointer_iterator<IT> it) noexcept
{
    return it + n;
}

//==============================================================================
// general operators
//
// Unchanged: these already operate purely through .ptr(), so they add no
// extra bloat beyond the IT1/IT2 pairs actually used.
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

} // namespace _priv
} // namespace vx
