#pragma once

#include "../base_type_defs.h"
#include "../../fn_exponential.h"
#include "../../fn_trigonometric.h"

namespace vx {
namespace math {
namespace detail {

// =============== meta ===============

template <typename T>
inline constexpr typename veci<3, T>::size_type veci<3, T>::size() noexcept
{
    return static_cast<typename veci<3, T>::size_type>(3);
}

// =============== implicit constructors ===============

template <typename T>
inline constexpr veci<3, T>::vec() noexcept
    : x(static_cast<T>(0))
    , y(static_cast<T>(0))
    , z(static_cast<T>(0)) {}

template <typename T>
inline constexpr veci<3, T>::vec(const type& v) noexcept
    : x(v.x), y(v.y), z(v.z) {}

// =============== explicit constructors ===============

template <typename T>
inline constexpr veci<3, T>::vec(T scaler) noexcept
    : x(scaler), y(scaler), z(scaler) {}

template <typename T>
inline constexpr veci<3, T>::vec(T x, T y, T z) noexcept
    : x(x), y(y), z(z) {}

// =============== conversion vector constructors ===============

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<3, T>::vec(U scaler)
    : x(static_cast<T>(scaler))
    , y(static_cast<T>(scaler))
    , z(static_cast<T>(scaler)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<3, T>::vec(U x, U y, U z)
    : x(static_cast<T>(x))
    , y(static_cast<T>(y))
    , z(static_cast<T>(z)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<3, T>::vec(const vecx<2, U>& vxy, U z)
    : x(static_cast<T>(vxy.x))
    , y(static_cast<T>(vxy.y))
    , z(static_cast<T>(z)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<3, T>::vec(U x, const vecx<2, U>& vyz)
    : x(static_cast<T>(x))
    , y(static_cast<T>(vyz.x))
    , z(static_cast<T>(vyz.y)) {}

template <typename T>
template <typename U>
inline constexpr veci<3, T>::vec(const vecx<3, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y))
    , z(static_cast<T>(v.z)) {}

template <typename T>
template <typename U>
inline constexpr veci<3, T>::vec(const vecx<4, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y))
    , z(static_cast<T>(v.z)) {}

template <typename T>
inline constexpr typename veci<3, T>::float_type veci<3, T>::to_float() const
{
    return float_type(*this);
}

// =============== assignment operators ===============

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator=(const type& v) noexcept
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

// =============== accessors ===============

template <typename T>
inline constexpr T& veci<3, T>::operator[](size_type i)
{
    assert(i < 3);
    return (&x)[i];
}

template <typename T>
inline constexpr const T& veci<3, T>::operator[](size_type i) const
{
    assert(i < 3);
    return (&x)[i];
}

// =============== unary constant operators ===============

template <typename T>
inline constexpr veci<3, T> veci<3, T>::operator+() const
{
    return *this;
}

template <typename T>
inline constexpr veci<3, T> veci<3, T>::operator-() const
{
    return type(-x, -y, -z);
}

// =============== incrememnt and decrement operators ===============

// increment (++)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator++()
{
    x++;
    y++;
    z++;
    return *this;
}

template <typename T>
inline constexpr veci<3, T> veci<3, T>::operator++(int)
{
    type result(*this);
    ++(*this);
    return result;
}

// decrement (--)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator--()
{
    x--;
    y--;
    z--;
    return *this;
}

template <typename T>
inline constexpr veci<3, T> veci<3, T>::operator--(int)
{
    type result(*this);
    --(*this);
    return result;
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator+=(T scaler)
{
    x += scaler;
    y += scaler;
    z += scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator+=(const type& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

// subtraction (-=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator-=(T scaler)
{
    x -= scaler;
    y -= scaler;
    z -= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator-=(const type& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

// multiplication (*=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator*=(T scaler)
{
    x *= scaler;
    y *= scaler;
    z *= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator*=(const type& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

// division (/=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator/=(T scaler)
{
    x /= scaler;
    y /= scaler;
    z /= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator/=(const type& v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
}

// modulo (%=)

template <typename T> constexpr veci<3, T>& veci<3, T>::operator%=(T scaler)
{
    x %= scaler;
    y %= scaler;
    z %= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator%=(const type& v)
{
    x %= v.x;
    y %= v.y;
    z %= v.z;
    return *this;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator&=(T scaler)
{
    x &= scaler;
    y &= scaler;
    z &= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator&=(const type& v)
{
    x &= v.x;
    y &= v.y;
    z &= v.z;
    return *this;
}

// or (|=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator|=(T scaler)
{
    x |= scaler;
    y |= scaler;
    z |= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator|=(const type& v)
{
    x |= v.x;
    y |= v.y;
    z |= v.z;
    return *this;
}

// xor (^=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator^=(T scaler)
{
    x ^= scaler;
    y ^= scaler;
    z ^= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator^=(const type& v)
{
    x ^= v.x;
    y ^= v.y;
    z ^= v.z;
    return *this;
}

// left shift (<<=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator<<=(T scaler)
{
    x <<= scaler;
    y <<= scaler;
    z <<= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator<<=(const type& v)
{
    x <<= v.x;
    y <<= v.y;
    z <<= v.z;
    return *this;
}

// right shift (>>=)

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator>>=(T scaler)
{
    x >>= scaler;
    y >>= scaler;
    z >>= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<3, T>& veci<3, T>::operator>>=(const type& v)
{
    x >>= v.x;
    y >>= v.y;
    z >>= v.z;
    return *this;
}

// =============== iterator ===============

template <typename T>
inline T* veci<3, T>::begin() noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<3, T>::begin() const noexcept
{
    return cbegin();
}

template <typename T>
inline T* veci<3, T>::end() noexcept
{
    return &z + 1;
}

template <typename T>
inline const T* veci<3, T>::end() const noexcept
{
    return cend();
}

template <typename T>
inline const T* veci<3, T>::cbegin() const noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<3, T>::cend() const noexcept
{
    return &z + 1;
}

template <typename T>
inline auto veci<3, T>::rbegin() noexcept
{
    return std::reverse_iterator<T*>(&z + 1);
}

template <typename T>
inline const auto veci<3, T>::rbegin() const noexcept
{
    return crbegin();
}

template <typename T>
inline auto veci<3, T>::rend() noexcept
{
    return std::reverse_iterator<T*>(&x);
}

template <typename T>
inline const auto veci<3, T>::rend() const noexcept
{
    return crend();
}

template <typename T>
inline auto veci<3, T>::crbegin() const noexcept
{
    return std::reverse_iterator<const T*>(&z + 1);
}

template <typename T>
inline auto veci<3, T>::crend() const noexcept
{
    return std::reverse_iterator<const T*>(&x);
}

// =============== swizzle ===============

template <typename T>
inline constexpr T veci<3, T>::swizzle(size_type i) const
{
    return operator[](i);
}

template <typename T>
inline constexpr typename veci<3, T>::vec2_type veci<3, T>::swizzle(
    size_type i,
    size_type j
) const
{
    return vec2_type(operator[](i), operator[](j));
}

template <typename T>
inline constexpr typename veci<3, T>::vec3_type veci<3, T>::swizzle(
    size_type i,
    size_type j,
    size_type k
) const
{
    return vec3_type(operator[](i), operator[](j), operator[](k));
}

template <typename T>
inline constexpr typename veci<3, T>::vec4_type veci<3, T>::swizzle(
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
inline constexpr std::string veci<3, T>::to_string(bool pretty_print) const
{
    std::ostringstream oss;
    oss << "{ " << x << ", " << y << ", " << z << " }";
    return oss.str();
}

// =============== comparison and testing ===============

template <typename T>
inline constexpr void veci<3, T>::set(T nxyz) { x = y = z = nxyz; }

template <typename T>
inline constexpr void veci<3, T>::set(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }

template <typename T>
inline constexpr T veci<3, T>::min() const { return math::min({ x, y, z }); }

template <typename T>
inline constexpr T veci<3, T>::max() const { return math::max({ x, y, z }); }

template <typename T>
inline constexpr T veci<3, T>::width() const { return x; }

template <typename T>
inline constexpr T veci<3, T>::height() const { return y; }

template <typename T>
inline constexpr T veci<3, T>::depth() const { return z; }

// =============== magnitude ===============

template <typename T>
inline constexpr typename veci<3, T>::float_value_type veci<3, T>::magnitude_squared() const
{
    return static_cast<FT>((x * x) + (y * y) + (z * z));
}

template <typename T>
inline constexpr typename veci<3, T>::float_value_type veci<3, T>::magnitude() const
{
    return math::sqrt(static_cast<FT>((x * x) + (y * y) + (z * z)));
}

// =============== constants ===============

template <typename T>
inline constexpr veci<3, T> veci<3, T>::ZERO() { return type(); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::ONE() { return type(static_cast<T>(1)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::RIGHT() { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::LEFT() { return type(static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::UP() { return type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::DOWN() { return type(static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)); }

#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_RH_BIT)

template <typename T>
inline constexpr veci<3, T> veci<3, T>::FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }

#else

template <typename T>
inline constexpr veci<3, T> veci<3, T>::FORWARD() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)); }

template <typename T>
inline constexpr veci<3, T> veci<3, T>::BACK() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)); }

#endif

// =============== comparison operators ===============

template <typename T>
inline constexpr bool operator==(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

template <typename T>
inline constexpr bool operator!=(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline constexpr bool operator<(const veci<3, T>& v1, const veci<3, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    if (v1.y != v2.y) return (v1.y < v2.y);
    else              return (v1.z < v2.z);
}

template <typename T>
inline constexpr bool operator>(const veci<3, T>& v1, const veci<3, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    if (v1.y != v2.y) return (v1.y > v2.y);
    else              return (v1.z > v2.z);
}

template <typename T>
inline constexpr bool operator<=(const veci<3, T>& v1, const veci<3, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    if (v1.y != v2.y) return (v1.y < v2.y);
    else              return (v1.z <= v2.z);
}

template <typename T>
inline constexpr bool operator>=(const veci<3, T>& v1, const veci<3, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    if (v1.y != v2.y) return (v1.y > v2.y);
    else              return (v1.z >= v2.z);
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T>
inline constexpr veci<3, T> operator+(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x + scaler, v.y + scaler, v.z + scaler);
}

template <typename T>
inline constexpr veci<3, T> operator+(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler + v.x, scaler + v.y, scaler + v.z);
}

template <typename T>
inline constexpr veci<3, T> operator+(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

// subtraction (-)

template <typename T>
inline constexpr veci<3, T> operator-(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x - scaler, v.y - scaler, v.z - scaler);
}

template <typename T>
inline constexpr veci<3, T> operator-(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler - v.x, scaler - v.y, scaler - v.z);
}

template <typename T>
inline constexpr veci<3, T> operator-(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

// multiplication (*)

template <typename T>
inline constexpr veci<3, T> operator*(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x * scaler, v.y * scaler, v.z * scaler);
}

template <typename T>
inline constexpr veci<3, T> operator*(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler * v.x, scaler * v.y, scaler * v.z);
}

template <typename T>
inline constexpr veci<3, T> operator*(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

// division (/)

template <typename T>
inline constexpr veci<3, T> operator/(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x / scaler, v.y / scaler, v.z / scaler);
}

template <typename T>
inline constexpr veci<3, T> operator/(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler / v.x, scaler / v.y, scaler / v.z);
}

template <typename T>
inline constexpr veci<3, T> operator/(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

// modulo (%)

template <typename T>
inline constexpr veci<3, T> operator%(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x % scaler, v.y % scaler, v.z % scaler);
}

template <typename T>
inline constexpr veci<3, T> operator%(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler % v.x, scaler % v.y, scaler % v.z);
}

template <typename T>
inline constexpr veci<3, T> operator%(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z);
}

// =============== binary bit operators ===============

// and (&)

template <typename T>
inline constexpr veci<3, T> operator&(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x & scaler, v.y & scaler, v.z & scaler);
}

template <typename T>
inline constexpr veci<3, T> operator&(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler & v.x, scaler & v.y, scaler & v.z);
}

template <typename T>
inline constexpr veci<3, T> operator&(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z);
}

// or (|)

template <typename T>
inline constexpr veci<3, T> operator|(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x | scaler, v.y | scaler, v.z | scaler);
}

template <typename T>
inline constexpr veci<3, T> operator|(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler | v.x, scaler | v.y, scaler | v.z);
}

template <typename T>
inline constexpr veci<3, T> operator|(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z);
}

// xor (^)

template <typename T>
inline constexpr veci<3, T> operator^(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler);
}

template <typename T>
inline constexpr veci<3, T> operator^(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z);
}

template <typename T>
inline constexpr veci<3, T> operator^(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z);
}

// left shift (<<)

template <typename T>
inline constexpr veci<3, T> operator<<(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x << scaler, v.y << scaler, v.z << scaler);
}

template <typename T>
inline constexpr veci<3, T> operator<<(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler << v.x, scaler << v.y, scaler << v.z);
}


template <typename T>
inline constexpr veci<3, T> operator<<(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z);
}

// right shift (>>)

template <typename T>
inline constexpr veci<3, T> operator>>(const veci<3, T>& v, T scaler)
{
    return veci<3, T>(v.x >> scaler, v.y >> scaler, v.z >> scaler);
}

template <typename T>
inline constexpr veci<3, T> operator>>(T scaler, const veci<3, T>& v)
{
    return veci<3, T>(scaler >> v.x, scaler >> v.y, scaler >> v.z);
}

template <typename T>
inline constexpr veci<3, T> operator>>(const veci<3, T>& v1, const veci<3, T>& v2)
{
    return veci<3, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z);
}

// not (~)

template <typename T>
inline constexpr veci<3, T> operator~(const veci<3, T>& v)
{
    return veci<3, T>(~v.x, ~v.y, ~v.z);
}

}
}
}