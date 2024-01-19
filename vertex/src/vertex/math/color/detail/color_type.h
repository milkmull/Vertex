#pragma once

#include <sstream>

#include "../../math/detail/base_type_defs.h"
#include "../../math/fn_common.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct vec<4, T, vec_t::col, val_t::floating_point>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;

    using type = vec<4, T, vec_t::col, val_t::floating_point>;

    using vec3_type = vec<3, T, vec_t::vec, val_t::floating_point>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::floating_point>;

    using size_type = length_type;
    static inline constexpr size_type size() { return 4; }

    using iterator = detail::iterator<T>;
    using const_iterator = detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    T r, g, b, a;

    // =============== implicit constructors ===============

    inline constexpr vec()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(static_cast<T>(1)) {}

    inline constexpr vec(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler)
        : r(scaler), g(scaler), b(scaler), a(scaler) {}

    inline constexpr vec(T r, T g, T b, T a = static_cast<T>(1))
        : r(r), g(g), b(b), a(a) {}

    // =============== conversion vector constructors ===============

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr explicit vec(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(static_cast<T>(scaler)) {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr vec(U r, U g, U b, U a = static_cast<U>(1))
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename U>
    inline constexpr explicit vec(const col<U>& c)
        : r(static_cast<T>(c.r))
        , g(static_cast<T>(c.g))
        , b(static_cast<T>(c.b))
        , a(static_cast<T>(c.a)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<3, U>& v, U a = static_cast<U>(1))
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(a)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<4, U>& v)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(v.w)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

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
        return *this;
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

    // =============== swizzle ===============

    inline constexpr type swizzle(size_type i, size_type j, size_type k) const
    {
        return type(operator[](i), operator[](j), operator[](k), a);
    }

    inline constexpr type swizzle(size_type i, size_type j, size_type k, size_type l) const
    {
        return type(operator[](i), operator[](j), operator[](k), operator[](l));
    }

    // =============== string ===============

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "rgba(" << r << ", " << g << ", " << b << ", " << a << ")";
        return oss.str();
    }

    // =============== comparison and testing ===============

    inline constexpr void set(T nrgba) { r = g = b = a = nrgba; }
    inline constexpr void set(T nr, T ng, T nb) { r = nr; g = ng; b = nb; }
    inline constexpr void set(T nr, T ng, T nb, T na) { r = nr; g = ng; b = nb; a = na; }

    /**
     * @brief Get the minimum component value of the color.
     *
     * This function returns the minimum value among the components of the color.
     *
     * @return The minimum component value.
     */
    inline constexpr T min() const { return math::min({ r, g, b, a }); }

    /**
     * @brief Get the minimum color component value of the color.
     *
     * This function returns the minimum value among the RGB components of the color.
     *
     * @return The minimum color component value.
     */
    inline constexpr T min_color() const { return math::min({ r, g, b }); }

    /**
     * @brief Get the maximum component value of the color.
     *
     * This function returns the maximum value among the components of the color.
     *
     * @return The maximum component value.
     */
    inline constexpr T max() const { return math::max({ r, g, b, a }); }

    /**
     * @brief Get the maximum color component value of the color.
     *
     * This function returns the maximum value among the RGB components of the color.
     *
     * @return The maximum color component value.
     */
    inline constexpr T max_color() const { return math::max({ r, g, b }); }

    // =============== magnitude ===============

    inline constexpr T magnitude_squared() const { return (r * r) + (g * g) + (b * b) + (a * a); }
    inline constexpr T magnitude() const { return math::sqrt((r * r) + (g * g) + (b * b) + (a * a)); }

    /**
     * @brief Clamp the color components to the range [0, 1].
     *
     * This function clamps each component of the color to the range [0, 1].
     *
     * @return A new color with clamped components.
     */
    inline constexpr type clamp() const
    {
        return type(
            math::clamp(r, static_cast<T>(0), static_cast<T>(1)),
            math::clamp(g, static_cast<T>(0), static_cast<T>(1)),
            math::clamp(b, static_cast<T>(0), static_cast<T>(1)),
            math::clamp(a, static_cast<T>(0), static_cast<T>(1))
        );
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
    inline constexpr T luminance() const
    {
        return (r * static_cast<T>(0.2126)) + (g * static_cast<T>(0.7152)) + (b * static_cast<T>(0.0722));
    }

    // =============== color modifiers ===============

    /**
     * @brief Invert the color components.
     *
     * This function returns a new color with inverted components, where each component
     * is replaced by its reciprocal (1 / component).
     *
     * @return A new color with inverted components.
     */
    inline constexpr type invert() const
    {
        return type(
            static_cast<T>(1) / r,
            static_cast<T>(1) / g,
            static_cast<T>(1) / b,
            a
        );
    }

    /**
     * @brief Lighten the color by a specified amount.
     *
     * This function returns a new color by lightening each component based on the provided amount.
     *
     * @param amount The amount by which to lighten the color (in the range [0, 1]).
     * @return A new color with lightened components.
     */
    inline constexpr type lighten(T amount) const
    {
        return type(
            r + (static_cast<T>(1) - r) * amount,
            g + (static_cast<T>(1) - g) * amount,
            b + (static_cast<T>(1) - b) * amount,
            a
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
    inline constexpr type darken(T amount) const
    {
        return type(
            r * (static_cast<T>(1) - amount),
            g * (static_cast<T>(1) - amount),
            b * (static_cast<T>(1) - amount),
            a
        );
    }

    // =============== hex ===============

    /**
     * @brief Create a color from a 32-bit hexadecimal value.
     *
     * This function creates a color by extracting RGBA components from a 32-bit
     * hexadecimal value and normalizing them to the range [0, 1].
     *
     * @param hex The 32-bit hexadecimal value representing the color.
     * @return A new color created from the hexadecimal value.
     */
    static inline constexpr type from_hex(uint32_t hex)
    {
        const T a = static_cast<T>((hex & 0xFF) / 255);
        hex >>= 8;
        const T b = static_cast<T>((hex & 0xFF) / 255);
        hex >>= 8;
        const T g = static_cast<T>((hex & 0xFF) / 255);
        hex >>= 8;
        const T r = static_cast<T>((hex & 0xFF) / 255);

        return type(r, g, b, a);
    }

    /**
     * @brief Convert the color to a 32-bit hexadecimal value.
     *
     * This function converts the color to a 32-bit hexadecimal value by clamping each
     * component to the range [0, 1] and then scaling them to the [0, 255] range.
     *
     * @return The 32-bit hexadecimal value representing the color.
     */
    inline constexpr uint32_t to_hex() const
    {
        const type cn = clamp();

        const uint8_t nr = static_cast<uint8_t>(cn.r * static_cast<T>(255));
        const uint8_t ng = static_cast<uint8_t>(cn.g * static_cast<T>(255));
        const uint8_t nb = static_cast<uint8_t>(cn.b * static_cast<T>(255));
        const uint8_t na = static_cast<uint8_t>(cn.a * static_cast<T>(255));

        return (nr << 24) | (ng << 16) | (nb << 8) | na;
    }

    // =============== hsv ===============

    // https://www.rapidtables.com/convert/color/hsv-to-rgb.html

    /**
     * @brief Create a color from HSV (Hue, Saturation, Value) values.
     *
     * This function creates a color using HSV values, where h is the hue in degrees,
     * s is the saturation, v is the value, and a is the alpha (default is 1). The function
     * clamps input values to their valid ranges and performs the HSV to RGB conversion.
     *
     * @param h The hue value in degrees (range: [0, 360]).
     * @param s The saturation value (range: [0, 1]).
     * @param v The value (brightness) value (range: [0, 1]).
     * @param a The alpha value (default is 1, range: [0, 1]).
     * @return A new color created from the HSV values.
     */
    static inline constexpr type from_hsv(T h, T s, T v, T a = static_cast<T>(1))
    {
        h = math::clamp(h, static_cast<T>(0), static_cast<T>(360));
        s = math::clamp(s, static_cast<T>(0), static_cast<T>(1));
        v = math::clamp(v, static_cast<T>(0), static_cast<T>(1));
        a = math::clamp(a, static_cast<T>(0), static_cast<T>(1));

        if (s == static_cast<T>(0))
        {
            return type(v, v, v, a);
        }

        const T sector = math::floor(h * static_cast<T>(0.0166666667));
        const T frac = (h * static_cast<T>(0.0166666667)) - sector;

        const T n = v;
        const T o = v * (static_cast<T>(1) - s);
        const T p = v * (static_cast<T>(1) - s * frac);
        const T q = v * (static_cast<T>(1) - s * (static_cast<T>(1) - frac));

        switch (static_cast<int>(sector))
        {
            case 0:		return type(n, q, o, a);
            case 1:		return type(p, n, o, a);
            case 2:		return type(o, n, q, a);
            case 3:		return type(o, p, n, a);
            case 4:		return type(q, o, n, a);
            case 5:		return type(n, o, p, a);
            default:	break;
        }

        return type();
    }

    /**
     * @brief Convert the color to HSV (Hue, Saturation, Value) values.
     *
     * This function calculates the HSV values from the color's RGB components.
     *
     * @return A 3D vector representing the HSV values (Hue in degrees, Saturation, Value).
     */
    static inline constexpr type from_hsv(const vec3_type& hsv, T a = static_cast<T>(1))
    {
        return from_hsv(hsv.x, hsv.y, hsv.z, a);
    }

    inline constexpr vec3_type to_hsv() const
    {
        const type cn = clamp();
        const T cmax = cn.max_color();

        if (cmax == static_cast<T>(0))
        {
            return vec3_type();
        }

        const T cmin = cn.min_color();
        const T v = cmax;

        if (cmin == cmax)
        {
            return vec3_type(static_cast<T>(0), static_cast<T>(0), v);
        }

        const T dc = cmax - cmin;
        T h, s;

        s = dc / cmax;

        if (cmax == cn.r)
        {
            h = static_cast<T>(0) + static_cast<T>(60) * (cn.g - cn.b) / dc;
        }
        else if (cmax == cn.g)
        {
            h = static_cast<T>(120) + static_cast<T>(60) * (cn.b - cn.r) / dc;
        }
        else if (cmax == cn.b)
        {
            h = static_cast<T>(240) + static_cast<T>(60) * (cn.r - cn.g) / dc;
        }
        else
        {
            h = static_cast<T>(0);
        }

        if (h < static_cast<T>(0))
        {
            h += static_cast<T>(360);
        }

        return vec3_type(h, s, v);
    }

    // =============== hsl ===============

    // https://www.rapidtables.com/convert/color/rgb-to-hsl.html

    /**
     * @brief Create a color from HSL (Hue, Saturation, Lightness) values.
     *
     * This function creates a color using HSL values, where h is the hue in degrees,
     * s is the saturation, l is the lightness, and a is the alpha (default is 1). The function
     * clamps input values to their valid ranges and performs the HSL to RGB conversion.
     *
     * @param h The hue value in degrees (range: [0, 360]).
     * @param s The saturation value (range: [0, 1]).
     * @param l The lightness value (range: [0, 1]).
     * @param a The alpha value (default is 1, range: [0, 1]).
     * @return A new color created from the HSL values.
     */
    static inline constexpr type from_hsl(T h, T s, T l, T a = static_cast<T>(1))
    {
        h = math::clamp(h, static_cast<T>(0), static_cast<T>(360));
        s = math::clamp(s, static_cast<T>(0), static_cast<T>(1));
        l = math::clamp(l, static_cast<T>(0), static_cast<T>(1));
        a = math::clamp(a, static_cast<T>(0), static_cast<T>(1));

        if (s == static_cast<T>(0))
        {
            return type(l, l, l, a);
        }

        const T sector = std::floor(h * static_cast<T>(0.0166666667));
        const T frac = (h * static_cast<T>(0.0166666667)) - sector;

        const T c = (static_cast<T>(1) - std::abs(static_cast<T>(2) * l - static_cast<T>(1))) * s;

        const T n = l + c * static_cast<T>(0.5);
        const T o = l - c * static_cast<T>(0.5);
        const T p = l + c * (static_cast<T>(0.5) - frac);
        const T q = l + c * (frac - static_cast<T>(0.5));

        switch (static_cast<int>(sector))
        {
            case 0:		return type(n, q, o, a);
            case 1:		return type(p, n, o, a);
            case 2:		return type(o, n, q, a);
            case 3:		return type(o, p, n, a);
            case 4:		return type(q, o, n, a);
            case 5:		return type(n, o, p, a);
            default:	break;
        }

        return type();
    }

    static inline constexpr type from_hsl(const vec3_type& hsl, T a = static_cast<T>(1))
    {
        return from_hsl(hsl.x, hsl.y, hsl.z, a);
    }

    /**
     * @brief Convert the color to HSL (Hue, Saturation, Lightness) values.
     *
     * This function calculates the HSL values from the color's RGB components.
     *
     * @return A 3D vector representing the HSL values (Hue in degrees, Saturation, Lightness).
     */
    inline constexpr vec3_type to_hsl() const
    {
        const type cn = clamp();
        const T cmax = cn.max_color();

        if (cmax == static_cast<T>(0))
        {
            return vec3_type();
        }

        const T cmin = cn.min_color();
        const T l = (cmin + cmax) * static_cast<T>(0.5);

        if (cmin == cmax)
        {
            return vec3_type(static_cast<T>(0), static_cast<T>(0), l);
        }

        const T dc = cmax - cmin;

        T h, s;

        s = (l > static_cast<T>(0.5)) ? dc * (static_cast<T>(0.5) - cmax - cmin) : dc / (cmax + cmin);

        if (cmax == cn.r)
        {
            h = static_cast<T>(0) + static_cast<T>(60) * (cn.g - cn.b) / dc;
        }
        else if (cmax == cn.g)
        {
            h = static_cast<T>(120) + static_cast<T>(60) * (cn.b - cn.r) / dc;
        }
        else if (cmax == cn.b)
        {
            h = static_cast<T>(240) + static_cast<T>(60) * (cn.r - cn.g) / dc;
        }
        else
        {
            h = static_cast<T>(0);
        }

        if (h < static_cast<T>(0))
        {
            h += static_cast<T>(360);
        }

        return vec3_type(h, s, l);
    }

    // =============== linear + srgb ===============

    // https://entropymine.com/imageworsener/srgbformula/

    /**
     * @brief Convert the color to sRGB color space.
     *
     * This function performs the conversion of the color to the sRGB color space.
     *
     * @return A new color representing the sRGB color space.
     */
    inline constexpr type to_srgb() const
    {
        const type cn = clamp();

        const T nr = (cn.r < static_cast<T>(0.0031308))
            ? (static_cast<T>(12.92) * cn.r)
            : (static_cast<T>(1.055)) * math::pow(cn.r, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
        const T ng = (cn.g < static_cast<T>(0.0031308))
            ? (static_cast<T>(12.92) * cn.g)
            : (static_cast<T>(1.055)) * math::pow(cn.g, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);
        const T nb = (cn.b < static_cast<T>(0.0031308))
            ? (static_cast<T>(12.92) * cn.b)
            : (static_cast<T>(1.055)) * math::pow(cn.b, static_cast<T>(0.4166666667)) - static_cast<T>(0.055);

        return type(nr, ng, nb, a);
    }

    /**
     * @brief Convert the color to linear color space.
     *
     * This function performs the conversion of the color to the linear color space.
     *
     * @return A new color representing the linear color space.
     */
    inline constexpr type to_linear() const
    {
        const type cn = clamp();

        const T nr = (cn.r < static_cast<T>(0.04045))
            ? (static_cast<T>(0.0773993808) * cn.r)
            : math::pow((cn.r + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
        const T ng = (cn.g < static_cast<T>(0.04045))
            ? (static_cast<T>(0.0773993808) * cn.g)
            : math::pow((cn.g + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));
        const T nb = (cn.b < static_cast<T>(0.04045))
            ? (static_cast<T>(0.0773993808) * cn.b)
            : math::pow((cn.b + static_cast<T>(0.055)) * static_cast<T>(0.9478672986), static_cast<T>(2.4));

        return type(nr, ng, nb, a);
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

}
}
}