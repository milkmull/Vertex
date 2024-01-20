#pragma once

#include "base_type_defs.h"
#include "../../math/fn_common.h"
#include "../../math/fn_comparison.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct rect<T, val_t::integral>
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

private:

    using FT = typename detail::to_float_type<T>::type;

public:

    using value_type = T;
    using float_value_type = FT;

    using type = rect<T, val_t::integral>;
    using float_type = rect<FT, val_t::floating_point>;

    using vec_type = vec<2, T, vec_t::vec, val_t::integral>;

    using size_type = length_type;

    using iterator = detail::iterator<T>;
    using const_iterator = detail::iterator<const T>;
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

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr explicit rect(
        const vecx<2, U>& position,
        const vecx<2, U>& size
    )
        : position(position), size(size) {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr rect(U x, U y, U width, U height)
        : position(static_cast<T>(x), static_cast<T>(y))
        , size(static_cast<T>(width), static_cast<T>(height)) {}

    template <typename U>
    inline constexpr explicit rect(const rectx<U>& r)
        : position(r.position), size(r.size) {}

    inline constexpr float_type to_float() const
    {
        return float_type(*this);
    }

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

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "{ " << position.to_string() << ", " << size.to_string() << " }";
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

    inline constexpr vec_type topleft() const
    {
        return position;
    }

    inline constexpr vec_type midtop() const
    {
        return vec_type(
            position.x + size.x / static_cast<T>(2),
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
            position.y + size.y / static_cast<T>(2)
        );
    }

    inline constexpr vec_type bottomright() const
    {
        return posiion + size;
    }

    inline constexpr vec_type midbottom() const
    {
        return vec_type(
            position.x + size.x / static_cast<T>(2),
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
            position.y + size.y / static_cast<T>(2)
        );
    }

    inline constexpr vec_type center() const
    {
        return position + size / static_cast<T>(2);
    }

    inline constexpr void set_topleft(const vec_type& topleft)
    {
        position = topleft;
    }

    inline constexpr void set_midtop(const vec_type& midtop)
    {
        position.x = midtop.x - size.x / static_cast<T>(2);
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
        position.y = midright.y - size.y / static_cast<T>(2);
    }

    inline constexpr void set_bottomright(const vec_type& bottomright)
    {
        position = bottomright - size;
    }

    inline constexpr void set_midbottom(const vec_type& midbottom)
    {
        position.x = midbottom.x - size.x / static_cast<T>(2);
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
        position.y = midleft.y - size.y / static_cast<T>(2);
    }

    inline constexpr void set_center(const vec_type& center)
    {
        position = center - size / static_cast<T>(2);
    }

    // =============== common getters ===============

    inline constexpr T min_x() const { return math::min(positition.x, right()); }
    inline constexpr T max_x() const { return math::max(positition.x, right()); }

    inline constexpr T min_y() const { return math::min(positition.y, bottom()); }
    inline constexpr T max_y() const { return math::max(positition.y, bottom()); }

    inline constexpr FT aspect() const { return size.aspect(); }
    inline constexpr T area() const { return size.x * size.y; }

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

    inline constexpr type scale(T s) const
    {
        return scale(vec_type(s));
    }

    inline constexpr type scale(T sx, T sy) const
    {
        return scale(vec_type(sx, sy));
    }

    inline constexpr type scale(const vec_type& dv) const
    {
        return type(
            position,
            size * dv
        );
    }

    inline constexpr type move(T dx, T dy) const
    {
        return move(vec_type(dx, dy));
    }

    inline constexpr type move(const vec_type& dv) const
    {
        return type(
            position + dv,
            size
        );
    }

    inline constexpr type inflate(T ix, T iy) const
    {
        return inflate(vec_type(ix, iy));
    }

    inline constexpr type inflate(const vec_type& iv) const
    {
        return type(
            position - iv,
            size + static_cast<T>(2) * iv
        );
    }

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

    inline constexpr type grow_sides(T left, T right, T bottom, T top) const
    {
        return type(
            position.x - left,
            position.y - top,
            size.x + left + right,
            size.y + bottom + top
        );
    }

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

    inline constexpr bool collide(const type& r)
    {
        return position.x < r.position.x + r.size.x
            && position.x + size.x > r.position.x
            && position.y < r.position.y + r.size.y
            && position.y + size.y > r.position.y;
    }

    inline constexpr bool collide(
        const type& r,
        bool include_borders
    )
    {
        if (!include_borders) return collide(r);
        return position.x <= r.position.x + r.size.x
            && position.x + size.x >= r.position.x
            && position.y <= r.position.y + r.size.y
            && position.y + size.y >= r.position.y;
    }

    inline constexpr bool collide(const vec_type& p)
    {
        return position.x < p.x
            && position.x + size.x > p.x
            && position.y < p.y
            && position.y + size.y > p.y;
    }

    inline constexpr bool collide(
        const vec_type& p,
        bool include_borders
    )
    {
        if (!include_borders) return collide(p);
        return position.x <= p.x
            && position.x + size.x >= p.x
            && position.y <= p.y
            && position.y + size.y >= p.y;
    }

    inline constexpr bool collide(
        T x, T y,
        bool include_borders
    )
    {
        return collide(vec_type(x, y), include_borders);
    }

    inline constexpr bool contains(const type& r) const
    {
        return position.x < r.position.x
            && position.x + size.x > r.position.x + r.size.x
            && position.y < r.position.y
            && position.y + size.y > r.position.y + r.size.y;
    }

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

}
}
}