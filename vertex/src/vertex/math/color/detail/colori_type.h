#pragma once

#include <sstream>

#include "../../math/math.h"

namespace vx {
namespace math {
namespace detail {

VX_PACK_PUSH()

template <typename T>
struct vec<4, T, vec_t::col, val_t::integral>
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

private:

    using FT = typename detail::to_float_type<float>::type;

public:

    using value_type = T;
    using float_value_type = FT;

    using type = vec<4, T, vec_t::col, val_t::integral>;
    using float_type = vec<4, FT, vec_t::col, val_t::floating_point>;

    using size_type = length_type;
    static inline constexpr size_type size() { return 4; }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

    static constexpr T MIN_CHANNEL_VALUE = std::numeric_limits<T>::min();
    static constexpr T MAX_CHANNEL_VALUE = std::numeric_limits<T>::max();

    // =============== data ===============

    T r, g, b, a;

    // =============== implicit constructors ===============

    inline constexpr vec()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(MAX_CHANNEL_VALUE) {}

    inline constexpr vec(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler)
        : r(scaler), g(scaler), b(scaler), a(scaler) {}

    inline constexpr vec(T r, T g, T b, T a = MAX_CHANNEL_VALUE)
        : r(r), g(g), b(b), a(a) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit vec(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(static_cast<T>(scaler)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U r, U g, U b, U a = MAX_CHANNEL_VALUE)
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename U>
    inline constexpr explicit vec(const coli<U>& c)
        : type(c.normalize()) {}

    template <typename U>
    inline constexpr vec(const colf<U>& c)
        : r(static_cast<T>(math::clamp(static_cast<FT>(c.r), static_cast<FT>(0), static_cast<FT>(1)) * static_cast<FT>(MAX_CHANNEL_VALUE)))
        , g(static_cast<T>(math::clamp(static_cast<FT>(c.g), static_cast<FT>(0), static_cast<FT>(1)) * static_cast<FT>(MAX_CHANNEL_VALUE)))
        , b(static_cast<T>(math::clamp(static_cast<FT>(c.b), static_cast<FT>(0), static_cast<FT>(1)) * static_cast<FT>(MAX_CHANNEL_VALUE)))
        , a(static_cast<T>(math::clamp(static_cast<FT>(c.a), static_cast<FT>(0), static_cast<FT>(1)) * static_cast<FT>(MAX_CHANNEL_VALUE))) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<3, U>& v, U a = MAX_CHANNEL_VALUE)
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

    // modulo (%)

    friend inline constexpr type operator%(const type& c, T scaler)
    {
        return type(c.r % scaler, c.g % scaler, c.b % scaler, c.a % scaler);
    }

    friend inline constexpr type operator%(T scaler, const type& c)
    {
        return type(scaler % c.r, scaler % c.g, scaler % c.b, scaler % c.a);
    }

    friend inline constexpr type operator%(const type& c1, const type& c2)
    {
        return type(c1.r % c2.r, c1.g % c2.g, c1.b % c2.b, c1.a % c2.a);
    }

    // =============== binarg bit operators ===============

    // and (&)

    friend inline constexpr type operator&(const type& c, T scaler)
    {
        return type(c.r & scaler, c.g & scaler, c.b & scaler, c.a & scaler);
    }

    friend inline constexpr type operator&(T scaler, const type& c)
    {
        return type(scaler & c.r, scaler & c.g, scaler & c.b, scaler & c.a);
    }

    friend inline constexpr type operator&(const type& c1, const type& c2)
    {
        return type(c1.r & c2.r, c1.g & c2.g, c1.b & c2.b, c1.a & c2.a);
    }

    // or (|)

    friend inline constexpr type operator|(const type& c, T scaler)
    {
        return type(c.r | scaler, c.g | scaler, c.b | scaler, c.a | scaler);
    }

    friend inline constexpr type operator|(T scaler, const type& c)
    {
        return type(scaler | c.r, scaler | c.g, scaler | c.b, scaler | c.a);
    }

    friend inline constexpr type operator|(const type& c1, const type& c2)
    {
        return type(c1.r | c2.r, c1.g | c2.g, c1.b | c2.b, c1.a | c2.a);
    }

    // ror (^)

    friend inline constexpr type operator^(const type& c, T scaler)
    {
        return type(c.r ^ scaler, c.g ^ scaler, c.b ^ scaler, c.a ^ scaler);
    }

    friend inline constexpr type operator^(T scaler, const type& c)
    {
        return type(scaler ^ c.r, scaler ^ c.g, scaler ^ c.b, scaler ^ c.a);
    }

    friend inline constexpr type operator^(const type& c1, const type& c2)
    {
        return type(c1.r ^ c2.r, c1.g ^ c2.g, c1.b ^ c2.b, c1.a ^ c2.a);
    }

    // left shift (<<)

    friend inline constexpr type operator<<(const type& c, T scaler)
    {
        return type(c.r << scaler, c.g << scaler, c.b << scaler, c.a << scaler);
    }

    friend inline constexpr type operator<<(T scaler, const type& c)
    {
        return type(scaler << c.r, scaler << c.g, scaler << c.b, scaler << c.a);
    }

    friend inline constexpr type operator<<(const type& c1, const type& c2)
    {
        return type(c1.r << c2.r, c1.g << c2.g, c1.b << c2.b, c1.a << c2.a);
    }

    // right shift (>>)

    friend inline constexpr type operator>>(const type& c, T scaler)
    {
        return type(c.r >> scaler, c.g >> scaler, c.b >> scaler, c.a >> scaler);
    }

    friend inline constexpr type operator>>(T scaler, const type& c)
    {
        return type(scaler >> c.r, scaler >> c.g, scaler >> c.b, scaler >> c.a);
    }

    friend inline constexpr type operator>>(const type& c1, const type& c2)
    {
        return type(c1.r >> c2.r, c1.g >> c2.g, c1.b >> c2.b, c1.a >> c2.a);
    }

    // not (~)

    friend inline constexpr type operator~(const type& c)
    {
        return type(~c.r, ~c.g, ~c.b, ~c.a);
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

    // modulo (%=)

    inline constexpr type& operator%=(T scaler)
    {
        r %= scaler;
        g %= scaler;
        b %= scaler;
        a %= scaler;
        return *this;
    }

    inline constexpr type& operator%=(const type& c)
    {
        r %= c.r;
        g %= c.g;
        b %= c.b;
        a %= c.a;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    inline constexpr type& operator&=(T scaler)
    {
        r &= scaler;
        g &= scaler;
        b &= scaler;
        a &= scaler;
        return *this;
    }

    inline constexpr type& operator&=(const type& c)
    {
        r &= c.r;
        g &= c.g;
        b &= c.b;
        a &= c.a;
        return *this;
    }

    // or (|=)

    inline constexpr type& operator|=(T scaler)
    {
        r |= scaler;
        g |= scaler;
        b |= scaler;
        a |= scaler;
        return *this;
    }

    inline constexpr type& operator|=(const type& c)
    {
        r |= c.r;
        g |= c.g;
        b |= c.b;
        a |= c.a;
        return *this;
    }

    // xor (^=)

    inline constexpr type& operator^=(T scaler)
    {
        r ^= scaler;
        g ^= scaler;
        b ^= scaler;
        a ^= scaler;
        return *this;
    }

    inline constexpr type& operator^=(const type& c)
    {
        r ^= c.r;
        g ^= c.g;
        b ^= c.b;
        a ^= c.a;
        return *this;
    }

    // left shift (<<=)

    inline constexpr type& operator<<=(T scaler)
    {
        r <<= scaler;
        g <<= scaler;
        b <<= scaler;
        a <<= scaler;
        return *this;
    }

    inline constexpr type& operator<<=(const type& c)
    {
        r <<= c.r;
        g <<= c.g;
        b <<= c.b;
        a <<= c.a;
        return *this;
    }

    // right shift (>>=)

    inline constexpr type& operator>>=(T scaler)
    {
        r >>= scaler;
        g >>= scaler;
        b >>= scaler;
        a >>= scaler;
        return *this;
    }

    inline constexpr type& operator>>=(const type& c)
    {
        r >>= c.r;
        g >>= c.g;
        b >>= c.b;
        a >>= c.a;
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

    inline constexpr float_type normalize() const
    {
        constexpr FT inv_max_channel_value = static_cast<FT>(1) / static_cast<FT>(MAX_CHANNEL_VALUE);

        return float_type(
            static_cast<FT>(r) * inv_max_channel_value,
            static_cast<FT>(g) * inv_max_channel_value,
            static_cast<FT>(b) * inv_max_channel_value,
            static_cast<FT>(a) * inv_max_channel_value
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
    inline constexpr FT luminance() const
    {
        return float_type(*this).luminance();
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
            MAX_CHANNEL_VALUE - r,
            MAX_CHANNEL_VALUE - g,
            MAX_CHANNEL_VALUE - b,
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
        return type(
            (hex >>  0) & MAX_CHANNEL_VALUE,
            (hex >>  8) & MAX_CHANNEL_VALUE,
            (hex >> 16) & MAX_CHANNEL_VALUE,
            (hex >> 24) & MAX_CHANNEL_VALUE
        );
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
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    // =============== colors ===============

    static inline constexpr type CLEAR()   { return type(static_cast<T>(0)); }
    static inline constexpr type WHITE()   { return type(MAX_CHANNEL_VALUE); }
    static inline constexpr type BLACK()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type GREY()    { return type(MAX_CHANNEL_VALUE / static_cast<T>(2)); }

    static inline constexpr type RED()     { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type GREEN()   { return type(static_cast<T>(0), MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static inline constexpr type BLUE()    { return type(static_cast<T>(0), static_cast<T>(0), MAX_CHANNEL_VALUE); }

    static inline constexpr type YELLOW()  { return type(MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static inline constexpr type MAGENTA() { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), MAX_CHANNEL_VALUE); }
    static inline constexpr type CYAN()	   { return type(static_cast<T>(0), MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE); }

};

VX_PACK_POP()

}

using color8 = detail::vec<4, uint8_t, detail::vec_t::col>;

}
}