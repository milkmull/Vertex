#pragma once

#include <limits>

#include "../detail/base_types.h"
#include "../fn/color_fn_common.h"
#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {

template <typename T> struct vec3_t;
template <typename T> struct vec4_t;

VX_PACK_PUSH()

template <typename T>
struct colori_t
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

    using value_type = T;
    using type = colori_t<T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = colorf_t<float_value_type>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr T MIN_CHANNEL_VALUE = static_cast<T>(0);
    static constexpr T MAX_CHANNEL_VALUE = std::numeric_limits<T>::max();

    // =============== data ===============

    T r, g, b, a;

    // =============== implicit constructors ===============

    inline constexpr colori_t()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(MAX_CHANNEL_VALUE) {}

    inline constexpr colori_t(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    inline constexpr colori_t(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit colori_t(T scaler)
        : r(scaler), g(scaler), b(scaler), a(scaler) {}

    inline constexpr colori_t(T r, T g, T b, T a = MAX_CHANNEL_VALUE)
        : r(r), g(g), b(b), a(a) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit colori_t(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(static_cast<T>(scaler)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr colori_t(U r, U g, U b, U a = static_cast<U>(MAX_CHANNEL_VALUE))
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename U>
    inline constexpr explicit colori_t(const colori_t<U>& c)
        : type(typename colori_t<U>::float_type(c)) {}

    template <typename U>
    inline constexpr colori_t(const colorf_t<U>& c)
        : r(static_cast<T>(math::clamp(c.r, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , g(static_cast<T>(math::clamp(c.g, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , b(static_cast<T>(math::clamp(c.b, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , a(static_cast<T>(math::clamp(c.a, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE)) {}

    template <typename U>
    inline constexpr explicit colori_t(const vec3_t<U>& v, U a = static_cast<U>(MAX_CHANNEL_VALUE))
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(a)) {}
    
    template <typename U>
    inline constexpr explicit colori_t(const vec4_t<U>& v)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(v.w)) {}

    // =============== destructor ===============

    ~colori_t() noexcept = default;

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

using color8 = colori_t<uint8_t>;

}
}