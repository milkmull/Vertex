#pragma once

#include "../../fn_exponential.h"
#include "../../fn_trigonometric.h"

namespace vx {
namespace math {
namespace detail {

// =============== meta ===============

template <typename T>
inline constexpr typename veci<4, T>::size_type veci<4, T>::size() noexcept
{
    return static_cast<typename veci<4, T>::size_type>(4);
}

// =============== implicit constructors ===============

template <typename T>
inline constexpr veci<4, T>::vec() noexcept
    : x(static_cast<T>(0))
    , y(static_cast<T>(0))
    , z(static_cast<T>(0))
    , w(static_cast<T>(0)) {}

template <typename T>
inline constexpr veci<4, T>::vec(const type& v) noexcept
    : x(v.x), y(v.y), z(v.z), w(v.w) {}

// =============== explicit constructors ===============

template <typename T>
inline constexpr veci<4, T>::vec(T scaler) noexcept
    : x(scaler), y(scaler), z(scaler), w(scaler) {}

template <typename T>
inline constexpr veci<4, T>::vec(T x, T y, T z, T w) noexcept
    : x(x), y(y), z(z), w(w) {}

// =============== conversion vector constructors ===============

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(U scaler)
    : x(static_cast<T>(scaler))
    , y(static_cast<T>(scaler))
    , z(static_cast<T>(scaler))
    , w(static_cast<T>(scaler)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(U x, U y, U z, U w)
    : x(static_cast<T>(x))
    , y(static_cast<T>(y))
    , z(static_cast<T>(z))
    , w(static_cast<T>(w)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(const vecx<2, U>& vxy, U z, U w)
    : x(static_cast<T>(vxy.x))
    , y(static_cast<T>(vxy.y))
    , z(static_cast<T>(z))
    , w(static_cast<T>(w)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(U x, U y, const vecx<2, U>& vzw)
    : x(static_cast<T>(x))
    , y(static_cast<T>(y))
    , z(static_cast<T>(vzw.x))
    , w(static_cast<T>(vzw.y)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(const vecx<3, U>& vxyz, U w)
    : x(static_cast<T>(vxyz.x))
    , y(static_cast<T>(vxyz.y))
    , z(static_cast<T>(vxyz.z))
    , w(static_cast<T>(w)) {}

template <typename T>
template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool>>
inline constexpr veci<4, T>::vec(U x, const vecx<3, U>& vyzw)
    : x(static_cast<T>(x))
    , y(static_cast<T>(vyzw.x))
    , z(static_cast<T>(vyzw.y))
    , w(static_cast<T>(vyzw.z)) {}

template <typename T>
template <typename U>
inline constexpr veci<4, T>::vec(const vecx<2, U>& vxy, const vecx<2, U>& vzw)
    : x(static_cast<T>(vxy.x))
    , y(static_cast<T>(vxy.y))
    , z(static_cast<T>(vzw.x))
    , w(static_cast<T>(vzw.y)) {}

template <typename T>
template <typename U>
inline constexpr veci<4, T>::vec(const vecx<4, U>& v)
    : x(static_cast<T>(v.x))
    , y(static_cast<T>(v.y))
    , z(static_cast<T>(v.z))
    , w(static_cast<T>(v.w)) {}

template <typename T>
inline constexpr typename veci<4, T>::float_type veci<4, T>::to_float() const
{
    return float_type(*this);
}

// =============== assignment operators ===============

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator=(const type& v) noexcept
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

// =============== accessors ===============

template <typename T>
inline constexpr T& veci<4, T>::operator[](size_type i)
{
    assert(i < 4);
    return (&x)[i];
}

template <typename T>
inline constexpr const T& veci<4, T>::operator[](size_type i) const
{
    assert(i < 4);
    return (&x)[i];
}

// =============== unary constant operators ===============

template <typename T>
inline constexpr veci<4, T> veci<4, T>::operator+() const
{
    return *this;
}

template <typename T>
inline constexpr veci<4, T> veci<4, T>::operator-() const
{
    return type(-x, -y, -z, -w);
}

// =============== incrememnt and decrement operators ===============

// incrememnt (++)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator++()
{
    x++;
    y++;
    z++;
    w++;
    return *this;
}

template <typename T>
inline constexpr veci<4, T> veci<4, T>::operator++(int)
{
    type result(*this);
    ++(*this);
    return result;
}

// decrement (--)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator--()
{
    x--;
    y--;
    z--;
    w--;
    return *this;
}

template <typename T>
inline constexpr veci<4, T> veci<4, T>::operator--(int)
{
    type result(*this);
    --(*this);
    return result;
}

// =============== unary arithmetic operators ===============

// addition (+=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator+=(T scaler)
{
    x += scaler;
    y += scaler;
    z += scaler;
    w += scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator+=(const type& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

// subtraction (-=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator-=(T scaler)
{
    x -= scaler;
    y -= scaler;
    z -= scaler;
    w -= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator-=(const type& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

// multiplication (*=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator*=(T scaler)
{
    x *= scaler;
    y *= scaler;
    z *= scaler;
    w *= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator*=(const type& v)
{
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
}

// division (/=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator/=(T scaler)
{
    x /= scaler;
    y /= scaler;
    z /= scaler;
    w /= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator/=(const type& v)
{
    x /= v.x;
    y /= v.y;
    z /= v.z;
    w /= v.w;
    return *this;
}

// modulo (%=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator%=(T scaler)
{
    x %= scaler;
    y %= scaler;
    z %= scaler;
    w %= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator%=(const type& v)
{
    x %= v.x;
    y %= v.y;
    z %= v.z;
    w %= v.w;
    return *this;
}

// =============== unary bit operators ===============

// and (&=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator&=(T scaler)
{
    x &= scaler;
    y &= scaler;
    z &= scaler;
    w &= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator&=(const type& v)
{
    x &= v.x;
    y &= v.y;
    z &= v.z;
    w &= v.w;
    return *this;
}

// or (|=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator|=(T scaler)
{
    x |= scaler;
    y |= scaler;
    z |= scaler;
    w |= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator|=(const type& v)
{
    x |= v.x;
    y |= v.y;
    z |= v.z;
    w |= v.w;
    return *this;
}

// xor (^=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator^=(T scaler)
{
    x ^= scaler;
    y ^= scaler;
    z ^= scaler;
    w ^= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator^=(const type& v)
{
    x ^= v.x;
    y ^= v.y;
    z ^= v.z;
    w ^= v.w;
    return *this;
}

// left shift (<<=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator<<=(T scaler)
{
    x <<= scaler;
    y <<= scaler;
    z <<= scaler;
    w <<= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator<<=(const type& v)
{
    x <<= v.x;
    y <<= v.y;
    z <<= v.z;
    w <<= v.w;
    return *this;
}

// right shift (>>=)

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator>>=(T scaler)
{
    x >>= scaler;
    y >>= scaler;
    z >>= scaler;
    w >>= scaler;
    return *this;
}

template <typename T>
inline constexpr veci<4, T>& veci<4, T>::operator>>=(const type& v)
{
    x >>= v.x;
    y >>= v.y;
    z >>= v.z;
    w >>= v.w;
    return *this;
}

// =============== iterator ===============

template <typename T>
inline T* veci<4, T>::begin() noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<4, T>::begin() const noexcept
{
    return cbegin();
}

template <typename T>
inline T* veci<4, T>::end() noexcept
{
    return &w + 1;
}

template <typename T>
inline const T* veci<4, T>::end() const noexcept
{
    return cend();
}

template <typename T>
inline const T* veci<4, T>::cbegin() const noexcept
{
    return &x;
}

template <typename T>
inline const T* veci<4, T>::cend() const noexcept
{
    return &w + 1;
}

template <typename T>
inline auto veci<4, T>::rbegin() noexcept
{
    return std::reverse_iterator<T*>(&w + 1);
}

template <typename T>
inline const auto veci<4, T>::rbegin() const noexcept
{
    return crbegin();
}

template <typename T>
inline auto veci<4, T>::rend() noexcept
{
    return std::reverse_iterator<T*>(&x);
}

template <typename T>
inline const auto veci<4, T>::rend() const noexcept
{
    return crend();
}

template <typename T>
inline auto veci<4, T>::crbegin() const noexcept
{
    return std::reverse_iterator<const T*>(&w + 1);
}

template <typename T>
inline auto veci<4, T>::crend() const noexcept
{
    return std::reverse_iterator<const T*>(&x);
}

// =============== swizzle ===============

template <typename T>
inline constexpr T veci<4, T>::swizzle(size_type i) const
{
    return operator[](i);
}

template <typename T>
inline constexpr typename veci<4, T>::vec2_type veci<4, T>::swizzle(
    size_type i,
    size_type j
) const
{
    return vec2_type(operator[](i), operator[](j));
}

template <typename T>
inline constexpr typename veci<4, T>::vec3_type veci<4, T>::swizzle(
    size_type i,
    size_type j,
    size_type k
) const
{
    return vec3_type(operator[](i), operator[](j), operator[](k));
}

template <typename T>
inline constexpr typename veci<4, T>::vec4_type veci<4, T>::swizzle(
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
inline constexpr std::string veci<4, T>::to_string(bool pretty_print) const
{
    std::ostringstream oss;
    oss << "{ " << x << ", " << y << ", " << z << ", " << w << " }";
    return oss.str();
}

// =============== comparison and testing ===============

template <typename T>
inline constexpr void veci<4, T>::set(T nxyzw) { x = y = z = w = nxyzw; }

template <typename T>
inline constexpr void veci<4, T>::set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }

template <typename T>
inline constexpr T veci<4, T>::min() const { return math::min({ x, y, z, w }); }

template <typename T>
inline constexpr T veci<4, T>::max() const { return math::max({ x, y, z, w }); }

template <typename T>
inline constexpr T veci<4, T>::width() const { return x; }

template <typename T>
inline constexpr T veci<4, T>::height() const { return y; }

template <typename T>
inline constexpr T veci<4, T>::depth() const { return z; }

// =============== magnitude ===============

template <typename T>
inline constexpr typename veci<4, T>::float_value_type veci<4, T>::magnitude_squared() const
{
    return static_cast<FT>((x * x) + (y * y) + (z * z) + (w * w));
}

template <typename T>
inline constexpr typename veci<4, T>::float_value_type veci<4, T>::magnitude() const
{
    return math::sqrt(static_cast<FT>((x * x) + (y * y) + (z * z) + (w * w)));
}

// =============== constants ===============

template <typename T>
inline constexpr veci<4, T> veci<4, T>::ZERO() { return type(); }

template <typename T>
inline constexpr veci<4, T> veci<4, T>::ONE() { return type(static_cast<T>(1)); }

// =============== boolean operators ===============

template <typename T>
inline constexpr bool operator==(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

template <typename T>
inline constexpr bool operator!=(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return !(v1 == v2);
}

template <typename T>
inline constexpr bool operator<(const veci<4, T>& v1, const veci<4, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    if (v1.y != v2.y) return (v1.y < v2.y);
    if (v1.z != v2.z) return (v1.z < v2.z);
    else              return (v1.w < v2.w);
}

template <typename T>
inline constexpr bool operator>(const veci<4, T>& v1, const veci<4, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    if (v1.y != v2.y) return (v1.y > v2.y);
    if (v1.z != v2.z) return (v1.z > v2.z);
    else              return (v1.w > v2.w);
}

template <typename T>
inline constexpr bool operator<=(const veci<4, T>& v1, const veci<4, T>& v2)
{
    if (v1.x != v2.x) return (v1.x < v2.x);
    if (v1.y != v2.y) return (v1.y < v2.y);
    if (v1.z != v2.z) return (v1.z < v2.z);
    else              return (v1.w <= v2.w);
}

template <typename T>
inline constexpr bool operator>=(const veci<4, T>& v1, const veci<4, T>& v2)
{
    if (v1.x != v2.x) return (v1.x > v2.x);
    if (v1.y != v2.y) return (v1.y > v2.y);
    if (v1.z != v2.z) return (v1.z > v2.z);
    else              return (v1.w >= v2.w);
}

// =============== binary arithmetic operators ===============

// addition (+)

template <typename T>
inline constexpr veci<4, T> operator+(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x + scaler, v.y + scaler, v.z + scaler, v.w + scaler);
}

template <typename T>
inline constexpr veci<4, T> operator+(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler + v.x, scaler + v.y, scaler + v.z, scaler + v.w);
}

template <typename T>
inline constexpr veci<4, T> operator+(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

// subtraction (-)

template <typename T>
inline constexpr veci<4, T> operator-(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x - scaler, v.y - scaler, v.z - scaler, v.w - scaler);
}

template <typename T>
inline constexpr veci<4, T> operator-(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler - v.x, scaler - v.y, scaler - v.z, scaler - v.w);
}

template <typename T>
inline constexpr veci<4, T> operator-(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

// multiplication (*)

template <typename T>
inline constexpr veci<4, T> operator*(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x * scaler, v.y * scaler, v.z * scaler, v.w * scaler);
}

template <typename T>
inline constexpr veci<4, T> operator*(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler * v.x, scaler * v.y, scaler * v.z, scaler * v.w);
}

template <typename T>
inline constexpr veci<4, T> operator*(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

// division (/)

template <typename T>
inline constexpr veci<4, T> operator/(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x / scaler, v.y / scaler, v.z / scaler, v.w / scaler);
}

template <typename T>
inline constexpr veci<4, T> operator/(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler / v.x, scaler / v.y, scaler / v.z, scaler / v.w);
}

template <typename T>
inline constexpr veci<4, T> operator/(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

// modulo (%)

template <typename T>
inline constexpr veci<4, T> operator%(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x % scaler, v.y % scaler, v.z % scaler, v.w % scaler);
}

template <typename T>
inline constexpr veci<4, T> operator%(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler % v.x, scaler % v.y, scaler % v.z, scaler % v.w);
}

template <typename T>
inline constexpr veci<4, T> operator%(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
}

// =============== binary bit operators ===============

// and (&)

template <typename T>
inline constexpr veci<4, T> operator&(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x & scaler, v.y & scaler, v.z & scaler, v.w & scaler);
}

template <typename T>
inline constexpr veci<4, T> operator&(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler & v.x, scaler & v.y, scaler & v.z, scaler & v.w);
}

template <typename T>
inline constexpr veci<4, T> operator&(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
}

// or (|)

template <typename T>
inline constexpr veci<4, T> operator|(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x | scaler, v.y | scaler, v.z | scaler, v.w | scaler);
}

template <typename T>
inline constexpr veci<4, T> operator|(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler | v.x, scaler | v.y, scaler | v.z, scaler | v.w);
}

template <typename T>
inline constexpr veci<4, T> operator|(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
}

// xor (^)

template <typename T>
inline constexpr veci<4, T> operator^(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x ^ scaler, v.y ^ scaler, v.z ^ scaler, v.w ^ scaler);
}

template <typename T>
inline constexpr veci<4, T> operator^(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler ^ v.x, scaler ^ v.y, scaler ^ v.z, scaler ^ v.w);
}

template <typename T>
inline constexpr veci<4, T> operator^(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
}

// left shift (<<)

template <typename T>
inline constexpr veci<4, T> operator<<(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x << scaler, v.y << scaler, v.z << scaler, v.w << scaler);
}

template <typename T>
inline constexpr veci<4, T> operator<<(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler << v.x, scaler << v.y, scaler << v.z, scaler << v.w);
}

template <typename T>
inline constexpr veci<4, T> operator<<(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
}

// right shift (>>)

template <typename T>
inline constexpr veci<4, T> operator>>(const veci<4, T>& v, T scaler)
{
    return veci<4, T>(v.x >> scaler, v.y >> scaler, v.z >> scaler, v.w >> scaler);
}

template <typename T>
inline constexpr veci<4, T> operator>>(T scaler, const veci<4, T>& v)
{
    return veci<4, T>(scaler >> v.x, scaler >> v.y, scaler >> v.z, scaler >> v.w);
}

template <typename T>
inline constexpr veci<4, T> operator>>(const veci<4, T>& v1, const veci<4, T>& v2)
{
    return veci<4, T>(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
}

// not (~)

template <typename T>
inline constexpr veci<4, T> operator~(const veci<4, T>& v)
{
    return veci<4, T>(~v.x, ~v.y, ~v.z, ~v.w);
}

}
}
}