#pragma once

#include "vertex/math/geometry/2d/types/point.hpp"

namespace vx {
namespace math {
namespace geometry2d {

template <typename T>
struct rect_t
{
    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    using type = rect_t<scalar_type>;
    using point_type = point_t<T>;
    using line_type = line_t<T>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    point_type position;
    vec<2, scalar_type> size;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr rect_t() noexcept = default;

    VX_FORCE_INLINE constexpr rect_t(const type& r) noexcept
        : position(r.position), size(r.size) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr rect_t(
        scalar_type x, scalar_type y,
        scalar_type width, scalar_type height
    ) noexcept
        : position(x, y), size(width, height) {}

    VX_FORCE_INLINE constexpr rect_t(
        const point_type& rposition,
        const vec<2, scalar_type>& rsize
    ) noexcept
        : position(rposition), size(rsize) {}

    VX_FORCE_INLINE constexpr rect_t(const vec<4, scalar_type>& r) noexcept
        : position(r.x, r.y), size(r.z, r.w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename P, typename S>
    VX_FORCE_INLINE constexpr rect_t(const P& rposition, const S& rsize) noexcept
        : position(rposition), size(rsize) {}

    template <typename X, typename Y, typename W, typename H>
    VX_FORCE_INLINE constexpr rect_t(X x, Y y, W width, H height) noexcept
        : position(x, y), size(width, height) {}

    template <typename U>
    VX_FORCE_INLINE constexpr rect_t(const vec<4, U>& r) noexcept
        : position(r.x, r.y), size(r.z, r.w) {}

    template <typename U>
    VX_FORCE_INLINE constexpr rect_t(const rect_t<U>& r) noexcept
        : position(r.position), size(r.size) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const noexcept
    {
        return vec<4, U>(
            static_cast<U>(position.x),
            static_cast<U>(position.y),
            static_cast<U>(size.x),
            static_cast<U>(size.y)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& r) noexcept
    {
        position = r.position;
        size = r.size;
        return *this;
    }

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

    VX_FORCE_INLINE constexpr scalar_type& operator[](size_t i) noexcept
    {
        VX_ASSERT(i < 4);

        switch (i)
        {
            default:
            case 0: return position.x;
            case 1: return position.y;
            case 2: return size.x;
            case 3: return size.y;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < 4);

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
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& r1, const type& r2) noexcept
    {
        return r1.position == r2.position && r1.size == r2.size;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& r1, const type& r2) noexcept
    {
        return r1.position != r2.position || r1.size != r2.size;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // get anchors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr scalar_type x()		        const noexcept { return position.x; }
    VX_FORCE_INLINE constexpr scalar_type y()		        const noexcept { return position.y; }
    VX_FORCE_INLINE constexpr scalar_type width()	        const noexcept { return size.x; }
    VX_FORCE_INLINE constexpr scalar_type height()          const noexcept { return size.y; }

    VX_FORCE_INLINE constexpr scalar_type left()	        const noexcept { return position.x; }
    VX_FORCE_INLINE constexpr scalar_type right()	        const noexcept { return position.x + size.x; }
    VX_FORCE_INLINE constexpr scalar_type top()	            const noexcept { return position.y; }
    VX_FORCE_INLINE constexpr scalar_type bottom()          const noexcept { return position.y + size.y; }
    VX_FORCE_INLINE constexpr scalar_type centerx()         const noexcept { return position.x + size.x / static_cast<scalar_type>(2); }
    VX_FORCE_INLINE constexpr scalar_type centery()         const noexcept { return position.y + size.y / static_cast<scalar_type>(2); }

    VX_FORCE_INLINE constexpr point_type topleft()          const noexcept { return position; }
    VX_FORCE_INLINE constexpr point_type midtop()           const noexcept { return point_type(position.x + size.x / static_cast<scalar_type>(2), position.y); }
    VX_FORCE_INLINE constexpr point_type topright()         const noexcept { return point_type(position.x + size.x, position.y); }
    VX_FORCE_INLINE constexpr point_type midright()         const noexcept { return point_type(position.x + size.x, position.y + size.y / static_cast<scalar_type>(2)); }
    VX_FORCE_INLINE constexpr point_type bottomright()      const noexcept { return position + size; }
    VX_FORCE_INLINE constexpr point_type midbottom()        const noexcept { return point_type(position.x + size.x / static_cast<scalar_type>(2), position.y + size.y); }
    VX_FORCE_INLINE constexpr point_type bottomleft()       const noexcept { return point_type(position.x, position.y + size.y); }
    VX_FORCE_INLINE constexpr point_type midleft()          const noexcept { return point_type(position.x, position.y + size.y / static_cast<scalar_type>(2)); }
    VX_FORCE_INLINE constexpr point_type center()           const noexcept { return position + size / static_cast<scalar_type>(2); }

    ///////////////////////////////////////////////////////////////////////////////
    // set anchors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr void set_left(scalar_type left)                           noexcept { position.x = left; }
    VX_FORCE_INLINE constexpr void set_right(scalar_type right)                         noexcept { position.x = right - size.x; }
    VX_FORCE_INLINE constexpr void set_top(scalar_type top)                             noexcept { position.y = top; }
    VX_FORCE_INLINE constexpr void set_bottom(scalar_type bottom)                       noexcept { position.y = bottom - size.y; }
    VX_FORCE_INLINE constexpr void set_centerx(scalar_type centerx)                     noexcept { position.x = centerx - size.x / static_cast<scalar_type>(2); }
    VX_FORCE_INLINE constexpr void set_centery(scalar_type centery)                     noexcept { position.y = centery - size.y / static_cast<scalar_type>(2); }

    VX_FORCE_INLINE constexpr void set_x(scalar_type x)                                 noexcept { position.x = x; }
    VX_FORCE_INLINE constexpr void set_y(scalar_type y)                                 noexcept { position.y = y; }
    VX_FORCE_INLINE constexpr void set_width(scalar_type width)                         noexcept { size.x = width; }
    VX_FORCE_INLINE constexpr void set_height(scalar_type height)                       noexcept { size.y = height; }

    VX_FORCE_INLINE constexpr void set_topleft(const point_type& topleft)               noexcept { position = topleft; }
    VX_FORCE_INLINE constexpr void set_midtop(const point_type& midtop)                 noexcept { position.x = midtop.x - size.x / static_cast<scalar_type>(2); position.y = midtop.y; }
    VX_FORCE_INLINE constexpr void set_topright(const point_type& topright)             noexcept { position.x = topright.x - size.x; position.y = topright.y; }
    VX_FORCE_INLINE constexpr void set_midright(const point_type& midright)             noexcept { position.x = midright.x - size.x; position.y = midright.y - size.y / static_cast<scalar_type>(2); }
    VX_FORCE_INLINE constexpr void set_bottomright(const point_type& bottomright)       noexcept { position = bottomright - size; }
    VX_FORCE_INLINE constexpr void set_midbottom(const point_type& midbottom)           noexcept { position.x = midbottom.x - size.x / static_cast<scalar_type>(2); position.y = midbottom.y - size.y; }
    VX_FORCE_INLINE constexpr void set_bottomleft(const point_type& bottomleft)         noexcept { position.x = bottomleft.x; position.y = bottomleft.y - size.y; }
    VX_FORCE_INLINE constexpr void set_midleft(const point_type& midleft)               noexcept { position.x = midleft.x; position.y = midleft.y - size.y / static_cast<scalar_type>(2); }
    VX_FORCE_INLINE constexpr void set_center(const point_type& center)                 noexcept { position = center - size / static_cast<scalar_type>(2); }

    ///////////////////////////////////////////////////////////////////////////////
    // sides
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr line_type top_side()          const noexcept { return line_type(topleft(), topright()); }
    VX_FORCE_INLINE constexpr line_type right_side()        const noexcept { return line_type(topright(), bottomright()); }
    VX_FORCE_INLINE constexpr line_type bottom_side()       const noexcept { return line_type(bottomright(), bottomleft()); }
    VX_FORCE_INLINE constexpr line_type left_side()         const noexcept { return line_type(bottomleft(), topleft()); }

    ///////////////////////////////////////////////////////////////////////////////
    // attributes
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr size_t side_count() noexcept { return 4; }
    static VX_FORCE_INLINE constexpr size_t vertex_count() noexcept { return 4; }

    VX_FORCE_INLINE constexpr line_type get_side(size_t i) const noexcept
    {
        switch (i % side_count())
        {
            default:
            case 0:     return top_side();
            case 1:     return right_side();
            case 2:     return bottom_side();
            case 3:     return left_side();
        }
    }

    VX_FORCE_INLINE constexpr scalar_type area() const noexcept { return size.x * size.y; }
    VX_FORCE_INLINE constexpr bool empty() const noexcept { return area() == static_cast<scalar_type>(0); }
    VX_FORCE_INLINE constexpr scalar_type perimeter() const noexcept { return static_cast<scalar_type>(2) * (size.x + size.y); }
};

} // namespace geometry2d
} // namespace math
} // namespace vx