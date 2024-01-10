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

    using vec2_type = vec<2, T, vec_t::vec, val_t::integral>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::integral>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::integral>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(3); }

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

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr vec(U scaler)
        : x(static_cast<T>(scaler))
        , y(static_cast<T>(scaler))
        , z(static_cast<T>(scaler)) {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr vec(U x, U y, U z)
        : x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr vec(const vecx<2, U>& vxy, U z)
        : x(static_cast<T>(vxy.x))
        , y(static_cast<T>(vxy.y))
        , z(static_cast<T>(z)) {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
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

    inline constexpr float_type to_float() const
    {
        return float_type(*this);
    }

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
        return type(scaler + v.x, scaler + v.y, scaler + v.z);
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
        return type(scaler - v.x, scaler - v.y, scaler - v.z);
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
        return type(scaler * v.x, scaler * v.y, scaler * v.z);
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
        return type(scaler / v.x, scaler / v.y, scaler / v.z);
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
        return type(scaler % v.x, scaler % v.y, scaler % v.z);
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
        return type(scaler & v.x, scaler & v.y, scaler & v.z);
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
        return type(scaler | v.x, scaler | v.y, scaler | v.z);
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
        return type(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z);
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
        return type(scaler << v.x, scaler << v.y, scaler << v.z);
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
        return type(scaler >> v.x, scaler >> v.y, scaler >> v.z);
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

    inline T* begin() noexcept
    {
        return &x;
    }

    inline const T* begin() const noexcept
    {
        return cbegin();
    }

    inline T* end() noexcept
    {
        return &z + 1;
    }

    inline const T* end() const noexcept
    {
        return cend();
    }

    inline const T* cbegin() const noexcept
    {
        return &x;
    }

    inline const T* cend() const noexcept
    {
        return &z + 1;
    }

    inline auto rbegin() noexcept
    {
        return std::reverse_iterator<T*>(&z + 1);
    }

    inline const auto rbegin() const noexcept
    {
        return crbegin();
    }

    inline auto rend() noexcept
    {
        return std::reverse_iterator<T*>(&x);
    }

    inline const auto rend() const noexcept
    {
        return crend();
    }

    inline auto crbegin() const noexcept
    {
        return std::reverse_iterator<const T*>(&z + 1);
    }

    inline auto crend() const noexcept
    {
        return std::reverse_iterator<const T*>(&x);
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

    inline constexpr type swizzle(size_type i, size_type j, size_type k) const
    {
        return type(operator[](i), operator[](j), operator[](k));
    }

    inline constexpr vec4_type swizzle(size_type i, size_type j, size_type k, size_type l) const
    {
        return vec4_type(operator[](i), operator[](j), operator[](k), operator[](l));
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        std::ostringstream oss;
        oss << "{ " << x << ", " << y << ", " << z << " }";
        return oss.str();
    }

    // =============== comparison and testing ===============

    inline constexpr void set(T nxyz) { x = y = z = nxyz; }
    inline constexpr void set(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }

    inline constexpr T min() const { return math::min({ x, y, z }); }
    inline constexpr T max() const { return math::max({ x, y, z }); }

    inline constexpr T width() const { return x; }
    inline constexpr T height() const { return y; }
    inline constexpr T depth() const { return z; }

    // =============== magnitude ===============

    inline constexpr FT magnitude_squared() const { return static_cast<FT>((x * x) + (y * y) + (z * z)); }
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
}
}