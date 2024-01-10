#pragma once

#include <iomanip>

#include "common.h"
#include "../fn_comparison.h"
#include "../fn_trigonometric.h"
#include "../vec_fn_geometric.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct mat<2, 2, T>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using type = mat<2, 2, T>;
    using col_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using row_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using transpose_type = mat<2, 2, T>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::floating_point>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::floating_point>;

    using mat3_type = mat<3, 3, T>;
    using mat4_type = mat<4, 4, T>;

    using value_type = T;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(4); }
    static inline constexpr size_type width() noexcept { return static_cast<T>(2); }
    static inline constexpr size_type height() noexcept { return static_cast<T>(2); }

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

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scaler)) } {}

    template <typename U, std::enable_if_t<std::is_arithmetic<U>::value, bool> = true>
    inline constexpr mat(U x1, U y1, U x2, U y2)
        : columns{ col_type(x1, y1), col_type(x2, y2) } {}

    template <typename U>
    inline constexpr mat(const vecx<2, U>& v1, const vecx<2, U>& v2)
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
        return m;
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

    inline col_type* begin() noexcept
    {
        return &columns[0];
    }

    inline const col_type* begin() const noexcept
    {
        return cbegin();
    }

    inline col_type* end() noexcept
    {
        return &columns[1] + 1;
    }

    inline const col_type* end() const noexcept
    {
        return cend();
    }

    inline const col_type* cbegin() const noexcept
    {
        return &columns[0];
    }

    inline const col_type* cend() const noexcept
    {
        return &columns[1] + 1;
    }

    inline auto rbegin() noexcept
    {
        return std::reverse_iterator<col_type*>(&columns[1] + 1);
    }

    inline const auto rbegin() const noexcept
    {
        return crbegin();
    }

    inline auto rend() noexcept
    {
        return std::reverse_iterator<col_type*>(&columns[0]);
    }

    inline const auto rend() const noexcept
    {
        return crend();
    }

    inline auto crbegin() const noexcept
    {
        return std::reverse_iterator<const col_type*>(&columns[1] + 1);
    }

    inline auto crend() const noexcept
    {
        return std::reverse_iterator<const col_type*>(&columns[0]);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        if (pretty_print)
        {
            return type(
                math::make_pretty(columns[0].x),
                math::make_pretty(columns[0].y),
                math::make_pretty(columns[1].x),
                math::make_pretty(columns[1].y)
            ).to_string();
        }

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

    inline constexpr T determinant() const
    {
        return (columns[0].x * columns[1].y) - (columns[1].x * columns[0].y);
    }

    inline constexpr type transpose() const
    {
        return type(
            columns[0].x, columns[1].x,
            columns[0].y, columns[1].y
        );
    }

    inline constexpr type invert() const
    {
        const T det = determinant();

        if (math::is_zero_approx(det))
        {
            return type(static_cast<T>(0));
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

    inline constexpr void set(
        T x1, T y1,
        T x2, T y2
    )
    {
        columns[0].x = x1;
        columns[0].y = y1;

        columns[1].x = x2;
        columns[1].y = y2;
    }

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

    // =============== matrix transform ===============

    static inline constexpr type make_rotation(T angle)
    {
        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);

        return type(cosa, sina, -sina, cosa);
    }

    inline constexpr T get_rotation() const
    {
        return math::atan2(columns[0].y, columns[0].x);
    }

    static inline constexpr type make_scale(const vec2_type& scale)
    {
        return type(
            scale.x, static_cast<T>(0),
            static_cast<T>(0), scale.y
        );
    }

    inline constexpr vec2_type get_scale() const
    {
        return vec2_type(
            math::length(columns[0]),
            math::length(columns[1])
        );
    }

    // =============== constants ===============

    static inline constexpr type ZERO() { return type(static_cast<T>(0)); }

};

}
}
}