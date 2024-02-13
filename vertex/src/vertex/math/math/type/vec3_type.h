#pragma once

#include <sstream>

#include "../math.h"
#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct vec<3, T>
{
    static_assert(std::is_arithmetic<T>::value, "type T must be arithmetic type");

    // =============== meta ===============

    using value_type = T;
    using type = vec<3, T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = vec<3, float_value_type>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(3); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    T x, y, z;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler), z(scaler) {}

    inline constexpr vec(T x, T y, T z) noexcept
        : x(x), y(y), z(z) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, U y, U z)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(const vec<2, U>& vxy, U z)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z)) {}
    
    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, const vec<2, U>& vyz)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyz.x))
        , z(static_cast<T>(vyz.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    template <typename U>
    inline constexpr explicit vec(const color_t<U>& c)
        : x(static_cast<T>(c.r))
        , y(static_cast<T>(c.g))
        , z(static_cast<T>(c.b)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 3);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 3);
        return (&x)[i];
    }

    // =============== comparison operators ===============

    friend inline constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    friend inline constexpr bool operator!=(const type& v1, const type& v2)
    {
        return !(v1 == v2);
    }

    friend inline constexpr bool operator<(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        else              return (v1.z < v2.z);
    }

    friend inline constexpr bool operator>(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        else              return (v1.z > v2.z);
    }

    friend inline constexpr bool operator<=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        else              return (v1.z <= v2.z);
    }

    friend inline constexpr bool operator>=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        else              return (v1.z >= v2.z);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return type(+x, +y, +z);
    }

    inline constexpr type operator-() const
    {
        return type(-x, -y, -z);
    }

    // =============== incrememnt and decrement operators ===============

    // increment (++)

    inline constexpr type& operator++()
    {
        x++;
        y++;
        z++;
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
        z--;
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
        return type(v.x + scaler, v.y + scaler, v.z + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& v)
    {
        return v + scaler;
    }

    friend inline constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler, v.z - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& v)
    {
        return v - scaler;
    }

    friend inline constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler, v.z * scaler);
    }

    friend inline constexpr type operator*(T scaler, const type& v)
    {
        return v * scaler;
    }

    friend inline constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }

    // division (/)

    friend inline constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler, v.z / scaler);
    }

    friend inline constexpr type operator/(T scaler, const type& v)
    {
        return v / scaler;
    }

    friend inline constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator~(const type& v)
    {
        return type(~v.x, ~v.y, ~v.z);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        x += scaler;
        y += scaler;
        z += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        x -= scaler;
        y -= scaler;
        z -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        return *this;
    }

    inline constexpr type& operator*=(const type& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        return *this;
    }

    inline constexpr type& operator/=(const type& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(T scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(const type& v)
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
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
        return iterator(&z + 1);
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
        return const_iterator(&z + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&z + 1);
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
        return const_reverse_iterator(&z + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&x);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << +x << ", " << +y << ", " << +z << " }";
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
    inline constexpr T min() const { return math::min({ x, y, z }); }

    /**
     * @brief Get the maximum component value of the vector.
     *
     * This function returns the maximum value between the x and y components of the vector.
     *
     * @return The maximum component value.
     */
    inline constexpr T max() const { return math::max({ x, y, z }); }

    inline constexpr T width() const { return x; }
    inline constexpr T height() const { return y; }
    inline constexpr T depth() const { return z; }

    // =============== magnitude ===============

    /**
     * @brief Calculates the squared magnitude of the vector.
     *
     * This function computes the squared magnitude of the vector.
     *
     * @return The squared length of the vector.
     */
    inline constexpr float_value_type magnitude_squared() const { return math::length_squared(*this); }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr float_value_type magnitude() const { return math::length(*this); }

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
        return math::normalize(*this);
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(); }
    static inline constexpr type ONE() { return type(static_cast<T>(1)); }

    static inline constexpr type RIGHT() { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)); }
    static inline constexpr type LEFT() { return type(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)); }

    static inline constexpr type UP() { return type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)); }
    static inline constexpr type DOWN() { return type(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)); }

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)

    static inline constexpr type FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }
    static inline constexpr type BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }

#else

    static inline constexpr type FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }
    static inline constexpr type BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }

#endif

};

VX_PACK_POP()

using vec3  = vec<3, float>;
using vec3f = vec<3, float>;
using vec3d = vec<3, double>;
using vec3u = vec<3, unsigned int>;
using vec3i = vec<3, int>;

}
}