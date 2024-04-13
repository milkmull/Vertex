#pragma once

#include "../../_priv/common.h"
#include "../../../math/fn/fn_common.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct color_t
{
    static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value, "type T must be float or uint8_t");

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scaler_type = T;
    using type = color_t<T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(4); }

    static constexpr T MIN_CHANNEL_VALUE = static_cast<T>(0);
    static constexpr T MAX_CHANNEL_VALUE = std::is_same<T, float>::value ? 1.0f : 255;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    T r, g, b, a;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr color_t()
        : r(static_cast<T>(0))
        , g(static_cast<T>(0))
        , b(static_cast<T>(0))
        , a(MAX_CHANNEL_VALUE) {}

    VX_FORCE_INLINE constexpr color_t(const type& c)
        : r(c.r), g(c.g), b(c.b), a(c.a) {}

    VX_FORCE_INLINE constexpr color_t(type&&) noexcept = default;

    template <typename U = T, typename std::enable_if<std::is_same<U, T>::value && std::is_floating_point<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr color_t(const vec<4, U>& v)
        : r(v.x), g(v.y), b(v.z), a(v.w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit color_t(T scaler)
        : r(scaler), g(scaler), b(scaler), a(MAX_CHANNEL_VALUE) {}

    VX_FORCE_INLINE constexpr color_t(T r, T g, T b, T a = MAX_CHANNEL_VALUE)
        : r(r), g(g), b(b), a(a) {}

    VX_FORCE_INLINE constexpr color_t(const vec<3, T>& rgb, T a = MAX_CHANNEL_VALUE)
        : r(rgb.x), g(rgb.y), b(rgb.z), a(a) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit color_t(U scaler)
        : r(static_cast<T>(scaler))
        , g(static_cast<T>(scaler))
        , b(static_cast<T>(scaler))
        , a(MAX_CHANNEL_VALUE) {}

    template <typename R, typename G, typename B, typename A, typename std::enable_if<
        std::is_arithmetic<R>::value &&
        std::is_arithmetic<G>::value &&
        std::is_arithmetic<B>::value &&
        std::is_arithmetic<A>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr color_t(R r, G g, B b, A a = MAX_CHANNEL_VALUE)
        : r(static_cast<T>(r))
        , g(static_cast<T>(g))
        , b(static_cast<T>(b))
        , a(static_cast<T>(a)) {}

    template <typename RGB, typename A = T, typename std::enable_if<std::is_arithmetic<A>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr color_t(const vec<3, RGB>& rgb, A a = MAX_CHANNEL_VALUE)
        : r(static_cast<float>(rgb.x))
        , g(static_cast<float>(rgb.y))
        , b(static_cast<float>(rgb.z))
        , a(static_cast<float>(a)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit color_t(const vec<4, U>& v)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(v.w)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // color conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    // int to int

    template <typename U, typename std::enable_if<std::is_integral<U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit color_t(const color_t<U>& c)
        : color_t(typename color_t<U>::float_type(c)) {}
    
    // int to float

    template <typename U, typename std::enable_if<std::is_integral<U>::value && std::is_floating_point<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , g(static_cast<T>(c.g) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , b(static_cast<T>(c.b) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE))
        , a(static_cast<T>(c.a) / static_cast<T>(color_t<U>::MAX_CHANNEL_VALUE)) {}

    // float to int

    template <typename U, typename std::enable_if<std::is_floating_point<U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr color_t(const color_t<U>& c)
        : r(static_cast<T>(math::clamp(c.r, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , g(static_cast<T>(math::clamp(c.g, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , b(static_cast<T>(math::clamp(c.b, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE))
        , a(static_cast<T>(math::clamp(c.a, static_cast<U>(0), static_cast<U>(1)) * MAX_CHANNEL_VALUE)) {}

    // float to float

    template <typename U, typename std::enable_if<std::is_floating_point<U>::value && std::is_floating_point<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit color_t(const color_t<U>& c)
        : r(static_cast<T>(c.r))
        , g(static_cast<T>(c.g))
        , b(static_cast<T>(c.b))
        , a(static_cast<T>(c.a)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~color_t() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const color_t<U>& c)
    {
        (*this) = type(c);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 4);
        return (&r)[i];
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);
        return (&r)[i];
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const
    {
        return vec<4, U>(
            static_cast<U>(r),
            static_cast<U>(g),
            static_cast<U>(b),
            static_cast<U>(a)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& c1, const type& c2)
    {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& c1, const type& c2)
    {
        return !(c1 == c2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const
    {
        return type(+r, +g, +b, +a);
    }

    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-r, -g, -b, -a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // incrememnt (++)

    VX_FORCE_INLINE constexpr type& operator++()
    {
        r++;
        g++;
        b++;
        a++;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    VX_FORCE_INLINE constexpr type& operator--()
    {
        r--;
        g--;
        b--;
        a--;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr type operator+(const type& c, T scaler)
    {
        return type(c.r + scaler, c.g + scaler, c.b + scaler, c.a + scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator+(T scaler, const type& c)
    {
        return type(scaler + c.r, scaler + c.g, scaler + c.b, scaler + c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator+(const type& c1, const type& c2)
    {
        return type(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& c, T scaler)
    {
        return type(c.r - scaler, c.g - scaler, c.b - scaler, c.a - scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator-(T scaler, const type& c)
    {
        return type(scaler - c.r, scaler - c.g, scaler - c.b, scaler - c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator-(const type& c1, const type& c2)
    {
        return type(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& c, T scaler)
    {
        return type(c.r * scaler, c.g * scaler, c.b * scaler, c.a * scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator*(T scaler, const type& c)
    {
        return type(scaler * c.r, scaler * c.g, scaler * c.b, scaler * c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator*(const type& c1, const type& c2)
    {
        return type(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& c, T scaler)
    {
        return type(c.r / scaler, c.g / scaler, c.b / scaler, c.a / scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator/(T scaler, const type& c)
    {
        return type(scaler / c.r, scaler / c.g, scaler / c.b, scaler / c.a);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& c1, const type& c2)
    {
        return type(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b, c1.a / c2.a);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& c, T scaler)
    {
        return type(c.r % scaler, c.g % scaler, c.b % scaler, c.a % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(T scaler, const type& c)
    {
        return type(scaler % c.r, scaler % c.g, scaler % c.b, scaler % c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator%(const type& c1, const type& c2)
    {
        return type(c1.r % c2.r, c1.g % c2.g, c1.b % c2.b, c1.a % c2.a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& c, T scaler)
    {
        return type(c.r & scaler, c.g & scaler, c.b & scaler, c.a & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(T scaler, const type& c)
    {
        return type(scaler & c.r, scaler & c.g, scaler & c.b, scaler & c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator&(const type& c1, const type& c2)
    {
        return type(c1.r & c2.r, c1.g & c2.g, c1.b & c2.b, c1.a & c2.a);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& c, T scaler)
    {
        return type(c.r | scaler, c.g | scaler, c.b | scaler, c.a | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(T scaler, const type& c)
    {
        return type(scaler | c.r, scaler | c.g, scaler | c.b, scaler | c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator|(const type& c1, const type& c2)
    {
        return type(c1.r | c2.r, c1.g | c2.g, c1.b | c2.b, c1.a | c2.a);
    }

    // ror (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& c, T scaler)
    {
        return type(c.r ^ scaler, c.g ^ scaler, c.b ^ scaler, c.a ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(T scaler, const type& c)
    {
        return type(scaler ^ c.r, scaler ^ c.g, scaler ^ c.b, scaler ^ c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator^(const type& c1, const type& c2)
    {
        return type(c1.r ^ c2.r, c1.g ^ c2.g, c1.b ^ c2.b, c1.a ^ c2.a);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& c, T scaler)
    {
        return type(c.r << scaler, c.g << scaler, c.b << scaler, c.a << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(T scaler, const type& c)
    {
        return type(scaler << c.r, scaler << c.g, scaler << c.b, scaler << c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator<<(const type& c1, const type& c2)
    {
        return type(c1.r << c2.r, c1.g << c2.g, c1.b << c2.b, c1.a << c2.a);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& c, T scaler)
    {
        return type(c.r >> scaler, c.g >> scaler, c.b >> scaler, c.a >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(T scaler, const type& c)
    {
        return type(scaler >> c.r, scaler >> c.g, scaler >> c.b, scaler >> c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator>>(const type& c1, const type& c2)
    {
        return type(c1.r >> c2.r, c1.g >> c2.g, c1.b >> c2.b, c1.a >> c2.a);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    friend VX_FORCE_INLINE constexpr type operator~(const type& c)
    {
        return type(~c.r, ~c.g, ~c.b, ~c.a);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(T scaler)
    {
        r += scaler;
        g += scaler;
        b += scaler;
        a += scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator+=(const type& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(T scaler)
    {
        r -= scaler;
        g -= scaler;
        b -= scaler;
        a -= scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator-=(const type& c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(T scaler)
    {
        r *= scaler;
        g *= scaler;
        b *= scaler;
        a *= scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& c)
    {
        r *= c.r;
        g *= c.g;
        b *= c.b;
        a *= c.a;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(T scaler)
    {
        r /= scaler;
        g /= scaler;
        b /= scaler;
        a /= scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& c)
    {
        r /= c.r;
        g /= c.g;
        b /= c.b;
        a /= c.a;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(T scaler)
    {
        r %= scaler;
        g %= scaler;
        b %= scaler;
        a %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator%=(const type& c)
    {
        r %= c.r;
        g %= c.g;
        b %= c.b;
        a %= c.a;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(T scaler)
    {
        r &= scaler;
        g &= scaler;
        b &= scaler;
        a &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator&=(const type& c)
    {
        r &= c.r;
        g &= c.g;
        b &= c.b;
        a &= c.a;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(T scaler)
    {
        r |= scaler;
        g |= scaler;
        b |= scaler;
        a |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator|=(const type& c)
    {
        r |= c.r;
        g |= c.g;
        b |= c.b;
        a |= c.a;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(T scaler)
    {
        r ^= scaler;
        g ^= scaler;
        b ^= scaler;
        a ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator^=(const type& c)
    {
        r ^= c.r;
        g ^= c.g;
        b ^= c.b;
        a ^= c.a;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(T scaler)
    {
        r <<= scaler;
        g <<= scaler;
        b <<= scaler;
        a <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator<<=(const type& c)
    {
        r <<= c.r;
        g <<= c.g;
        b <<= c.b;
        a <<= c.a;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(T scaler)
    {
        r >>= scaler;
        g >>= scaler;
        b >>= scaler;
        a >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_integral<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr type& operator>>=(const type& c)
    {
        r >>= c.r;
        g >>= c.g;
        b >>= c.b;
        a >>= c.a;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // channels
    ///////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the RGB components of the color.
    ///
    /// @return A vector representing the RGB components of the color.
    ////////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr auto rgb() const
    {
        using U = typename std::conditional<std::is_same<T, uint8_t>::value, int32_t, float>::type;
        return vec<3, U>(r, g, b);
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Sets the RGB components of the color.
    ///
    /// @param A vector representing the RGB components of the color.
    ////////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr void set_rgb(
        const vec<3, typename std::conditional<std::is_same<T, uint8_t>::value, int32_t, float>::type>& rgb
    )
    { 
        r = rgb.x;
        g = rgb.y;
        b = rgb.z; 
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Get the minimum RGB component of the color.
    ///
    /// @return The minimum color component value.
    ////////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T min_color() const { return math::min({ r, g, b }); }

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Get the maximum RGB component of the color.
    ///
    /// @return The maximum color component value.
    ////////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T max_color() const { return math::max({ r, g, b }); }

    ///////////////////////////////////////////////////////////////////////////////
    // colors
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type CLEAR()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type WHITE()   { return type(MAX_CHANNEL_VALUE); }
    static VX_FORCE_INLINE constexpr type BLACK()   { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }

    static VX_FORCE_INLINE constexpr type GRAY()    { return type(MAX_CHANNEL_VALUE / static_cast<T>(2)); }
    static VX_FORCE_INLINE constexpr type GREY()    { return GRAY(); }

    static VX_FORCE_INLINE constexpr type RED()     { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type GREEN()   { return type(static_cast<T>(0), MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type BLUE()    { return type(static_cast<T>(0), static_cast<T>(0), MAX_CHANNEL_VALUE); }

    static VX_FORCE_INLINE constexpr type YELLOW()  { return type(MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type MAGENTA() { return type(MAX_CHANNEL_VALUE, static_cast<T>(0), MAX_CHANNEL_VALUE); }
    static VX_FORCE_INLINE constexpr type CYAN()	{ return type(static_cast<T>(0), MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE); }

};

VX_PACK_POP()

}
}