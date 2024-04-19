#pragma once

#include <iterator>

#include "_priv/base_types.h"
#include "pixel.h"

#include "vertex/math/math/type/vec2.h"
#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
/// @brief Iterator for accessing pixels in an image.
///
/// This class supports forward iteration and provides methods for accessing
/// pixel information such as color, position, and UV coordinates.
///
/// @tparam T The pixel type.
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class pixel_iterator
{
private:

    using other_iterator = pixel_iterator<
        typename std::conditional<std::is_const<T>::value, typename std::remove_const<T>::type, const T>::type
    >;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // iterator traits
    ///////////////////////////////////////////////////////////////////////////////

    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

public:

    ///////////////////////////////////////////////////////////////////////////////
    // constructors and destructor
    ///////////////////////////////////////////////////////////////////////////////

    inline constexpr pixel_iterator() = default;

    inline constexpr pixel_iterator(pointer ptr, size_t x, size_t y, size_t width, size_t height) noexcept
        : m_current(ptr), m_x(x), m_y(y), m_w(width), m_h(height) {}

    inline constexpr pixel_iterator(const pixel_iterator<const T>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_w(other.m_w), m_h(other.m_h) {}

    inline constexpr pixel_iterator(const pixel_iterator<typename std::remove_const<T>::type>& other) noexcept
        : m_current(other.m_current), m_x(other.m_x), m_y(other.m_y), m_w(other.m_w), m_h(other.m_h) {}

    inline constexpr pixel_iterator(pixel_iterator&&) noexcept = default;

    ~pixel_iterator() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline constexpr pixel_iterator& operator=(const pixel_iterator<const T>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_w = other.m_w;
        m_h = other.m_h;
        return *this;
    }

    inline constexpr pixel_iterator& operator=(const pixel_iterator<typename std::remove_const<T>::type>& other) noexcept
    {
        m_current = other.m_current;
        m_x = other.m_x;
        m_y = other.m_y;
        m_w = other.m_w;
        m_h = other.m_h;
        return *this;
    }

    inline constexpr pixel_iterator& operator=(pixel_iterator&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // reference operators
    ///////////////////////////////////////////////////////////////////////////////

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

    ///////////////////////////////////////////////////////////////////////////////
    // increment operators
    ///////////////////////////////////////////////////////////////////////////////

    inline constexpr pixel_iterator& operator++()
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

    inline constexpr pixel_iterator operator++(int)
    {
        pixel_iterator result = *this;
        ++(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename IT, typename std::enable_if<std::is_same<IT, pixel_iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator==(const IT & other) const
    {
        return m_current == other.m_current && m_x == other.m_x && m_y == other.m_y && m_w == other.m_w && m_h == other.m_h;
    }

    template <typename IT, typename std::enable_if<std::is_same<IT, pixel_iterator>::value || std::is_same<IT, other_iterator>::value, bool>::type = true>
    bool inline constexpr operator!=(const IT & other) const
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
    inline constexpr math::vec2 resolution() const
    {
        return math::vec2(m_w, m_h);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the color of the current pixel.
    ///
    /// @return The color of the current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr math::color color() const
    {
        return math::color(operator*());
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the color of the current pixel.
    ///
    /// @param c The color to set.
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr void set_color(const math::color& c)
    {
        operator*() = c;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the integer position of the current pixel.
    ///
    /// @return A vector representing the integer position of the current pixel
    /// (x, y).
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr math::vec2i position() const
    {
        return math::vec2i(m_x, m_y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the floating point position of the current pixel.
    ///
    /// @return A vector representing the floating point position of the current
    /// pixel (x, y).
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr math::vec2 xy() const
    {
        return math::vec2(m_x, m_y);
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the normalized coordinates (uv) of the current pixel.
    ///
    /// @return A vector representing the normalized coordinates (uv) of the
    /// current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr math::vec2 uv() const
    {
        return xy() / resolution();
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Gets the floating point coordinates of the current pixel relative
    /// to the center of the image.
    /// 
    /// @return A vector representing the local coordinates of the current pixel.
    ///////////////////////////////////////////////////////////////////////////////
    inline constexpr math::vec2 local() const
    {
        return xy() - resolution() * 0.5f;
    }

private:

    pointer m_current = nullptr;

    size_t m_x = 0;
    size_t m_y = 0;

    size_t m_w = 0;
    size_t m_h = 0;

};

///////////////////////////////////////////////////////////////////////////////
// types
///////////////////////////////////////////////////////////////////////////////

using pixel_iterator_r8 = pixel_iterator<pixel_r8>;
using pixel_iterator_rg8 = pixel_iterator<pixel_rg8>;
using pixel_iterator_rgb8 = pixel_iterator<pixel_rgb8>;
using pixel_iterator_rgba8 = pixel_iterator<pixel_rgba8>;

///////////////////////////////////////////////////////////////////////////////
// iterators
///////////////////////////////////////////////////////////////////////////////

class image;

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline constexpr auto begin(image& i) noexcept
{
    return pixel_iterator<T>((T*)i.data(), 0, 0, i.width(), i.height());
}

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline auto cbegin(const image& i) noexcept
{
    return pixel_iterator<typename std::add_const<T>::type>((const T*)i.data(), 0, 0, i.width(), i.height());
}

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline auto begin(const image& i) noexcept
{
    return cbegin<T>(i);
}

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline constexpr auto end(image& i) noexcept
{
    return pixel_iterator<T>((T*)(i.data() + i.data_size()), 0, i.height(), i.width(), i.height());
}

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline auto cend(const image& i) noexcept
{
    return pixel_iterator<typename std::add_const<T>::type>((const T*)(i.data() + i.data_size()), 0, i.height(), i.width(), i.height());
}

template <typename T, typename std::enable_if<type_traits::is_pixel<T>::value, bool>::type = true>
inline auto end(const image& i) noexcept
{
    return cend<T>(i);
}

}
}