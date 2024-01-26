#pragma once

#include <sstream>

#include "base_type_defs.h"
#include "../fn_exponential.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct vec<4, T, vec_t::vec, val_t::integral>
{
    static_assert(std::is_integral<T>::value, "type T must be integral type");

    // =============== meta ===============

private:

    using FT = typename detail::to_float_type<T>::type;

public:

    using value_type = T;
    using float_value_type = FT;

    using type = vec<4, T, vec_t::vec, val_t::integral>;
    using float_type = vec<4, FT, vec_t::vec, val_t::floating_point>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(4); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::integral>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, U y, U z, U w)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(const vecx<2, U>& vxy, U z, U w)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z))
        , w(static_cast<T>(w)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, U y, const vecx<2, U>& vzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(const vecx<3, U>& vxyz, U w)
        : x(static_cast<T>(vxyz.x))
        , y(static_cast<T>(vxyz.y))
        , z(static_cast<T>(vxyz.z))
        , w(static_cast<T>(w)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr vec(U x, const vecx<3, U>& vyzw)
        : x(static_cast<T>(x))
        , y(static_cast<T>(vyzw.x))
        , z(static_cast<T>(vyzw.y))
        , w(static_cast<T>(vyzw.z)) {}

    template <typename U>
    inline constexpr vec(const vecx<2, U>& vxy, const vecx<2, U>& vzw)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(vzw.x))
        , w(static_cast<T>(vzw.y)) {}

    template <typename U>
    inline constexpr explicit vec(const vecx<4, U>& v)
        : x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z))
        , w(static_cast<T>(v.w)) {}

    template <typename U>
    inline constexpr explicit vec(const colx<U>& v)
        : x(static_cast<T>(v.r))
        , y(static_cast<T>(v.g))
        , z(static_cast<T>(v.b))
        , w(static_cast<T>(v.a)) {}

    // =============== destructor ===============

    ~vec() noexcept = default;

    // =============== assignment operators ===============

    template <typename U>
    inline constexpr type& operator=(const vecx<4, U>& v)
    {
        x = static_cast<T>(v.x);
        y = static_cast<T>(v.y);
        z = static_cast<T>(v.z);
        w = static_cast<T>(v.w);
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

    template <typename U>
    friend inline constexpr bool operator==(const type& v1, const vecx<4, U>& v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }

    template <typename U>
    friend inline constexpr bool operator!=(const type& v1, const vecx<4, U>& v2)
    {
        return !(v1 == v2);
    }

    template <typename U>
    friend inline constexpr bool operator<(const type& v1, const vecx<4, U>& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        if (v1.z != v2.z) return (v1.z < v2.z);
        else              return (v1.w < v2.w);
    }

    template <typename U>
    friend inline constexpr bool operator>(const type& v1, const vecx<4, U>& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        if (v1.z != v2.z) return (v1.z > v2.z);
        else              return (v1.w > v2.w);
    }

    template <typename U>
    friend inline constexpr bool operator<=(const type& v1, const vecx<4, U>& v2)
    {
        if (v1.x != v2.x) return (v1.x < v2.x);
        if (v1.y != v2.y) return (v1.y < v2.y);
        if (v1.z != v2.z) return (v1.z < v2.z);
        else              return (v1.w <= v2.w);
    }

    template <typename U>
    friend inline constexpr bool operator>=(const type& v1, const vecx<4, U>& v2)
    {
        if (v1.x != v2.x) return (v1.x > v2.x);
        if (v1.y != v2.y) return (v1.y > v2.y);
        if (v1.z != v2.z) return (v1.z > v2.z);
        else              return (v1.w >= v2.w);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return *this;
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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator+(const type& v, U scaler)
    {
        return type(v.x + scaler, v.y + scaler, v.z + scaler, v.w + scaler);
    }

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator+(U scaler, const type& v)
    {
        return v + scaler;
    }

    template <typename U>
    friend inline constexpr type operator+(const type& v1, const vecx<4, U>& v2)
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator-(const type& v, U scaler)
    {
        return type(v.x - scaler, v.y - scaler, v.z - scaler, v.w - scaler);
    }

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator-(U scaler, const type& v)
    {
        return v - scaler;
    }

    template <typename U>
    friend inline constexpr type operator-(const type& v1, const vecx<4, U>& v2)
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

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator/(const type& v, U scaler)
    {
        return type(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
    }

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    friend inline constexpr type operator/(U scaler, const type& v)
    {
        return v / scaler;
    }

    template <typename U>
    friend inline constexpr type operator/(const type& v1, const vecx<4, U>& v2)
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    // modulo (%)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& v, T scaler)
    {
        return type(v.x % scaler, v.y % scaler, v.z % scaler, v.w % scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& v)
    {
        return v % scaler;
    }

    template <typename U>
    friend inline constexpr type operator%(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& v, T scaler)
    {
        return type(v.x & scaler, v.y & scaler, v.z & scaler, v.w & scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& v)
    {
        return v & scaler;
    }

    template <typename U>
    friend inline constexpr type operator&(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
    }

    // or (|)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& v, T scaler)
    {
        return type(v.x | scaler, v.y | scaler, v.z | scaler, v.w | scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& v)
    {
        return v | scaler;
    }

    template <typename U>
    friend inline constexpr type operator|(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
    }

    // xor (^)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& v, T scaler)
    {
        return type(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler, v.w ^ scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& v)
    {
        return v ^ scaler;
    }

    template <typename U>
    friend inline constexpr type operator^(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
    }

    // left shift (<<)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& v, T scaler)
    {
        return type(v.x << scaler, v.y << scaler, v.z << scaler, v.w << scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& v)
    {
        return v << scaler;
    }

    template <typename U>
    friend inline constexpr type operator<<(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
    }

    // right shift (>>)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& v, T scaler)
    {
        return type(v.x >> scaler, v.y >> scaler, v.z >> scaler, v.w >> scaler);
    }

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& v)
    {
        return v >> scaler;
    }

    template <typename U>
    friend inline constexpr type operator>>(const type& v1, const veci<4, U>& v2)
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
    }

    // not (~)

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
    inline constexpr type& operator+=(const vecx<4, U>& v)
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
    inline constexpr type& operator-=(const vecx<4, U>& v)
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
    inline constexpr type& operator*=(const vecx<4, U>& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator*=(const mat<4, 4, U>& m)
    {
        return ((*this) = (*this) * m);
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
    inline constexpr type& operator/=(const vecx<4, U>& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // modulo (%=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(U scaler)
    {
        x %= scaler;
        y %= scaler;
        z %= scaler;
        w %= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator%=(const veci<4, U>& v)
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        w %= v.w;
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(U scaler)
    {
        x &= scaler;
        y &= scaler;
        z &= scaler;
        w &= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator&=(const veci<4, U>& v)
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        w &= v.w;
        return *this;
    }

    // or (|=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(U scaler)
    {
        x |= scaler;
        y |= scaler;
        z |= scaler;
        w |= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator|=(const veci<4, U>& v)
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        w |= v.w;
        return *this;
    }

    // xor (^=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(U scaler)
    {
        x ^= scaler;
        y ^= scaler;
        z ^= scaler;
        w ^= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator^=(const veci<4, U>& v)
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        w ^= v.w;
        return *this;
    }

    // left shift (<<=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(U scaler)
    {
        x <<= scaler;
        y <<= scaler;
        z <<= scaler;
        w <<= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator<<=(const veci<4, U>& v)
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        w <<= v.w;
        return *this;
    }

    // right shift (>>=)

    template <typename U, typename std::enable_if<std::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(U scaler)
    {
        x >>= scaler;
        y >>= scaler;
        z >>= scaler;
        w >>= scaler;
        return *this;
    }

    template <typename U>
    inline constexpr type& operator>>=(const veci<4, U>& v)
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

    inline constexpr type swizzle(size_type i, size_type j, size_type k, size_type l) const
    {
        return type(operator[](i), operator[](j), operator[](k), operator[](l));
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << x << ", " << y << ", " << z << ", " << w << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

    inline constexpr void set(T nxyzw) { x = y = z = w = nxyzw; }
    inline constexpr void set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }

    /**
     * @brief Get the minimum component value of the vector.
     *
     * This function returns the minimum value between the x and y components of the vector.
     *
     * @return The minimum component value.
     */
    inline constexpr T min() const { return math::min({ x, y, z, w }); }

    /**
     * @brief Get the maximum component value of the vector.
     *
     * This function returns the maximum value between the x and y components of the vector.
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
     * This function computes the squared magnitude of the vector.
     *
     * @return The squared length of the vector.
     */
    inline constexpr FT magnitude_squared() const { return static_cast<FT>((x * x) + (y * y) + (z * z) + (w * w)); }
    
    /**
     * @brief Calculates the magnitude of the vector.
     *
     * This function computes the magnitude of the vector.
     *
     * @return The magnitude of the vector.
     */
    inline constexpr FT magnitude() const { return math::sqrt(static_cast<FT>((x * x) + (y * y) + (z * z) + (w * w))); }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(); }
    static inline constexpr type ONE() { return type(static_cast<T>(1)); }

};

}
}
}