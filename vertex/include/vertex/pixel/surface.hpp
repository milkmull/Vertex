#pragma once

#include <cstring>
#include <memory>

#include "vertex/pixel/iterator.hpp"
#include "vertex/math/rect.hpp"

namespace vx {
namespace pixel {

template <pixel_format F>
class surface
{
public:

    ///////////////////////////////////////////////////////////////////////////////
    // format details
    ///////////////////////////////////////////////////////////////////////////////

    static constexpr pixel_format format = F;

    static VX_FORCE_INLINE constexpr size_t channels() noexcept { return get_pixel_channel_count(format); }
    static VX_FORCE_INLINE constexpr bool has_alpha() noexcept { return pixel_has_alpha(format); }
    static VX_FORCE_INLINE constexpr size_t pixel_size() noexcept { return get_pixel_size(format); }
    static VX_FORCE_INLINE constexpr bool unknown_format() noexcept { return format == pixel_format::UNKNOWN; }

    using raw_pixel_type = raw_pixel<format>;
    using pixel_type = typename raw_pixel_type::pixel_type;
    using channel_type = typename raw_pixel_type::channel_type;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    surface() = default;

    surface(size_t width, size_t height)
        : m_width(width), m_height(height)
        , m_data(new raw_pixel_type[m_width * m_height]) {}

    surface(const byte_type* data, size_t width, size_t height)
        : m_width(width), m_height(height)
        , m_data(new raw_pixel_type[m_width * m_height])
    {
        std::memcpy(m_data.get(), data, data_size());
    }

    ///////////////////////////////////////////////////////////////////////////////
    // copy & move
    ///////////////////////////////////////////////////////////////////////////////

    surface(const surface& other)
        : m_width(other.m_width)
        , m_height(other.m_height)
        , m_data(new raw_pixel_type[other.m_width * other.m_height])
    {
        std::memcpy(m_data.get(), other.m_data.get(), data_size());
    }

    surface& operator=(const surface& other)
    {
        if (this != &other)
        {
            m_width = other.m_width;
            m_height = other.m_height;
            m_data.reset(new raw_pixel_type[m_width * m_height]);
            std::memcpy(m_data.get(), other.m_data.get(), data_size());
        }

        return *this;
    }

    surface(surface&& other) noexcept = default;
    surface& operator=(surface&& other) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    bool operator==(const surface& other) const noexcept
    {
        return m_width == other.m_width
            && m_height == other.m_height
            && (std::memcmp(data(), other.data(), data_size()) == 0);
    }

    bool operator!=(const surface& other) const noexcept
    {
        return !(*this == other);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    size_t width() const noexcept { return m_width; }
    size_t height() const noexcept { return m_height; }
    size_t stride() const noexcept { return m_width * pixel_size(); }

    math::vec2i size() const noexcept { return math::vec2i(m_width, m_height); }
    math::recti get_rect() const noexcept { return math::recti(0, 0, m_width, m_height); }

    ///////////////////////////////////////////////////////////////////////////////
    // raw data
    ///////////////////////////////////////////////////////////////////////////////

    byte_type* data() noexcept { return reinterpret_cast<byte_type*>(m_data.get()); }
    const byte_type* data() const noexcept { return reinterpret_cast<const byte_type*>(m_data.get()); }
    size_t data_size() const noexcept { return m_width * m_height * pixel_size(); }
    bool empty() const noexcept { return m_data == nullptr || m_width == 0 || m_height == 0; }

    ///////////////////////////////////////////////////////////////////////////////
    // pixel access
    ///////////////////////////////////////////////////////////////////////////////

    raw_pixel_type& at(size_t x, size_t y) noexcept
    {
        return m_data[y * m_width + x];
    }

    const raw_pixel_type& at(size_t x, size_t y) const noexcept
    {
        return m_data[y * m_width + x];
    }

    math::color get_pixel(size_t x, size_t y, const math::color& default_color = math::color()) const noexcept
    {
        if (x >= m_width || y >= m_height)
        {
            return default_color;
        }

        return static_cast<math::color>(at(x, y));
    }

    math::color get_pixel(const math::vec2i& p, const math::color& default_color = math::color()) const noexcept
    {
        return get_pixel(
            static_cast<size_t>(p.x),
            static_cast<size_t>(p.y),
            default_color
        );
    }

    void set_pixel(size_t x, size_t y, const math::color& color) noexcept
    {
        if (x >= m_width || y >= m_height)
        {
            return;
        }

        at(x, y) = raw_pixel_type(color);
    }

    void set_pixel(const math::vec2i& p, const math::color& color) noexcept
    {
        set_pixel(static_cast<size_t>(p.x), static_cast<size_t>(p.y), color);
    }

    void fill(const math::color& color)
    {
        raw_pixel_type px(color);
        std::fill(m_data.get(), m_data.get() + (m_width * m_height), px);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion
    ///////////////////////////////////////////////////////////////////////////////

    template <pixel_format F2>
    surface<F2> convert() const
    {
        surface<F2> converted(m_width, m_height);

        VX_IF_CONSTEXPR(F == F2)
        {
            std::memcpy(converted.m_data.get(), m_data.get(), data_size());
        }
        else
        {
            for (size_t y = 0; y < m_height; ++y)
            {
                for (size_t x = 0; x < m_width; ++x)
                {
                    converted.set_pixel(x, y, get_pixel(x, y));
                }
            }
        }

        return converted;
    }

    template <pixel_format F2>
    static surface<F2> reinterpret(surface<F>&& src, size_t new_width, size_t new_height)
    {
        static_assert(F2 != pixel_format::UNKNOWN, "Cannot reinterpret to UNKNOWN format");

        const size_t expected_bytes = new_width * new_height * surface<F2>::pixel_size();
        const size_t actual_bytes = src.data_size();

        if (expected_bytes != actual_bytes)
        {
            throw std::runtime_error("Size mismatch in reinterpretation");
        }

        surface<F2> dst;
        dst.m_width = new_width;
        dst.m_height = new_height;

        dst.m_data.reset(
            reinterpret_cast<raw_pixel_type*>(src.m_data.release())
        );

        return dst;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // iteration
    ///////////////////////////////////////////////////////////////////////////////

    using iterator = pixel_iterator<F>;
    using const_iterator = const_pixel_iterator<F>;

    iterator begin()
    {
        return iterator(m_data.get(), 0, 0, m_width, m_height);
    }

    iterator end()
    {
        return iterator(m_data.get() + m_width * m_height, 0, m_height, m_width, m_height);
    }

    const_iterator begin() const
    {
        return const_iterator(m_data.get(), 0, 0, m_width, m_height);
    }

    const_iterator end() const
    {
        return const_iterator(m_data.get() + m_width * m_height, 0, m_height, m_width, m_height);
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
    std::unique_ptr<raw_pixel_type[]> m_data;
};

} // namespace pixel
} // namespace vx