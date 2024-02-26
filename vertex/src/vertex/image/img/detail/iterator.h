#pragma once

#include <iterator>

#include "base_type_defs.h"
#include "../pixel.h"
#include "vertex/math/geometry/type/rect_type.h"

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

    inline constexpr iterator(pointer ptr, const math::rectu& area) noexcept
        : m_current(ptr), m_area(area), m_position(area.position) {}

    inline constexpr iterator(const iterator<const T>& other) noexcept
        : m_current(other.m_current), m_area(other.m_area), m_position(other.m_position) {}

    inline constexpr iterator(const iterator<typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current), m_area(other.m_area), m_position(other.m_position) {}

    inline constexpr iterator(iterator&&) noexcept = default;

    ~iterator() noexcept = default;

    // =============== assignment ===============

    inline constexpr iterator& operator=(const iterator<const T>& other) noexcept
    {
        m_current = other.m_current;
        m_area = other.m_area;
        m_position = other.m_position;
        return *this;
    }

    inline constexpr iterator& operator=(const iterator<typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        m_area = other.m_area;
        m_position = other.m_position;
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
        assert(m_position.y < m_area.position.y + m_area.size.y);

        ++m_current;
        ++m_position;

        if (m_position >= m_area.position.x + m_area.size.x)
        {
            m_position.x = m_area.position.x;
            ++m_position.y;
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
        return m_current == other.m_current && m_area = other.m_area && m_position == other.m_position;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
    {
        return !(*this == other);
    }

    // extra

    inline constexpr math::vec2 resolution() const
    {
        return math::vec2(m_area.size);
    }

    inline constexpr math::color color() const
    {
        return static_cast<math::color>(operator*());
    }

    inline constexpr math::vec2 coord() const
    {
        return math::vec2(m_position);
    }

    inline constexpr math::vec2 frag_coord() const
    {
        return math::vec2(m_position - m_area.position) / math::vec2(m_area.size);
    }

    inline constexpr math::vec2 local() const
    {
        return coord() - resolution() * 0.5f;
    }

private:

    pointer m_current = nullptr;
    math::rectu m_area;
    math::vec2u m_position;

};

}
}
}