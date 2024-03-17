#pragma once

#include "../../detail/common.h"
#include "../../../math/fn/fn_common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct color_t
{
    static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value, "type T must be float or uint8_t");

    // =============== meta ===============

    using scaler_type = T;
    using type = color_t<T>;

    static inline constexpr size_t size() noexcept { return static_cast<size_t>(4); }

    static constexpr T MIN_CHANNEL_VALUE = static_cast<T>(0);
    static constexpr T MAX_CHANNEL_VALUE = std::is_same<T, float>::value ? 1.0f : 255;

    // =============== data ===============

    T r, g, b, a;

    // =============== implicit constructors ===============

    inline constexpr color_t()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(MAX_CHANNEL_VALUE) {}

    inline constexpr color_t(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    inline constexpr color_t(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit color_t(T scaler)
        : r(scaler), g(scaler), b(scaler), a(MAX_CHANNEL_VALUE) {}

    inline constexpr color_t(T r, T g, T b, T a = MAX_CHANNEL_VALUE)
        : r(r), g(g), b(b), a(a) {}

    inline constexpr explicit color_t(const vec<3, T>& rgb, T a = MAX_CHANNEL_VALUE)
        : r(rgb.x), g(rgb.y), b(rgb.z), a(a) {}

    inline constexpr explicit color_t(const vec<4, T>& v)
        : r(v.x), g(v.y), b(v.z), a(v.w) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit color_t(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(MAX_CHANNEL_VALUE) {}

    template <typename R, typename G, typename B, typename A, typename std::enable_if<
        std::is_arithmetic<R>::value &&
        std::is_arithmetic<G>::value &&
        std::is_arithmetic<B>::value &&
        std::is_arithmetic<A>::value, bool>::type = true>
    inline constexpr color_t(R r, G g, B b, A a = MAX_CHANNEL_VALUE)
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename RGB, typename A = T, typename std::enable_if<std::is_arithmetic<A>::value, bool>::type = true>
    inline constexpr explicit color_t(const vec<3, RGB>& rgb, A a = MAX_CHANNEL_VALUE)
        : r(static_cast<float>(rgb.x))
        , g(static_cast<float>(rgb.y))
        , b(static_cast<float>(rgb.z))
        , a(static_cast<float>(a)) {}

    template <typename U>
    inline constexpr explicit color_t(const vec<4, U>& v)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(v.w)) {}

    // =============== conversions ===============

    // int to int

    template <typename U, typename std::enable_if<std::is_integral<U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr explicit color_t(const color_t<U>& c)
        : color_t(typename color_t<U>::float_type(c)) {}
    
    // int to float

    template <typename U, typename std::enable_if<std::is_integral<U>::value && std::is_floating_point<T>::value, bool>::type = true>
    inline constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , g(static_cast<T>(c.g) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , b(static_cast<T>(c.b) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , a(static_cast<T>(c.a) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE)) {}

    // float to int

    template <typename U, typename std::enable_if<std::is_floating_point<U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(math::clamp(c.r, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , g(static_cast<T>(math::clamp(c.g, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , b(static_cast<T>(math::clamp(c.b, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , a(static_cast<T>(math::clamp(c.a, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE)) {}

    // float to float

    template <typename U, typename std::enable_if<std::is_floating_point<U>::value && std::is_floating_point<T>::value, bool>::type = true>
    inline constexpr explicit color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r))
        , g(static_cast<T>(c.g))
        , b(static_cast<T>(c.b))
        , a(static_cast<T>(c.a)) {}

    // =============== destructor ===============

    ~color_t() noexcept = default;

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

    template <typename U>
    inline constexpr type& operator=(const color_t<U>& c)
    {
        (*this) = type(c);
        return *this;
    }

    // =============== accessors ===============

    inline constexpr T& operator[](size_t i)
    {
        assert(i < 4);
        return (&r)[i];
    }

    inline constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);
        return (&r)[i];
    }

    // =============== conversion operators ===============

    template <typename U>
    inline constexpr explicit operator vec<4, U>() const
    {
        return vec<4, U>(
            static_cast<U>(r),
            static_cast<U>(g),
            static_cast<U>(b),
            static_cast<U>(a)
        );
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& c, T scaler)
    {
        return type(c.r % scaler, c.g % scaler, c.b % scaler, c.a % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& c)
    {
        return type(scaler % c.r, scaler % c.g, scaler % c.b, scaler % c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& c1, const type& c2)
    {
        return type(c1.r % c2.r, c1.g % c2.g, c1.b % c2.b, c1.a % c2.a);
    }

    // =============== binarg bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& c, T scaler)
    {
        return type(c.r & scaler, c.g & scaler, c.b & scaler, c.a & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& c)
    {
        return type(scaler & c.r, scaler & c.g, scaler & c.b, scaler & c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& c1, const type& c2)
    {
        return type(c1.r & c2.r, c1.g & c2.g, c1.b & c2.b, c1.a & c2.a);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& c, T scaler)
    {
        return type(c.r | scaler, c.g | scaler, c.b | scaler, c.a | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& c)
    {
        return type(scaler | c.r, scaler | c.g, scaler | c.b, scaler | c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& c1, const type& c2)
    {
        return type(c1.r | c2.r, c1.g | c2.g, c1.b | c2.b, c1.a | c2.a);
    }

    // ror (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& c, T scaler)
    {
        return type(c.r ^ scaler, c.g ^ scaler, c.b ^ scaler, c.a ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& c)
    {
        return type(scaler ^ c.r, scaler ^ c.g, scaler ^ c.b, scaler ^ c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& c1, const type& c2)
    {
        return type(c1.r ^ c2.r, c1.g ^ c2.g, c1.b ^ c2.b, c1.a ^ c2.a);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& c, T scaler)
    {
        return type(c.r << scaler, c.g << scaler, c.b << scaler, c.a << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& c)
    {
        return type(scaler << c.r, scaler << c.g, scaler << c.b, scaler << c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& c1, const type& c2)
    {
        return type(c1.r << c2.r, c1.g << c2.g, c1.b << c2.b, c1.a << c2.a);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& c, T scaler)
    {
        return type(c.r >> scaler, c.g >> scaler, c.b >> scaler, c.a >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& c)
    {
        return type(scaler >> c.r, scaler >> c.g, scaler >> c.b, scaler >> c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& c1, const type& c2)
    {
        return type(c1.r >> c2.r, c1.g >> c2.g, c1.b >> c2.b, c1.a >> c2.a);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend inline constexpr type operator~(const type& c)
    {
        return type(~c.r, ~c.g, ~c.b, ~c.a);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        r += scaler;
        g += scaler;
        b += scaler;
        a += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        r -= scaler;
        g -= scaler;
        b -= scaler;
        a -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        r *= scaler;
        g *= scaler;
        b *= scaler;
        a *= scaler;
        return *this;
    }

    inline constexpr type& operator*=(const type& c)
    {
        r *= c.r;
        g *= c.g;
        b *= c.b;
        a *= c.a;
        return *this;
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        r /= scaler;
        g /= scaler;
        b /= scaler;
        a /= scaler;
        return *this;
    }

    inline constexpr type& operator/=(const type& c)
    {
        r /= c.r;
        g /= c.g;
        b /= c.b;
        a /= c.a;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator%=(T scaler)
    {
        r %= scaler;
        g %= scaler;
        b %= scaler;
        a %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
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

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator&=(T scaler)
    {
        r &= scaler;
        g &= scaler;
        b &= scaler;
        a &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator&=(const type& c)
    {
        r &= c.r;
        g &= c.g;
        b &= c.b;
        a &= c.a;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator|=(T scaler)
    {
        r |= scaler;
        g |= scaler;
        b |= scaler;
        a |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator|=(const type& c)
    {
        r |= c.r;
        g |= c.g;
        b |= c.b;
        a |= c.a;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator^=(T scaler)
    {
        r ^= scaler;
        g ^= scaler;
        b ^= scaler;
        a ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator^=(const type& c)
    {
        r ^= c.r;
        g ^= c.g;
        b ^= c.b;
        a ^= c.a;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator<<=(T scaler)
    {
        r <<= scaler;
        g <<= scaler;
        b <<= scaler;
        a <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator<<=(const type& c)
    {
        r <<= c.r;
        g <<= c.g;
        b <<= c.b;
        a <<= c.a;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator>>=(T scaler)
    {
        r >>= scaler;
        g >>= scaler;
        b >>= scaler;
        a >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    inline constexpr type& operator>>=(const type& c)
    {
        r >>= c.r;
        g >>= c.g;
        b >>= c.b;
        a >>= c.a;
        return *this;
    }

    // =============== comparison and testing ===============

    /**
     * @brief Retrieves the RGB components of the color.
     *
     * @return A vector representing the RGB components of the color.
     */
    inline constexpr auto rgb() const
    {
        using U = typename std::conditional<std::is_same<T, uint8_t>::value, int32_t, float>::type;
        return vec<3, U>(r, g, b);
    }

    inline constexpr void set_rgb(
        const vec<3, typename std::conditional<std::is_same<T, uint8_t>::value, int32_t, float>::type>& rgb
    )
    { 
        r = rgb.x;
        g = rgb.y;
        b = rgb.z; 
    }

    /**
     * @brief Get the minimum RGB component of the color.
     *
     * @return The minimum color component value.
     */
    inline constexpr T min_color() const { return math::min({ r, g, b }); }

    /**
     * @brief Get the maximum coRGBlor component of the color.
     *
     * @return The maximum color component value.
     */
    inline constexpr T max_color() const { return math::max({ r, g, b }); }

    // =============== colors ===============

    static inline constexpr type CLEAR()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type WHITE()   { return type(MAX_CHANNEL_VALUE); }
    static inline constexpr type BLACK()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }

    static inline constexpr type GRAY()    { return type(MAX_CHANNEL_VALUE / static_cast<T>(2)); }
    static inline constexpr type GREY()    { return GRAY(); }

    static inline constexpr type RED()     { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type GREEN()   { return type(static_cast<T>(0), MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static inline constexpr type BLUE()    { return type(static_cast<T>(0), static_cast<T>(0), MAX_CHANNEL_VALUE); }

    static inline constexpr type YELLOW()  { return type(MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static inline constexpr type MAGENTA() { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), MAX_CHANNEL_VALUE); }
    static inline constexpr type CYAN()	   { return type(static_cast<T>(0), MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE); }

};

VX_PACK_POP()

}
}