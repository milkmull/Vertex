#pragma once

#include <iterator>

#include "pixel_color.h"
#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace pixel {

class const_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = math::color;
    using difference_type = std::ptrdiff_t;
    using pointer = const byte_type*;
    using reference = value_type;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors and destructor
    ///////////////////////////////////////////////////////////////////////////////

    inline const_iterator() = default;

    inline const_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height, pixel_format format) noexcept
        : m_ptr(ptr), m_x(x), m_y(y), m_w(width), m_h(height), m_format(format) {}

    inline const_iterator(const const_iterator&) noexcept = default;
    inline const_iterator(const_iterator&&) noexcept = default;
    ~const_iterator() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline const_iterator& operator=(const const_iterator& other) noexcept = default;
    inline const_iterator& operator=(const_iterator&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    inline pointer operator*() const
    {
        return m_ptr;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline const_iterator& operator++()
    {
        m_ptr += get_pixel_size(m_format);

        if (++m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

        return *this;
    }

    inline const_iterator operator++(int)
    {
        const_iterator result = *this;
        ++(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    inline bool operator==(const const_iterator& other) const
    {
        return m_ptr == other.m_ptr
            && m_x == other.m_x
            && m_y == other.m_y
            && m_w == other.m_w
            && m_h == other.m_h
            && m_format == other.m_format;
    }

    inline bool operator!=(const const_iterator& other) const
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // extra stuff
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the resolution of the image.
    ///
    /// @return A vector representing the resolution of the image (width, height).
    ///////////////////////////////////////////////////////////////////////////////
    inline math::vec2 resolution() const
    {
        return math::vec2(m_w, m_h);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the color of the current pixel.
    ///
    /// @return The color of the current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline math::color color() const
    {
        return pixel_to_color(m_ptr, m_format);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the integer position of the current pixel.
    ///
    /// @return A vector representing the integer position of the current pixel
    /// (x, y).
    ///////////////////////////////////////////////////////////////////////////////
    inline math::vec2i position() const
    {
        return math::vec2i(m_x, m_y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the floating point position of the current pixel.
    ///
    /// @return A vector representing the floating point position of the current
    /// pixel (x, y).
    ///////////////////////////////////////////////////////////////////////////////
    inline math::vec2 xy() const
    {
        return math::vec2(m_x, m_y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the normalized coordinates (uv) of the current pixel.
    ///
    /// @return A vector representing the normalized coordinates (uv) of the
    /// current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline math::vec2 uv() const
    {
        return xy() / resolution();
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the floating point coordinates of the current pixel relative
    /// to the center of the image.
    /// 
    /// @return A vector representing the local coordinates of the current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline math::vec2 local() const
    {
        return xy() - resolution() * 0.5f;
    }

protected:

    pointer m_ptr;

    size_t m_x;
    size_t m_y;

    size_t m_w;
    size_t m_h;

    pixel_format m_format;
};

class iterator : public const_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = math::color;
    using difference_type = std::ptrdiff_t;
    using pointer = byte_type*;
    using reference = value_type;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors and destructor
    ///////////////////////////////////////////////////////////////////////////////

    inline iterator() = default;

    inline iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height, pixel_format format) noexcept
        : const_iterator(ptr, x, y, width, height, format) {}

    inline iterator(const iterator&) noexcept = default;
    inline iterator(iterator&&) noexcept = default;
    ~iterator() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline iterator& operator=(const iterator& other) noexcept = default;
    inline iterator& operator=(iterator&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operator
    ///////////////////////////////////////////////////////////////////////////////

    inline operator const_iterator() const noexcept
    {
        return const_iterator(m_ptr, m_x, m_y, m_w, m_h, m_format);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    inline pointer operator*() const
    {
        return const_cast<pointer>(const_iterator::operator*());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline iterator& operator++()
    {
        const_iterator::operator++();
        return *this;
    }

    inline iterator operator++(int)
    {
        iterator result = *this;
        ++(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel modification
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the color of the current pixel.
    ///
    /// @param color The color to set for the current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline void set_pixel(const math::color& color)
    {
        color_to_pixel(color, const_cast<pointer>(m_ptr), m_format);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    inline bool operator==(const iterator& other) const
    {
        return const_iterator::operator==(other);
    }

    inline bool operator!=(const iterator& other) const
    {
        return !(*this == other);
    }
};

}
}