#pragma once

#include <iomanip>

#include "../math.h"

namespace vx {
namespace math {
namespace detail {

// https://www.andre-gaschler.com/rotationconverter/

template <typename T>
struct mat<4, 4, T>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;

    using type = mat<4, 4, T>;
    using col_type = vec<4, T, vec_t::vec>;
    using row_type = vec<4, T, vec_t::vec>;
    using transpose_type = mat<4, 4, T>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(16); }
    static inline constexpr size_type width() noexcept { return static_cast<size_type>(4); }
    static inline constexpr size_type height() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::detail::iterator<col_type>;
    using const_iterator = ::vx::detail::iterator<const col_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec>;
    using vec3_type = vec<3, T, vec_t::vec>;
    using vec4_type = vec<4, T, vec_t::vec>;

    using mat2_type = mat<2, 2, T>;
    using mat3_type = mat<3, 3, T>;

    using quat_type = quat<T>;

    // =============== data ===============

    col_type columns[4];

    // =============== implicit constructors ===============

    inline constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

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

    inline constexpr mat(
        const col_type& v1,
        const col_type& v2,
        const col_type& v3,
        const col_type& v4
    ) noexcept
        : columns{ v1, v2, v3, v4 } {}

    // =============== conversion matrix constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scaler), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(scaler), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(scaler)) } {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr mat(
        U x1, U y1, U z1, U w1,
        U x2, U y2, U z2, U w2,
        U x3, U y3, U z3, U w3,
        U x4, U y4, U z4, U w4
    )
        : columns{ col_type(x1, y1, z1, w1),
                   col_type(x2, y2, z2, w2),
                   col_type(x3, y3, z3, w3),
                   col_type(x4, y4, z4, w4) } {}

    template <typename U>
    inline constexpr mat(
        const vecx<4, U>& v1,
        const vecx<4, U>& v2,
        const vecx<4, U>& v3,
        const vecx<4, U>& v4
    )
        : columns{ v1, v2, v3, v4 } {}

    template <typename U>
    inline constexpr explicit mat(const mat<2, 2, U>& m)
        : columns{ col_type(m.columns[0],						  static_cast<T>(0), static_cast<T>(0)),
                   col_type(m.columns[1],						  static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

    template <typename U>
    inline constexpr explicit mat(const mat<3, 3, U>& m)
        : columns{ col_type(m.columns[0],											 static_cast<T>(0)),
                   col_type(m.columns[1],											 static_cast<T>(0)),
                   col_type(m.columns[2],											 static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

    template <typename U>
    inline constexpr explicit mat(const mat<4, 4, U>& m)
        : columns{ m.columns[0], m.columns[1], m.columns[2], m.columns[4] } {}

    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm

    inline constexpr explicit mat(const quat_type& q)
    {
        const quat_type qn(q.normalize());

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qxz = qn.x * qn.z;
        const T qxy = qn.x * qn.y;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;
        const T qwz = qn.w * qn.z;

        columns[0].x = static_cast<T>(1) - (static_cast<T>(2) * (qyy + qzz));
        columns[0].y = static_cast<T>(2) * (qxy + qwz);
        columns[0].z = static_cast<T>(2) * (qxz - qwy);
        columns[0].w = static_cast<T>(0);

        columns[1].x = static_cast<T>(2) * (qxy - qwz);
        columns[1].y = static_cast<T>(1) - (static_cast<T>(2) * (qxx + qzz));
        columns[1].z = static_cast<T>(2) * (qyz + qwx);
        columns[1].w = static_cast<T>(0);

        columns[2].x = static_cast<T>(2) * (qxz + qwy);
        columns[2].y = static_cast<T>(2) * (qyz - qwx);
        columns[2].z = static_cast<T>(1) - (static_cast<T>(2) * (qxx + qyy));
        columns[2].w = static_cast<T>(0);

        columns[3].x = static_cast<T>(0);
        columns[3].y = static_cast<T>(0);
        columns[3].z = static_cast<T>(0);
        columns[3].w = static_cast<T>(1);
    }

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

    // =============== accessors ===============

    inline constexpr col_type& operator[](size_type i)
    {
        assert(i < 4);
        return columns[i];
    }

    inline constexpr const col_type& operator[](size_type i) const
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

    friend inline constexpr bool operator<(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] < m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] < m2.columns[1]);
        if (m1.columns[2] != m2.columns[2])	return (m1.columns[2] < m2.columns[2]);
        else                                return (m1.columns[3] < m2.columns[3]);
    }

    friend inline constexpr bool operator>(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] > m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] > m2.columns[1]);
        if (m1.columns[2] != m2.columns[2])	return (m1.columns[2] > m2.columns[2]);
        else                                return (m1.columns[3] > m2.columns[3]);
    }

    friend inline constexpr bool operator<=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] <  m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] <  m2.columns[1]);
        if (m1.columns[2] != m2.columns[2])	return (m1.columns[2] <  m2.columns[2]);
        else                                return (m1.columns[3] <= m2.columns[3]);
    }

    friend inline constexpr bool operator>=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] >  m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] >  m2.columns[1]);
        if (m1.columns[2] != m2.columns[2])	return (m1.columns[2] >  m2.columns[2]);
        else                                return (m1.columns[3] >= m2.columns[3]);
    }

    // =============== unary constant operators ===============

    friend inline constexpr type operator+(const type& m)
    {
        return m;
    }

    friend inline constexpr type operator-(const type& m)
    {
        return type(
            -m.columns[0],
            -m.columns[1],
            -m.columns[2],
            -m.columns[3]
        );
    }

    // =============== incrememnt and decrement operators ===============

    // incrememnt (++)

    inline constexpr type& operator++()
    {
        ++columns[0];
        ++columns[1];
        ++columns[2];
        ++columns[3];
        return *this;
    }

    inline constexpr type& operator++(int)
    {
        type result(*this);
        ++(*this);
        return result;
    }

    // decrement (--)

    inline constexpr type& operator--()
    {
        --columns[0];
        --columns[1];
        --columns[2];
        --columns[3];
        return *this;
    }

    inline constexpr type& operator--(int)
    {
        type result(*this);
        --(*this);
        return result;
    }

    // =============== binary arithmetic operators ===============

    // addition (+)

    friend inline constexpr type operator+(const type& m, T scaler)
    {
        return type(
            m.columns[0] + scaler,
            m.columns[1] + scaler,
            m.columns[2] + scaler,
            m.columns[3] + scaler
        );
    }

    friend inline constexpr type operator+(T scaler, const type& m)
    {
        return type(
            scaler + m.columns[0],
            scaler + m.columns[1],
            scaler + m.columns[2],
            scaler + m.columns[3]
        );
    }

    friend inline constexpr type operator+(const type& m1, const type& m2)
    {
        return type(
            m1.columns[0] + m2.columns[0],
            m1.columns[1] + m2.columns[1],
            m1.columns[2] + m2.columns[2],
            m1.columns[3] + m2.columns[3]
        );
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& m, T scaler)
    {
        return type(
            m.columns[0] - scaler,
            m.columns[1] - scaler,
            m.columns[2] - scaler,
            m.columns[3] - scaler
        );
    }

    friend inline constexpr type operator-(T scaler, const type& m)
    {
        return type(
            scaler - m.columns[0],
            scaler - m.columns[1],
            scaler - m.columns[2],
            scaler - m.columns[3]
        );
    }

    friend inline constexpr type operator-(const type& m1, const type& m2)
    {
        return type(
            m1.columns[0] - m2.columns[0],
            m1.columns[1] - m2.columns[1],
            m1.columns[2] - m2.columns[2],
            m1.columns[3] - m2.columns[3]
        );
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& m, T scaler)
    {
        return type(
            m.columns[0] * scaler,
            m.columns[1] * scaler,
            m.columns[2] * scaler,
            m.columns[3] * scaler
        );
    }

    friend inline constexpr type operator*(T scaler, const type& m)
    {
        return type(
            scaler * m.columns[0],
            scaler * m.columns[1],
            scaler * m.columns[2],
            scaler * m.columns[3]
        );
    }

    friend inline constexpr vec3_type operator*(const type& m, const vec3_type& v)
    {
        return vec3_type(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + m.columns[3].x,
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + m.columns[3].y,
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + m.columns[3].z
        );
    }

    friend inline constexpr col_type operator*(const type& m, const row_type& v)
    {
        return col_type(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z) + (m.columns[3].x * v.w),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z) + (m.columns[3].y * v.w),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z) + (m.columns[3].z * v.w),
            (m.columns[0].w * v.x) + (m.columns[1].w * v.y) + (m.columns[2].w * v.z) + (m.columns[3].w * v.w)
        );
    }

    friend inline constexpr row_type operator*(const col_type& v, const type& m)
    {
        return row_type(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x) + (v.w * m.columns[3].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y) + (v.w * m.columns[3].y),
            (v.x * m.columns[0].z) + (v.y * m.columns[1].z) + (v.z * m.columns[2].z) + (v.w * m.columns[3].z),
            (v.x * m.columns[0].w) + (v.y * m.columns[1].w) + (v.z * m.columns[2].w) + (v.w * m.columns[3].w)
        );
    }

    friend inline constexpr type operator*(const type& m1, const type& m2)
    {
        return type(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),
            (m1.columns[0].w * m2.columns[0].x) + (m1.columns[1].w * m2.columns[0].y) + (m1.columns[2].w * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z),
            (m1.columns[0].w * m2.columns[1].x) + (m1.columns[1].w * m2.columns[1].y) + (m1.columns[2].w * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z),
            (m1.columns[0].w * m2.columns[2].x) + (m1.columns[1].w * m2.columns[2].y) + (m1.columns[2].w * m2.columns[2].z),

            (m1.columns[0].x * m2.columns[3].x) + (m1.columns[1].x * m2.columns[3].y) + (m1.columns[2].x * m2.columns[3].z),
            (m1.columns[0].y * m2.columns[3].x) + (m1.columns[1].y * m2.columns[3].y) + (m1.columns[2].y * m2.columns[3].z),
            (m1.columns[0].z * m2.columns[3].x) + (m1.columns[1].z * m2.columns[3].y) + (m1.columns[2].z * m2.columns[3].z),
            (m1.columns[0].w * m2.columns[3].x) + (m1.columns[1].w * m2.columns[3].y) + (m1.columns[2].w * m2.columns[3].z)
        );
    }

    // division (/)

    friend inline constexpr type operator/(T scaler, const type& m)
    {
        return type(
            scaler / m.columns[0],
            scaler / m.columns[1],
            scaler / m.columns[2],
            scaler / m.columns[3]
        );
    }

    friend inline constexpr type operator/(const type& m, T scaler)
    {
        return type(
            m.columns[0] / scaler,
            m.columns[1] / scaler,
            m.columns[2] / scaler,
            m.columns[3] / scaler
        );
    }

    friend inline constexpr col_type operator/(const type& m, const row_type& v)
    {
        return m.invert() * v;
    }

    template <typename U>
    friend inline constexpr row_type operator/(const col_type& v, const type& m)
    {
        return v * m.invert();
    }

    friend inline constexpr type operator/(
        const type& m1,
        const type& m2
    )
    {
        return m1 * m2.invert();
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        columns[0] += scaler;
        columns[1] += scaler;
        columns[2] += scaler;
        columns[3] += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& m)
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        columns[3] += m.columns[3];
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        columns[0] -= scaler;
        columns[1] -= scaler;
        columns[2] -= scaler;
        columns[3] -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& m)
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        columns[3] -= m.columns[3];
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        columns[0] *= scaler;
        columns[1] *= scaler;
        columns[2] *= scaler;
        columns[3] *= scaler;
        return *this;
    }

    inline constexpr type& operator*=(const type& m)
    {
        return ((*this) = (*this) * m);
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        columns[0] /= scaler;
        columns[1] /= scaler;
        columns[2] /= scaler;
        columns[3] /= scaler;
        return *this;
    }

    inline constexpr type& operator/=(const type& m)
    {
        return ((*this) = (*this) / m);
    }

    // =============== iterator ===============

    inline constexpr iterator begin() noexcept
    {
        return iterator(&columns[0]);
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    inline constexpr iterator end() noexcept
    {
        return iterator(&columns[3] + 1);
    }

    inline constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    inline constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(&columns[0]);
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return const_iterator(&columns[3] + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&columns[3] + 1);
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    inline constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(&columns[0]);
    }

    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&columns[3] + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&columns[0]);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        if (pretty_print)
        {
            return type(
                math::make_pretty(columns[0].x),
                math::make_pretty(columns[0].y),
                math::make_pretty(columns[0].z),
                math::make_pretty(columns[0].w),

                math::make_pretty(columns[1].x),
                math::make_pretty(columns[1].y),
                math::make_pretty(columns[1].z),
                math::make_pretty(columns[1].w),

                math::make_pretty(columns[2].x),
                math::make_pretty(columns[2].y),
                math::make_pretty(columns[2].z),
                math::make_pretty(columns[2].w),

                math::make_pretty(columns[3].x),
                math::make_pretty(columns[3].y),
                math::make_pretty(columns[3].z),
                math::make_pretty(columns[3].w)
            ).to_string();
        }

        size_t wmax = 0;
        for (size_t i = 0; i < 4; i++)
        {
            for (size_t j = 0; j < 4; j++)
            {
                std::ostringstream oss;
                oss << columns[j][i];
                const size_t w = oss.str().length();
                if (w > wmax) wmax = w;
            }
        }

        std::ostringstream oss;
        for (size_t i = 0; i < 4; i++)
        {
            oss << '{';
            for (size_t j = 0; j < 4; j++)
            {
                oss << std::setw(wmax + 1) << columns[j][i];
            }
            oss << " }";
            if (i <= 2) oss << '\n';
        }

        return oss.str();
    }

    // =============== operations ===============

    /**
     * @brief Calculates the determinant of the 4x4 matrix.
     *
     * This function computes the determinant of the matrix using the expansion by minors method.
     *
     * @return The determinant of the matrix.
     */
    inline constexpr T determinant() const
    {
        const T subfac00 = (columns[2].z * columns[3].w) - (columns[2].w * columns[3].z);
        const T subfac01 = (columns[2].y * columns[3].w) - (columns[2].w * columns[3].y);
        const T subfac02 = (columns[2].y * columns[3].z) - (columns[2].z * columns[3].y);
        const T subfac03 = (columns[2].x * columns[3].w) - (columns[2].w * columns[3].x);
        const T subfac04 = (columns[2].x * columns[3].z) - (columns[2].z * columns[3].x);
        const T subfac05 = (columns[2].x * columns[3].y) - (columns[2].y * columns[3].x);

        return (
            +(columns[0].x * ((columns[1].y * subfac00) - (columns[1].z * subfac01) + (columns[1].w * subfac02)))
            -(columns[0].y * ((columns[1].x * subfac00) - (columns[1].z * subfac03) + (columns[1].w * subfac04)))
            +(columns[0].z * ((columns[1].x * subfac01) - (columns[1].y * subfac03) + (columns[1].w * subfac05)))
            -(columns[0].w * ((columns[1].x * subfac02) - (columns[1].y * subfac04) + (columns[1].z * subfac05)))
        );
    }

    /**
     * @brief Computes the transpose of the 4x4 matrix.
     *
     * This function returns a new matrix where the rows become columns and vice versa.
     *
     * @return The transposed matrix.
     */
    inline constexpr type transpose() const
    {
        return type(
            columns[0].x, columns[1].x, columns[2].x, columns[3].x,
            columns[0].y, columns[1].y, columns[2].y, columns[3].y,
            columns[0].z, columns[1].z, columns[2].z, columns[3].z,
            columns[0].w, columns[1].w, columns[2].w, columns[3].w
        );
    }

    /**
     * @brief Computes the inverse of the 4x4 matrix.
     *
     * This function calculates the inverse matrix for the 4x4 matrix if it exists.
     * If the matrix is not invertible (determinant is approximately zero), it returns a matrix with zeros.
     *
     * @return The inverted 4x4 matrix if invertible, otherwise a matrix with zeros.
     */
    inline constexpr type invert() const
    {
        const T coef00 = (columns[2].z * columns[3].w) - (columns[3].z * columns[2].w);
        const T coef01 = (columns[2].y * columns[3].w) - (columns[3].y * columns[2].w);
        const T coef02 = (columns[2].y * columns[3].z) - (columns[3].y * columns[2].z);

        const T coef03 = (columns[2].x * columns[3].w) - (columns[2].w * columns[3].x);
        const T coef04 = (columns[2].x * columns[3].z) - (columns[2].z * columns[3].x);
        const T coef05 = (columns[2].x * columns[3].y) - (columns[2].y * columns[3].x);

        const T coef06 = (columns[1].z * columns[3].w) - (columns[1].w * columns[3].z);
        const T coef07 = (columns[1].y * columns[3].w) - (columns[1].w * columns[3].y);
        const T coef08 = (columns[1].y * columns[3].z) - (columns[1].z * columns[3].y);

        const T coef09 = (columns[1].z * columns[2].w) - (columns[1].w * columns[2].z);
        const T coef10 = (columns[1].y * columns[2].w) - (columns[1].w * columns[2].y);
        const T coef11 = (columns[1].y * columns[2].z) - (columns[1].z * columns[2].y);

        const T coef12 = (columns[1].x * columns[3].w) - (columns[1].w * columns[3].x);
        const T coef13 = (columns[1].x * columns[3].z) - (columns[1].z * columns[3].x);
        const T coef14 = (columns[1].x * columns[2].w) - (columns[1].w * columns[2].x);

        const T coef15 = (columns[1].x * columns[2].z) - (columns[1].z * columns[2].x);
        const T coef16 = (columns[1].x * columns[3].y) - (columns[1].y * columns[3].x);
        const T coef17 = (columns[1].x * columns[2].y) - (columns[1].y * columns[2].x);

        const T det = (
            +(columns[0].x * ((columns[1].y * coef00) - (columns[1].z * coef01) + (columns[1].w * coef02)))
            -(columns[0].y * ((columns[1].x * coef00) - (columns[1].z * coef03) + (columns[1].w * coef04)))
            +(columns[0].z * ((columns[1].x * coef01) - (columns[1].y * coef03) + (columns[1].w * coef05)))
            -(columns[0].w * ((columns[1].x * coef02) - (columns[1].y * coef04) + (columns[1].z * coef05)))
        );

        if (math::is_zero_approx(det))
        {
            return type(static_cast<T>(0));
        }

        const T idet = static_cast<T>(1) / det;

        return type(
            +((columns[1].y * coef00) - (columns[1].z * coef01) + (columns[1].w * coef02)) * idet,
            -((columns[0].y * coef00) - (columns[0].z * coef01) + (columns[0].w * coef02)) * idet,
            +((columns[0].y * coef06) - (columns[0].z * coef07) + (columns[0].w * coef08)) * idet,
            -((columns[0].y * coef09) - (columns[0].z * coef10) + (columns[0].w * coef11)) * idet,

            -((columns[1].x * coef00) - (columns[1].z * coef03) + (columns[1].w * coef04)) * idet,
            +((columns[0].x * coef00) - (columns[0].z * coef03) + (columns[0].w * coef04)) * idet,
            -((columns[0].x * coef06) - (columns[0].z * coef12) + (columns[0].w * coef13)) * idet,
            +((columns[0].x * coef09) - (columns[0].z * coef14) + (columns[0].w * coef15)) * idet,

            +((columns[1].x * coef01) - (columns[1].y * coef03) + (columns[1].w * coef05)) * idet,
            -((columns[0].x * coef01) - (columns[0].y * coef03) + (columns[0].w * coef05)) * idet,
            +((columns[0].x * coef07) - (columns[0].y * coef12) + (columns[0].w * coef16)) * idet,
            -((columns[0].x * coef10) - (columns[0].y * coef14) + (columns[0].w * coef17)) * idet,

            -((columns[1].x * coef02) - (columns[1].y * coef04) + (columns[1].z * coef05)) * idet,
            +((columns[0].x * coef02) - (columns[0].y * coef04) + (columns[0].z * coef05)) * idet,
            -((columns[0].x * coef08) - (columns[0].y * coef13) + (columns[0].z * coef16)) * idet,
            +((columns[0].x * coef11) - (columns[0].y * coef15) + (columns[0].z * coef17)) * idet
        );
    }

    // =============== comparison and testing ===============

    inline constexpr void set(
        T x1, T y1, T z1, T w1,
        T x2, T y2, T z2, T w2,
        T x3, T y3, T z3, T w3,
        T x4, T y4, T z4, T w4
    )
    {
        columns[0].x = x1;
        columns[0].y = y1;
        columns[0].z = z1;
        columns[0].w = w1;

        columns[1].x = x2;
        columns[1].y = y2;
        columns[1].z = z2;
        columns[1].w = w2;

        columns[2].x = x3;
        columns[2].y = y3;
        columns[2].z = z3;
        columns[2].w = w3;

        columns[3].x = x4;
        columns[3].y = y4;
        columns[3].z = z4;
        columns[3].w = w4;
    }

    inline constexpr row_type get_row(size_type i) const
    {
        assert(i < 4);
        return row_type(
            columns[0][i],
            columns[1][i],
            columns[2][i],
            columns[3][i]
        );
    }

    inline constexpr void set_row(size_type i, const row_type& row)
    {
        assert(i < 4);
        columns[0][i] = row.x;
        columns[1][i] = row.y;
        columns[2][i] = row.z;
        columns[3][i] = row.w;
    }

    inline constexpr col_type get_column(size_type i) const
    {
        return operator[](i);
    }

    inline constexpr void set_column(size_type i, const col_type& column)
    {
        operator[](i) = column;
    }

    // =============== translation ===============

    /**
     * @brief Creates a translation matrix.
     *
     * This function generates a translation matrix based on the specified translation vector in 3D space.
     *
     * @param translation The translation vector in 3D space.
     * @return The 3D translation matrix.
     */
    static inline constexpr type make_translation(const vec3_type& translation)
    {
        return type(
            static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
            translation.x,     translation.y,     translation.z,     static_cast<T>(1)
        );
    }

    /**
     * @brief Retrieves the translation vector from the matrix.
     *
     * This function extracts the translation vector from the matrix.
     *
     * @return The translation vector in 3D space.
     */
    inline constexpr vec3_type get_translation() const
    {
        return vec3_type(columns[3].x, columns[3].y, columns[3].z);
    }

    // =============== rotation ===============

    /**
     * @brief Creates a rotation matrix from an axis and an angle.
     *
     * This function generates a rotation matrix representing a rotation around the specified axis by the given angle.
     *
     * @param axis The axis of rotation (must be normalized).
     * @param angle The angle of rotation in radians.
     * @return The rotation matrix.
     */
    static inline constexpr type from_axis_angle(const vec3_type& axis, T angle)
    {
        const vec3_type naxis(axis.normalize());

        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);
        const vec3_type t((static_cast<T>(1) - cosa) * naxis);

        return type(
            cosa + (t.x * naxis.x),
            (t.x * naxis.y) + (sina * naxis.z),
            (t.x * naxis.z) - (sina * naxis.y),
            static_cast<T>(0),

            (t.y * naxis.x) - (sina * naxis.z),
            cosa + (t.y * naxis.y),
            (t.y * naxis.z) + (sina * naxis.x),
            static_cast<T>(0),

            (t.z * naxis.x) + (sina * naxis.y),
            (t.z * naxis.y) - (sina * naxis.x),
            cosa + (t.z * naxis.z),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a rotation matrix from a quaternion.
     *
     * This function constructs a rotation matrix using the rotational part of the provided quaternion.
     *
     * @param rotation The quaternion representing the rotation.
     * @return The rotation matrix.
     */
    static inline constexpr type make_rotation(const quat_type& rotation)
    {
        return type(rotation);
    }

    /**
     * @brief Retrieves the rotation quaternion from the matrix.
     *
     * This function extracts the rotation quaternion representing the orientation encoded in the matrix.
     *
     * @return The rotation quaternion.
     */
    inline constexpr quat_type get_rotation() const
    {
        return quat_type(*this);
    }

    // =============== scale ===============

    /**
     * @brief Creates a scaling matrix.
     *
     * This function generates a scaling matrix based on the specified scaling factors along each axis.
     *
     * @param scale The scaling factors along the x, y, and z axes.
     * @return The scaling matrix.
     */
    static inline constexpr type make_scale(const vec3_type& scale)
    {
        return type(
            scale.x,
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            scale.y,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            scale.z,
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Retrieves the scaling factors from the matrix.
     *
     * This function calculates the scaling factors along the x, y, and z axes from the matrix.
     *
     * @return The vector representing the scaling factors along each axis.
     */
    inline constexpr vec3_type get_scale() const
    {
        return vec3_type(
            math::length(columns[0]),
            math::length(columns[1]),
            math::length(columns[2])
        );
    }

    // =============== transform ===============

    /**
     * @brief Creates a transformation matrix combining translation, rotation, and scaling.
     *
     * This function generates a transformation matrix by combining translation, rotation, and scaling.
     *
     * @param translation The translation vector in 3D space.
     * @param axis The axis of rotation.
     * @param angle The angle of rotation in radians.
     * @param scale The scaling factors along the x, y, and z axes.
     * @return The transformation matrix.
     */
    static inline constexpr type make_trs(
        const vec3_type& translation,
        const vec3_type& axis, T angle,
        const vec3_type& scale
    )
    {
        const vec3_type naxis(normalize(axis));

        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);
        const vec3_type t((static_cast<T>(1) - cosa) * naxis);

        return type(
            scale.x * (cosa + (t.x * naxis.x)),
            scale.x * ((t.x * naxis.y) + (sina * naxis.z)),
            scale.x * ((t.x * naxis.z) - (sina * naxis.y)),
            static_cast<T>(0),

            scale.y * ((t.y * naxis.x) - (sina * naxis.z)),
            scale.y * (cosa + (t.y * naxis.y)),
            scale.y * ((t.y * naxis.z) + (sina * naxis.x)),
            static_cast<T>(0),

            scale.z * ((t.z * naxis.x) + (sina * naxis.y)),
            scale.z * ((t.z * naxis.y) - (sina * naxis.x)),
            scale.z * (cosa + (t.z * naxis.z)),
            static_cast<T>(0),

            translation.x,
            translation.y,
            translation.z,
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a transformation matrix combining translation, rotation, and scaling.
     *
     * This function generates a transformation matrix by combining translation, rotation, and scaling.
     *
     * @param translation The translation vector in 3D space.
     * @param rotation The quaternion representing the rotation.
     * @param scale The scaling factors along the x, y, and z axes.
     * @return The transformation matrix.
     */
    static inline constexpr type make_trs(
        const vec3_type& translation,
        const quat_type& rotation,
        const vec3_type& scale
    )
    {
        const quat_type qn(rotation.normalize());

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qxz = qn.x * qn.z;
        const T qxy = qn.x * qn.y;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;
        const T qwz = qn.w * qn.z;

        return type(
            scale.x * (static_cast<T>(1) - (static_cast<T>(2) * (qyy + qzz))),
            scale.x * (static_cast<T>(2) * (qxy + qwz)),
            scale.x * (static_cast<T>(2) * (qxz - qwy)),
            static_cast<T>(0),

            scale.y * (static_cast<T>(2) * (qxy - qwz)),
            scale.y * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qzz))),
            scale.y * (static_cast<T>(2) * (qyz + qwx)),
            static_cast<T>(0),

            scale.z * (static_cast<T>(2) * (qxz + qwy)),
            scale.z * (static_cast<T>(2) * (qyz - qwx)),
            scale.z * (static_cast<T>(1) - (static_cast<T>(2) * (qxx + qyy))),
            static_cast<T>(0),

            translation.x,
            translation.y,
            translation.z,
            static_cast<T>(1)
        );
    }

    /**
     * @brief Computes the inverse of the transformation matrix, effectively reversing applied transformations.
     *
     * This function calculates the inverse matrix for a transformation matrix, providing a matrix that undoes
     * the original transformations.
     *
     * @return The inverted transformation matrix.
     */
    inline constexpr type affine_invert() const
    {
        const mat3_type inv(mat3_type(*this).invert());

        return type(
            col_type(inv.columns[0],               static_cast<T>(0)),
            col_type(inv.columns[1],               static_cast<T>(0)),
            col_type(inv.columns[2],               static_cast<T>(0)),
            col_type(inv * vec3_type(-columns[3]), static_cast<T>(1))
        );
    }

    /**
     * @brief Transforms a 3D vector using the matrix.
     *
     * This function applies the matrix transformation to a 3D vector.
     *
     * @param v The 3D vector to be transformed.
     * @return The transformed 3D vector.
     */
    inline constexpr vec3_type transform(const vec3_type& v) const
    {
        return (*this) * v;
    }

    // =============== look at ===============

    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh

    // https://www.3dgep.com/understanding-the-view-matrix/
    // https://webglfundamentals.org/webgl/lessons/webgl-3d-camera.html
    // https://stackoverflow.com/questions/31040378/whats-the-difference-between-these-two-ways-of-computing-a-lookat-matrix

    /**
     * @brief Creates a matrix representing a left-handed look-at rotation.
     *
     * Given an 'eye' position, a 'target' position, and an optional 'up' vector,
     * this function computes a matrix representing a left-handed look-at rotation.
     *
     * @param eye The position of the viewer's eye.
     * @param target The position the viewer is looking at.
     * @param up The up vector (default is positive y-axis).
     * @return The matrix representing the left-handed look-at rotation.
     */
    static inline constexpr type make_look_at_lh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        const vec3_type z(normalize(target - eye));
        const vec3_type x(normalize(cross(normalize(up), z)));
        const vec3_type y(cross(z, x));

        return type(
            x.x,
            y.x,
            z.x,
            static_cast<T>(0),

            x.y,
            y.y,
            z.y,
            static_cast<T>(0),

            x.z,
            y.z,
            z.z,
            static_cast<T>(0),

            -dot(x, eye),
            -dot(y, eye),
            -dot(z, eye),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a matrix representing a right-handed look-at rotation.
     *
     * Given an 'eye' position, a 'target' position, and an optional 'up' vector,
     * this function computes a matrix representing a right-handed look-at rotation.
     *
     * @param eye The position of the viewer's eye.
     * @param target The position the viewer is looking at.
     * @param up The up vector (default is positive y-axis).
     * @return The matrix representing the right-handed look-at rotation.
     */
    static inline constexpr type make_look_at_rh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        // In a right handed coordinate system, the camera is looking down the
        // -z axis, so to get a vector from the front of the camera to the
        // target, we reverse the subtraction.
        const vec3_type z(normalize(eye - target));
        const vec3_type x(normalize(cross(normalize(up), z)));
        const vec3_type y(cross(z, x));

        return type(
            x.x,
            y.x,
            z.x, 
            static_cast<T>(0),

            x.y,
            y.y,
            z.y,
            static_cast<T>(0),

            x.z,
            y.z,
            z.z,
            static_cast<T>(0),

            -dot(x, eye),
            -dot(y, eye),
            +dot(z, eye),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a matrix representing a look-at rotation.
     *
     * Given an 'eye' position, a 'target' position, and an optional 'up' vector,
     * this function computes a quaternion representing a look-at rotation based on
     * the configured clip control (left-handed or right-handed).
     *
     * @param eye The position of the viewer's eye.
     * @param target The position the viewer is looking at.
     * @param up The up vector (default is positive y-axis).
     * @return The quaternion representing the look-at rotation.
     */
    static inline constexpr type make_look_at(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return make_look_at_lh(eye, target, up);
#	else
        return make_look_at_rh(eye, target, up);
#	endif
    }

    // =============== ortho ===============

    /**
     * @brief Creates a left-handed orthographic projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_lh_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            static_cast<T>(2) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(2) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(1) / (zfar - znear),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -znear / (zfar - znear),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a left-handed orthographic projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_lh_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            static_cast<T>(2) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(2) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(2) / (zfar - znear),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -(zfar + znear) / (zfar - znear),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a right-handed orthographic projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_rh_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            static_cast<T>(2) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(2) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(-1) / (zfar - znear),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -znear / (zfar - znear),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates a right-handed orthographic projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_rh_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            static_cast<T>(2) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(2) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(-2) / (zfar - znear),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            -(zfar + znear) / (zfar - znear),
            static_cast<T>(1)
        );
    }

    /**
     * @brief Creates an orthographic projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#	else
        return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates an orthographic projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return ortho_lh_no(left, right, bottom, top, znear, zfar);
#	else
        return ortho_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a left-handed orthographic projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_lh(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#	else
        return ortho_lh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a right-handed orthographic projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho_rh(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#	else
        return ortho_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates an orthographic projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting orthographic projection matrix.
     */
    static inline constexpr type ortho(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
        return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
        return ortho_lh_no(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
        return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
        return ortho_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    // =============== frustrum ===============

    /**
     * @brief Creates a left-handed frustum projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_lh_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            (static_cast<T>(2) * znear) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            (static_cast<T>(2) * znear) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            zfar / (zfar - znear),
            static_cast<T>(1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a left-handed frustum projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_lh_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            (static_cast<T>(2) * znear) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            (static_cast<T>(2) * znear) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            -(right + left) / (right - left),
            -(top + bottom) / (top - bottom),
            (zfar + znear) / (zfar - znear),
            static_cast<T>(1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a right-handed frustum projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_rh_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            (static_cast<T>(2) * znear) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            (static_cast<T>(2) * znear) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            (right + left) / (right - left),
            (top + bottom) / (top - bottom),
            -zfar / (zfar - znear),
            static_cast<T>(-1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a right-handed frustum projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_rh_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
        return type(
            (static_cast<T>(2) * znear) / (right - left),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            (static_cast<T>(2) * znear) / (top - bottom),
            static_cast<T>(0),
            static_cast<T>(0),

            (right + left) / (right - left),
            (top + bottom) / (top - bottom),
            -(zfar + znear) / (zfar - znear),
            static_cast<T>(-1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a frustum projection matrix with a depth range of 0 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_zo(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#	else
        return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a frustum projection matrix with a depth range of -1 to 1.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_no(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#	else
        return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a left-handed frustum projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_lh(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#	else
        return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a right-handed frustum projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum_rh(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#	else
        return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a frustum projection matrix based on the specified clip control configuration.
     *
     * @param left   The left clipping plane coordinate.
     * @param right  The right clipping plane coordinate.
     * @param top    The top clipping plane coordinate.
     * @param bottom The bottom clipping plane coordinate.
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting frustum projection matrix.
     */
    static inline constexpr type frustrum(
        T left, T right,
        T top, T bottom,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
        return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
        return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
        return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
        return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#	endif
    }

    // =============== perspective ===============

    /**
     * @brief Creates a left-handed perspective projection matrix with a depth range of 0 to 1.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_lh_zo(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
        assert(!math::is_zero_approx(aspect));

        const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

        return type(
            static_cast<T>(1) / (aspect * tanfovy2),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(1) / tanfovy2,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            zfar / (zfar - znear),
            static_cast<T>(1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a left-handed perspective projection matrix with a depth range of -1 to 1.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_lh_no(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
        assert(!math::is_zero_approx(aspect));

        const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

        return type(
            static_cast<T>(1) / (aspect * tanfovy2),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(1) / tanfovy2,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            (zfar + znear) / (zfar - znear),
            static_cast<T>(1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a right-handed perspective projection matrix with a depth range of 0 to 1.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     *
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_rh_zo(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
        assert(!math::is_zero_approx(aspect));

        const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

        return type(
            static_cast<T>(1) / (aspect * tanfovy2),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(1) / tanfovy2,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            -zfar / (zfar - znear),
            static_cast<T>(-1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a right-handed perspective projection matrix with a depth range of -1 to 1.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_rh_no(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
        assert(!math::is_zero_approx(aspect));

        const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

        return type(
            static_cast<T>(1) / (aspect * tanfovy2),
            static_cast<T>(0),
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(1) / tanfovy2,
            static_cast<T>(0),
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            -(zfar + znear) / (zfar - znear),
            static_cast<T>(-1),

            static_cast<T>(0),
            static_cast<T>(0),
            -(static_cast<T>(2) * zfar * znear) / (zfar - znear),
            static_cast<T>(0)
        );
    }

    /**
     * @brief Creates a perspective projection matrix with a depth range of 0 to 1 based on the specified clip control configuration.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_zo(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return perspective_lh_zo(fovy, aspect, znear, zfar);
#	else
        return perspective_rh_zo(fovy, aspect, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a perspective projection matrix with a depth range of -1 to 1 based on the specified clip control configuration.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_no(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return perspective_lh_no(fovy, aspect, znear, zfar);
#	else
        return perspective_rh_no(fovy, aspect, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a left-handed perspective projection matrix based on the specified clip control configuration.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_lh(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return perspective_lh_zo(fovy, aspect, znear, zfar);
#	else
        return perspective_lh_no(fovy, aspect, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a right-handed perspective projection matrix based on the specified clip control configuration.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective_rh(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
        return perspective_rh_zo(fovy, aspect, znear, zfar);
#	else
        return perspective_rh_no(fovy, aspect, znear, zfar);
#	endif
    }

    /**
     * @brief Creates a perspective projection matrix based on the specified clip control configuration.
     *
     * @param fovy   The vertical field of view angle in radians.
     * @param aspect The aspect ratio of the viewport (width / height).
     * @param znear  The near depth clipping plane.
     * @param zfar   The far depth clipping plane.
     * @return The resulting perspective projection matrix.
     */
    static inline constexpr type perspective(
        T fovy, T aspect,
        T znear, T zfar
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
        return perspective_lh_zo(fovy, aspect, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
        return perspective_lh_no(fovy, aspect, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
        return perspective_rh_zo(fovy, aspect, znear, zfar);
#	elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
        return perspective_rh_no(fovy, aspect, znear, zfar);
#	endif
    }

    // =============== projection ===============

    /**
     * @brief Projects a 3D vector using the current matrix, resulting in a normalized device coordinates (NDC) vector.
     *
     * This function takes a 3D vector as input, transforms it using the current matrix, and returns the resulting vector
     * in normalized device coordinates (NDC). The NDC vector represents the position of the input vector after perspective
     * division, ensuring it is within the viewing volume.
     *
     * @param v The 3D vector to be projected.
     *
     * @return The resulting normalized device coordinates (NDC) vector.
     */
    inline constexpr vec3_type project(const vec3_type& v) const
    {
        vec4_type v4(v, static_cast<T>(1));
        v4 = (*this) * v4;
        const T invw = static_cast<T>(1) / v4.w;
        return vec3_type(v4.x * invw, v4.y * invw, v4.z * invw);
    }

    /**
     * @brief Unprojects a 3D vector using the inverse of the current matrix, converting from normalized device coordinates (NDC) to world space.
     *
     * This function takes a 3D vector in normalized device coordinates (NDC), applies the inverse of the current matrix,
     * and returns the resulting vector in world space. It reverses the process of projecting a vector to NDC, allowing for
     * conversion from screen space back to the original world coordinates.
     *
     * @param v The 3D vector in normalized device coordinates (NDC) to be unprojected.
     * @return The resulting vector in world space.
     */
    inline constexpr vec3_type unproject(const vec3_type& v) const
    {
        vec4_type v4(v, static_cast<T>(1));
        v4 = invert() * v4;
        const T invw = static_cast<T>(1) / v4.w;
        return vec3_type(v4.x * invw, v4.y * invw, v4.z * invw);
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(static_cast<T>(0)); }
    static inline constexpr type ZERO() { return type(static_cast<T>(0)); }

};

}

using mat4  = detail::mat<4, 4,  float>;
using mat4f = detail::mat<4, 4,  float>;
using mat4d = detail::mat<4, 4, double>;

}
}