#pragma once

#include <sstream>

#include "base_type_defs.h"
#include "../fn_common.h"
#include "../fn_comparison.h"
#include "../fn_exponential.h"
#include "../fn_trigonometric.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct vec<2, T, vec_t::vec, val_t::floating_point>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using float_value_type = T;

    using type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using float_type = type;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(2); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::floating_point>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::floating_point>;

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
        return type(scaler + v.x, scaler + v.y);
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
        return type(scaler - v.x, scaler - v.y);
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
        return type(scaler * v.x, scaler * v.y);
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
        return type(scaler / v.x, scaler / v.y);
    }

    friend inline constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y);
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

    inline constexpr type& operator*=(const mat<2, 2, T>& m)
    {
        return ((*this) = (*this) * m);
    }

    inline constexpr type& operator*=(const mat<3, 3, T>& m)
    {
        return ((*this) = (*this) * m);
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

    // =============== swizzle ===============

    inline constexpr T swizzle(size_type i) const
    {
        return operator[](i);
    }

    inline constexpr vec2_type swizzle(size_type i, size_type j) const
    {
        return vec2_type(operator[](i), operator[](j));
    }

    inline constexpr vec3_type swizzle(size_type i, size_type j, size_type k) const
    {
        return vec3_type(operator[](i), operator[](j), operator[](k));
    }

    inline constexpr vec4_type swizzle(size_type i, size_type j, size_type k, size_type l) const
    {
        return vec4_type(operator[](i), operator[](j), operator[](k), operator[](l));
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        if (pretty_print)
        {
            return type(
                math::make_pretty(x),
                math::make_pretty(y)
            ).to_string();
        }

        std::ostringstream oss;
        oss << "{ " << x << ", " << y << " }";
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
    inline constexpr T aspect() const
    {
        return math::is_zero_approx(y) ? static_cast<T>(0) : (x / y);
    }

    /**
     * @brief Calculate the angle of the vector.
     *
     * This function computes the angle (in radians) of the vector using the arctangent of the y and x components.
     *
     * @return The angle of the vector in radians.
     */
    inline constexpr T angle() const
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
    inline constexpr T magnitude_squared() const { return (x * x) + (y * y); }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr T magnitude() const { return math::sqrt((x * x) + (y * y)); }

    /**
     * @brief Normalizes the vector.
     *
     * This function normalizes the vector.
     *
     * @return The normalized vector. If the length of the vector is 0,
     * a zero vector will be returned.
     */
    inline constexpr type normalize() const
    {
        const T magsq = magnitude_squared();
        if (magsq < math::epsilon<T>)
            return type();
        return (*this) * math::inverse_sqrt(magsq);
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
     * @brief Create a vector that is orthogonal (perpendicular) to the given vector.
     *
     * This static function returns a new vector that is orthogonal to the provided vector.
     * The orthogonal vector is obtained by swapping the x and y components and negating the new y component.
     *
     * @param v The input vector.
     * @return A new vector orthogonal to the input vector.
     */
    static inline constexpr type orthogonal(const type& v)
    {
        return type(v.y, -v.x);
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(); }
    static inline constexpr type ONE() { return type(static_cast<T>(1)); }

    static inline constexpr type RIGHT() { return type(static_cast<T>(1), static_cast<T>(0)); }
    static inline constexpr type LEFT() { return type(static_cast<T>(-1), static_cast<T>(0)); }

    static inline constexpr type UP() { return type(static_cast<T>(0), static_cast<T>(1)); }
    static inline constexpr type DOWN() { return type(static_cast<T>(0), static_cast<T>(-1)); }

};

}
}
}