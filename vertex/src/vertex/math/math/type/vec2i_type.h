#pragma once

#include <sstream>

#include "../math.h"

namespace vx {
namespace math {
namespace detail {

VX_PACK_PUSH()

template <typename T>
struct vec<2, T, vec_t::vec, val_t::integral>
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

private:

    using FT = typename detail::to_float_type<T>::type;

public:

    using value_type = T;
    using float_value_type = FT;

    using type = vec<2, T, vec_t::vec, val_t::integral>;
    using float_type = vec<2, FT, vec_t::vec, val_t::floating_point>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(2); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::integral>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

    // =============== data ===============

    T x, y;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler) {}

    inline constexpr vec(T x, T y) noexcept
        : x(x), y(y) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, U y)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<2, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 2);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

    // =============== comparison operators ===============

    friend inline constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y;
    }

    friend inline constexpr bool operator!=(const type& v1, const type& v2)
    {
        return !(v1 == v2);
    }

    friend inline constexpr bool operator<(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        else              return (v1.y < v2.y);
    }

    friend inline constexpr bool operator>(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        else              return (v1.y > v2.y);
    }

    friend inline constexpr bool operator<=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        else              return (v1.y <= v2.y);
    }

    friend inline constexpr bool operator>=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        else              return (v1.y >= v2.y);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return *this;
    }

    inline constexpr type operator-() const
    {
        return type(-x, -y);
    }

    // =============== incrememnt and decrement operators ===============

    // increment (++)

    inline constexpr type& operator++()
    {
        x++;
        y++;
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
        x--;
        y--;
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

    friend inline constexpr type operator+(const type& v, T scaler)
    {
        return type(v.x + scaler, v.y + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& v)
    {
        return v + scaler;
    }

    friend inline constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& v)
    {
        return v - scaler;
    }

    friend inline constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y);
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler);
    }

    friend inline constexpr type operator*(T scaler, const type& v)
    {
        return v * scaler;
    }

    friend inline constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y);
    }

    // division (/)

    friend inline constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler);
    }

    friend inline constexpr type operator/(T scaler, const type& v)
    {
        return v / scaler;
    }

    friend inline constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y);
    }

    // modulo (%)

    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler);
    }

    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y);
    }

    // =============== binary bit operators ===============

    // and (&)

    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler);
    }

    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y);
    }

    // or (|)

    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler);
    }

    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y);
    }

    // xor (^)

    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler);
    }

    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y);
    }

    // left shift (<<)

    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler);
    }

    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y);
    }

    // right shift (>>)

    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler);
    }

    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y);
    }

    // not (~)

    friend inline constexpr type operator~(const type& v)
    {
        return type(~v.x, ~v.y);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        x += scaler;
        y += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        x -= scaler;
        y -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        x *= scaler;
        y *= scaler;
        return *this;
    }

    inline constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        x /= scaler;
        y /= scaler;
        return *this;
    }

    inline constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    // modulo (%=)

    inline constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        return *this;
    }

    inline constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    inline constexpr type& operator&=(T scaler)
    {
        x &= scaler;
        y &= scaler;
        return *this;
    }

    inline constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        return *this;
    }

    // or (|=)

    inline constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        return *this;
    }

    inline constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        return *this;
    }

    // xor (^=)

    inline constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        return *this;
    }

    inline constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        return *this;
    }

    // left shift (<<=)

    inline constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        return *this;
    }

    inline constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        return *this;
    }

    // right shift (>>=)

    inline constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        return *this;
    }

    inline constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        return *this;
    }

    // =============== iterator ===============

    inline constexpr iterator begin() noexcept
    {
        return iterator(&x);
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    inline constexpr iterator end() noexcept
    {
        return iterator(&y + 1);
    }

    inline constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    inline constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(&x);
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return const_iterator(&y + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&y + 1);
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    inline constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(&x);
    }

    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&y + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&x);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << +x << ", " << +y << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

    inline constexpr void set(T nxy) { x = y = nxy; }
    inline constexpr void set(T nx, T ny) { x = nx; y = ny; }

    /**
     * @brief Get the minimum component value of the vector.
     *
     * This function returns the minimum value between the x and y components of the vector.
     *
     * @return The minimum component value.
     */
    inline constexpr T min() const { return math::min(x, y); }

    /**
     * @brief Get the maximum component value of the vector.
     *
     * This function returns the maximum value between the x and y components of the vector.
     *
     * @return The maximum component value.
     */
    inline constexpr T max() const { return math::max(x, y); }

    inline constexpr T width() const { return x; }
    inline constexpr T height() const { return y; }

    /**
     * @brief Calculate the aspect ratio of the vector.
     *
     * This function computes the aspect ratio by dividing the x component by the y component.
     * If the y component is approximately zero, the function returns 0 to avoid division by zero.
     *
     * @return The aspect ratio of the vector.
     */
    inline constexpr FT aspect() const
    {
        return math::is_zero_approx(y) ? static_cast<FT>(0) : (static_cast<FT>(x) / static_cast<FT>(y));
    }

    /**
     * @brief Calculate the angle of the vector.
     *
     * This function computes the angle (in radians) of the vector using the arctangent of the y and x components.
     *
     * @return The angle of the vector in radians.
     */
    inline constexpr FT angle() const
    {
        return math::atan2(static_cast<FT>(y), static_cast<FT>(x));
    }

    // =============== magnitude ===============

    /**
     * @brief Calculates the squared magnitude of the vector.
     *
     * This function computes the squared magnitude of the vector.
     *
     * @return The squared length of the vector.
     */
    inline constexpr FT magnitude_squared() const { return static_cast<FT>((x * x) + (y * y)); }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr FT magnitude() const { return math::sqrt(static_cast<FT>((x * x) + (y * y))); }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(); }
    static inline constexpr type ONE() { return type(static_cast<T>(1)); }

    static inline constexpr type RIGHT() { return type(static_cast<T>(1), static_cast<T>(0)); }
    static inline constexpr type LEFT() { return type(static_cast<T>(-1), static_cast<T>(0)); }

    static inline constexpr type UP() { return type(static_cast<T>(0), static_cast<T>(1)); }
    static inline constexpr type DOWN() { return type(static_cast<T>(0), static_cast<T>(-1)); }

};

VX_PACK_POP()

}

using vec2i  = detail::vec<2,  int32_t, detail::vec_t::vec>;
using vec2ui = detail::vec<2, uint32_t, detail::vec_t::vec>;
using vec2b  = detail::vec<2,     bool, detail::vec_t::vec>;

}
}