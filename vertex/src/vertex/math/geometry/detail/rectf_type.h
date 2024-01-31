#pragma once

#include <sstream>

#include "base_type_defs.h"
#include "../../math/fn_common.h"
#include "../../math/fn_comparison.h"

namespace vx {
namespace math {
namespace detail {

VX_PACK_PUSH()

template <typename T>
struct rect<T, val_t::floating_point>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using float_value_type = T;

    using type = rect<T, val_t::floating_point>;
    using float_type = type;

    using vec_type = vec<2, T, vec_t::vec, val_t::floating_point>;

    using size_type = length_type;

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    vec_type position;
    vec_type size;

    // =============== implicit constructors ===============

    inline constexpr rect() noexcept = default;

    inline constexpr rect(const type& r) noexcept
        : position(r.position), size(r.size) {}

    // =============== explicit constructors ===============

    inline constexpr explicit rect(
        const vec_type& position,
        const vec_type& size
    ) noexcept
        : position(position), size(size) {}

    inline constexpr rect(T x, T y, T width, T height) noexcept
        : position(x, y), size(width, height) {}

    // =============== conversion constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit rect(
        const vecx<2, U>& position,
        const vecx<2, U>& size
    )
        : position(position), size(size) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr rect(U x, U y, U width, U height)
        : position(static_cast<T>(x), static_cast<T>(y))
        , size(static_cast<T>(width), static_cast<T>(height)) {}

    template <typename U>
    inline constexpr explicit rect(const rectx<U>& r)
        : position(r.position), size(r.size) {}

    // =============== destructor ===============

    ~rect() noexcept = default;

    // =============== assignment operator ===============

    inline constexpr type& operator=(const type& r) noexcept
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

    // =============== unary bit operators ===============

    inline constexpr type& operator|=(const type& r)
    {
        (*this) = merge(*this, r);
        return *this;
    }

    // =============== binary bit operators ===============

    friend inline constexpr type operator|(const type& r1, const type& r2)
    {
        return merge(r1, r2);
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

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << position.to_string(pretty_print) << ", " << size.to_string(pretty_print) << " }";
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
    inline constexpr T centerx() const { return position.x + size.x * static_cast<T>(0.5); }
    inline constexpr T centery() const { return position.y + size.y * static_cast<T>(0.5); }

    inline constexpr void set_left(T left)        { position.x = left; }
    inline constexpr void set_right(T right)      { position.x = right - size.x; }
    inline constexpr void set_top(T top)          { position.y = top; }
    inline constexpr void set_bottom(T bottom)    { position.y = bottom - size.y; }
    inline constexpr void set_centerx(T centerx)  { position.x = centerx - size.x * static_cast<T>(0.5); }
    inline constexpr void set_centery(T cenrtery) { position.y = cenrtery - size.y * static_cast<T>(0.5); }

    inline constexpr vec_type topleft() const
    {
        return position;
    }

    inline constexpr vec_type midtop() const
    {
        return vec_type(
            position.x + size.x * static_cast<T>(0.5),
            position.y
        );
    }

    inline constexpr vec_type topright() const
    {
        return vec_type(
            position.x + size.x,
            position.y
        );
    }

    inline constexpr vec_type midright() const
    {
        return vec_type(
            position.x + size.x,
            position.y + size.y * static_cast<T>(0.5)
        );
    }

    inline constexpr vec_type bottomright() const
    {
        return position + size;
    }

    inline constexpr vec_type midbottom() const
    {
        return vec_type(
            position.x + size.x * static_cast<T>(0.5),
            position.y + size.y
        );
    }

    inline constexpr vec_type bottomleft() const
    {
        return vec_type(
            position.x,
            position.y + size.y
        );
    }

    inline constexpr vec_type midleft() const
    {
        return vec_type(
            position.x,
            position.y + size.y * static_cast<T>(0.5)
        );
    }

    inline constexpr vec_type center() const
    {
        return position + size * static_cast<T>(0.5);
    }

    inline constexpr void set_topleft(const vec_type& topleft)
    {
        position = topleft;
    }

    inline constexpr void set_midtop(const vec_type& midtop)
    {
        position.x = midtop.x - size.x * static_cast<T>(0.5);
        position.y = midtop.y;
    }

    inline constexpr void set_topright(const vec_type& topright)
    {
        position.x = topright.x - size.x;
        position.y = topright.y;
    }

    inline constexpr void set_midright(const vec_type& midright)
    {
        position.x = midright.x - size.x;
        position.y = midright.y - size.y * static_cast<T>(0.5);
    }

    inline constexpr void set_bottomright(const vec_type& bottomright)
    {
        position = bottomright - size;
    }

    inline constexpr void set_midbottom(const vec_type& midbottom)
    {
        position.x = midbottom.x - size.x * static_cast<T>(0.5);
        position.y = midbottom.y - size.y;
    }

    inline constexpr void set_bottomleft(const vec_type& bottomleft)
    {
        position.x = bottomleft.x;
        position.y = bottomleft.y - size.y;
    }

    inline constexpr void set_midleft(const vec_type& midleft)
    {
        position.x = midleft.x;
        position.y = midleft.y - size.y * static_cast<T>(0.5);
    }

    inline constexpr void set_center(const vec_type& center)
    {
        position = center - size * static_cast<T>(0.5);
    }

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
     * @brief Get the aspect ratio of the rectangle.
     *
     * The aspect ratio is calculated as the ratio of width to height.
     *
     * @return The aspect ratio of the rectangle.
     */
    inline constexpr T aspect() const { return size.aspect(); }

    /**
     * @brief Calculate the area of the rectangle.
     *
     * @return The area of the rectangle.
     */
    inline constexpr T area() const { return size.x * size.y; }

    /**
     * @brief Check if the rectangle has a positive area.
     *
     * This function determines whether both the width and height of the rectangle are greater than zero.
     *
     * @return True if the rectangle has a positive area, false otherwise.
     */
    inline constexpr bool has_area() const
    {
        return size.x > static_cast<T>(0) && size.y > static_cast<T>(0);
    }

    // =============== common modifiers ===============

    inline constexpr void set(T x, T y, T width, T height)
    {
        position.set(x, y);
        size.set(width, height);
    }

    /**
     * @brief Create a normalized version of the rectangle.
     *
     * This function returns a new rectangle with non-negative width and height.
     * If the original rectangle has negative width or height, the position and size are adjusted accordingly.
     *
     * @return A normalized rectangle.
     */
    inline constexpr type normalize() const
    {
        type nr(*this);

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
     * @brief Scale the rectangle uniformly by a scalar factor.
     *
     * This function scales the rectangle uniformly in both width and height by a specified scalar factor.
     *
     * @param s The scalar factor for scaling.
     * @return A new rectangle scaled by the given factor.
     */
    inline constexpr type scale(T s) const
    {
        return type(
            position,
            size * s
        );
    }

    /**
     * @brief Scale the rectangle independently in both width and height.
     *
     * This function scales the rectangle independently in width and height using the provided factors.
     *
     * @param sx The scaling factor for the width.
     * @param sy The scaling factor for the height.
     * @return A new rectangle scaled by the specified factors.
     */
    inline constexpr type scale(T sx, T sy) const
    {
        return type(
            position,
            vec_type(size.x * sx, size.y * sy)
        );
    }

    /**
     * @brief Scale the rectangle using a vector of scaling factors.
     *
     * This function scales the rectangle using a vector of scaling factors for width and height.
     *
     * @param dv The vector of scaling factors.
     * @return A new rectangle scaled by the specified vector.
     */
    inline constexpr type scale(const vec_type& dv) const
    {
        return type(
            position,
            size * dv
        );
    }

    /**
     * @brief Move the rectangle by specified displacements along both axes.
     *
     * This function moves the rectangle by the specified displacements along the x and y axes.
     *
     * @param dx The displacement along the x-axis.
     * @param dy The displacement along the y-axis.
     * @return A new rectangle moved by the specified displacements.
     */
    inline constexpr type move(T dx, T dy) const
    {
        return type(
            vec_type(position.x + dx, position.y + dy),
            size
        );
    }

    /**
     * @brief Move the rectangle by a vector of displacements.
     *
     * This function moves the rectangle by the specified vector of displacements.
     *
     * @param dv The vector of displacements.
     * @return A new rectangle moved by the specified vector.
     */
    inline constexpr type move(const vec_type& dv) const
    {
        return type(
            position + dv,
            size
        );
    }

    /**
     * @brief Inflate the rectangle by specified amounts along both axes.
     *
     * This function inflates the rectangle by the specified amounts along the x and y axes.
     *
     * @param ix The amount to inflate the rectangle's width.
     * @param iy The amount to inflate the rectangle's height.
     * @return A new rectangle inflated by the specified amounts.
     */
    inline constexpr type inflate(T ix, T iy) const
    {
        return type(
            vec_type(position.x - ix, position.y - iy),
            vec_type(size.x + static_cast<T>(2) * ix, size.y + static_cast<T>(2) * iy)
        );
    }

    /**
     * @brief Inflate the rectangle by a vector of amounts.
     *
     * This function inflates the rectangle by the specified vector of amounts.
     *
     * @param iv The vector of amounts to inflate the rectangle.
     * @return A new rectangle inflated by the specified vector of amounts.
     */
    inline constexpr type inflate(const vec_type& iv) const
    {
        return type(
            position - iv,
            size + static_cast<T>(2) * iv
        );
    }

    /**
     * @brief Crop 'r' inside of this rectangle.
     *
     * This function crops the specified rectangle 'r' based on the current rectangle.
     *
     * @param r The rectangle to be cropped using the current rectangle as the cropping region.
     * @return A new rectangle representing the cropped region within the specified rectangle.
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
     * @brief Merge two rectangles to create a new rectangle containing both.
     *
     * This static function merges two rectangles, creating a new rectangle that contains both input rectangles.
     *
     * @param r1 The first rectangle to be merged.
     * @param r2 The second rectangle to be merged.
     * @return A new rectangle containing both input rectangles.
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
     * @brief Grow the sides of the rectangle by specified amounts.
     *
     * This function expands the sides of the rectangle by the specified amounts for left, right, bottom, and top.
     *
     * @param left The amount to grow the left side of the rectangle.
     * @param right The amount to grow the right side of the rectangle.
     * @param bottom The amount to grow the bottom side of the rectangle.
     * @param top The amount to grow the top side of the rectangle.
     * @return A new rectangle with its sides grown by the specified amounts.
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
     * @brief Expand the rectangle to include a specified point.
     *
     * This function expands the rectangle to ensure it includes the specified point 'p'.
     *
     * @param p The point to include in the expanded rectangle.
     * @return A new rectangle expanded to include the specified point.
     */
    inline constexpr type expand_to(const vec_type& p) const
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

    // =============== collision ===============

    /**
     * @brief Check if the current rectangle collides with another rectangle.
     *
     * This function checks whether the current rectangle collides with the specified rectangle 'r'.
     *
     * @param r The rectangle to check for collision.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(const type& r) const
    {
        return position.x < r.position.x + r.size.x
            && position.x + size.x > r.position.x
            && position.y < r.position.y + r.size.y
            && position.y + size.y > r.position.y;
    }

    /**
     * @brief Check if the current rectangle collides with another rectangle.
     *
     * This function checks whether the current rectangle collides with the specified rectangle 'r'.
     * The 'include_borders' parameter determines whether to include borders in the collision check.
     *
     * @param r The rectangle to check for collision.
     * @param include_borders If true, include borders in the collision check.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(const type& r, bool include_borders) const
    {
        if (!include_borders) return collide(r);
        return position.x <= r.position.x + r.size.x
            && position.x + size.x >= r.position.x
            && position.y <= r.position.y + r.size.y
            && position.y + size.y >= r.position.y;
    }

    /**
     * @brief Check if a point collides with the current rectangle.
     *
     * This function checks whether the specified point 'p' collides with the current rectangle.
     *
     * @param p The point to check for collision.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(const vec_type& p) const
    {
        return position.x < p.x
            && position.x + size.x > p.x
            && position.y < p.y
            && position.y + size.y > p.y;
    }

    /**
     * @brief Check if a point collides with the current rectangle.
     *
     * This function checks whether the specified point 'p' collides with the current rectangle.
     * The 'include_borders' parameter determines whether to include borders in the collision check.
     *
     * @param p The point to check for collision.
     * @param include_borders If true, include borders in the collision check.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(const vec_type& p, bool include_borders) const
    {
        if (!include_borders) return collide(p);
        return position.x <= p.x
            && position.x + size.x >= p.x
            && position.y <= p.y
            && position.y + size.y >= p.y;
    }

    /**
     * @brief Check if a point collides with the current rectangle.
     *
     * This function checks whether the point specified by coordinates 'x' and 'y' collides with the current rectangle.
     *
     * @param x The x-coordinate of the point to check.
     * @param y The y-coordinate of the point to check.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(T x, T y) const
    {
        return position.x < x
            && position.x + size.x > x
            && position.y < y
            && position.y + size.y > y;
    }

    /**
     * @brief Check if a point collides with the current rectangle.
     *
     * This function checks whether the point specified by coordinates 'x' and 'y' collides with the current rectangle.
     * The 'include_borders' parameter determines whether to include borders in the collision check.
     *
     * @param x The x-coordinate of the point to check.
     * @param y The y-coordinate of the point to check.
     * @param include_borders If true, include borders in the collision check.
     * @return True if collision occurs, false otherwise.
     */
    inline constexpr bool collide(T x, T y, bool include_borders) const
    {
        if (!include_borders) return collide(x, y);
        return position.x <= x
            && position.x + size.x >= x
            && position.y <= y
            && position.y + size.y >= y;
    }

    /**
     * @brief Check if the current rectangle contains another rectangle.
     *
     * This function checks whether the current rectangle contains the specified rectangle 'r' entirely.
     *
     * @param r The rectangle to check for containment.
     * @return True if the current rectangle contains the specified rectangle, false otherwise.
     */
    inline constexpr bool contains(const type& r) const
    {
        return position.x < r.position.x
            && position.x + size.x > r.position.x + r.size.x
            && position.y < r.position.y
            && position.y + size.y > r.position.y + r.size.y;
    }

    /**
     * @brief Check if the current rectangle contains another rectangle.
     *
     * This function checks whether the current rectangle contains the specified rectangle 'r' entirely.
     * The 'include_borders' parameter determines whether to include borders in the containment check.
     *
     * @param r The rectangle to check for containment.
     * @param include_borders If true, include borders in the containment check.
     * @return True if the current rectangle contains the specified rectangle, false otherwise.
     */
    inline constexpr bool contains(const type& r, bool include_borders) const
    {
        if (!include_borders) return contains(r);
        return position.x <= r.position.x
            && position.x + size.x >= r.position.x + r.size.x
            && position.y <= r.position.y
            && position.y + size.y >= r.position.y + r.size.y;
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(); }
    static inline constexpr type UNIT() { return type(static_cast<T>(-1), static_cast<T>(-1), static_cast<T>(1), static_cast<T>(1)); }

};

VX_PACK_POP()

}

using rect2  = detail::rect<float>;
using rect2f = detail::rect<float>;

}
}