#pragma once

#include <cassert>

#include "../../../detail/setup.h"
#include "../../detail/type_traits.h"
#include "../../fn/mat_fn_operations.h"

namespace vx {
namespace math {

VX_PACK_PUSH()

template <typename T>
struct mat<4, 4, T>
{
    // =============== meta ===============

    using scaler_type = T;
    using type = mat<4, 4, T>;

    using row_type = vec<4, T>;
    using col_type = vec<4, T>;
    using transpose_type = mat<4, 4, T>;

    static inline constexpr size_t size() noexcept { return static_cast<size_t>(16); }
    static inline constexpr size_t width() noexcept { return static_cast<size_t>(4); }
    static inline constexpr size_t height() noexcept { return static_cast<size_t>(4); }

    // =============== data ===============

    col_type columns[4];

    // =============== implicit constructors ===============

    inline constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)),
                   col_type(static_cast<T>(0)) } {}

    inline constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2], m.columns[3] } {}

    inline constexpr mat(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit mat(T scaler) noexcept
        : columns{ col_type(scaler, static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), scaler, static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), scaler, static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), scaler) } {}

    inline constexpr mat(
        T x1, T y1, T z1, T w1,
        T x2, T y2, T z2, T w2,
        T x3, T y3, T z3, T w3,
        T x4, T y4, T z4, T w4
    ) noexcept
        : columns{ col_type(x1, y1, z1, w1),
                   col_type(x2, y2, z2, w2),
                   col_type(x3, y3, z3, w3),
                   col_type(x4, y4, z4, w4) } {}

    inline constexpr mat(const col_type& c1, const col_type& c2, const col_type& c3, const col_type& c4) noexcept
        : columns{ c1, c2, c3, c4 } {}

    // =============== conversion matrix constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scaler), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(scaler), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(scaler)) } {}

    template <
        typename X1, typename Y1, typename Z1, typename W1,
        typename X2, typename Y2, typename Z2, typename W2,
        typename X3, typename Y3, typename Z3, typename W3,
        typename X4, typename Y4, typename Z4, typename W4,
        typename std::enable_if<
        std::is_arithmetic<X1>::value &&
        std::is_arithmetic<Y1>::value &&
        std::is_arithmetic<Z1>::value &&
        std::is_arithmetic<W1>::value &&
        std::is_arithmetic<X2>::value &&
        std::is_arithmetic<Y2>::value &&
        std::is_arithmetic<Z2>::value &&
        std::is_arithmetic<W2>::value &&
        std::is_arithmetic<X3>::value &&
        std::is_arithmetic<Y3>::value &&
        std::is_arithmetic<Z3>::value &&
        std::is_arithmetic<W3>::value &&
        std::is_arithmetic<X4>::value &&
        std::is_arithmetic<Y4>::value &&
        std::is_arithmetic<Z4>::value &&
        std::is_arithmetic<W4>::value, bool>::type = true>
    inline constexpr mat(
        X1 x1, Y1 y1, Z1 z1, W1 w1,
        X2 x2, Y2 y2, Z2 z2, W2 w2,
        X3 x3, Y3 y3, Z3 z3, W3 w3,
        X4 x4, Y4 y4, Z4 z4, W4 w4
    )
        : columns{ col_type(static_cast<T>(x1), static_cast<T>(y1), static_cast<T>(z1), static_cast<T>(w1)),
                   col_type(static_cast<T>(x2), static_cast<T>(y2), static_cast<T>(z2), static_cast<T>(w2)),
                   col_type(static_cast<T>(x3), static_cast<T>(y3), static_cast<T>(z3), static_cast<T>(w3)),
                   col_type(static_cast<T>(x4), static_cast<T>(y4), static_cast<T>(z4), static_cast<T>(w4)) } {}

    template <typename C1, typename C2, typename C3, typename C4>
    inline constexpr mat(const vec<2, C1>& c1, const vec<2, C2>& c2, const vec<2, C3>& c3, const vec<2, C4>& c4)
        : columns{ static_cast<col_type>(c1),
                   static_cast<col_type>(c2),
                   static_cast<col_type>(c3),
                   static_cast<col_type>(c4) } {}

    template <size_t M, size_t N, typename U, typename std::enable_if<(M >= 4 && N >= 4), bool>::type = true>
    inline constexpr explicit mat(const mat<M, N, U>& m)
        : columns{ static_cast<col_type>(m.columns[0]),
                   static_cast<col_type>(m.columns[1]),
                   static_cast<col_type>(m.columns[2]),
                   static_cast<col_type>(m.columns[3]) } {}

    // =============== destructor ===============

    ~mat() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        columns[3] = m.columns[3];
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    inline constexpr type& operator=(const mat<4, 4, U>& m) noexcept
    {
        columns[0] = static_cast<col_type>(m.columns[0]);
        columns[1] = static_cast<col_type>(m.columns[1]);
        columns[2] = static_cast<col_type>(m.columns[2]);
        columns[3] = static_cast<col_type>(m.columns[3]);
        return *this;
    }

    // =============== accessors ===============

    inline constexpr col_type& operator[](size_t i)
    {
        assert(i < 4);
        return columns[i];
    }

    inline constexpr const col_type& operator[](size_t i) const
    {
        assert(i < 4);
        return columns[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& m1, const type& m2)
    {
        return (m1.columns[0] == m2.columns[0])
            && (m1.columns[1] == m2.columns[1])
            && (m1.columns[2] == m2.columns[2])
            && (m1.columns[3] == m2.columns[3]);
    }

    friend inline constexpr bool operator!=(const type& m1, const type& m2)
    {
        return !(m1 == m2);
    }

    // =============== unary constant operators ===============

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator+() const
    {
        return type(+columns[0], +columns[1], +columns[2], +columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator-() const
    {
        return type(-columns[0], -columns[1], -columns[2], -columns[3]);
    }

    // =============== incrememnt and decrement operators ===============

    // increment (++)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator++()
    {
        columns[0]++;
        columns[1]++;
        columns[2]++;
        columns[3]++;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator--()
    {
        columns[0]--;
        columns[1]--;
        columns[2]--;
        columns[3]--;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    // =============== binary arithmetic operators ===============

    // addition (+)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(const type& m, T scaler)
    {
        return type(m.columns[0] + scaler, m.columns[1] + scaler, m.columns[2] + scaler, m.columns[3] + scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(T scaler, const type& m)
    {
        return type(scaler + m.columns[0], scaler + m.columns[1], scaler + m.columns[2], scaler + m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator+(const type& m1, const type& m2)
    {
        return type(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1], m1.columns[2] + m2.columns[2], m1.columns[3] + m2.columns[3]);
    }

    // subtraction (-)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(const type& m, T scaler)
    {
        return type(m.columns[0] - scaler, m.columns[1] - scaler, m.columns[2] - scaler, m.columns[3] - scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(T scaler, const type& m)
    {
        return type(scaler - m.columns[0], scaler - m.columns[1], scaler - m.columns[2], scaler - m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator-(const type& m1, const type& m2)
    {
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1], m1.columns[2] - m2.columns[2], m1.columns[3] - m2.columns[3]);
    }

    // multiplication (*)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(const type& m, T scaler)
    {
        return type(m.columns[0] * scaler, m.columns[1] * scaler, m.columns[2] * scaler, m.columns[3] * scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(T scaler, const type& m)
    {
        return type(scaler * m.columns[0], scaler * m.columns[1], scaler * m.columns[2], scaler * m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr vec<4, T> operator*(const type& m, const vec<4, T>& v)
    {
        return vec<4, T>(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + (m.columns[3].x * v.w),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + (m.columns[3].y * v.w),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + (m.columns[3].z * v.w),
            (m.columns[0].w * v.x) + (m.columns[1].w * v.y) + (m.columns[2].w * v.z) + (m.columns[3].w * v.w)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr vec<4, T> operator*(const vec<4, T>& v, const type& m)
    {
        return vec<4, T>(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x) + (v.w * m.columns[3].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y) + (v.w * m.columns[3].y),
            (v.x * m.columns[0].z) + (v.y * m.columns[1].z) + (v.z * m.columns[2].z) + (v.w * m.columns[3].z),
            (v.x * m.columns[0].w) + (v.y * m.columns[1].w) + (v.z * m.columns[2].w) + (v.w * m.columns[3].w)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr mat<2, 4, T> operator*(const type& m1, const mat<2, 4, T>& m2)
    {
        return mat<2, 4, T>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr mat<3, 4, T> operator*(const type& m1, const mat<3, 4, T>& m2)
    {
        return mat<3, 4, T>(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z) + (m1.columns[3].x * m2.columns[2].w),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z) + (m1.columns[3].y * m2.columns[2].w),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z) + (m1.columns[3].z * m2.columns[2].w),
            (m1.columns[0].w * m2.columns[2].x) + (m1.columns[1].w * m2.columns[2].y) + (m1.columns[2].w * m2.columns[2].z) + (m1.columns[3].w * m2.columns[2].w)
        );
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator*(const type& m1, const type& m2)
    {
        return type(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z) + (m1.columns[3].x * m2.columns[0].w),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z) + (m1.columns[3].y * m2.columns[0].w),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z) + (m1.columns[3].z * m2.columns[0].w),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z) + (m1.columns[3].w * m2.columns[0].w),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z) + (m1.columns[3].x * m2.columns[1].w),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z) + (m1.columns[3].y * m2.columns[1].w),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z) + (m1.columns[3].z * m2.columns[1].w),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z) + (m1.columns[3].w * m2.columns[1].w),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z) + (m1.columns[3].x * m2.columns[2].w),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z) + (m1.columns[3].y * m2.columns[2].w),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z) + (m1.columns[3].z * m2.columns[2].w),
            (m1.columns[0].w * m2.columns[2].x) + (m1.columns[1].w * m2.columns[2].y) + (m1.columns[2].w * m2.columns[2].z) + (m1.columns[3].w * m2.columns[2].w),

            (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y) + (m1.columns[2].x * m2.columns[3].z) + (m1.columns[3].x * m2.columns[3].w),
            (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y) + (m1.columns[2].y * m2.columns[3].z) + (m1.columns[3].y * m2.columns[3].w),
            (m1.columns[0].z * m2.columns[3].x) + (m1.columns[1].z * m2.columns[3].y) + (m1.columns[2].z * m2.columns[3].z) + (m1.columns[3].z * m2.columns[3].w),
            (m1.columns[0].w * m2.columns[3].x) + (m1.columns[1].w * m2.columns[3].y) + (m1.columns[2].w * m2.columns[3].z) + (m1.columns[3].w * m2.columns[3].w)
        );
    }

    // division (/)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator/(const type& m, T scaler)
    {
        return type(m.columns[0] / scaler, m.columns[1] / scaler, m.columns[2] / scaler, m.columns[3] / scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    friend inline constexpr type operator/(T scaler, const type& m)
    {
        return type(scaler / m.columns[0], scaler / m.columns[1], scaler / m.columns[2], scaler / m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    friend inline constexpr vec<2, T> operator/(const type& m, const vec<2, T>& v)
    {
        return invert(m) * v;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<U>::value, bool>::type = true>
    friend inline constexpr vec<2, T> operator/(const vec<2, T>& v, const type& m)
    {
        return v * invert(m);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_floating_point<T>::value, bool>::type = true>
    friend inline constexpr type operator/(const type& m1, const type& m2)
    {
        return invert(m1) * m2;
    }

    // modulo (%)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& m, T scaler)
    {
        return type(m.columns[0] % scaler, m.columns[1] % scaler, m.columns[2] % scaler, m.columns[3] % scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(T scaler, const type& m)
    {
        return type(scaler % m.columns[0], scaler % m.columns[1], scaler % m.columns[2], scaler % m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator%(const type& m1, const type& m2)
    {
        return type(m1.columns[0] % m2.columns[0], m1.columns[1] % m2.columns[1], m1.columns[2] % m1.columns[2], m1.columns[3] % m2.columns[3]);
    }

    // =============== binary bit operators ===============

    // and (&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& m, T scaler)
    {
        return type(m.columns[0] & scaler, m.columns[1] & scaler, m.columns[2] & scaler, m.columns[3] & scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(T scaler, const type& m)
    {
        return type(scaler & m.columns[0], scaler & m.columns[1], scaler & m.columns[2], scaler & m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator&(const type& m1, const type& m2)
    {
        return type(m1.columns[0] & m2.columns[0], m1.columns[1] & m2.columns[1], m1.columns[2] & m1.columns[2], m1.columns[3] & m2.columns[3]);
    }

    // or (|)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& m, T scaler)
    {
        return type(m.columns[0] | scaler, m.columns[1] | scaler, m.columns[2] | scaler, m.columns[3] | scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(T scaler, const type& m)
    {
        return type(scaler | m.columns[0], scaler | m.columns[1], scaler | m.columns[2], scaler | m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator|(const type& m1, const type& m2)
    {
        return type(m1.columns[0] | m2.columns[0], m1.columns[1] | m2.columns[1], m1.columns[2] | m1.columns[2], m1.columns[3] | m2.columns[3]);
    }

    // xor (^)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& m, T scaler)
    {
        return type(m.columns[0] ^ scaler, m.columns[1] ^ scaler, m.columns[2] ^ scaler, m.columns[3] ^ scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(T scaler, const type& m)
    {
        return type(scaler ^ m.columns[0], scaler ^ m.columns[1], scaler ^ m.columns[2], scaler ^ m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator^(const type& m1, const type& m2)
    {
        return type(m1.columns[0] ^ m2.columns[0], m1.columns[1] ^ m2.columns[1], m1.columns[2] ^ m1.columns[2], m1.columns[3] ^ m2.columns[3]);
    }

    // left shift (<<)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& m, T scaler)
    {
        return type(m.columns[0] << scaler, m.columns[1] << scaler, m.columns[2] << scaler, m.columns[3] << scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(T scaler, const type& m)
    {
        return type(scaler << m.columns[0], scaler << m.columns[1], scaler << m.columns[2], scaler << m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator<<(const type& m1, const type& m2)
    {
        return type(m1.columns[0] << m2.columns[0], m1.columns[1] << m2.columns[1], m1.columns[2] << m1.columns[2], m1.columns[3] << m2.columns[3]);
    }

    // right shift (>>)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& m, T scaler)
    {
        return type(m.columns[0] >> scaler, m.columns[1] >> scaler, m.columns[2] >> scaler, m.columns[3] >> scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(T scaler, const type& m)
    {
        return type(scaler >> m.columns[0], scaler >> m.columns[1], scaler >> m.columns[2], scaler >> m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    friend inline constexpr type operator>>(const type& m1, const type& m2)
    {
        return type(m1.columns[0] >> m2.columns[0], m1.columns[1] >> m2.columns[1], m1.columns[2] >> m1.columns[2], m1.columns[3] >> m2.columns[3]);
    }

    // not (~)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type operator~() const
    {
        return type(~columns[0], ~columns[1], ~columns[2], ~columns[3]);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator+=(T scaler)
    {
        columns[0] += scaler;
        columns[1] += scaler;
        columns[2] += scaler;
        columns[3] += scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator+=(const type& m)
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        columns[3] += m.columns[3];
        return *this;
    }

    // subtraction (-=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator-=(T scaler)
    {
        columns[0] -= scaler;
        columns[1] -= scaler;
        columns[2] -= scaler;
        columns[3] -= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator-=(const type& m)
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        columns[3] -= m.columns[3];
        return *this;
    }

    // multiplication (*=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator*=(T scaler)
    {
        columns[0] *= scaler;
        columns[1] *= scaler;
        columns[2] *= scaler;
        columns[3] *= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator*=(const mat<4, 4, T>& m)
    {
        (*this) = (*this) * m;
        return *this;
    }

    // division (/=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator/=(T scaler)
    {
        columns[0] /= scaler;
        columns[1] /= scaler;
        columns[2] /= scaler;
        columns[3] /= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_numeric<U>::value, bool>::type = true>
    inline constexpr type& operator/=(const type& m)
    {
        (*this) = (*this) / m;
        return *this;
    }

    // modulo (%=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(T scaler)
    {
        columns[0] %= scaler;
        columns[1] %= scaler;
        columns[2] %= scaler;
        columns[3] %= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator%=(const type& m)
    {
        columns[0] %= m.columns[0];
        columns[1] %= m.columns[1];
        columns[2] %= m.columns[2];
        columns[3] %= m.columns[3];
        return *this;
    }

    // =============== unary bit operators ===============

    // and (&=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(T scaler)
    {
        columns[0] &= scaler;
        columns[1] &= scaler;
        columns[2] &= scaler;
        columns[3] &= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator&=(const type& m)
    {
        columns[0] &= m.columns[0];
        columns[1] &= m.columns[1];
        columns[2] &= m.columns[2];
        columns[3] &= m.columns[3];
        return *this;
    }

    // or (|=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(T scaler)
    {
        columns[0] |= scaler;
        columns[1] |= scaler;
        columns[2] |= scaler;
        columns[3] |= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator|=(const type& m)
    {
        columns[0] |= m.columns[0];
        columns[1] |= m.columns[1];
        columns[2] |= m.columns[2];
        columns[3] |= m.columns[3];
        return *this;
    }

    // xor (^=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(T scaler)
    {
        columns[0] ^= scaler;
        columns[1] ^= scaler;
        columns[2] ^= scaler;
        columns[3] ^= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator^=(const type& m)
    {
        columns[0] ^= m.columns[0];
        columns[1] ^= m.columns[1];
        columns[2] ^= m.columns[2];
        columns[3] ^= m.columns[3];
        return *this;
    }

    // left shift (<<=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(T scaler)
    {
        columns[0] <<= scaler;
        columns[1] <<= scaler;
        columns[2] <<= scaler;
        columns[3] <<= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator<<=(const type& m)
    {
        columns[0] <<= m.columns[0];
        columns[1] <<= m.columns[1];
        columns[2] <<= m.columns[2];
        columns[3] <<= m.columns[3];
        return *this;
    }

    // right shift (>>=)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(T scaler)
    {
        columns[0] >>= scaler;
        columns[1] >>= scaler;
        columns[2] >>= scaler;
        columns[3] >>= scaler;
        return *this;
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && type_traits::is_integral<U>::value, bool>::type = true>
    inline constexpr type& operator>>=(const type& m)
    {
        columns[0] >>= m.columns[0];
        columns[1] >>= m.columns[1];
        columns[2] >>= m.columns[2];
        columns[3] >>= m.columns[3];
        return *this;
    }

    // =============== boolean operators ===============

    // and (&&)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(const type& m, T scaler)
    {
        return type(m.columns[0] && scaler, m.columns[1] && scaler, m.columns[2] && scaler, m.columns[3] && scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(T scaler, const type& m)
    {
        return type(scaler && m.columns[0], scaler && m.columns[1], scaler && m.columns[2], scaler && m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator&&(const type& m1, const type& m2)
    {
        return type(m1.columns[0] && m2.columns[0], m1.columns[1] && m2.columns[1], m1.columns[2] && m2.columns[2], m1.columns[3] && m2.columns[3]);
    }

    // or (||)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(const type& m, T scaler)
    {
        return type(m.columns[0] || scaler, m.columns[1] || scaler, m.columns[2] || scaler, m.columns[3] || scaler);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(T scaler, const type& m)
    {
        return type(scaler || m.columns[0], scaler || m.columns[1], scaler || m.columns[2], scaler || m.columns[3]);
    }

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value && std::is_same<U, bool>::value, bool>::type = true>
    friend inline constexpr type operator||(const type& m1, const type& m2)
    {
        return type(m1.columns[0] || m2.columns[0], m1.columns[1] || m2.columns[1], m1.columns[2] || m2.columns[2], m1.columns[3] || m2.columns[3]);
    }

    // not (!)

    template <typename U = T, typename std::enable_if<std::is_same<T, U>::value&& std::is_same<U, bool>::value, bool>::type = true>
    inline constexpr type operator!() const
    {
        return type(!columns[0], !columns[1], !columns[2], !columns[3]);
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(static_cast<T>(1)); }
    static inline constexpr type ZERO() { return type(static_cast<T>(0)); }

};

VX_PACK_POP()

}
}