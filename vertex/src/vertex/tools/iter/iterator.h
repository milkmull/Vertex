#pragma once

#include <iterator>

namespace vx {
namespace tools {

template <typename T>
class iterator
{
private:

    using other_iterator = iterator<
        typename std::conditional<std::is_const<T>::value, typename std::remove_const<T>::type, const T>::type
    >;

public:

    // =============== iterator traits ===============

    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

public:

    // =============== constructors and destructor ===============

    inline constexpr iterator() = default;

    inline constexpr iterator(pointer ptr) noexcept
        : m_current(ptr) {}

    inline constexpr iterator(const iterator<const T>& other) noexcept
        : m_current(other.m_current) {}

    inline constexpr iterator(const iterator<typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current) {}

    inline constexpr iterator(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    // =============== assignment ===============

    inline constexpr iterator& operator=(const iterator<const T>& other) noexcept
    {
        m_current = other.m_current;
        return *this;
    }

    inline constexpr iterator& operator=(const iterator<typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        return *this;
    }

    inline constexpr iterator& operator=(iterator&&) noexcept = default;

    // =============== operators ===============

    inline constexpr reference operator*() const
    {
        return *m_current;
    }

    inline constexpr pointer operator->() const
    {
        return m_current;
    }

    inline constexpr reference operator[](difference_type i) const
    {
        return m_current[i];
    }

    // addition (+)

    inline constexpr iterator& operator++()
    {
        ++m_current;
        return *this;
    }

    inline constexpr iterator operator++(int)
    {
        iterator result = *this;
        ++(*this);
        return result;
    }

    inline constexpr iterator& operator+=(difference_type i)
    {
        m_current += i;
        return *this;
    }

    friend inline constexpr iterator operator+(const iterator& it, difference_type i)
    {
        iterator result = it;
        result += i;
        return result;
    }

    friend inline constexpr iterator operator+(difference_type i, const iterator& it)
    {
        iterator result = it;
        result += i;
        return result;
    }

    // subtraction (-)

    inline constexpr iterator& operator--()
    {
        --m_current;
        return *this;
    }

    inline constexpr iterator operator--(int)
    {
        iterator result = *this;
        --(*this);
        return result;
    }

    inline constexpr iterator& operator-=(difference_type i)
    {
        m_current -= i;
        return *this;
    }

    friend inline constexpr iterator operator-(const iterator& it, difference_type i)
    {
        iterator result = it;
        result -= i;
        return result;
    }

    friend inline constexpr iterator operator-(difference_type i, const iterator& it)
    {
        iterator result = it;
        result -= i;
        return result;
    }

    // comparison

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator==(const IT & other) const
    {
        return m_current == other.m_current;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
    {
        return !(*this == other);
    }

private:

    pointer m_current = nullptr;

};

}
}