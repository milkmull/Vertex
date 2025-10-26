#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>

#include "vertex/math/rect.hpp"
#include "vertex/pixel/surface.hpp"
#include "vertex/pixel/palette_surface_iterator.hpp"

namespace vx {
namespace pixel {

template <palette_format F>
class palette_surface
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr palette_format format = F;

    static VX_FORCE_INLINE constexpr size_t channels() noexcept { return 4; }
    static VX_FORCE_INLINE constexpr bool has_alpha() noexcept { return true; }
    static VX_FORCE_INLINE constexpr size_t block_size() noexcept { return get_block_size(format); }
    static VX_FORCE_INLINE constexpr size_t palette_size() noexcept { return get_max_palette_size(format); }

    using block_type = raw_palette_block<format>;
    using proxy_type = palette_pixel_proxy<format>;
    using color_type = math::color;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    palette_surface() = default;

    palette_surface(size_t width, size_t height)
        : m_width(width)
        , m_height(height)
        , m_data(std::make_unique<block_type[]>(block_count()))
        , m_palette(palette_size())
    {
        std::memset(m_data.get(), 0, data_size());
    }

    palette_surface(size_t width, size_t height, const palette& pal)
        : m_width(width)
        , m_height(height)
        , m_data(std::make_unique<block_type[]>(block_count()))
        , m_palette(pal)
    {
        std::memset(m_data.get(), 0, data_size());
    }

    palette_surface(const palette_surface& other)
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_data(std::make_unique<block_type[]>(other.block_count()))
        , m_palette(other.m_palette)
    {
        std::memcpy(m_data.get(), other.m_data.get(), data_size());
    }

    palette_surface& operator=(const palette_surface& other)
    {
        if (this != &other)
        {
            m_width = other.m_width;
            m_height = other.m_height;

            m_data = std::make_unique<block_type[]>(other.block_count());
            std::memcpy(m_data.get(), other.m_data.get(), data_size());

            m_palette = other.m_palette;
        }
        return *this;
    }

    palette_surface(palette_surface&&) noexcept = default;
    palette_surface& operator=(palette_surface&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    size_t width() const noexcept { return m_width; }
    size_t height() const noexcept { return m_height; }
    size_t stride() const noexcept { return m_width * block_size(); }

    size_t block_count() const noexcept { return m_width * m_height / get_sub_index_count(format); }
    math::vec2i size() const noexcept { return math::vec2i(m_width, m_height); }
    math::recti get_rect() const noexcept { return math::recti(0, 0, m_width, m_height); }

    ///////////////////////////////////////////////////////////////////////////////
    // raw data
    ///////////////////////////////////////////////////////////////////////////////

    byte_type* data() noexcept { return reinterpret_cast<byte_type*>(m_data.get()); }
    const byte_type* data() const noexcept { return reinterpret_cast<const byte_type*>(m_data.get()); }
    size_t data_size() const noexcept { return block_count() * block_size(); }
    bool empty() const noexcept { return m_data == nullptr || m_width == 0 || m_height == 0; }

    ///////////////////////////////////////////////////////////////////////////////
    // palette
    ///////////////////////////////////////////////////////////////////////////////

    const palette& get_palette() const noexcept
    {
        return m_palette;
    }

    void set_palette_colors(const color_type* colors, size_t count) noexcept
    {
        m_palette.set_colors(colors, count);
    }

    void set_palette_color(size_t index, const color_type& color) noexcept
    {
        m_palette[index] = color;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel access
    ///////////////////////////////////////////////////////////////////////////////

private:

    void get_index_and_sub_index(size_t x, size_t y, size_t& index, size_t& sub_index) const noexcept
    {
        constexpr size_t sub_count = get_sub_index_count(format);
        const size_t offset = y * m_width + x;

        index = offset / sub_count;
        sub_index = offset % sub_count;
    }

public:

    proxy_type at(size_t x, size_t y) noexcept
    {
        size_t index, sub_index;
        get_index_and_sub_index(x, y, index, sub_index);
        return proxy_type(m_data[index], sub_index, &m_palette);
    }

    const proxy_type at(size_t x, size_t y) const noexcept
    {
        size_t index, sub_index;
        get_index_and_sub_index(x, y, index, sub_index);
        return proxy_type(m_data[index], sub_index, &m_palette);
    }

    size_t get_pixel_index(size_t x, size_t y, size_t default_index = 0) const noexcept
    {
        if (x >= m_width || y >= m_height)
        {
            return default_index;
        }

        return at(x, y).index();
    }

    size_t get_pixel_index(const math::vec2i& p, size_t default_index = 0) const noexcept
    {
        return get_pixel_index(
            static_cast<size_t>(p.x),
            static_cast<size_t>(p.y),
            default_index
        );
    }

    void set_pixel_index(size_t x, size_t y, size_t i) noexcept
    {
        if (x >= m_width || y >= m_height || i > m_palette.size())
        {
            return;
        }

        at(x, y) = i;
    }

    void set_pixel_index(const math::vec2i& p, size_t i) noexcept
    {
        set_pixel_index(static_cast<size_t>(p.x), static_cast<size_t>(p.y), i);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel color
    ///////////////////////////////////////////////////////////////////////////////

    math::color get_pixel(size_t x, size_t y, const math::color& default_color = math::color()) const noexcept
    {
        if (x >= m_width || y >= m_height)
        {
            return default_color;
        }

        return at(x, y).color();
    }

    math::color get_pixel(const math::vec2i& p, const math::color& default_color = math::color()) const noexcept
    {
        return get_pixel(
            static_cast<size_t>(p.x),
            static_cast<size_t>(p.y),
            default_color
        );
    }

    void set_pixel(size_t x, size_t y, const math::color& color, bool use_closest = true) noexcept
    {
        if (x >= m_width || y >= m_height)
        {
            return;
        }

        at(x, y) = color;
    }

    void set_pixel(const math::vec2i& p, const math::color& color, bool use_closest = true) noexcept
    {
        set_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), color, use_closest);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion
    ///////////////////////////////////////////////////////////////////////////////

    template <pixel_format F>
    surface<F> to_surface() const
    {
        surface<F> surf(m_width, m_height);

        for (size_t y = 0; y < m_height; ++y)
        {
            for (size_t x = 0; x < m_width; ++x)
            {
                surf.set_pixel(x, y, get_pixel(x, y));
            }
        }

        return surf;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // iteration
    ///////////////////////////////////////////////////////////////////////////////

    using iterator = palette_surface_iterator<F>;
    using const_iterator = const_palette_surface_iterator<F>;

    iterator begin()
    {
        return iterator(m_data.get(), 0, 0, m_width, m_height, &m_palette);
    }

    iterator end()
    {
        return iterator(m_data.get() + block_count(), 0, m_height, m_width, m_height, &m_palette);
    }

    const_iterator begin() const
    {
        return const_iterator(m_data.get(), 0, 0, m_width, m_height, &m_palette);
    }

    const_iterator end() const
    {
        return const_iterator(m_data.get() + block_count(), 0, m_height, m_width, m_height, &m_palette);
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    const_iterator cend() const
    {
        return end();
    }

private:

    size_t m_width = 0;
    size_t m_height = 0;
    std::unique_ptr<block_type[]> m_data;
    palette m_palette;
};

} // namespace pixel
} // namespace vx