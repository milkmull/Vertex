#pragma once

#include <iterator>

#include "vertex/pixel/raw_pixel.hpp"
#include "vertex/math/core/types.hpp"

namespace vx {
namespace pixel {

template <pixel_format F>
class const_pixel_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr pixel_format format = F;
    using raw_pixel_type = typename raw_pixel<format>;

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = raw_pixel_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const raw_pixel_type*;
    using reference = const value_type&;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    const_pixel_iterator() noexcept = default;

    explicit const_pixel_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height) noexcept
        : m_ptr(ptr), m_x(x), m_y(y), m_w(width), m_h(height) {}

    const_pixel_iterator(const const_pixel_iterator&) noexcept = default;
    const_pixel_iterator& operator=(const const_pixel_iterator&) noexcept = default;

    const_pixel_iterator(const_pixel_iterator&&) noexcept = default;
    const_pixel_iterator& operator=(const_pixel_iterator&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    reference operator*() const noexcept
    {
        return *m_ptr;
    }

    pointer operator->() const noexcept
    {
        return m_ptr;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    const_pixel_iterator& operator++() noexcept
    {
        ++m_ptr;

        if (++m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

        return *this;
    }

    const_pixel_iterator operator++(int) noexcept
    {
        const_pixel_iterator result = *this;
        ++(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const const_pixel_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr
            && m_x == other.m_x && m_y == other.m_y
            && m_w == other.m_w && m_h == other.m_h;
    }

    bool operator!=(const const_pixel_iterator& other) const noexcept
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // extra stuff
    ///////////////////////////////////////////////////////////////////////////////

    math::vec2 resolution() const noexcept
    {
        return math::vec2(m_w, m_h);
    }

    math::color color() const noexcept
    {
        return static_cast<math::color>(operator*());
    }

    math::vec2i position() const noexcept
    {
        return math::vec2i(m_x, m_y);
    }

    math::vec2 xy() const noexcept
    {
        return math::vec2(m_x, m_y);
    }

    math::vec2 uv() const noexcept
    {
        return xy() / resolution();
    }

    math::vec2 local() const noexcept
    {
        return xy() - resolution() * 0.5f;
    }

protected:

    pointer m_ptr = nullptr;
    size_t m_x = 0, m_y = 0;
    size_t m_w = 0, m_h = 0;
};

///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
class pixel_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr pixel_format format = F;
    using raw_pixel_type = typename raw_pixel<format>;

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = raw_pixel_type;
    using difference_type = std::ptrdiff_t;
    using pointer = raw_pixel_type*;
    using reference = value_type&;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    pixel_iterator() noexcept = default;

    explicit pixel_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height) noexcept
        : m_ptr(ptr), m_x(x), m_y(y), m_w(width), m_h(height) {}

    pixel_iterator(const pixel_iterator&) noexcept = default;
    pixel_iterator& operator=(const pixel_iterator&) noexcept = default;

    pixel_iterator(pixel_iterator&&) noexcept = default;
    pixel_iterator& operator=(pixel_iterator&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operator
    ///////////////////////////////////////////////////////////////////////////////

    operator const_pixel_iterator<F>() const noexcept
    {
        return const_pixel_iterator<F>(m_ptr, m_x, m_y, m_w, m_h);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    reference operator*() const noexcept
    {
        return *m_ptr;
    }

    pointer operator->() const noexcept
    {
        return m_ptr;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    pixel_iterator& operator++() noexcept
    {
        ++m_ptr;

        if (++m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

        return *this;
    }

    pixel_iterator operator++(int) noexcept
    {
        pixel_iterator temp = *this;
        ++(*this);
        return temp;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const pixel_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr
            && m_x == other.m_x && m_y == other.m_y
            && m_w == other.m_w && m_h == other.m_h;
    }

    bool operator!=(const pixel_iterator& other) const noexcept
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // extra stuff
    ///////////////////////////////////////////////////////////////////////////////

    math::vec2 resolution() const noexcept
    {
        return math::vec2(m_w, m_h);
    }

    math::color color() const noexcept
    {
        return static_cast<math::color>(operator*());
    }

    math::vec2i position() const noexcept
    {
        return math::vec2i(m_x, m_y);
    }

    math::vec2 xy() const noexcept
    {
        return math::vec2(m_x, m_y);
    }

    math::vec2 uv() const noexcept
    {
        return xy() / resolution();
    }

    math::vec2 local() const noexcept
    {
        return xy() - resolution() * 0.5f;
    }

    void set_pixel(const math::color& color) noexcept
    {
        operator*() = static_cast<raw_pixel_type>(color);
    }

private:

    pointer m_ptr = nullptr;
    size_t m_x = 0, m_y = 0;
    size_t m_w = 0, m_h = 0;
};

///////////////////////////////////////////////////////////////////////////////
// begin/end functions
///////////////////////////////////////////////////////////////////////////////

template <pixel_format F>
class surface;

template <pixel_format F>
inline pixel_iterator<F> begin(surface<F>& surf) noexcept
{
    return pixel_iterator<F>(reinterpret_cast<typename pixel_iterator<F>::pointer>(surf.data()), 0, 0, surf.width(), surf.height());
}

template <pixel_format F>
inline pixel_iterator<F> end(surface<F>& surf) noexcept
{
    return pixel_iterator<F>(reinterpret_cast<typename pixel_iterator<F>::pointer>(surf.data()) + surf.width() * surf.height(), 0, surf.height(), surf.width(), surf.height());
}

template <pixel_format F>
inline const_pixel_iterator<F> begin(const surface<F>& surf) noexcept
{
    return const_pixel_iterator<F>(reinterpret_cast<typename const_pixel_iterator<F>::pointer>(surf.data()), 0, 0, surf.width(), surf.height());
}

template <pixel_format F>
const_pixel_iterator<F> end(const surface<F>& surf) noexcept
{
    return const_pixel_iterator<F>(reinterpret_cast<typename const_pixel_iterator<F>::pointer>(surf.data()) + surf.width() * surf.height(), 0, surf.height(), surf.width(), surf.height());
}

} // namespace pixel
} // namespace vx