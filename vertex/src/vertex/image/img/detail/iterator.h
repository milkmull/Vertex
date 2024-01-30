#pragma once

#include <iterator>

#include "base_type_defs.h"
#include "pixel.h"
#include "vertex/math/math/detail/vec2f_type.h"

namespace vx {
namespace img {
namespace detail {

template <typename T>
class iterator
{
private:

    using other_iterator = iterator<
        typename std::conditional<std::is_const<T>::value, typename std::remove_const<T>::type, const T>::type
    >;

public:

    // =============== iterator traits ===============

    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

public:

    // =============== constructors and destructor ===============

    inline constexpr iterator() = default;

    inline constexpr iterator(pointer ptr, size_type x, size_type y, size_type width, size_type height) noexcept
        : m_current(ptr), m_x(x), m_y(y), m_width(width), m_height(height) {}

    inline constexpr iterator(const iterator<const T>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_width(other.width), m_height(other.height) {}

    inline constexpr iterator(const iterator<typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_width(other.width), m_height(other.height) {}

    inline constexpr iterator(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    // =============== assignment ===============

    inline constexpr iterator& operator=(const iterator<const T>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_width = other.m_width;
        m_height = other.m_height;
        return *this;
    }

    inline constexpr iterator& operator=(const iterator<typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_width = other.m_width;
        m_height = other.m_height;
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
        m_x = (m_x + 1) % m_width;
        if (m_x == 0) ++m_y;
        return *this;
    }

    inline constexpr iterator operator++(int)
    {
        iterator result = *this;
        ++(*this);
        return result;
    }

    // comparison

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator==(const IT & other) const
    {
        return m_current == other.m_current && m_x == other.m_x && m_y == other.m_y && m_width == other.m_width && m_height == other.m_height;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
    {
        return !(*this == other);
    }

    // extra

    inline constexpr math::vec2 resolution() const
    {
        return math::vec2(m_width, m_height);
    }

    inline constexpr math::color color() const
    {
        return static_cast<math::color>(operator*());
    }

    inline constexpr math::vec2 coord() const
    {
        return math::vec2(m_x, m_y);
    }

    inline constexpr math::vec2 frag_coord() const
    {
        return math::vec2(
            static_cast<float>(m_x) / static_cast<float>(m_width),
            static_cast<float>(m_y) / static_cast<float>(m_height)
        );
    }

    inline constexpr math::vec2 local() const
    {
        return coord() - resolution() * 0.5f;
    }

private:

    pointer m_current = nullptr;
    size_type m_x = 0;
    size_type m_y = 0;
    size_type m_width = 0;
    size_type m_height = 0;

};

}
}
}