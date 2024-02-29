#pragma once

#include <sstream>

#include "../math.h"
#include "vertex/tools/iter/iterator.h"

namespace vx {
namespace math {

template <typename T> struct color_t;

VX_PACK_PUSH()

template <typename T>
struct vec<4, T>
{
    static_assert(std::is_arithmetic<T>::value, "type T must be arithmetic type");

    // =============== meta ===============

    using value_type = T;
    using type = vec<4, T>;

    using float_value_type = typename detail::to_float_type<T>::type;
    using float_type = vec<4, float_value_type>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::tools::iterator<T>;
    using const_iterator = ::vx::tools::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    T x, y, z, w;

    // =============== implicit constructors ===============

    inline constexpr vec() noexcept
        : x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0))
        , w(static_cast<T>(0)) {}

    inline constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z), w(v.w) {}

    inline constexpr vec(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit vec(T scaler) noexcept
        : x(scaler), y(scaler), z(scaler), w(scaler) {}

    inline constexpr vec(T x, T y, T z, T w) noexcept
        : x(x), y(y), z(z), w(w) {}

    // =============== conversion vector constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler))
        , w(static_cast<T>(scaler)) {}

    template <typename X, typename Y , typename Z, typename W, typename std::enable_if<
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value &&
        std::is_arithmetic<W>::value, bool>::type = true>
    inline constexpr vec(X x, Y y, Z z, W w)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(const vec<2, U>& vxy, U z, U w)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}
    
    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, U y, const vec<2, U>& vzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(const vec<3, U>& vxyz, U w)
        : x(static_cast<T>(vxyz.x))
        , y(static_cast<T>(vxyz.y))
        , z(static_cast<T>(vxyz.z))
        , w(static_cast<T>(w)) {}
    
    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, const vec<3, U>& vyzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyzw.x))
        , z(static_cast<T>(vyzw.y))
        , w(static_cast<T>(vyzw.z)) {}
    
    template <typename U>
    inline constexpr vec(const vec<2, U>& vxy, const vec<2, U>& vzw)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}
    
    template <typename U>
    inline constexpr explicit vec(const vec<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z))
        , w(static_cast<T>(v.w)) {}
    
    template <typename U>
    inline constexpr explicit vec(const color_t<U>& c)
        : x(static_cast<T>(c.r))
        , y(static_cast<T>(c.g))
        , z(static_cast<T>(c.b))
        , w(static_cast<T>(c.a)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator=(const vec<4, U>& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 4);
        return (&x)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 4);
        return (&x)[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& v1, const type& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }

    friend inline constexpr bool operator!=(const type& v1, const type& v2)
    {
        return !(v1 == v2);
    }

    friend inline constexpr bool operator<(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        if (v1.z != v2.z) return (v1.z < v2.z);
        else              return (v1.w < v2.w);
    }

    friend inline constexpr bool operator>(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        if (v1.z != v2.z) return (v1.z > v2.z);
        else              return (v1.w > v2.w);
    }

    friend inline constexpr bool operator<=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        if (v1.z != v2.z) return (v1.z < v2.z);
        else              return (v1.w <= v2.w);
    }

    friend inline constexpr bool operator>=(const type& v1, const type& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        if (v1.z != v2.z) return (v1.z > v2.z);
        else              return (v1.w >= v2.w);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return type(+x, +y, +z, +w);
    }

    inline constexpr type operator-() const
    {
        return type(-x, -y, -z, -w);
    }

    // =============== incrememnt and decrement operators ===============

    // incrememnt (++)

    inline constexpr type& operator++()
    {
        x++;
        y++;
        z++;
        w++;
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
        w--;
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
        return type(v.x + scaler, v.y + scaler, v.z + scaler, v.w + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& v)
    {
        return v + scaler;
    }

    friend inline constexpr type operator+(const type& v1, const type& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& v, T scaler)
    {
        return type(v.x - scaler, v.y - scaler, v.z - scaler, v.w - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& v)
    {
        return v - scaler;
    }

    friend inline constexpr type operator-(const type& v1, const type& v2)
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& v, T scaler)
    {
        return type(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
    }

    friend inline constexpr type operator*(T scaler, const type& v)
    {
        return type(scaler * v.x, scaler * v.y, scaler * v.z, scaler * v.w);
    }

    friend inline constexpr type operator*(const type& v1, const type& v2)
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    // division (/)

    friend inline constexpr type operator/(const type& v, T scaler)
    {
        return type(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
    }

    friend inline constexpr type operator/(T scaler, const type& v)
    {
        return v / scaler;
    }

    friend inline constexpr type operator/(const type& v1, const type& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler, v.w % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v1, const type& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler, v.w & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v1, const type& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler, v.w | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v1, const type& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler, v.w ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v1, const type& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler, v.w << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v1, const type& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler, v.w >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v1, const type& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator~(const type& v)
    {
        return type(~v.x, ~v.y, ~v.z, ~v.w);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator+=(U scaler)
    {
        x += scaler;
        y += scaler;
        z += scaler;
        w += scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator+=(const vec<4, U>& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // subtraction (-=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator-=(U scaler)
    {
        x -= scaler;
        y -= scaler;
        z -= scaler;
        w -= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator-=(const vec<4, U>& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // multiplication (*=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator*=(U scaler)
    {
        x *= scaler;
        y *= scaler;
        z *= scaler;
        w *= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator*=(const vec<4, U>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // division (/=)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr type& operator/=(U scaler)
    {
        x /= scaler;
        y /= scaler;
        z /= scaler;
        w /= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator/=(const vec<4, U>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // modulo (%=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(U scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        w %= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(const vec<4, U>& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        w %= v.w;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(U scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        w &= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const vec<4, U>& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        w &= v.w;
        return *this;
    }

    // or (|=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(U scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        w |= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const vec<4, U>& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        w |= v.w;
        return *this;
    }

    // xor (^=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(U scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        w ^= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const vec<4, U>& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        w ^= v.w;
        return *this;
    }

    // left shift (<<=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(U scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        w <<= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const vec<4, U>& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        w <<= v.w;
        return *this;
    }

    // right shift (>>=)

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(U scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        w >>= scaler;
        return *this;
    }

    template <typename U, typename std::enable_if<std::is_integral<T>::value && std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(const vec<4, U>& v)
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
        w >>= v.w;
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
        return iterator(&w + 1);
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
        return const_iterator(&w + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&w + 1);
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
        return const_reverse_iterator(&w + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&x);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << +x << ", " << +y << ", " << +z << ", " << +w << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

    /**
     * @brief Get the minimum component value of the vector.
     *
     * @return The minimum component value.
     */
    inline constexpr T min() const { return math::min({ x, y, z, w }); }

    /**
     * @brief Get the maximum component value of the vector.
     *
     * @return The maximum component value.
     */
    inline constexpr T max() const { return math::max({ x, y, z, w }); }

    inline constexpr T width() const { return x; }
    inline constexpr T height() const { return y; }
    inline constexpr T depth() const { return z; }

    // =============== magnitude ===============

    /**
     * @brief Calculates the squared magnitude of the vector.
     *
     * @return The squared length of the vector.
     */
    inline constexpr float_value_type magnitude_squared() const
    {
        return static_cast<float_value_type>((x * x) + (y * y) + (z * z) + (w * w));
    }

    /**
     * @brief Calculates the magnitude of the vector.
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

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(0); }
    static inline constexpr type ONE() { return type(1); }

};

VX_PACK_POP()

using vec4  = vec<4, float>;
using vec4f = vec<4, float>;
using vec4d = vec<4, double>;
using vec4u = vec<4, uint32_t>;
using vec4i = vec<4, int32_t>;

}
}