#pragma once

#include <sstream>

#include "base_type_defs.h"
#include "../fn_exponential.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct vec<3, T, vec_t::vec, val_t::integral>
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

private:

    using FT = typename detail::to_float_type<T>::type;

public:

    using value_type = T;
    using float_value_type = FT;

    using type = vec<3, T, vec_t::vec, val_t::integral>;
    using float_type = vec<3, FT, vec_t::vec, val_t::floating_point>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(3); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::integral>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

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
    inline constexpr vec(const vecx<2, U>& vxy, U z)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, const vecx<2, U>& vyz)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyz.x))
        , z(static_cast<T>(vyz.y)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<3, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

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
        return *this;
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

    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler);
    }

    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
    }

    // =============== binary bit operators ===============

    // and (&)

    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler);
    }

    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
    }

    // or (|)

    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler);
    }

    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
    }

    // xor (^)

    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler);
    }

    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
    }

    // left shift (<<)

    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler);
    }

    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
    }

    // right shift (>>)

    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler);
    }

    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
    }

    // not (~)

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

    inline constexpr type& operator%=(T scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        return *this;
    }

    inline constexpr type& operator%=(const type& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    inline constexpr type& operator&=(T scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        return *this;
    }

    inline constexpr type& operator&=(const type& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        return *this;
    }

    // or (|=)

    inline constexpr type& operator|=(T scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        return *this;
    }

    inline constexpr type& operator|=(const type& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        return *this;
    }

    // xor (^=)

    inline constexpr type& operator^=(T scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        return *this;
    }

    inline constexpr type& operator^=(const type& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        return *this;
    }

    // left shift (<<=)

    inline constexpr type& operator<<=(T scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        return *this;
    }

    inline constexpr type& operator<<=(const type& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        return *this;
    }

    // right shift (>>=)

    inline constexpr type& operator>>=(T scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        return *this;
    }

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

    // =============== swizzle ===============

    inline constexpr T swiz(char i) const
    {
        return operator[](static_cast<size_type>(i - 'x'));
    }

    inline constexpr vec2_type swiz(char i, char j) const
    {
        return vec2_type(
            operator[](static_cast<size_type>(i - 'x')),
            operator[](static_cast<size_type>(j - 'x'))
        );
    }

    inline constexpr vec3_type swiz(char i, char j, char k) const
    {
        return vec3_type(
            operator[](static_cast<size_type>(i - 'x')),
            operator[](static_cast<size_type>(j - 'x')),
            operator[](static_cast<size_type>(k - 'x'))
        );
    }

    inline constexpr vec4_type swiz(char i, char j, char k, char l) const
    {
        return vec4_type(
            operator[](static_cast<size_type>(i - 'x')),
            operator[](static_cast<size_type>(j - 'x')),
            operator[](static_cast<size_type>(k - 'x')),
            operator[](static_cast<size_type>(l - 'x'))
        );
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << +x << ", " << +y << ", " << +z << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

    inline constexpr void set(T nxyz) { x = y = z = nxyz; }
    inline constexpr void set(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }

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
    inline constexpr FT magnitude_squared() const { return static_cast<FT>((x * x) + (y * y) + (z * z)); }

    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr FT magnitude() const { return math::sqrt(static_cast<FT>((x * x) + (y * y) + (z * z))); }

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

}

using vec3i  = detail::vec<3,  int32_t, detail::vec_t::vec>;
using vec3ui = detail::vec<3, uint32_t, detail::vec_t::vec>;
using vec3sz = detail::vec<3,   size_t, detail::vec_t::vec>;
using vec3b  = detail::vec<3,     bool, detail::vec_t::vec>;

}
}