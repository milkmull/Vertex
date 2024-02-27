#pragma once

#include <iterator>

#include "detail/base_type_defs.h"
#include "pixel.h"

namespace vx {
namespace img {

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
        : m_current(ptr), m_x(x), m_y(y), m_w(width), m_h(height) {}

    inline constexpr iterator(const iterator<const T>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_w(other.m_w), m_h(other.m_h) {}

    inline constexpr iterator(const iterator<typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_w(other.m_w), m_h(other.m_h) {}

    inline constexpr iterator(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    // =============== assignment ===============

    inline constexpr iterator& operator=(const iterator<const T>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_w = other.m_w;
        m_h = other.m_h;
        return *this;
    }

    inline constexpr iterator& operator=(const iterator<typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_w = other.m_w;
        m_h = other.m_h;
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

    // addition (++)

    inline constexpr iterator& operator++()
    {
        ++m_current;

        ++m_x;
        if (m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

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
        return m_current == other.m_current && m_x == other.m_x && m_y == other.m_y && m_w == other.m_w && m_h == other.m_h;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
    {
        return !(*this == other);
    }

    // extra

    inline constexpr size_type x() const { return m_x; }
    inline constexpr size_type y() const { return m_y; }

    inline constexpr math::vec2 resolution() const
    {
        return math::vec2(m_w, m_h);
    }

    inline constexpr math::color color() const
    {
        return static_cast<math::color>(operator*());
    }

    inline constexpr math::vec2i position() const
    {
        return math::vec2i(m_x, m_y);
    }

    inline constexpr math::vec2 coord() const
    {
        return math::vec2(m_x, m_y);
    }

    inline constexpr math::vec2 frag_coord() const
    {
        return math::vec2(
            static_cast<float>(m_x) / static_cast<float>(m_w),
            static_cast<float>(m_y) / static_cast<float>(m_h)
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

    size_type m_w = 0;
    size_type m_h = 0;

};

// =============== iterator ===============

class image;

template <typename T>
inline constexpr auto begin(image& i)
{
    return iterator<T>((T*)i.raw_data(), 0, 0, i.width(), i.height());
}

template <typename T>
auto cbegin(const image& i) noexcept
{
    return iterator<typename std::add_const<T>::type>((const T*)i.raw_data(), 0, 0, i.width(), i.height());
}

template <typename T>
auto begin(const image& i) noexcept
{
    return cbegin<T>(i);
}

template <typename T>
inline constexpr auto end(image& i)
{
    return iterator<T>((T*)(i.raw_data() + i.data().size()), 0, i.height(), i.width(), i.height());
}

template <typename T>
auto cend(const image& i) noexcept
{
    return iterator<typename std::add_const<T>::type>((const T*)(i.raw_data() + i.data().size()), 0, i.height(), i.width(), i.height());
}

template <typename T>
auto end(const image& i) noexcept
{
    return cend<T>(i);
}

}
}