#pragma once

#include <sstream>

#include "../detail/base_types.h"
#include "../../math/type/vec2_type.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct rect_t
{
    static_assert(std::is_arithmetic<T>::value, "type T must be arithmetic type");

    // =============== meta ===============

    using value_type = T;
    using type = rect_t<T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = rect_t<float_value_type>;

    using size_type = math::size_type;

    using iterator = ::vx::tools::iterator<T>;
    using const_iterator = ::vx::tools::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    vec<2, T> position;
    vec<2, T> size;

    // =============== implicit constructors ===============

    inline constexpr rect_t() noexcept = default;

    inline constexpr rect_t(const type& r) noexcept
        : position(r.position), size(r.size) {}

    // =============== explicit constructors ===============

    inline constexpr explicit rect_t(
        const vec<2, T>& position,
        const vec<2, T>& size
    ) noexcept
        : position(position), size(size) {}

    inline constexpr rect_t(T x, T y, T width, T height) noexcept
        : position(x, y), size(width, height) {}

    // =============== conversion constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit rect_t(
        const vec<2, U>& position,
        const vec<2, U>& size
    )
        : position(position), size(size) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr rect_t(U x, U y, U width, U height)
        : position(static_cast<T>(x), static_cast<T>(y))
        , size(static_cast<T>(width), static_cast<T>(height)) {}

    template <typename U>
    inline constexpr explicit rect_t(const rect_t<U>& r)
        : position(r.position), size(r.size) {}

    // =============== destructor ===============

    ~rect_t() noexcept = default;

    // =============== assignment operator ===============

    inline constexpr type& operator=(const type& r) noexcept
    {
        position = r.position;
        size = r.size;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator=(const rect_t<U>& r) noexcept
    {
        position = r.position;
        size = r.size;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 4);
        return (&position.x)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 4);
        return (&position.x)[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& r1, const type& r2)
    {
        return r1.position == r2.position && r1.size == r2.size;
    }

    friend inline constexpr bool operator!=(const type& r1, const type& r2)
    {
        return !(r1 == r2);
    }

    // =============== iterator ===============

    inline constexpr iterator begin() noexcept
    {
        return iterator(&position.x);
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    inline constexpr iterator end() noexcept
    {
        return iterator(&size.y + 1);
    }

    inline constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    inline constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(&position.x);
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return const_iterator(&size.y + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&size.y + 1);
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    inline constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(&position.x);
    }

    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&size.y + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&position.x);
    }

    // =============== string ===============

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "rect(" << position.to_string() << ", " << size.to_string() << ')';
        return oss.str();
    }

    // =============== anchors ===============

    inline constexpr T x()		const { return position.x; }
    inline constexpr T y()		const { return position.y; }
    inline constexpr T width()	const { return size.x; }
    inline constexpr T height() const { return size.y; }

    inline constexpr void set_x(T x)           { position.x = x; }
    inline constexpr void set_y(T y)           { position.y = y; }
    inline constexpr void set_width(T width)   { size.x = width; }
    inline constexpr void set_height(T height) { size.y = height; }

    inline constexpr T left()	 const { return position.x; }
    inline constexpr T right()	 const { return position.x + size.x; }
    inline constexpr T top()	 const { return position.y; }
    inline constexpr T bottom()	 const { return position.y + size.y; }
    inline constexpr T centerx() const { return position.x + size.x / static_cast<T>(2); }
    inline constexpr T centery() const { return position.y + size.y / static_cast<T>(2); }

    inline constexpr void set_left(T left)        { position.x = left; }
    inline constexpr void set_right(T right)      { position.x = right - size.x; }
    inline constexpr void set_top(T top)          { position.y = top; }
    inline constexpr void set_bottom(T bottom)    { position.y = bottom - size.y; }
    inline constexpr void set_centerx(T centerx)  { position.x = centerx - size.x / static_cast<T>(2); }
    inline constexpr void set_centery(T cenrtery) { position.y = cenrtery - size.y / static_cast<T>(2); }

    inline constexpr vec<2, T> topleft()     const { return position; }
    inline constexpr vec<2, T> midtop()      const { return vec<2, T>(position.x + size.x / static_cast<T>(2), position.y); }
    inline constexpr vec<2, T> topright()    const { return vec<2, T>(position.x + size.x, position.y); }
    inline constexpr vec<2, T> midright()    const { return vec<2, T>(position.x + size.x, position.y + size.y / static_cast<T>(2)); }
    inline constexpr vec<2, T> bottomright() const { return position + size; }
    inline constexpr vec<2, T> midbottom()   const { return vec<2, T>(position.x + size.x / static_cast<T>(2), position.y + size.y); }
    inline constexpr vec<2, T> bottomleft()  const { return vec<2, T>(position.x, position.y + size.y); }
    inline constexpr vec<2, T> midleft()     const { return vec<2, T>(position.x, position.y + size.y / static_cast<T>(2)); }
    inline constexpr vec<2, T> center()      const { return position + size / static_cast<T>(2); }

    inline constexpr void set_topleft(const vec<2, T>& topleft)         { position = topleft; }
    inline constexpr void set_midtop(const vec<2, T>& midtop)           { position.x = midtop.x - size.x / static_cast<T>(2); position.y = midtop.y; }
    inline constexpr void set_topright(const vec<2, T>& topright)       { position.x = topright.x - size.x; position.y = topright.y; }
    inline constexpr void set_midright(const vec<2, T>& midright)       { position.x = midright.x - size.x; position.y = midright.y - size.y / static_cast<T>(2); }
    inline constexpr void set_bottomright(const vec<2, T>& bottomright) { position = bottomright - size; }
    inline constexpr void set_midbottom(const vec<2, T>& midbottom)     { position.x = midbottom.x - size.x / static_cast<T>(2); position.y = midbottom.y - size.y; }
    inline constexpr void set_bottomleft(const vec<2, T>& bottomleft)   { position.x = bottomleft.x; position.y = bottomleft.y - size.y; }
    inline constexpr void set_midleft(const vec<2, T>& midleft)         { position.x = midleft.x; position.y = midleft.y - size.y / static_cast<T>(2); }
    inline constexpr void set_center(const vec<2, T>& center)           { position = center - size / static_cast<T>(2); }

    // =============== common getters ===============

    /**
     * @brief Get the minimum x-coordinate of the rectangle.
     *
     * @return The minimum x-coordinate value.
     */
    inline constexpr T min_x() const { return math::min(position.x, right()); }

    /**
     * @brief Get the maximum x-coordinate of the rectangle.
     *
     * @return The maximum x-coordinate value.
     */
    inline constexpr T max_x() const { return math::max(position.x, right()); }

    /**
     * @brief Get the minimum y-coordinate of the rectangle.
     *
     * @return The minimum y-coordinate value.
     */
    inline constexpr T min_y() const { return math::min(position.y, bottom()); }

    /**
     * @brief Get the maximum y-coordinate of the rectangle.
     *
     * @return The maximum y-coordinate value.
     */
    inline constexpr T max_y() const { return math::max(position.y, bottom()); }

    /**
     * @brief Calculate the area of the rectangle.
     *
     * @return The area of the rectangle.
     */
    inline constexpr T area() const { return size.x * size.y; }

    // =============== common modifiers ===============

    /**
     * @brief Normalizes the rectangle.
     *
     * Normalizes the rectangle by ensuring that its size components are non-negative.
     *
     * @return The normalized rectangle.
     */
    inline constexpr type normalize() const
    {
        type nr = *this;

        if (size.x < static_cast<T>(0))
        {
            nr.position.x += size.x;
            nr.size.x = -size.x;
        }
        if (size.y < static_cast<T>(0))
        {
            nr.position.y += size.y;
            nr.size.y = -size.y;
        }

        return nr;
    }

    // =============== transformations ===============

    /**
     * @brief Scales the rectangle by a specified factor.
     *
     * @param s The scaling factor.
     * @return The scaled rectangle.
     */
    inline constexpr type scale(const vec<2, T>& s) const
    {
        return type(
            position,
            size * s
        );
    }

    /**
     * @brief Moves the rectangle by a specified offset.
     *
     * @param offset The offset by which to move the rectangle.
     * @return The moved rectangle.
     */
    inline constexpr type move(const vec<2, T>& offset) const
    {
        return type(
            position + offset,
            size
        );
    }

    /**
     * @brief Increases the size of the rectangle by a specified amount.
     *
     * @param s The amount by which to increase the size of the rectangle.
     * @return The inflated rectangle.
     */
    inline constexpr type inflate(const vec<2, T>& s) const
    {
        return type(
            position - s,
            size + static_cast<T>(2) * s
        );
    }

    /**
     * @brief Crops the rectangle to the intersection with another rectangle.
     *
     * Returns a new rectangle representing the intersection of this rectangle with the specified rectangle.
     *
     * @param r The rectangle to intersect with.
     * @return The intersected rectangle.
     */
    inline constexpr type crop(const type& r) const
    {
        const T nl = math::clamp(r.position.x, position.x, right());
        const T nr = math::clamp(r.right(), position.x, right());

        const T nt = math::clamp(r.position.y, position.y, bottom());
        const T nb = math::clamp(r.bottom(), position.y, bottom());

        return type(
            nl,
            nt,
            nr - nl,
            nb - nt
        );
    }

    /**
     * @brief Merges two rectangles to form a new rectangle containing both rectangles.
     *
     * Returns a new rectangle that contains both the specified rectangles.
     *
     * @param r1 The first rectangle.
     * @param r2 The second rectangle.
     * @return The merged rectangle.
     */
    static inline constexpr type merge(const type& r1, const type& r2)
    {
        const T nl = math::min(r1.position.x, r2.position.x);
        const T nr = math::max(r1.right(), r2.right());

        const T nt = math::min(r1.position.y, r2.position.y);
        const T nb = math::max(r1.bottom(), r2.bottom());

        return type(
            nl,
            nt,
            nr - nl,
            nb - nt
        );
    }

    /**
     * @brief Grows the sides of the rectangle by the specified amounts.
     *
     * Returns a new rectangle with its sides grown by the specified amounts.
     *
     * @param left   The amount to grow the left side of the rectangle.
     * @param right  The amount to grow the right side of the rectangle.
     * @param bottom The amount to grow the bottom side of the rectangle.
     * @param top    The amount to grow the top side of the rectangle.
     * @return The new rectangle with grown sides.
     */
    inline constexpr type grow_sides(T left, T right, T bottom, T top) const
    {
        return type(
            position.x - left,
            position.y - top,
            size.x + left + right,
            size.y + bottom + top
        );
    }

    /**
     * @brief Expands the rectangle to include the specified point.
     *
     * Returns a new rectangle expanded to include the specified point.
     *
     * @param p The point to include in the expanded rectangle.
     * @return The new rectangle expanded to include the specified point.
     */
    inline constexpr type expand_to(const vec<2, T>& p) const
    {
        const T nl = math::min(position.x, p.x);
        const T nr = math::max(right(), p.x);

        const T nt = math::min(position.y, p.y);
        const T nb = math::max(bottom(), p.y);

        return type(
            nl,
            nt,
            nr - nl,
            nb - nt
        );
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(0, 0, 0, 0); }

};

VX_PACK_POP()

using rect  = rect_t<float>;
using rectf = rect_t<float>;
using rectd = rect_t<double>;
using rectu = rect_t<unsigned int>;
using recti = rect_t<int>;

}
}