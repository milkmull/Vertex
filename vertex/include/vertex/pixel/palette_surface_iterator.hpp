#pragma once

#include <iterator>
#include <iostream>

#include "vertex/pixel/raw_palette_block.hpp"
#include "vertex/math/core/types.hpp"

namespace vx {
namespace pixel {

template <palette_format F>
class const_palette_surface_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr palette_format format = F;
    using block_type = raw_palette_block<format>;
    using proxy_type = palette_pixel_proxy<format>;

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = const proxy_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const block_type*;
    using reference = value_type;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    const_palette_surface_iterator() noexcept = default;

    explicit const_palette_surface_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height, const palette* pal) noexcept
        : m_ptr(ptr), m_x(x), m_y(y), m_w(width), m_h(height), m_pal(pal) {}

    const_palette_surface_iterator(const const_palette_surface_iterator&) noexcept = default;
    const_palette_surface_iterator& operator=(const const_palette_surface_iterator&) noexcept = default;

    const_palette_surface_iterator(const_palette_surface_iterator&&) noexcept = default;
    const_palette_surface_iterator& operator=(const_palette_surface_iterator&&) noexcept = default;

private:

    ///////////////////////////////////////////////////////////////////////////////
    // helpers
    ///////////////////////////////////////////////////////////////////////////////

    size_t index() const noexcept
    {
        constexpr size_t sub_count = get_sub_index_count(format);
        const size_t offset = m_y * m_w + m_x;
        return offset / sub_count;
    }

    size_t sub_index() const noexcept
    {
        constexpr size_t sub_count = get_sub_index_count(format);
        const size_t offset = m_y * m_w + m_x;
        return offset % sub_count;
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    reference operator*() const noexcept
    {
        return proxy_type(*m_ptr, sub_index(), m_pal);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    const_palette_surface_iterator& operator++() noexcept
    {
        if (sub_index() + 1 == get_sub_index_count(format))
        {
            ++m_ptr;
        }

        if (++m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

        return *this;
    }

    const_palette_surface_iterator operator++(int) noexcept
    {
        const_palette_surface_iterator result = *this;
        ++(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const const_palette_surface_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr
            && m_x == other.m_x && m_y == other.m_y
            && m_w == other.m_w && m_h == other.m_h
            && m_pal == other.m_pal;
    }

    bool operator!=(const const_palette_surface_iterator& other) const noexcept
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

    size_t x() const noexcept { return m_x; }
    size_t y() const noexcept { return m_y; }

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
    const palette* m_pal = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

template <palette_format F>
class palette_surface_iterator
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr palette_format format = F;
    using block_type = raw_palette_block<format>;
    using proxy_type = palette_pixel_proxy<format>;

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = proxy_type;
    using difference_type = std::ptrdiff_t;
    using pointer = block_type*;
    using reference = value_type;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    palette_surface_iterator() noexcept = default;

    explicit palette_surface_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height, const palette* pal) noexcept
        : m_ptr(ptr), m_x(x), m_y(y), m_w(width), m_h(height), m_pal(pal) {}

    palette_surface_iterator(const palette_surface_iterator&) noexcept = default;
    palette_surface_iterator& operator=(const palette_surface_iterator&) noexcept = default;

    palette_surface_iterator(palette_surface_iterator&&) noexcept = default;
    palette_surface_iterator& operator=(palette_surface_iterator&&) noexcept = default;

private:

    ///////////////////////////////////////////////////////////////////////////////
    // helpers
    ///////////////////////////////////////////////////////////////////////////////

    size_t index() const noexcept
    {
        constexpr size_t sub_count = get_sub_index_count(format);
        const size_t offset = m_y * m_w + m_x;
        return offset / sub_count;
    }

    size_t sub_index() const noexcept
    {
        constexpr size_t sub_count = get_sub_index_count(format);
        const size_t offset = m_y * m_w + m_x;
        return offset % sub_count;
    }

public:

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operator
    ///////////////////////////////////////////////////////////////////////////////

    operator const_palette_surface_iterator<F>() const noexcept
    {
        return const_palette_surface_iterator<F>(m_ptr, m_x, m_y, m_w, m_h, m_pal);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

    reference operator*() const noexcept
    {
        return proxy_type(*m_ptr, sub_index(), m_pal);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    palette_surface_iterator& operator++() noexcept
    {
        if (sub_index() + 1 == get_sub_index_count(format))
        {
            ++m_ptr;
        }

        if (++m_x >= m_w)
        {
            m_x = 0;
            ++m_y;
        }

        return *this;
    }

    palette_surface_iterator operator++(int) noexcept
    {
        palette_surface_iterator temp = *this;
        ++(*this);
        return temp;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const palette_surface_iterator& other) const noexcept
    {
        return m_ptr == other.m_ptr
            && m_x == other.m_x && m_y == other.m_y
            && m_w == other.m_w && m_h == other.m_h
            && m_pal == other.m_pal;
    }

    bool operator!=(const palette_surface_iterator& other) const noexcept
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

    size_t x() const noexcept { return m_x; }
    size_t y() const noexcept { return m_y; }

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

    void set_pixel(size_t index) noexcept
    {
        operator*() = index;
    }

    void set_pixel(const math::color& color) noexcept
    {
        operator*() = color;
    }

private:

    pointer m_ptr = nullptr;
    size_t m_x = 0, m_y = 0;
    size_t m_w = 0, m_h = 0;
    const palette* m_pal = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
// begin/end functions
///////////////////////////////////////////////////////////////////////////////

template <palette_format F>
class palette_surface;

template <palette_format F>
inline palette_surface_iterator<F> begin(palette_surface<F>& surf) noexcept
{
    return palette_surface_iterator<F>(reinterpret_cast<typename palette_surface_iterator<F>::pointer>(surf.data()), 0, 0, surf.width(), surf.height(), &surf.get_palette());
}

template <palette_format F>
inline palette_surface_iterator<F> end(palette_surface<F>& surf) noexcept
{
    return palette_surface_iterator<F>(reinterpret_cast<typename palette_surface_iterator<F>::pointer>(surf.data()) + surf.block_count(), 0, surf.height(), surf.width(), surf.height(), &surf.get_palette());
}

template <palette_format F>
inline const_palette_surface_iterator<F> begin(const palette_surface<F>& surf) noexcept
{
    return const_palette_surface_iterator<F>(reinterpret_cast<typename const_palette_surface_iterator<F>::pointer>(surf.data()), 0, 0, surf.width(), surf.height(), &surf.get_palette());
}

template <palette_format F>
const_palette_surface_iterator<F> end(const palette_surface<F>& surf) noexcept
{
    return const_palette_surface_iterator<F>(reinterpret_cast<typename const_palette_surface_iterator<F>::pointer>(surf.data()) + surf.block_count(), 0, surf.height(), surf.width(), surf.height(), &surf.get_palette());
}

} // namespace pixel
} // namespace vx