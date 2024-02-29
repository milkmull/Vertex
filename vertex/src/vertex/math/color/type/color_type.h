#pragma once

#include <limits>

#include "../detail/base_types.h"
#include "../fn/color_fn_common.h"
#include "../../math/type/vec3_type.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct color_t
{
    static_assert(std::is_same<T, float>::value || std::is_same<T, uint8_t>::value, "type T must be float or uint8_t");

    // =============== meta ===============

    using value_type = T;
    using type = color_t<T>;

    using float_value_type = float;
    using float_type = color_t<float_value_type>;

    using int_value_type = uint8_t;
    using vec_value_type = typename std::conditional<std::is_integral<T>::value && (sizeof(value_type) <= sizeof(uint16_t)), uint32_t, value_type>::type;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::tools::iterator<T>;
    using const_iterator = ::vx::tools::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr T MIN_CHANNEL_VALUE = static_cast<T>(0);
    static constexpr T MAX_CHANNEL_VALUE = std::is_floating_point<T>::value ? static_cast<T>(1) : std::numeric_limits<T>::max();

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

    template <typename U>
    inline constexpr explicit color_t(const vec<3, U>& v, U a = MAX_CHANNEL_VALUE)
        : r(static_cast<T>(v.x))
        , g(static_cast<T>(v.y))
        , b(static_cast<T>(v.z))
        , a(static_cast<T>(a)) {}

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

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& c, T scaler)
    {
        return type(c.r % scaler, c.g % scaler, c.b % scaler, c.a % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& c)
    {
        return type(scaler % c.r, scaler % c.g, scaler % c.b, scaler % c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& c1, const type& c2)
    {
        return type(c1.r % c2.r, c1.g % c2.g, c1.b % c2.b, c1.a % c2.a);
    }

    // =============== binarg bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& c, T scaler)
    {
        return type(c.r & scaler, c.g & scaler, c.b & scaler, c.a & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& c)
    {
        return type(scaler & c.r, scaler & c.g, scaler & c.b, scaler & c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& c1, const type& c2)
    {
        return type(c1.r & c2.r, c1.g & c2.g, c1.b & c2.b, c1.a & c2.a);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& c, T scaler)
    {
        return type(c.r | scaler, c.g | scaler, c.b | scaler, c.a | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& c)
    {
        return type(scaler | c.r, scaler | c.g, scaler | c.b, scaler | c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& c1, const type& c2)
    {
        return type(c1.r | c2.r, c1.g | c2.g, c1.b | c2.b, c1.a | c2.a);
    }

    // ror (^)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& c, T scaler)
    {
        return type(c.r ^ scaler, c.g ^ scaler, c.b ^ scaler, c.a ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& c)
    {
        return type(scaler ^ c.r, scaler ^ c.g, scaler ^ c.b, scaler ^ c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& c1, const type& c2)
    {
        return type(c1.r ^ c2.r, c1.g ^ c2.g, c1.b ^ c2.b, c1.a ^ c2.a);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& c, T scaler)
    {
        return type(c.r << scaler, c.g << scaler, c.b << scaler, c.a << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& c)
    {
        return type(scaler << c.r, scaler << c.g, scaler << c.b, scaler << c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& c1, const type& c2)
    {
        return type(c1.r << c2.r, c1.g << c2.g, c1.b << c2.b, c1.a << c2.a);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& c, T scaler)
    {
        return type(c.r >> scaler, c.g >> scaler, c.b >> scaler, c.a >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& c)
    {
        return type(scaler >> c.r, scaler >> c.g, scaler >> c.b, scaler >> c.a);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& c1, const type& c2)
    {
        return type(c1.r >> c2.r, c1.g >> c2.g, c1.b >> c2.b, c1.a >> c2.a);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator~(const type& c)
    {
        return type(~c.r, ~c.g, ~c.b, ~c.a);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator+=(U scaler)
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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator-=(U scaler)
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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator*=(U scaler)
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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator/=(U scaler)
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

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(U scaler)
    {
        r %= scaler;
        g %= scaler;
        b %= scaler;
        a %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
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

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(T scaler)
    {
        r &= scaler;
        g &= scaler;
        b &= scaler;
        a &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const type& c)
    {
        r &= c.r;
        g &= c.g;
        b &= c.b;
        a &= c.a;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(T scaler)
    {
        r |= scaler;
        g |= scaler;
        b |= scaler;
        a |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const type& c)
    {
        r |= c.r;
        g |= c.g;
        b |= c.b;
        a |= c.a;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(T scaler)
    {
        r ^= scaler;
        g ^= scaler;
        b ^= scaler;
        a ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const type& c)
    {
        r ^= c.r;
        g ^= c.g;
        b ^= c.b;
        a ^= c.a;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(T scaler)
    {
        r <<= scaler;
        g <<= scaler;
        b <<= scaler;
        a <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const type& c)
    {
        r <<= c.r;
        g <<= c.g;
        b <<= c.b;
        a <<= c.a;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(T scaler)
    {
        r >>= scaler;
        g >>= scaler;
        b >>= scaler;
        a >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
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

    static inline constexpr size_type channels() { return static_cast<size_type>(4); }

    /**
     * @brief Retrieves the RGB components of the color.
     *
     * @return A vector representing the RGB components of the color.
     */
    inline constexpr vec<3, vec_value_type> rgb() const { return vec<3, vec_value_type>(r, g, b); }

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

    /**
     * @brief Clamps the color components within the range [0, 1].
     *
     * This function clamps the color components of the color to the range [0, 1].
     * This function is only enabled for floating-point color types.
     *
     * @return The color with its components clamped to the range [0, 1].
     */
    template <typename U = T, typename std::enable_if<std::is_floating_point<U>::value, bool>::type = true>
    inline constexpr type clamp() const
    {
        return math::clamp(*this);
    }

    // =============== color attributes ===============

    /**
     * @brief Calculates the luminance of the color.
     *
     * This function calculates the luminance of the color using the formula:
     * luminance = (0.2126 * r) + (0.7152 * g) + (0.0722 * b)
     *
     * @return The luminance of the color.
     */
    inline constexpr float_value_type luminance() const
    {
        const float_type c = *this;

        return (
            (c.r * static_cast<float_value_type>(0.2126)) +
            (c.g * static_cast<float_value_type>(0.7152)) +
            (c.b * static_cast<float_value_type>(0.0722))
        );
    }

    // =============== color modifiers ===============

    /**
     * @brief Creates a lighter version of the color.
     *
     * Returns a new color that is lightened by the specified amount.
     *
     * @param amount The amount by which to lighten the color. Should be in the range [0, 1].
     * @return The new color lightened by the specified amount.
     */
    inline constexpr type lighten(float_value_type amount) const
    {
        const float_type c = *this;

        return float_type(
            c.r + (static_cast<T>(1) - c.r) * amount,
            c.g + (static_cast<T>(1) - c.g) * amount,
            c.b + (static_cast<T>(1) - c.b) * amount,
            c.a
        );
    }

    /**
     * @brief Creates a darker version of the color.
     *
     * Returns a new color that is darkened by the specified amount.
     *
     * @param amount The amount by which to darken the color. Should be in the range [0, 1].
     * @return The new color darkened by the specified amount.
     */
    inline constexpr type darken(float_value_type amount) const
    {
        const float_type c = *this;

        return type(
            c.r * (static_cast<T>(1) - amount),
            c.g * (static_cast<T>(1) - amount),
            c.b * (static_cast<T>(1) - amount),
            c.a
        );
    }

    /**
     * @brief Creates the inverted color.
     *
     * Returns a new color where each channel value is subtracted from the maximum channel value.
     *
     * @return The inverted color.
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

    // =============== colors ===============

    static inline constexpr type CLEAR()   { return type(0, 0, 0, 0); }
    static inline constexpr type WHITE()   { return type(MAX_CHANNEL_VALUE); }
    static inline constexpr type BLACK()   { return type(0, 0, 0); }

    static inline constexpr type GRAY()    { return type(MAX_CHANNEL_VALUE / static_cast<T>(2)); }
    static inline constexpr type GREY()    { return GRAY(); }

    static inline constexpr type RED()     { return type(MAX_CHANNEL_VALUE, 0, 0); }
    static inline constexpr type GREEN()   { return type(0, MAX_CHANNEL_VALUE, 0); }
    static inline constexpr type BLUE()    { return type(0, 0, MAX_CHANNEL_VALUE); }

    static inline constexpr type YELLOW()  { return type(MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE, 0); }
    static inline constexpr type MAGENTA() { return type(MAX_CHANNEL_VALUE, 0, MAX_CHANNEL_VALUE); }
    static inline constexpr type CYAN()	   { return type(0, MAX_CHANNEL_VALUE, MAX_CHANNEL_VALUE); }

};

VX_PACK_POP()

using color  = color_t<float>;
using color8 = color_t<uint8_t>;

}
}