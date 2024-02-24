#pragma once

#include <sstream>

#include "../math.h"
#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<2, T>
{
    static_assert(std::is_arithmetic<T>::value, "type T must be arithmetic type");

    // =============== meta ===============

    using value_type = T;
    using type = vec<2, T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = vec<2, float_value_type>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(2); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
    inline constexpr explicit vec(const vec<2, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<4, U>& v)
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

    template <typename U>
    inline constexpr type& operator=(const vec<2, U>& v)
    {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
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
        if (v1.x != v2.x) return (v1.x <  v2.x);
        else              return (v1.y <= v2.y);
    }

    friend inline constexpr bool operator>=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x >  v2.x);
        else              return (v1.y >= v2.y);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return type(+x, +y);
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

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator~(const type& v)
    {
        return type(~v.x, ~v.y);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator+=(U scaler)
    {
        x += scaler;
        y += scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator+=(const vec<2, U>& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    // subtraction (-=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator-=(U scaler)
    {
        x -= scaler;
        y -= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator-=(const vec<2, U>& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    // multiplication (*=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator*=(U scaler)
    {
        x *= scaler;
        y *= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator*=(const vec<2, U>& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    // division (/=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator/=(U scaler)
    {
        x /= scaler;
        y /= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator/=(const vec<2, U>& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }

    // modulo (%=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(U scaler)
    {
        x %= scaler;
        y %= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(const vec<2, U>& v)
    {
        x %= v.x;
        y %= v.y;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(U scaler)
    {
        x &= scaler;
        y &= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const vec<2, U>& v)
    {
        x &= v.x;
        y &= v.y;
        return *this;
    }

    // or (|=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(U scaler)
    {
        x |= scaler;
        y |= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const vec<2, U>& v)
    {
        x |= v.x;
        y |= v.y;
        return *this;
    }

    // xor (^=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(U scaler)
    {
        x ^= scaler;
        y ^= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const vec<2, U>& v)
    {
        x ^= v.x;
        y ^= v.y;
        return *this;
    }

    // left shift (<<=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(U scaler)
    {
        x <<= scaler;
        y <<= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const vec<2, U>& v)
    {
        x <<= v.x;
        y <<= v.y;
        return *this;
    }

    // right shift (>>=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(U scaler)
    {
        x >>= scaler;
        y >>= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(const vec<2, U>& v)
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

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "{ " << +x << ", " << +y << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

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
    inline constexpr float_value_type aspect() const
    {
        return static_cast<float_value_type>(math::is_zero_approx(y)) * (static_cast<float_value_type>(x) / static_cast<float_value_type>(y));
    }

    /**
     * @brief Calculate the angle of the vector.
     *
     * This function computes the angle (in radians) of the vector using the arctangent of the y and x components.
     *
     * @return The angle of the vector in radians.
     */
    inline constexpr float_value_type angle() const
    {
        return math::atan2(y, x);
    }

    // =============== magnitude ===============

    /**
     * @brief Calculates the squared magnitude of the vector.
     *
     * This function computes the squared magnitude of the vector.
     *
     * @return The squared length of the vector.
     */
    inline constexpr float_value_type magnitude_squared() const
    {
        return static_cast<float_value_type>((x * x) + (y * y));
    }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr float_value_type magnitude() const
    {
        return math::sqrt(magnitude_squared());
    }

    /**
     * @brief Normalizes the vector.
     *
     * This function normalizes the vector.
     *
     * @return The normalized vector. If the length of the vector is 0,
     * a zero vector will be returned.
     */
    inline constexpr float_type normalize() const
    {
        const float_value_type magsq = magnitude_squared();

        if VX_UNLIKELY(magsq < math::epsilon<float_value_type>)
        {
            return float_type(0);
        }

        return float_type(*this) * math::inverse_sqrt(magsq);
    }

    // =============== direction and orientation ===============

    /**
     * @brief Create a vector from a given angle.
     *
     * This static function creates a vector with components calculated from the
     * cosine and sine of the specified angle.
     *
     * @param angle The angle in radians.
     * @return A new vector created from the given angle.
     */
    static inline constexpr type from_angle(T angle)
    {
        return type(math::cos(angle), math::sin(angle));
    }

    /**
     * @brief Create a vector that is orthogonal (perpendicular) to this vector.
     *
     * This function returns a new vector that is orthogonal to the provided vector.
     * The orthogonal vector is obtained by swapping the x and y components and negating the new y component.
     *
     * @param v The input vector.
     * @return A new vector orthogonal to the input vector.
     */
    inline constexpr type orthogonal()
    {
        return type(y, -x);
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(0); }
    static inline constexpr type ONE() { return type(1); }

    static inline constexpr type RIGHT() { return type(1, 0); }
    static inline constexpr type LEFT() { return type(-1, 0); }

    static inline constexpr type UP() { return type(0, 1); }
    static inline constexpr type DOWN() { return type(0, -1); }

};

VX_PACK_POP()

using vec2  = vec<2, float>;
using vec2f = vec<2, float>;
using vec2d = vec<2, double>;
using vec2u = vec<2, unsigned int>;
using vec2i = vec<2, int>;

}
}