#pragma once

#include <iomanip>

#include "../math.h"

namespace vx {
namespace math {

template <typename T>
struct mat<2, 2, T>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;

    using type = mat<2, 2, T>;
    using col_type = vec<2, T>;
    using row_type = vec<2, T>;
    using transpose_type = mat<2, 2, T>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }
    static inline constexpr size_type width() noexcept { return static_cast<size_type>(2); }
    static inline constexpr size_type height() noexcept { return static_cast<size_type>(2); }

    using iterator = ::vx::detail::iterator<col_type>;
    using const_iterator = ::vx::detail::iterator<const col_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    col_type columns[2];

    // =============== implicit constructors ===============

    inline constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(1), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(1)) } {}

    inline constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1] } {}

    inline constexpr mat(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit mat(T scaler) noexcept
        : columns{ col_type(scaler, static_cast<T>(0)),
                   col_type(static_cast<T>(0), scaler) } {}

    inline constexpr mat(T x1, T y1, T x2, T y2) noexcept
        : columns{ col_type(x1, y1), col_type(x2, y2) } {}

    inline constexpr mat(const col_type& v1, const col_type& v2) noexcept
        : columns{ v1, v2 } {}

    // =============== conversion matrix constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scaler)) } {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr mat(U x1, U y1, U x2, U y2)
        : columns{ col_type(x1, y1), col_type(x2, y2) } {}

    template <typename U>
    inline constexpr mat(const vec<2, U>& v1, const vec<2, U>& v2)
        : columns{ v1, v2 } {}

    template <typename U>
    inline constexpr explicit mat(const mat<2, 2, U>& m)
        : columns{ m.columns[0], m.columns[1] } {}

    template <typename U>
    inline constexpr explicit mat(const mat<3, 3, U>& m)
        : columns{ col_type(m.columns[0]), col_type(m.columns[1]) } {}

    template <typename U>
    inline constexpr explicit mat(const mat<4, 4, U>& m)
        : columns{ col_type(m.columns[0]), col_type(m.columns[1]) } {}

    // =============== destructor ===============

    ~mat() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        return *this;
    }

    template <typename U>
    inline constexpr type& operator=(const mat<2, 2, U>& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr col_type& operator[](size_type i)
    {
        assert(i < 2);
        return columns[i];
    }

    inline constexpr const col_type& operator[](size_type i) const
    {
        assert(i < 2);
        return columns[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& m1, const type& m2)
    {
        return (m1.columns[0] == m2.columns[0])
            && (m1.columns[1] == m2.columns[1]);
    }

    friend inline constexpr bool operator!=(const type& m1, const type& m2)
    {
        return !(m1 == m2);
    }
    
    friend inline constexpr bool operator<(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0]) return (m1.columns[0] < m2.columns[0]);
        else                                return (m1.columns[1] < m2.columns[1]);
    }

    friend inline constexpr bool operator>(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0]) return (m1.columns[0] > m2.columns[0]);
        else                                return (m1.columns[1] > m2.columns[1]);
    }

    friend inline constexpr bool operator<=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0]) return (m1.columns[0] <  m2.columns[0]);
        else                                return (m1.columns[1] <= m2.columns[1]);
    }

    friend inline constexpr bool operator>=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0]) return (m1.columns[0] >  m2.columns[0]);
        else                                return (m1.columns[1] >= m2.columns[1]);
    }

    // =============== unary constant operators ===============

    friend inline constexpr type operator+(const type& m)
    {
        return type(+m.columns[0], +m.columns[1]);
    }

    friend inline constexpr type operator-(const type& m)
    {
        return type(-m.columns[0], -m.columns[1]);
    }

    // =============== incrememnt and decrement operators ===============

    // increment (++)

    inline constexpr type& operator++()
    {
        ++columns[0];
        ++columns[1];
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
        return type(m.columns[0] + scaler, m.columns[1] + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& m)
    {
        return type(scaler + m.columns[0], scaler + m.columns[1]);
    }

    friend inline constexpr type operator+(const type& m1, const type& m2)
    {
        return type(m1.columns[0] + m2.columns[0], m1.columns[1] + m2.columns[1]);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& m, T scaler)
    {
        return type(m.columns[0] - scaler, m.columns[1] - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& m)
    {
        return type(scaler - m.columns[0], scaler - m.columns[1]);
    }

    friend inline constexpr type operator-(const type& m1, const type& m2)
    {
        return type(m1.columns[0] - m2.columns[0], m1.columns[1] - m2.columns[1]);
    }

    // multiplication (*)

    friend inline constexpr type operator*(T scaler, const type& m)
    {
        return type(scaler * m.columns[0], scaler * m.columns[1]);
    }

    friend inline constexpr type operator*(const type& m, T scaler)
    {
        return type(m.columns[0] * scaler, m.columns[1] * scaler);
    }

    friend inline constexpr col_type operator*(const type& m, const row_type& v)
    {
        return col_type(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y)
        );
    }

    friend inline constexpr row_type operator*(const col_type& v, const type& m)
    {
        return row_type(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y)
        );
    }

    friend inline constexpr type operator*(const type& m1, const type& m2)
    {
        return type(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y),
            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y)
        );
    }

    // division (/)

    friend inline constexpr type operator/(T scaler, const type& m)
    {
        return type(scaler / m.columns[0], scaler / m.columns[1]);
    }

    friend inline constexpr type operator/(const type& m, T scaler)
    {
        return type(m.columns[0] / scaler, m.columns[1] / scaler);
    }

    friend inline constexpr col_type operator/(const type& m, const row_type& v)
    {
        return m.invert() * v;
    }

    friend inline constexpr row_type operator/(const col_type& v, const type& m)
    {
        return v * m.invert();
    }

    friend inline constexpr type operator/(const type& m1, const type& m2)
    {
        return m1 * m2.invert();
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(T scaler)
    {
        columns[0] += scaler;
        columns[1] += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& m)
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        columns[0] -= scaler;
        columns[1] -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& m)
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        columns[0] *= scaler;
        columns[1] *= scaler;
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
        return iterator(&columns[1] + 1);
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
        return const_iterator(&columns[1] + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&columns[1] + 1);
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
        return const_reverse_iterator(&columns[1] + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&columns[0]);
    }

    // =============== string ===============

    inline constexpr std::string to_string() const
    {
        size_t wmax = 0;
        for (size_t i = 0; i < 2; i++)
        {
            for (size_t j = 0; j < 2; j++)
            {
                std::ostringstream oss;
                oss << columns[j][i];
                const size_t w = oss.str().length();
                if (w > wmax) wmax = w;
            }
        }

        std::ostringstream oss;
        for (size_t i = 0; i < 2; i++)
        {
            oss << '{';
            for (size_t j = 0; j < 2; j++)
            {
                oss << std::setw(wmax + 1) << columns[j][i];
            }
            oss << " }";
            if (i == 0) oss << '\n';
        }

        return oss.str();
    }

    // =============== operations ===============

    /**
     * @brief Transposes the matrix.
     *
     * @return The transposed matrix.
     */
    inline constexpr type transpose() const
    {
        return type(
            columns[0].x, columns[1].x,
            columns[0].y, columns[1].y
        );
    }

    /**
     * @brief Calculates the determinant of the matrix.
     *
     * @return The determinant of the matrix.
     */
    inline constexpr T determinant() const
    {
        return (columns[0].x * columns[1].y) - (columns[1].x * columns[0].y);
    }

    /**
     * @brief Calculates the inverse of the matrix.
     *
     * This function calculates the inverse of the 2x2 matrix.
     * If the determinant is close to zero, indicating singularity,
     * a zero matrix is returned.
     *
     * @return The inverse of the matrix, or a zero matrix if the determinant is close to zero.
     */
    inline constexpr type invert() const
    {
        const T det = determinant();

        if (math::is_zero_approx(det))
        {
            return type(0);
        }

        const T idet = static_cast<T>(1) / det;

        return type(
            +columns[1].y * idet,
            -columns[0].y * idet,
            -columns[1].x * idet,
            +columns[0].x * idet
        );
    }

    // =============== comparison and testing ===============

    inline constexpr row_type get_row(size_type i) const
    {
        assert(i < 2);
        return row_type(columns[0][i], columns[1][i]);
    }

    inline constexpr void set_row(size_type i, const row_type& row)
    {
        assert(i < 2);
        columns[0][i] = row.x;
        columns[1][i] = row.y;
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

using mat2  = mat<2, 2, float>;
using mat2f = mat<2, 2, float>;
using mat2d = mat<2, 2, double>;

}
}