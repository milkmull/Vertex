#pragma once

#include "../../_priv/common.h"
#include "../../../math/fn/fn_common.h"
#include "../../fn/rect_fn_collision.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct rect_t
{
    static_assert(std::is_arithmetic<T>::value, "type T must be arithmetic type");

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scaler_type = T;
    using type = rect_t<T>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    vec<2, T> position;
    vec<2, T> size;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr rect_t() noexcept = default;

    VX_FORCE_INLINE constexpr rect_t(const type& r) noexcept
        : position(r.position), size(r.size) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit rect_t(
        const vec<2, T>& position,
        const vec<2, T>& size
    ) noexcept
        : position(position), size(size) {}

    VX_FORCE_INLINE constexpr rect_t(T x, T y, T width, T height) noexcept
        : position(x, y), size(width, height) {}

    VX_FORCE_INLINE constexpr explicit rect_t(const vec<4, T>& v) noexcept
        : position(v.x, v.y), size(v.z, v.w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename P, typename S>
    VX_FORCE_INLINE constexpr explicit rect_t(
        const vec<2, P>& position,
        const vec<2, S>& size
    )
        : position(position), size(size) {}

    template <typename X, typename Y, typename W, typename H, typename std::enable_if<
        std::is_arithmetic<X>::value&&
        std::is_arithmetic<Y>::value&&
        std::is_arithmetic<W>::value&&
        std::is_arithmetic<H>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr rect_t(X x, Y y, W width, H height)
        : position(static_cast<T>(x), static_cast<T>(y))
        , size(static_cast<T>(width), static_cast<T>(height)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit rect_t(const rect_t<U>& r)
        : position(r.position), size(r.size) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit rect_t(const vec<4, U>& v)
        : position(static_cast<T>(v.x), static_cast<T>(v.y))
        , size(static_cast<T>(v.z), static_cast<T>(v.w)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~rect_t() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& r) noexcept
    {
        position = r.position;
        size = r.size;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const rect_t<U>& r) noexcept
    {
        position = r.position;
        size = r.size;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return position.x;
            case 1: return position.y;
            case 2: return size.x;
            case 3: return size.y;
        }
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return position.x;
            case 1: return position.y;
            case 2: return size.x;
            case 3: return size.y;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const
    {
        return vec<4, U>(
            static_cast<U>(position.x),
            static_cast<U>(position.y),
            static_cast<U>(size.x),
            static_cast<U>(size.y)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& r1, const type& r2)
    {
        return r1.position == r2.position && r1.size == r2.size;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& r1, const type& r2)
    {
        return !(r1 == r2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // anchors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T x()		const { return position.x; }
    VX_FORCE_INLINE constexpr T y()		const { return position.y; }
    VX_FORCE_INLINE constexpr T width()	const { return size.x; }
    VX_FORCE_INLINE constexpr T height() const { return size.y; }

    VX_FORCE_INLINE constexpr void set_x(T x) { position.x = x; }
    VX_FORCE_INLINE constexpr void set_y(T y) { position.y = y; }
    VX_FORCE_INLINE constexpr void set_width(T width) { size.x = width; }
    VX_FORCE_INLINE constexpr void set_height(T height) { size.y = height; }

    VX_FORCE_INLINE constexpr T left()	 const { return position.x; }
    VX_FORCE_INLINE constexpr T right()	 const { return position.x + size.x; }
    VX_FORCE_INLINE constexpr T top()	 const { return position.y; }
    VX_FORCE_INLINE constexpr T bottom()	 const { return position.y + size.y; }
    VX_FORCE_INLINE constexpr T centerx() const { return position.x + size.x / static_cast<T>(2); }
    VX_FORCE_INLINE constexpr T centery() const { return position.y + size.y / static_cast<T>(2); }

    VX_FORCE_INLINE constexpr void set_left(T left) { position.x = left; }
    VX_FORCE_INLINE constexpr void set_right(T right) { position.x = right - size.x; }
    VX_FORCE_INLINE constexpr void set_top(T top) { position.y = top; }
    VX_FORCE_INLINE constexpr void set_bottom(T bottom) { position.y = bottom - size.y; }
    VX_FORCE_INLINE constexpr void set_centerx(T centerx) { position.x = centerx - size.x / static_cast<T>(2); }
    VX_FORCE_INLINE constexpr void set_centery(T cenrtery) { position.y = cenrtery - size.y / static_cast<T>(2); }

    VX_FORCE_INLINE constexpr vec<2, T> topleft()     const { return position; }
    VX_FORCE_INLINE constexpr vec<2, T> midtop()      const { return vec<2, T>(position.x + size.x / static_cast<T>(2), position.y); }
    VX_FORCE_INLINE constexpr vec<2, T> topright()    const { return vec<2, T>(position.x + size.x, position.y); }
    VX_FORCE_INLINE constexpr vec<2, T> midright()    const { return vec<2, T>(position.x + size.x, position.y + size.y / static_cast<T>(2)); }
    VX_FORCE_INLINE constexpr vec<2, T> bottomright() const { return position + size; }
    VX_FORCE_INLINE constexpr vec<2, T> midbottom()   const { return vec<2, T>(position.x + size.x / static_cast<T>(2), position.y + size.y); }
    VX_FORCE_INLINE constexpr vec<2, T> bottomleft()  const { return vec<2, T>(position.x, position.y + size.y); }
    VX_FORCE_INLINE constexpr vec<2, T> midleft()     const { return vec<2, T>(position.x, position.y + size.y / static_cast<T>(2)); }
    VX_FORCE_INLINE constexpr vec<2, T> center()      const { return position + size / static_cast<T>(2); }

    VX_FORCE_INLINE constexpr void set_topleft(const vec<2, T>& topleft) { position = topleft; }
    VX_FORCE_INLINE constexpr void set_midtop(const vec<2, T>& midtop) { position.x = midtop.x - size.x / static_cast<T>(2); position.y = midtop.y; }
    VX_FORCE_INLINE constexpr void set_topright(const vec<2, T>& topright) { position.x = topright.x - size.x; position.y = topright.y; }
    VX_FORCE_INLINE constexpr void set_midright(const vec<2, T>& midright) { position.x = midright.x - size.x; position.y = midright.y - size.y / static_cast<T>(2); }
    VX_FORCE_INLINE constexpr void set_bottomright(const vec<2, T>& bottomright) { position = bottomright - size; }
    VX_FORCE_INLINE constexpr void set_midbottom(const vec<2, T>& midbottom) { position.x = midbottom.x - size.x / static_cast<T>(2); position.y = midbottom.y - size.y; }
    VX_FORCE_INLINE constexpr void set_bottomleft(const vec<2, T>& bottomleft) { position.x = bottomleft.x; position.y = bottomleft.y - size.y; }
    VX_FORCE_INLINE constexpr void set_midleft(const vec<2, T>& midleft) { position.x = midleft.x; position.y = midleft.y - size.y / static_cast<T>(2); }
    VX_FORCE_INLINE constexpr void set_center(const vec<2, T>& center) { position = center - size / static_cast<T>(2); }

    ///////////////////////////////////////////////////////////////////////////////
    // attributes
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the minimum x-coordinate of the rectangle.
    ///
    /// @return The minimum x-coordinate value.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T min_x() const { return math::min(position.x, right()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the maximum x-coordinate of the rectangle.
    ///
    /// @return The maximum x-coordinate value.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T max_x() const { return math::max(position.x, right()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the minimum y-coordinate of the rectangle.
    ///
    /// @return The minimum y-coordinate value.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T min_y() const { return math::min(position.y, bottom()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Get the maximum y-coordinate of the rectangle.
    ///
    /// @return The maximum y-coordinate value.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T max_y() const { return math::max(position.y, bottom()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Calculate the area of the rectangle.
    ///
    /// @return The area of the rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T area() const { return size.x * size.y; }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Check if the rectangle has no area.
    ///
    /// @return True if the rectangle's area is 0, otherwise false.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr bool empty() const { return area() == static_cast<T>(0); }

    ///////////////////////////////////////////////////////////////////////////////
    // modifiers
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Normalizes the rectangle by ensuring that its size components are
    /// non-negative.
    ///
    /// @return The normalized rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type normalize() const
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

    ///////////////////////////////////////////////////////////////////////////////
    // transform
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Scales the rectangle by specified factors along the x and y axes.
    ///
    /// @param sx The x-axis scaling factor.
    /// @param sy The y-axis scaling factor.
    /// 
    /// @return The scaled rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type scale(T sx, T sy) const
    {
        return type(
            position.x,
            position.y,
            size.x * sx,
            size.y * sy
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Moves the rectangle by specified offsets along the x and y axes.
    ///
    /// @param dx The x-axis offset.
    /// @param dy The y-axis offset.
    /// 
    /// @return The moved rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type move(T dx, T dy) const
    {
        return type(
            position.x + dx,
            position.y + dy,
            size.x,
            size.y
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Increases the size of the rectangle uniformly by a specified amount.
    ///
    /// @param s The amount by which to increase the size of the rectangle.
    /// 
    /// @return The inflated rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type inflate(T ix, T iy) const
    {
        return type(
            position.x - ix,
            position.y - iy,
            size.x + static_cast<T>(2) * ix,
            size.y + static_cast<T>(2) * iy
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Returns a new rectangle representing the intersection of this
    /// rectangle and the specified rectangle.
    ///
    /// @param r The rectangle to intersect with.
    /// 
    /// @return The intersected rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type crop(const type& r) const
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Merges two rectangles to form a new rectangle containing both
    /// rectangles.
    ///
    /// @param r1 The first rectangle.
    /// @param r2 The second rectangle.
    /// 
    /// @return The merged rectangle.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type merge(const type& r1, const type& r2)
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Returns a new rectangle with its sides grown by the specified amounts.
    ///
    /// @param left   The amount to grow the left side of the rectangle.
    /// @param right  The amount to grow the right side of the rectangle.
    /// @param bottom The amount to grow the bottom side of the rectangle.
    /// @param top    The amount to grow the top side of the rectangle.
    /// 
    /// @return The new rectangle with grown sides.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type grow_sides(T left, T right, T bottom, T top) const
    {
        return type(
            position.x - left,
            position.y - top,
            size.x + left + right,
            size.y + bottom + top
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Expands the rectangle to include the specified point.
    ///
    /// @param p The point to include in the expanded rectangle.
    /// 
    /// @return The new rectangle expanded to include the specified point.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr type expand_to(const vec<2, T>& p) const
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

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type ZERO() { return type(0, 0, 0, 0); }

};

VX_PACK_POP()

}
}