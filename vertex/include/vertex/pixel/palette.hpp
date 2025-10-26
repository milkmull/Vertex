#pragma once

#include <memory>
#include <cstddef>
#include <algorithm>

#include "vertex/math/color.hpp"

namespace vx {
namespace pixel {

class palette
{
public:

    using color_type = math::color;
    using size_type = std::size_t;
    using iterator = color_type*;
    using const_iterator = const color_type*;

    ///////////////////////////////////////////////////////////////////////////////
    // constructors
    ///////////////////////////////////////////////////////////////////////////////

    palette() noexcept = default;

    explicit palette(size_type count) : m_size(count), m_colors(std::make_unique<color_type[]>(count)) {}

    palette(std::initializer_list<color_type> init)
        : m_size(init.size())
        , m_colors(std::make_unique<color_type[]>(init.size()))
    {
        std::copy(init.begin(), init.end(), m_colors.get());
    }

    palette(const palette& other)
        : m_size(other.m_size)
        , m_colors(std::make_unique<color_type[]>(other.m_size))
    {
        std::copy(other.begin(), other.end(), begin());
    }

    palette& operator=(const palette& other)
    {
        if (this != &other)
        {
            if (other.m_size != m_size)
            {
                m_colors = std::make_unique<color_type[]>(other.m_size);
                m_size = other.m_size;
            }

            std::copy(other.begin(), other.end(), begin());
        }

        return *this;
    }

    palette(palette&&) noexcept = default;
    palette& operator=(palette&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // info
    ///////////////////////////////////////////////////////////////////////////////

    constexpr size_type size() const noexcept { return m_size; }
    constexpr size_type empty() const noexcept { return size() == 0; }

    ///////////////////////////////////////////////////////////////////////////////
    // Iteration
    ///////////////////////////////////////////////////////////////////////////////

    iterator begin() noexcept { return m_colors.get(); }
    const_iterator begin() const noexcept { return m_colors.get(); }
    const_iterator cbegin() const noexcept { return m_colors.get(); }

    iterator end() noexcept { return m_colors.get() + m_size; }
    const_iterator end() const noexcept { return m_colors.get() + m_size; }
    const_iterator cend() const noexcept { return m_colors.get() + m_size; }

    ///////////////////////////////////////////////////////////////////////////////
    // access
    ///////////////////////////////////////////////////////////////////////////////

    color_type& operator[](size_type index) noexcept { return m_colors[index]; }
    const color_type& operator[](size_type index) const noexcept { return m_colors[index]; }

    color_type* data() noexcept { return m_colors.get(); }
    const color_type* data() const noexcept { return m_colors.get(); }

    ///////////////////////////////////////////////////////////////////////////////
    // lookup
    ///////////////////////////////////////////////////////////////////////////////

    bool find_color(const color_type& c, size_type* index) const noexcept
    {
        for (size_type i = 0; i < m_size; ++i)
        {
            if (m_colors[i] == c)
            {
                if (index)
                {
                    *index = i;
                }

                return true;
            }
        }

        return false;
    }

    bool has_color(const color_type& c) const noexcept
    {
        return find_color(c, nullptr);
    }

    bool find_closest(const color_type& c, size_type* index) const noexcept
    {
        if (empty())
        {
            return false;
        }

        size_type best_index = 0;

        if (find_color(c, &best_index))
        {
            if (index)
            {
                *index = best_index;
            }

            return true;
        }

        float best_distance = std::numeric_limits<float>::max();

        for (size_type i = 0; i < m_size; ++i)
        {
            const float d = math::distance_squared(m_colors[i], c);
            if (d < best_distance)
            {
                best_distance = d;
                best_index = i;
            }
        }

        if (index)
        {
            *index = best_index;
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // modification
    ///////////////////////////////////////////////////////////////////////////////

    void set_colors(const color_type* colors, size_t count) noexcept
    {
        const size_t end = math::min(count, m_size);

        for (size_t i = 0; i < end; ++i)
        {
            m_colors[i] = colors[i];
        }
    }

    void fill(const color_type& color) noexcept
    {
        std::fill(m_colors.get(), m_colors.get() + m_size, color);
    }

    void resize(size_type new_size)
    {
        auto new_colors = std::make_unique<color_type[]>(new_size);
        size_type n = std::min(m_size, new_size);
        std::copy_n(m_colors.get(), n, new_colors.get());
        m_colors = std::move(new_colors);
        m_size = new_size;
    }

private:

    size_type m_size = 0;
    std::unique_ptr<color_type[]> m_colors;
};

} // namespace pixel
} // namespace vx