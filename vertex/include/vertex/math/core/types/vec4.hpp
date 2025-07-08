#pragma once

#include "vertex/config/assert.hpp"
#include "./base.hpp"

#include "../../simd/vec4f.hpp"

namespace vx {
namespace math {

template <typename T>
struct alignas(simd::vec<4, T>::calulate_alignment()) vec<4, T>
{
#if defined(VXM_ENABLE_SIMD)

#   define __SIMD_OVERLOAD(cond) template <typename __T = T, VXM_REQ( (simd::vec<4, __T>::cond))>
#   define __SIMD_FALLBACK(cond) template <typename __T = T, VXM_REQ(!(simd::vec<4, __T>::cond))>

#else

#   define __SIMD_OVERLOAD(cond) template <typename __T = T, VXM_REQ(!(is_same<__T, __T>::value))>
#   define __SIMD_FALLBACK(cond)

#endif

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scalar_type = T;
    static constexpr size_t size = 4;

    using type = vec<size, scalar_type>;

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    scalar_type x, y, z, w;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr vec() noexcept
        : x(static_cast<scalar_type>(0))
        , y(static_cast<scalar_type>(0))
        , z(static_cast<scalar_type>(0))
        , w(static_cast<scalar_type>(0)) {}

    VX_FORCE_INLINE constexpr vec(const type& v) noexcept
        : x(v.x), y(v.y), z(v.z), w(v.w) {}

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr explicit vec(scalar_type scalar) noexcept
        : x(scalar), y(scalar), z(scalar), w(scalar) {}

    VX_FORCE_INLINE constexpr vec(
        scalar_type vx,
        scalar_type vy,
        scalar_type vz,
        scalar_type vw
    ) noexcept
        : x(vx), y(vy), z(vz), w(vw) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(U scalar) noexcept
        : x(static_cast<scalar_type>(scalar))
        , y(static_cast<scalar_type>(scalar))
        , z(static_cast<scalar_type>(scalar))
        , w(static_cast<scalar_type>(scalar)) {}

    template <typename X, typename Y, typename Z, typename W>
    VX_FORCE_INLINE constexpr vec(X vx, Y vy, Z vz, W vw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy))
        , z(static_cast<scalar_type>(vz))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename XY, typename Z, typename W>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, Z vz, W vw) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(vz))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename YZ, typename W>
    VX_FORCE_INLINE constexpr vec(X vx, const vec<2, YZ>& vyz, W vw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vyz.x))
        , z(static_cast<scalar_type>(vyz.y))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename Y, typename ZW>
    VX_FORCE_INLINE constexpr vec(X vx, Y vy, const vec<2, ZW>& vzw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vy))
        , z(static_cast<scalar_type>(vzw.x))
        , w(static_cast<scalar_type>(vzw.y)) {}

    template <typename XYZ, typename W>
    VX_FORCE_INLINE constexpr vec(const vec<3, XYZ>& vxyz, W vw) noexcept
        : x(static_cast<scalar_type>(vxyz.x))
        , y(static_cast<scalar_type>(vxyz.y))
        , z(static_cast<scalar_type>(vxyz.z))
        , w(static_cast<scalar_type>(vw)) {}

    template <typename X, typename YZW>
    VX_FORCE_INLINE constexpr vec(X vx, const vec<3, YZW>& vyzw) noexcept
        : x(static_cast<scalar_type>(vx))
        , y(static_cast<scalar_type>(vyzw.x))
        , z(static_cast<scalar_type>(vyzw.y))
        , w(static_cast<scalar_type>(vyzw.z)) {}

    template <typename XY, typename ZW>
    VX_FORCE_INLINE constexpr vec(const vec<2, XY>& vxy, const vec<2, ZW>& vzw) noexcept
        : x(static_cast<scalar_type>(vxy.x))
        , y(static_cast<scalar_type>(vxy.y))
        , z(static_cast<scalar_type>(vzw.x))
        , w(static_cast<scalar_type>(vzw.y)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit vec(const vec<4, U>& v) noexcept
        : x(static_cast<scalar_type>(v.x))
        , y(static_cast<scalar_type>(v.y))
        , z(static_cast<scalar_type>(v.z))
        , w(static_cast<scalar_type>(v.w)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const vec<4, U>& v) noexcept
    {
        x = static_cast<scalar_type>(v.x);
        y = static_cast<scalar_type>(v.y);
        z = static_cast<scalar_type>(v.z);
        w = static_cast<scalar_type>(v.w);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr scalar_type& operator[](size_t i) noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    VX_FORCE_INLINE constexpr const scalar_type& operator[](size_t i) const noexcept
    {
        VX_ASSERT(i < size);

        switch (i)
        {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    // equal (==)

    friend VX_FORCE_INLINE constexpr bool operator==(const type& v1, const type& v2) noexcept
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
    }

    // not equal (!=)

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& v1, const type& v2) noexcept
    {
        return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
    }

    // greater than (>)

    friend VX_FORCE_INLINE constexpr bool operator>(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        if (v1.y != v2.y) return v1.y > v2.y;
        if (v1.z != v2.z) return v1.z > v2.z;
        return v1.w > v2.w;
    }

    // less than (<)

    friend VX_FORCE_INLINE constexpr bool operator<(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        if (v1.y != v2.y) return v1.y < v2.y;
        if (v1.z != v2.z) return v1.z < v2.z;
        return v1.w < v2.w;
    }

    // greater than or equal to (>=)

    friend VX_FORCE_INLINE constexpr bool operator>=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x > v2.x;
        if (v1.y != v2.y) return v1.y > v2.y;
        if (v1.z != v2.z) return v1.z > v2.z;
        return v1.w >= v2.w;
    }

    // less than or equal to (<=)

    friend VX_FORCE_INLINE constexpr bool operator<=(const type& v1, const type& v2) noexcept
    {
        if (v1.x != v2.x) return v1.x < v2.x;
        if (v1.y != v2.y) return v1.y < v2.y;
        if (v1.z != v2.z) return v1.z < v2.z;
        return v1.w <= v2.w;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const noexcept
    {
        return type(+x, +y, +z, +w);
    }

    VX_FORCE_INLINE constexpr type operator-() const noexcept
    {
        return type(-x, -y, -z, -w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // increment and decrement operators
    ///////////////////////////////////////////////////////////////////////////////

    // incrememnt (++)

    VX_FORCE_INLINE constexpr type& operator++() noexcept
    {
        x++;
        y++;
        z++;
        w++;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator++(int) noexcept
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    VX_FORCE_INLINE constexpr type& operator--() noexcept
    {
        x--;
        y--;
        z--;
        w--;
        return *this;
    }

    VX_FORCE_INLINE constexpr type operator--(int) noexcept
    {
        type result(*this);
        --(*this);
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    __SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar);
    }

    __SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w);
    }

    __SIMD_FALLBACK(HAVE_ADD)
    friend VX_FORCE_INLINE constexpr type operator+(const type& v1, const type& v2) noexcept
    {
        return type(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
    }

    // subtraction (-)

    __SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar);
    }

    __SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w);
    }

    __SIMD_FALLBACK(HAVE_SUB)
    friend VX_FORCE_INLINE constexpr type operator-(const type& v1, const type& v2) noexcept
    {
        return type(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
    }

    // multiplication (*)

    __SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
    }

    __SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE constexpr type operator*(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
    }

    __SIMD_FALLBACK(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(const type& v1, const type& v2) noexcept
    {
        return type(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
    }

    // division (/)

    __SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
    }

    __SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
    }

    __SIMD_FALLBACK(HAVE_DIV)
    friend VX_FORCE_INLINE constexpr type operator/(const type& v1, const type& v2) noexcept
    {
        return type(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
    }

    // modulo (%)

    friend VX_FORCE_INLINE constexpr type operator%(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x % scalar, v.y % scalar, v.z % scalar, v.w % scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator%(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar % v.x, scalar % v.y, scalar % v.z, scalar % v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator%(const type& v1, const type& v2) noexcept
    {
        return type(v1.x % v2.x, v1.y % v2.y, v1.z % v2.z, v1.w % v2.w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&)

    friend VX_FORCE_INLINE constexpr type operator&(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x & scalar, v.y & scalar, v.z & scalar, v.w & scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator&(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar & v.x, scalar & v.y, scalar & v.z, scalar & v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator&(const type& v1, const type& v2) noexcept
    {
        return type(v1.x & v2.x, v1.y & v2.y, v1.z & v2.z, v1.w & v2.w);
    }

    // or (|)

    friend VX_FORCE_INLINE constexpr type operator|(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x | scalar, v.y | scalar, v.z | scalar, v.w | scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator|(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar | v.x, scalar | v.y, scalar | v.z, scalar | v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator|(const type& v1, const type& v2) noexcept
    {
        return type(v1.x | v2.x, v1.y | v2.y, v1.z | v2.z, v1.w | v2.w);
    }

    // xor (^)

    friend VX_FORCE_INLINE constexpr type operator^(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x ^ scalar, v.y ^ scalar, v.z ^ scalar, v.w ^ scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator^(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar ^ v.x, scalar ^ v.y, scalar ^ v.z, scalar ^ v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator^(const type& v1, const type& v2) noexcept
    {
        return type(v1.x ^ v2.x, v1.y ^ v2.y, v1.z ^ v2.z, v1.w ^ v2.w);
    }

    // left shift (<<)

    friend VX_FORCE_INLINE constexpr type operator<<(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x << scalar, v.y << scalar, v.z << scalar, v.w << scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator<<(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar << v.x, scalar << v.y, scalar << v.z, scalar << v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator<<(const type& v1, const type& v2) noexcept
    {
        return type(v1.x << v2.x, v1.y << v2.y, v1.z << v2.z, v1.w << v2.w);
    }

    // right shift (>>)

    friend VX_FORCE_INLINE constexpr type operator>>(const type& v, scalar_type scalar) noexcept
    {
        return type(v.x >> scalar, v.y >> scalar, v.z >> scalar, v.w >> scalar);
    }

    friend VX_FORCE_INLINE constexpr type operator>>(scalar_type scalar, const type& v) noexcept
    {
        return type(scalar >> v.x, scalar >> v.y, scalar >> v.z, scalar >> v.w);
    }

    friend VX_FORCE_INLINE constexpr type operator>>(const type& v1, const type& v2) noexcept
    {
        return type(v1.x >> v2.x, v1.y >> v2.y, v1.z >> v2.z, v1.w >> v2.w);
    }

    // not (~)

    VX_FORCE_INLINE constexpr type operator~() const noexcept
    {
        return type(~x, ~y, ~z, ~w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    __SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator+=(scalar_type scalar) noexcept
    {
        x += scalar;
        y += scalar;
        z += scalar;
        w += scalar;
        return *this;
    }

    __SIMD_FALLBACK(HAVE_ADD)
    VX_FORCE_INLINE constexpr type& operator+=(const type& v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
        return *this;
    }

    // subtraction (-=)

    __SIMD_FALLBACK(HAVE_SUB)
    VX_FORCE_INLINE constexpr type& operator-=(scalar_type scalar) noexcept
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        w -= scalar;
        return *this;
    }

    __SIMD_FALLBACK(HAVE_SUB)
    VX_FORCE_INLINE constexpr type& operator-=(const type& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
        return *this;
    }

    // multiplication (*=)

    __SIMD_FALLBACK(HAVE_MUL)
    VX_FORCE_INLINE constexpr type& operator*=(scalar_type scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    __SIMD_FALLBACK(HAVE_MUL)
    VX_FORCE_INLINE constexpr type& operator*=(const type& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
        return *this;
    }

    // division (/=)

    __SIMD_FALLBACK(HAVE_DIV)
    VX_FORCE_INLINE constexpr type& operator/=(scalar_type scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    __SIMD_FALLBACK(HAVE_DIV)
    VX_FORCE_INLINE constexpr type& operator/=(const type& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
        return *this;
    }

    // modulo (%=)

    VX_FORCE_INLINE constexpr type& operator%=(scalar_type scalar) noexcept
    {
        x %= scalar;
        y %= scalar;
        z %= scalar;
        w %= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator%=(const type& v) noexcept
    {
        x %= v.x;
        y %= v.y;
        z %= v.z;
        w %= v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary bit operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&=)

    VX_FORCE_INLINE constexpr type& operator&=(scalar_type scalar) noexcept
    {
        x &= scalar;
        y &= scalar;
        z &= scalar;
        w &= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator&=(const type& v) noexcept
    {
        x &= v.x;
        y &= v.y;
        z &= v.z;
        w &= v.w;
        return *this;
    }

    // or (|=)

    VX_FORCE_INLINE constexpr type& operator|=(scalar_type scalar) noexcept
    {
        x |= scalar;
        y |= scalar;
        z |= scalar;
        w |= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator|=(const type& v) noexcept
    {
        x |= v.x;
        y |= v.y;
        z |= v.z;
        w |= v.w;
        return *this;
    }

    // xor (^=)

    VX_FORCE_INLINE constexpr type& operator^=(scalar_type scalar) noexcept
    {
        x ^= scalar;
        y ^= scalar;
        z ^= scalar;
        w ^= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator^=(const type& v) noexcept
    {
        x ^= v.x;
        y ^= v.y;
        z ^= v.z;
        w ^= v.w;
        return *this;
    }

    // left shift (<<=)

    VX_FORCE_INLINE constexpr type& operator<<=(scalar_type scalar) noexcept
    {
        x <<= scalar;
        y <<= scalar;
        z <<= scalar;
        w <<= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator<<=(const type& v) noexcept
    {
        x <<= v.x;
        y <<= v.y;
        z <<= v.z;
        w <<= v.w;
        return *this;
    }

    // right shift (>>=)

    VX_FORCE_INLINE constexpr type& operator>>=(scalar_type scalar) noexcept
    {
        x >>= scalar;
        y >>= scalar;
        z >>= scalar;
        w >>= scalar;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator>>=(const type& v) noexcept
    {
        x >>= v.x;
        y >>= v.y;
        z >>= v.z;
        w >>= v.w;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // boolean operators
    ///////////////////////////////////////////////////////////////////////////////

    // and (&&)

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const type& v, scalar_type scalar) noexcept
    {
        return vec<size, bool>(v.x && scalar, v.y && scalar, v.z && scalar, v.w && scalar);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(scalar_type scalar, const type& v) noexcept
    {
        return vec<size, bool>(scalar && v.x, scalar && v.y, scalar && v.z, scalar && v.w);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator&&(const type& v1, const type& v2) noexcept
    {
        return vec<size, bool>(v1.x && v2.x, v1.y && v2.y, v1.z && v2.z, v1.w && v2.w);
    }

    // or (||)

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const type& v, scalar_type scalar) noexcept
    {
        return vec<size, bool>(v.x || scalar, v.y || scalar, v.z || scalar, v.w || scalar);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(scalar_type scalar, const type& v) noexcept
    {
        return vec<size, bool>(scalar || v.x, scalar || v.y, scalar || v.z, scalar || v.w);
    }

    friend VX_FORCE_INLINE constexpr vec<size, bool> operator||(const type& v1, const type& v2) noexcept
    {
        return vec<size, bool>(v1.x || v2.x, v1.y || v2.y, v1.z || v2.z, v1.w || v2.w);
    }

    // not (!)

    VX_FORCE_INLINE constexpr vec<size, bool> operator!() const noexcept
    {
        return vec<size, bool>(!x, !y, !z, !w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type zero() noexcept { return type(static_cast<scalar_type>(0)); }
    static VX_FORCE_INLINE constexpr type one() noexcept { return type(static_cast<scalar_type>(1)); }

    ///////////////////////////////////////////////////////////////////////////////
    // simd conversion
    ///////////////////////////////////////////////////////////////////////////////

    using simd_type = typename simd::vec<4, T>;
    using simd_data_type = typename simd_type::data_type;

    VX_FORCE_INLINE vec(const simd_data_type& d) noexcept
        : vec(*(const vec*)(&d)) {}

    VX_FORCE_INLINE operator simd_data_type& () noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(simd_data_type*)(this);
    }

    VX_FORCE_INLINE operator const simd_data_type& () const noexcept
    {
        VX_STATIC_ASSERT(sizeof(type) == sizeof(simd_data_type), "invalid conversion");
        VX_STATIC_ASSERT(alignof(type) >= alignof(simd_data_type), "invalid conversion");
        return *(const simd_data_type*)(this);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    __SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE type operator+(const type& v, scalar_type scalar) noexcept
    {
        return simd_type::add(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE type operator+(scalar_type scalar, const type& v) noexcept
    {
        return simd_type::add(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_ADD)
    friend VX_FORCE_INLINE type operator+(const type& v1, const type& v2) noexcept
    {
        return simd_type::add(v1, v2);
    }

    // subtraction (-)

    __SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE type operator-(const type& v, scalar_type scalar) noexcept
    {
        return simd_type::sub(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE type operator-(scalar_type scalar, const type& v) noexcept
    {
        return simd_type::sub(scalar, v);
    }

    __SIMD_OVERLOAD(HAVE_SUB)
    friend VX_FORCE_INLINE type operator-(const type& v1, const type& v2) noexcept
    {
        return simd_type::sub(v1, v2);
    }

    // multiplication (*)

    __SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(const type& v, scalar_type scalar) noexcept
    {
        return simd_type::mul(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(scalar_type scalar, const type& v) noexcept
    {
        return simd_type::mul(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_MUL)
    friend VX_FORCE_INLINE type operator*(const type& v1, const type& v2) noexcept
    {
        return simd_type::mul(v1, v2);
    }

    // division (/)

    __SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE type operator/(const type& v, scalar_type scalar) noexcept
    {
        return simd_type::div(v, scalar);
    }

    __SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE type operator/(scalar_type scalar, const type& v) noexcept
    {
        return simd_type::div(scalar, v);
    }

    __SIMD_OVERLOAD(HAVE_DIV)
    friend VX_FORCE_INLINE type operator/(const type& v1, const type& v2) noexcept
    {
        return simd_type::div(v1, v2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // simd unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    __SIMD_OVERLOAD(HAVE_ADD)
    VX_FORCE_INLINE type& operator+=(scalar_type scalar) noexcept
    {
        (*this) = (*this) + scalar;
        return *this;
    }

    __SIMD_OVERLOAD(HAVE_ADD)
    VX_FORCE_INLINE type& operator+=(const type& v) noexcept
    {
        (*this) = (*this) + v;
        return *this;
    }

    // subtraction (-=)

    __SIMD_OVERLOAD(HAVE_SUB)
    VX_FORCE_INLINE type& operator-=(scalar_type scalar) noexcept
    {
        (*this) = (*this) - scalar;
        return *this;
    }

    __SIMD_OVERLOAD(HAVE_SUB)
    VX_FORCE_INLINE type& operator-=(const type& v) noexcept
    {
        (*this) = (*this) - v;
        return *this;
    }

    // multiplication (*=)

    __SIMD_OVERLOAD(HAVE_MUL)
    VX_FORCE_INLINE type& operator*=(scalar_type scalar) noexcept
    {
        (*this) = (*this) * scalar;
        return *this;
    }

    __SIMD_OVERLOAD(HAVE_MUL)
    VX_FORCE_INLINE type& operator*=(const type& v) noexcept
    {
        (*this) = (*this) * v;
        return *this;
    }

    // division (/=)

    __SIMD_OVERLOAD(HAVE_DIV)
    VX_FORCE_INLINE type& operator/=(scalar_type scalar) noexcept
    {
        (*this) = (*this) / scalar;
        return *this;
    }

    __SIMD_OVERLOAD(HAVE_DIV)
    VX_FORCE_INLINE type& operator/=(const type& v) noexcept
    {
        (*this) = (*this) / v;
        return *this;
    }

#   undef __SIMD_OVERLOAD
#   undef __SIMD_FALLBACK
};

} // namespace math
} // namespace vx