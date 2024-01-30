#pragma once

#include <iterator>

#include "pixel.h"
#include "vertex/math/math/detail/vec2f_type.h"

namespace vx {
namespace img {
namespace detail {

template <typename image_type, typename T>
class iterator
{
private:

    using other_iterator = iterator<
        image_type,
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

    inline constexpr iterator(pointer ptr, const image_type& image, size_type x, size_type y) noexcept
        : m_current(ptr), m_image(image), m_x(x), m_y(y) {}

    inline constexpr iterator(const iterator<image_type, const T>& other) noexcept
        : m_current(other.m_current), m_image(other.m_image), m_x(other.m_x), m_y(other.m_y) {}

    inline constexpr iterator(const iterator<image_type, typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current), m_image(other.m_image), m_x(other.m_x), m_y(other.m_y) {}

    inline constexpr iterator(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    // =============== assignment ===============

    inline constexpr iterator& operator=(const iterator<image_type, const T>& other) noexcept
    {
        m_current = other.m_current;
        m_image = other.m_image;
        m_x = other.m_x;
        m_y = other.m_y;
        return *this;
    }

    inline constexpr iterator& operator=(const iterator<image_type, typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        m_image = other.m_image;
        m_x = other.m_x;
        m_y = other.m_y;
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
        m_current += m_image.pixel_size();
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
        m_current += (i * m_image.pixel_size());
        return *this;
    }

    friend inline constexpr iterator operator+(const iterator& it, difference_type i)
    {
        iterator result = it;
        result += (i * m_image.pixel_size());
        return result;
    }

    friend inline constexpr iterator operator+(difference_type i, const iterator& it)
    {
        iterator result = it;
        result += (i * m_image.pixel_size());
        return result;
    }

    // subtraction (-)

    inline constexpr iterator& operator--()
    {
        m_current -= m_image.pixel_size();
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
        m_current -= (i * m_image.pixel_size());
        return *this;
    }

    friend inline constexpr iterator operator-(const iterator& it, difference_type i)
    {
        iterator result = it;
        result -= (i * m_image.pixel_size());
        return result;
    }

    friend inline constexpr iterator operator-(difference_type i, const iterator& it)
    {
        iterator result = it;
        result -= (i * m_image.pixel_size());
        return result;
    }

    // comparison

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator==(const IT & other) const
    {
        return m_current == other.m_current && m_image == other.m_image && m_x == other.m_x && m_y == other.m_y;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
    {
        return !(*this == other);
    }

private:

    pointer m_current = nullptr;
    image_type* m_image = nullptr;
    size_type m_x = 0;
    size_type m_y = 0;

};

}
}
}