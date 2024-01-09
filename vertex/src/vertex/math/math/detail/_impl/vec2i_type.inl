#pragma once

#include "../../fn_common.h"
#include "../../fn_comparison.h"
#include "../../fn_exponential.h"
#include "../../fn_trigonometric.h"

namespace vx {
namespace math {
namespace detail {

// =============== meta ===============

template <typename T>
inline constexpr typename veci<2, T>::size_type veci<2, T>::size() noexcept
{
    return static_cast<typename veci<2, T>::size_type>(2);
}

// =============== implicit constructors ===============

template <typename T>
inline constexpr veci<2, T>::vec() noexcept
    : x(static_cast<T>(0)), y(static_cast<T>(0)) {}

template <typename T>
inline constexpr veci<2, T>::vec(const type& v) noexcept
    : x(v.x), y(v.y) {}

// =============== explicit constructors ===============

template <typename T>
inline constexpr veci<2, T>::vec(T scaler) noexcept
    : x(scaler), y(scaler) {}

template <typename T>
inline constexpr veci<2, T>::vec(T x, T y) noexcept
    : x(x), y(y) {}

// =============== conversion vector constructors ===============

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<2, T>::vec(U scaler)
    : x(static_cast<T>(scaler))
    , y(static_cast<T>(scaler)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<2, T>::vec(U x, U y)
    : x(static_cast<T>(x))
    , y(static_cast<T>(y)) {}

template <typename T>
template <typename U>
inline constexpr veci<2, T>::vec(const vecx<2, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y)) {}

template <typename T>
template <typename U>
inline constexpr veci<2, T>::vec(const vecx<3, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y)) {}

template <typename T>
template <typename U>
inline constexpr veci<2, T>::vec(const vecx<4, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y)) {}

template <typename T>
inline constexpr typename veci<2, T>::float_type veci<2, T>::to_float() const
{
    return float_type(*this);
}

// =============== assignment operators ===============

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator=(const type& v) noexcept
{
    x = v.x;
    y = v.y;
    return *this;
}

// =============== accessors ===============

template <typename T>
inline constexpr T& veci<2, T>::operator[](size_type i)
{
    assert(i < 2);
    return (&x)[i];
}

template <typename T>
inline constexpr const T& veci<2, T>::operator[](size_type i) const
{
    assert(i < 2);
    return (&x)[i];
}

// =============== unary constant operators ===============

template <typename T>
inline constexpr veci<2, T> veci<2, T>::operator+() const
{
    return *this;
}

template <typename T>
inline constexpr veci<2, T> veci<2, T>::operator-() const
{
    return type(-x, -y);
}

// =============== incrememnt and decrement operators ===============

// increment (++)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator++()
{
    x++;
    y++;
    return *this;
}

template <typename T>
inline constexpr veci<2, T> veci<2, T>::operator++(int)
{
    type result(*this);
    ++(*this);
    return result;
}

// decrement (--)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator--()
{
    x--;
    y--;
    return *this;
}

template <typename T>
inline constexpr veci<2, T> veci<2, T>::operator--(int)
{
    type result(*this);
    --(*this);
    return result;
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator+=(T scaler)
{
    x += scaler;
    y += scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator+=(const type& v)
{
    x += v.x;
    y += v.y;
    return *this;
}

// subtraction (-=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator-=(T scaler)
{
    x -= scaler;
    y -= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator-=(const type& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

// multiplication (*=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator*=(T scaler)
{
    x *= scaler;
    y *= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator*=(const type& v)
{
    x *= v.x;
    y *= v.y;
    return *this;
}

// division (/=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator/=(T scaler)
{
    x /= scaler;
    y /= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator/=(const type& v)
{
    x /= v.x;
    y /= v.y;
    return *this;
}

// modulo (%=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator%=(T scaler)
{
    x %= scaler;
    y %= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator%=(const type& v)
{
    x %= v.x;
    y %= v.y;
    return *this;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator&=(T scaler)
{
    x &= scaler;
    y &= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator&=(const type& v)
{
    x &= v.x;
    y &= v.y;
    return *this;
}

// or (|=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator|=(T scaler)
{
    x |= scaler;
    y |= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator|=(const type& v)
{
    x |= v.x;
    y |= v.y;
    return *this;
}

// xor (^=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator^=(T scaler)
{
    x ^= scaler;
    y ^= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator^=(const type& v)
{
    x ^= v.x;
    y ^= v.y;
    return *this;
}

// left shift (<<=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator<<=(T scaler)
{
    x <<= scaler;
    y <<= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator<<=(const type& v)
{
    x <<= v.x;
    y <<= v.y;
    return *this;
}

// right shift (>>=)

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator>>=(T scaler)
{
    x >>= scaler;
    y >>= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<2, T>& veci<2, T>::operator>>=(const type& v)
{
    x >>= v.x;
    y >>= v.y;
    return *this;
}

// =============== iterator ===============

template <typename T>
inline T* veci<2, T>::begin() noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<2, T>::begin() const noexcept
{
    return cbegin();
}

template <typename T>
inline T* veci<2, T>::end() noexcept
{
    return &y + 1;
}

template <typename T>
inline const T* veci<2, T>::end() const noexcept
{
    return cend();
}

template <typename T>
inline const T* veci<2, T>::cbegin() const noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<2, T>::cend() const noexcept
{
    return &y + 1;
}

template <typename T>
inline auto veci<2, T>::rbegin() noexcept
{
    return std::reverse_iterator<T*>(&y + 1);
}

template <typename T>
inline const auto veci<2, T>::rbegin() const noexcept
{
    return crbegin();
}

template <typename T>
inline auto veci<2, T>::rend() noexcept
{
    return std::reverse_iterator<T*>(&x);
}

template <typename T>
inline const auto veci<2, T>::rend() const noexcept
{
    return crend();
}

template <typename T>
inline auto veci<2, T>::crbegin() const noexcept
{
    return std::reverse_iterator<const T*>(&y + 1);
}

template <typename T>
inline auto veci<2, T>::crend() const noexcept
{
    return std::reverse_iterator<const T*>(&x);
}

// =============== swizzle ===============

template <typename T>
inline constexpr T veci<2, T>::swizzle(size_type i) const
{
    return operator[](i);
}

template <typename T>
inline constexpr typename veci<2, T>::vec2_type veci<2, T>::swizzle(
    size_type i,
    size_type j
) const
{
    return vec2_type(operator[](i), operator[](j));
}

template <typename T>
inline constexpr typename veci<2, T>::vec3_type veci<2, T>::swizzle(
    size_type i,
    size_type j,
    size_type k
) const
{
    return vec3_type(operator[](i), operator[](j), operator[](k));
}

template <typename T>
inline constexpr typename veci<2, T>::vec4_type veci<2, T>::swizzle(
    size_type i,
    size_type j,
    size_type k,
    size_type l
) const
{
    return vec4_type(operator[](i), operator[](j), operator[](k), operator[](l));
}

// =============== string ===============

template <typename T>
inline constexpr std::string veci<2, T>::to_string(bool pretty_print) const
{
    std::ostringstream oss;
    oss << "{ " << x << ", " << y << " }";
    return oss.str();
}

// =============== comparison and testing ===============

template <typename T>
inline constexpr void veci<2, T>::set(T nxy) { x = y = nxy; }

template <typename T>
inline constexpr void veci<2, T>::set(T nx, T ny) { x = nx; y = ny; }

template <typename T>
inline constexpr T veci<2, T>::min() const { return math::min(x, y); }

template <typename T>
inline constexpr T veci<2, T>::max() const { return math::max(x, y); }

template <typename T>
inline constexpr T veci<2, T>::width() const { return x; }

template <typename T>
inline constexpr T veci<2, T>::height() const { return y; }

template <typename T>
inline constexpr typename veci<2, T>::float_value_type veci<2, T>::aspect() const
{
    if (math::is_zero_approx(y)) return static_cast<FT>(0);
    return (static_cast<FT>(x) / static_cast<FT>(y));
}

template <typename T>
inline constexpr typename veci<2, T>::float_value_type veci<2, T>::angle() const
{
    return math::atan2<FT>(y, x);
}

// =============== magnitude ===============

template <typename T>
inline constexpr typename veci<2, T>::float_value_type veci<2, T>::magnitude_squared() const { return static_cast<FT>((x * x) + (y * y)); }

template <typename T>
inline constexpr typename veci<2, T>::float_value_type veci<2, T>::magnitude() const { return math::sqrt(static_cast<FT>((x * x) + (y * y))); }

// =============== constants ===============

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::ZERO() { return type(); }

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::ONE() { return type(static_cast<T>(1)); }

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::RIGHT() { return type(static_cast<T>(1), static_cast<T>(0)); }

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::LEFT() { return type(static_cast<T>(-1), static_cast<T>(0)); }

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::UP() { return type(static_cast<T>(0), static_cast<T>(1)); }

template <typename T>
static inline constexpr veci<2, T> veci<2, T>::DOWN() { return type(static_cast<T>(0), static_cast<T>(-1)); }

// =============== comparison operators ===============

template <typename T>
inline constexpr bool operator==(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

template <typename T>
inline constexpr bool operator!=(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline constexpr bool operator<(const veci<2, T>& v1, const veci<2, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    else              return (v1.y < v2.y);
}

template <typename T>
inline constexpr bool operator>(const veci<2, T>& v1, const veci<2, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    else              return (v1.y > v2.y);
}

template <typename T>
inline constexpr bool operator<=(const veci<2, T>& v1, const veci<2, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    else              return (v1.y <= v2.y);
}

template <typename T>
inline constexpr bool operator>=(const veci<2, T>& v1, const veci<2, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    else              return (v1.y >= v2.y);
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T>
inline constexpr veci<2, T> operator+(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x + scaler, v.y + scaler);
}

template <typename T>
inline constexpr veci<2, T> operator+(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler + v.x, scaler + v.y);
}

template <typename T>
inline constexpr veci<2, T> operator+(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x + v2.x, v1.y + v2.y);
}

// subtraction (-)

template <typename T>
inline constexpr veci<2, T> operator-(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x - scaler, v.y - scaler);
}

template <typename T>
inline constexpr veci<2, T> operator-(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler - v.x, scaler - v.y);
}

template <typename T>
inline constexpr veci<2, T> operator-(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x - v2.x, v1.y - v2.y);
}

// multiplication (*)

template <typename T>
inline constexpr veci<2, T> operator*(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x * scaler, v.y * scaler);
}

template <typename T>
inline constexpr veci<2, T> operator*(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler * v.x, scaler * v.y);
}

template <typename T>
inline constexpr veci<2, T> operator*(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x * v2.x, v1.y * v2.y);
}

// division (/)

template <typename T>
inline constexpr veci<2, T> operator/(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x / scaler, v.y / scaler);
}

template <typename T>
inline constexpr veci<2, T> operator/(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler / v.x, scaler / v.y);
}

template <typename T>
inline constexpr veci<2, T> operator/(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x / v2.x, v1.y / v2.y);
}

// modulo (%)

template <typename T>
inline constexpr veci<2, T> operator%(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x % scaler, v.y % scaler);
}

template <typename T>
inline constexpr veci<2, T> operator%(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler % v.x, scaler % v.y);
}

template <typename T>
inline constexpr veci<2, T> operator%(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x % v2.x, v1.y % v2.y);
}

// =============== binary bit operators ===============

// and (&)

template <typename T>
inline constexpr veci<2, T> operator&(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x & scaler, v.y & scaler);
}

template <typename T>
inline constexpr veci<2, T> operator&(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler & v.x, scaler & v.y);
}

template <typename T>
inline constexpr veci<2, T> operator&(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x & v2.x, v1.y & v2.y);
}

// or (|)

template <typename T>
inline constexpr veci<2, T> operator|(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x | scaler, v.y | scaler);
}

template <typename T>
inline constexpr veci<2, T> operator|(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler | v.x, scaler | v.y);
}

template <typename T>
inline constexpr veci<2, T> operator|(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x | v2.x, v1.y | v2.y);
}

// xor (^)

template <typename T>
inline constexpr veci<2, T> operator^(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x ^ scaler, v.y ^ scaler);
}

template <typename T>
inline constexpr veci<2, T> operator^(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler ^ v.x, scaler ^ v.y);
}

template <typename T>
inline constexpr veci<2, T> operator^(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x ^ v2.x, v1.y ^ v2.y);
}

// left shift (<<)

template <typename T>
inline constexpr veci<2, T> operator<<(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x << scaler, v.y << scaler);
}

template <typename T>
inline constexpr veci<2, T> operator<<(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler << v.x, scaler << v.y);
}

template <typename T>
inline constexpr veci<2, T> operator<<(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x << v2.x, v1.y << v2.y);
}

// right shift (>>)

template <typename T>
inline constexpr veci<2, T> operator>>(const veci<2, T>& v, T scaler)
{
    return veci<2, T>(v.x >> scaler, v.y >> scaler);
}

template <typename T>
inline constexpr veci<2, T> operator>>(T scaler, const veci<2, T>& v)
{
    return veci<2, T>(scaler >> v.x, scaler >> v.y);
}

template <typename T>
inline constexpr veci<2, T> operator>>(const veci<2, T>& v1, const veci<2, T>& v2)
{
    return veci<2, T>(v1.x >> v2.x, v1.y >> v2.y);
}

// not (~)

template <typename T>
inline constexpr veci<2, T> operator~(const veci<2, T>& v)
{
    return veci<2, T>(~v.x, ~v.y);
}

}
}
}