#pragma once

#include "../detail/base_types.h"
#include "../fn/color_fn_common.h"
#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {

template <size_type L, typename T> struct vec;

VX_PACK_PUSH()

template <typename T>
struct colorf_t
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using type = colorf_t<T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = colorf_t<float_value_type>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr T MIN_CHANNEL_VALUE = static_cast<T>(0);
    static constexpr T MAX_CHANNEL_VALUE = static_cast<T>(1);

    // =============== data ===============

    T r, g, b, a;

    // =============== implicit constructors ===============

    inline constexpr colorf_t()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(static_cast<T>(1)) {}

    inline constexpr colorf_t(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    inline constexpr colorf_t(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit colorf_t(T scaler)
        : r(scaler), g(scaler), b(scaler), a(scaler) {}

    inline constexpr colorf_t(T r, T g, T b, T a = static_cast<T>(1))
        : r(r), g(g), b(b), a(a) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit colorf_t(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(static_cast<T>(scaler)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr colorf_t(U r, U g, U b, U a = static_cast<U>(1))
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename U>
    inline constexpr explicit colorf_t(const colorf_t<U>& c)
        : r(static_cast<T>(c.r))
        , g(static_cast<T>(c.g))
        , b(static_cast<T>(c.b))
        , a(static_cast<T>(c.a)) {}

    template <typename U>
    inline constexpr colorf_t(const colori_t<U>& c)
        : r(static_cast<T>(c.r) / static_cast<T>(colori_t<U>::MAX_CHANNEL_VALUE))
        , g(static_cast<T>(c.g) / static_cast<T>(colori_t<U>::MAX_CHANNEL_VALUE))
        , b(static_cast<T>(c.b) / static_cast<T>(colori_t<U>::MAX_CHANNEL_VALUE))
        , a(static_cast<T>(c.a) / static_cast<T>(colori_t<U>::MAX_CHANNEL_VALUE)) {}

    template <typename U>
    inline constexpr explicit colorf_t(const vec<3, U>& v, U a = static_cast<U>(1))
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(a)) {}
    
    template <typename U>
    inline constexpr explicit colorf_t(const vec<4, U>& v)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(v.w)) {}

    // =============== destructor ===============

    ~colorf_t() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 4);
        return (&r)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 4);
        return (&r)[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& c1, const type& c2)
    {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
    }

    friend inline constexpr bool operator!=(const type& c1, const type& c2)
    {
        return !(c1 == c2);
    }

    friend inline constexpr bool operator<(const type& c1, const type& c2)
    {
        if (c1.r != c2.r) return (c1.r < c2.r);
        if (c1.g != c2.g) return (c1.g < c2.g);
        if (c1.b != c2.b) return (c1.b < c2.b);
        else              return (c1.a < c2.a);
    }

    friend inline constexpr bool operator>(const type& c1, const type& c2)
    {
        if (c1.r != c2.r) return (c1.r > c2.r);
        if (c1.g != c2.g) return (c1.g > c2.g);
        if (c1.b != c2.b) return (c1.b > c2.b);
        else              return (c1.a > c2.a);
    }

    friend inline constexpr bool operator<=(const type& c1, const type& c2)
    {
        if (c1.r != c2.r) return (c1.r <  c2.r);
        if (c1.g != c2.g) return (c1.g <  c2.g);
        if (c1.b != c2.b) return (c1.b <  c2.b);
        else              return (c1.a <= c2.a);
    }

    friend inline constexpr bool operator>=(const type& c1, const type& c2)
    {
        if (c1.r != c2.r) return (c1.r >  c2.r);
        if (c1.g != c2.g) return (c1.g >  c2.g);
        if (c1.b != c2.b) return (c1.b >  c2.b);
        else              return (c1.a >= c2.a);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return type(+r, +g, +b, +a);
    }

    inline constexpr type operator-() const
    {
        return type(-r, -g, -b, -a);
    }

    // =============== incrememnt and decrement operators ===============

    // incrememnt (++)

    inline constexpr type& operator++()
    {
        r++;
        g++;
        b++;
        a++;
        return *this;
    }

    inline constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    inline constexpr type& operator--()
    {
        r--;
        g--;
        b--;
        a--;
        return *this;
    }

    inline constexpr type operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    // =============== binary arithmetic operators ===============

    // addition (+)

    friend inline constexpr type operator+(const type& c, T scaler)
    {
        return type(c.r + scaler, c.g + scaler, c.b + scaler, c.a + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& c)
    {
        return type(scaler + c.r, scaler + c.g, scaler + c.b, scaler + c.a);
    }

    friend inline constexpr type operator+(const type& c1, const type& c2)
    {
        return type(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& c, T scaler)
    {
        return type(c.r - scaler, c.g - scaler, c.b - scaler, c.a - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& c)
    {
        return type(scaler - c.r, scaler - c.g, scaler - c.b, scaler - c.a);
    }

    friend inline constexpr type operator-(const type& c1, const type& c2)
    {
        return type(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& c, T scaler)
    {
        return type(c.r * scaler, c.g * scaler, c.b * scaler, c.a * scaler);
    }

    friend inline constexpr type operator*(T scaler, const type& c)
    {
        return type(scaler * c.r, scaler * c.g, scaler * c.b, scaler * c.a);
    }

    friend inline constexpr type operator*(const type& c1, const type& c2)
    {
        return type(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
    }

    // division (/)

    friend inline constexpr type operator/(const type& c, T scaler)
    {
        return type(c.r / scaler, c.g / scaler, c.b / scaler, c.a / scaler);
    }

    friend inline constexpr type operator/(T scaler, const type& c)
    {
        return type(scaler / c.r, scaler / c.g, scaler / c.b, scaler / c.a);
    }

    friend inline constexpr type operator/(const type& c1, const type& c2)
    {
        return type(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b, c1.a / c2.a);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        r += static_cast<T>(scaler);
        g += static_cast<T>(scaler);
        b += static_cast<T>(scaler);
        a += static_cast<T>(scaler);
        return *this;
    }

    inline constexpr type& operator+=(const type& v)
    {
        r += static_cast<T>(v.r);
        g += static_cast<T>(v.g);
        b += static_cast<T>(v.b);
        a += static_cast<T>(v.a);
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        r -= static_cast<T>(scaler);
        g -= static_cast<T>(scaler);
        b -= static_cast<T>(scaler);
        a -= static_cast<T>(scaler);
        return *this;
    }

    inline constexpr type& operator-=(const type& v)
    {
        r -= static_cast<T>(v.r);
        g -= static_cast<T>(v.g);
        b -= static_cast<T>(v.b);
        a -= static_cast<T>(v.a);
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        r *= static_cast<T>(scaler);
        g *= static_cast<T>(scaler);
        b *= static_cast<T>(scaler);
        a *= static_cast<T>(scaler);
        return *this;
    }

    inline constexpr type& operator*=(const type& v)
    {
        r *= static_cast<T>(v.r);
        g *= static_cast<T>(v.g);
        b *= static_cast<T>(v.b);
        a *= static_cast<T>(v.a);
        return *this;
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        r /= static_cast<T>(scaler);
        g /= static_cast<T>(scaler);
        b /= static_cast<T>(scaler);
        a /= static_cast<T>(scaler);
        return *this;
    }

    inline constexpr type& operator/=(const type& v)
    {
        r /= static_cast<T>(v.r);
        g /= static_cast<T>(v.g);
        b /= static_cast<T>(v.b);
        a /= static_cast<T>(v.a);
        return *this;
    }

    // =============== iterator ===============

    inline constexpr iterator begin() noexcept
    {
        return iterator(&r);
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    inline constexpr iterator end() noexcept
    {
        return iterator(&a + 1);
    }

    inline constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    inline constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(&r);
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return const_iterator(&a + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&a + 1);
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    inline constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(&r);
    }

    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&a + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&r);
    }

    // =============== string ===============

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "rgba(" << +r << ", " << +g << ", " << +b << ", " << +a << ")";
        return oss.str();
    }

    // =============== comparison and testing ===============

    /**
     * @brief Get the minimum color component value of the color.
     *
     * This function returns the minimum value among the RGB components of the color.
     *
     * @return The minimum color component value.
     */
    inline constexpr T min_color() const { return math::min({ r, g, b }); }

    /**
     * @brief Get the maximum color component value of the color.
     *
     * This function returns the maximum value among the RGB components of the color.
     *
     * @return The maximum color component value.
     */
    inline constexpr T max_color() const { return math::max({ r, g, b }); }

    // =============== color attributes ===============

    inline constexpr type clamp() const
    {
        return math::clamp(*this);
    }

    // =============== color attributes ===============

    /**
     * @brief Calculate the luminance of the color.
     *
     * This function computes the luminance of the color.
     * Luminance represents the perceived brightness of the color, taking into account
     * human vision sensitivity to different color channels. The coefficients used in
     * the formula correspond to the standard luminance weights for the RGB color space.
     *
     * @return The luminance value.
     */
    static inline constexpr T luminance(const type& c)
    {
        return (
            (c.r * static_cast<T>(0.2126)) +
            (c.g * static_cast<T>(0.7152)) +
            (c.b * static_cast<T>(0.0722))
        );
    }

    static inline constexpr T grayscale(const type& c)
    {
        return (c.r + c.g + c.b) / static_cast<T>(3);
    }

    // =============== color modifiers ===============

    /**
     * @brief Lighten the color by a specified amount.
     *
     * This function returns a new color by lightening each component based on the provided amount.
     *
     * @param amount The amount by which to lighten the color (in the range [0, 1]).
     * @return A new color with lightened components.
     */
    static inline constexpr type lighten(const type& c, T amount)
    {
        return type(
            c.r + (static_cast<T>(1) - c.r) * amount,
            c.g + (static_cast<T>(1) - c.g) * amount,
            c.b + (static_cast<T>(1) - c.b) * amount,
            c.a
        );
    }

    /**
     * @brief Darken the color by a specified amount.
     *
     * This function returns a new color by darkening each component based on the provided amount.
     *
     * @param amount The amount by which to darken the color (in the range [0, 1]).
     * @return A new color with darkened components.
     */
    static inline constexpr type darken(const type& c, T amount)
    {
        return type(
            c.r * (static_cast<T>(1) - amount),
            c.g * (static_cast<T>(1) - amount),
            c.b * (static_cast<T>(1) - amount),
            c.a
        );
    }

    /**
     * @brief Invert the color components.
     *
     * This function returns a new color with inverted components, where each component
     * is replaced by its reciprocal (1 / component).
     *
     * @return A new color with inverted components.
     */
    static inline constexpr type invert() const
    {
        return type(
            static_cast<T>(1) - c.r,
            static_cast<T>(1) - c.g,
            static_cast<T>(1) - c.b,
            c.a
        );
    }

    // =============== colors ===============

    static inline constexpr type CLEAR()   { return type(static_cast<T>(0)); }
    static inline constexpr type WHITE()   { return type(static_cast<T>(1)); }
    static inline constexpr type BLACK()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type GREY()    { return type(static_cast<T>(0.5), static_cast<T>(0.5), static_cast<T>(0.5)); }

    static inline constexpr type RED()     { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type GREEN()   { return type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)); }
    static inline constexpr type BLUE()    { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }

    static inline constexpr type YELLOW()  { return type(static_cast<T>(1), static_cast<T>(1), static_cast<T>(0)); }
    static inline constexpr type MAGENTA() { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(1)); }
    static inline constexpr type CYAN()	   { return type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(1)); }

};

VX_PACK_POP()

using color = colorf_t<float>;

}
}