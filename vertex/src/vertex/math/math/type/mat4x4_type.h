#pragma once

#include <iomanip>

#include "../math.h"

namespace vx {
namespace math {

// https://www.andre-gaschler.com/rotationconverter/

template <typename T>
struct mat<4, 4, T>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;

    using type = mat<4, 4, T>;
    using col_type = vec<4, T>;
    using row_type = vec<4, T>;
    using transpose_type = mat<4, 4, T>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(16); }
    static inline constexpr size_type width() noexcept { return static_cast<size_type>(4); }
    static inline constexpr size_type height() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::detail::iterator<col_type>;
    using const_iterator = ::vx::detail::iterator<const col_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

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
        const vec<4, U>& v1,
        const vec<4, U>& v2,
        const vec<4, U>& v3,
        const vec<4, U>& v4
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

    template <typename U>
    inline constexpr type& operator=(const mat<4, 4, U>& m) noexcept
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
        return type(
            +m.columns[0],
            +m.columns[1],
            +m.columns[2],
            +m.columns[3]
        );
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

    inline constexpr std::string to_string() const
    {
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
            return type(0);
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

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(1); }
    static inline constexpr type ZERO() { return type(0); }

};

using mat4  = mat<4, 4,  float>;
using mat4f = mat<4, 4,  float>;
using mat4d = mat<4, 4, double>;

}
}