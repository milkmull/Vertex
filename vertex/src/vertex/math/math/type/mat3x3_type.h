#pragma once

#include <iomanip>

#include "../math.h"

namespace vx {
namespace math {
namespace detail {

template <typename T>
struct mat<3, 3, T>
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;

    using type = mat<3, 3, T>;
    using col_type = vec<3, T, vec_t::vec>;
    using row_type = vec<3, T, vec_t::vec>;
    using transpose_type = mat<3, 3, T>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(9); }
    static inline constexpr size_type width() noexcept { return static_cast<size_type>(3); }
    static inline constexpr size_type height() noexcept { return static_cast<size_type>(3); }

    using iterator = ::vx::detail::iterator<col_type>;
    using const_iterator = ::vx::detail::iterator<const col_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec>;
    using vec3_type = vec<3, T, vec_t::vec>;
    using vec4_type = vec<4, T, vec_t::vec>;

    using mat2_type = mat<2, 2, T>;
    using mat4_type = mat<4, 4, T>;

    using quat_type = quat<T>;

    // =============== data ===============

    col_type columns[3];

    // =============== implicit constructors ===============

    inline constexpr mat() noexcept
        : columns{ col_type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

    inline constexpr mat(const type& m) noexcept
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    inline constexpr mat(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr explicit mat(T scaler) noexcept
        : columns{ col_type(scaler, static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), scaler, static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), scaler) } {}

    inline constexpr mat(
        T x1, T y1, T z1,
        T x2, T y2, T z2,
        T x3, T y3, T z3
    ) noexcept
        : columns{ col_type(x1, y1, z1),
                   col_type(x2, y2, z2),
                   col_type(x3, y3, z3) } {}

    inline constexpr mat(
        const col_type& v1,
        const col_type& v2,
        const col_type& v3
    ) noexcept
        : columns{ v1, v2, v3 } {}

    // =============== conversion matrix constructors ===============

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr explicit mat(U scaler)
        : columns{ col_type(static_cast<T>(scaler), static_cast<T>(0), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(scaler), static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(scaler)) } {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr mat(
        U x1, U y1, U z1,
        U x2, U y2, U z2,
        U x3, U y3, U z3
    )
        : columns{ col_type(x1, y1, z1),
                   col_type(x2, y2, z2),
                   col_type(x3, y3, z3) } {}

    template <typename U>
    inline constexpr mat(
        const vecx<3, U>& v1,
        const vecx<3, U>& v2,
        const vecx<3, U>& v3
    )
        : columns{ v1, v2, v3 } {}

    template <typename U>
    inline constexpr explicit mat(const mat<2, 2, U>& m)
        : columns{ col_type(m.columns[0],						  static_cast<T>(0)),
                   col_type(m.columns[1],						  static_cast<T>(0)),
                   col_type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)) } {}

    template <typename U>
    inline constexpr explicit mat(const mat<3, 3, U>& m)
        : columns{ m.columns[0], m.columns[1], m.columns[2] } {}

    template <typename U>
    inline constexpr explicit mat(const mat<4, 4, U>& m)
        : columns{ col_type(m.columns[0]),
                   col_type(m.columns[1]),
                   col_type(m.columns[2]) } {}

    /**
     * @brief Constructs a 3x3 matrix from a quaternion.
     *
     * This function initializes the matrix using the rotational part of the provided quaternion.
     * The resulting matrix represents the rotation encoded by the quaternion.
     *
     * @param q The quaternion to convert into a rotation matrix.
     * 
     * @ref https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
     */
    inline constexpr explicit mat(const quat_type& q)
    {
        const quat_type qn = q.normalize();

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qxz = qn.x * qn.z;
        const T qxy = qn.x * qn.y;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;
        const T qwz = qn.w * qn.z;

        columns[0].x = static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz);
        columns[0].y = static_cast<T>(2) * (qxy + qwz);
        columns[0].z = static_cast<T>(2) * (qxz - qwy);

        columns[1].x = static_cast<T>(2) * (qxy - qwz);
        columns[1].y = static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz);
        columns[1].z = static_cast<T>(2) * (qyz + qwx);

        columns[2].x = static_cast<T>(2) * (qxz + qwy);
        columns[2].y = static_cast<T>(2) * (qyz - qwx);
        columns[2].z = static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy);
    }

    // =============== destructor ===============

    ~mat() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& m) noexcept
    {
        columns[0] = m.columns[0];
        columns[1] = m.columns[1];
        columns[2] = m.columns[2];
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr col_type& operator[](size_type i)
    {
        assert(i < 3);
        return columns[i];
    }

    inline constexpr const col_type& operator[](size_type i) const
    {
        assert(i < 3);
        return columns[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& m1, const type& m2)
    {
        return (m1.columns[0] == m2.columns[0])
            && (m1.columns[1] == m2.columns[1])
            && (m1.columns[2] == m2.columns[2]);
    }

    friend inline constexpr bool operator!=(const type& m1, const type& m2)
    {
        return !(m1 == m2);
    }

    friend inline constexpr bool operator<(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] < m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] < m2.columns[1]);
        else                                return (m1.columns[2] < m2.columns[2]);
    }

    friend inline constexpr bool operator>(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] > m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] > m2.columns[1]);
        else                                return (m1.columns[2] > m2.columns[2]);
    }

    friend inline constexpr bool operator<=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] <  m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] <  m2.columns[1]);
        else								return (m1.columns[2] <= m2.columns[2]);
    }

    friend inline constexpr bool operator>=(const type& m1, const type& m2)
    {
        if (m1.columns[0] != m2.columns[0])	return (m1.columns[0] >  m2.columns[0]);
        if (m1.columns[1] != m2.columns[1])	return (m1.columns[1] >  m2.columns[1]);
        else								return (m1.columns[2] >= m2.columns[2]);
    }

    // =============== unary constant operators ===============

    friend inline constexpr type operator+(const type& m)
    {
        return m;
    }

    friend inline constexpr type operator-(const type& m)
    {
        return type(-m.columns[0], -m.columns[1], -m.columns[2]);
    }

    // =============== incrememnt and decrement operators ===============

    // incrememnt (++)

    inline constexpr type& operator++()
    {
        ++columns[0];
        ++columns[1];
        ++columns[2];
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
        return type(m.columns[0] + scaler, m.columns[1] + scaler, m.columns[2] + scaler);
    }

    friend inline constexpr type operator+(T scaler, const type& m)
    {
        return type(scaler + m.columns[0], scaler + m.columns[1], scaler + m.columns[2]);
    }

    friend inline constexpr type operator+(const type& m1, const type& m2)
    {
        return type(
            m1.columns[0] + m2.columns[0],
            m1.columns[1] + m2.columns[1],
            m1.columns[2] + m2.columns[2]
        );
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& m, T scaler)
    {
        return type(m.columns[0] - scaler, m.columns[1] - scaler, m.columns[2] - scaler);
    }

    friend inline constexpr type operator-(T scaler, const type& m)
    {
        return type(scaler - m.columns[0], scaler - m.columns[1], scaler - m.columns[2]);
    }

    friend inline constexpr type operator-(const type& m1, const type& m2)
    {
        return type(
            m1.columns[0] - m2.columns[0],
            m1.columns[1] - m2.columns[1],
            m1.columns[2] - m2.columns[2]
        );
    }

    // multiplication (*)

    friend inline constexpr type operator*(T scaler, const type& m)
    {
        return type(scaler * m.columns[0], scaler * m.columns[1], scaler * m.columns[2]);
    }

    friend inline constexpr type operator*(const type& m, T scaler)
    {
        return type(m.columns[0] * scaler, m.columns[1] * scaler, m.columns[2] * scaler);
    }

    friend inline constexpr vec2_type operator*(const type& m, const vec2_type& v)
    {
        return vec2_type(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + m.columns[2].x,
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + m.columns[2].y
        );
    }

    friend inline constexpr col_type operator*(const type& m, const row_type& v)
    {
        return col_type(
            (m.columns[0].x * v.x) + (m.columns[1].x * v.y) + (m.columns[2].x * v.z),
            (m.columns[0].y * v.x) + (m.columns[1].y * v.y) + (m.columns[2].y * v.z),
            (m.columns[0].z * v.x) + (m.columns[1].z * v.y) + (m.columns[2].z * v.z)
        );
    }

    friend inline constexpr row_type operator*(const col_type& v, const type& m)
    {
        return row_type(
            (v.x * m.columns[0].x) + (v.y * m.columns[1].x) + (v.z * m.columns[2].x),
            (v.x * m.columns[0].y) + (v.y * m.columns[1].y) + (v.z * m.columns[2].y),
            (v.x * m.columns[0].z) + (v.y * m.columns[1].z) + (v.z * m.columns[2].z)
        );
    }

    friend inline constexpr type operator*(const type& m1, const type& m2)
    {
        return type(
            (m1.columns[0].x * m2.columns[0].x) + (m1.columns[1].x * m2.columns[0].y) + (m1.columns[2].x * m2.columns[0].z),
            (m1.columns[0].y * m2.columns[0].x) + (m1.columns[1].y * m2.columns[0].y) + (m1.columns[2].y * m2.columns[0].z),
            (m1.columns[0].z * m2.columns[0].x) + (m1.columns[1].z * m2.columns[0].y) + (m1.columns[2].z * m2.columns[0].z),

            (m1.columns[0].x * m2.columns[1].x) + (m1.columns[1].x * m2.columns[1].y) + (m1.columns[2].x * m2.columns[1].z),
            (m1.columns[0].y * m2.columns[1].x) + (m1.columns[1].y * m2.columns[1].y) + (m1.columns[2].y * m2.columns[1].z),
            (m1.columns[0].z * m2.columns[1].x) + (m1.columns[1].z * m2.columns[1].y) + (m1.columns[2].z * m2.columns[1].z),

            (m1.columns[0].x * m2.columns[2].x) + (m1.columns[1].x * m2.columns[2].y) + (m1.columns[2].x * m2.columns[2].z),
            (m1.columns[0].y * m2.columns[2].x) + (m1.columns[1].y * m2.columns[2].y) + (m1.columns[2].y * m2.columns[2].z),
            (m1.columns[0].z * m2.columns[2].x) + (m1.columns[1].z * m2.columns[2].y) + (m1.columns[2].z * m2.columns[2].z)
        );
    }

    // division (/)

    friend inline constexpr type operator/(T scaler, const type& m)
    {
        return type(scaler / m.columns[0], scaler / m.columns[1], scaler / m.columns[2]);
    }

    friend inline constexpr type operator/(const type& m, T scaler)
    {
        return type(m.columns[0] / scaler, m.columns[1] / scaler, m.columns[2] / scaler);
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
        columns[2] += scaler;
        return *this;
    }

    inline constexpr type& operator+=(const type& m)
    {
        columns[0] += m.columns[0];
        columns[1] += m.columns[1];
        columns[2] += m.columns[2];
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(T scaler)
    {
        columns[0] -= scaler;
        columns[1] -= scaler;
        columns[2] -= scaler;
        return *this;
    }

    inline constexpr type& operator-=(const type& m)
    {
        columns[0] -= m.columns[0];
        columns[1] -= m.columns[1];
        columns[2] -= m.columns[2];
        return *this;
    }

    // multplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        columns[0] *= scaler;
        columns[1] *= scaler;
        columns[2] *= scaler;
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
        return iterator(&columns[2] + 1);
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
        return const_iterator(&columns[2] + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&columns[2] + 1);
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
        return const_reverse_iterator(&columns[2] + 1);
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

                math::make_pretty(columns[1].x),
                math::make_pretty(columns[1].y),
                math::make_pretty(columns[1].z),

                math::make_pretty(columns[2].x),
                math::make_pretty(columns[2].y),
                math::make_pretty(columns[2].z)
            ).to_string();
        }

        size_t wmax = 0;
        for (size_t i = 0; i < 3; i++)
        {
            for (size_t j = 0; j < 3; j++)
            {
                std::ostringstream oss;
                oss << columns[j][i];
                const size_t w = oss.str().length();
                if (w > wmax) wmax = w;
            }
        }

        std::ostringstream oss;
        for (size_t i = 0; i < 3; i++)
        {
            oss << '{';
            for (size_t j = 0; j < 3; j++)
            {
                oss << std::setw(wmax + 1) << columns[j][i];
            }
            oss << " }";
            if (i <= 1) oss << '\n';
        }

        return oss.str();
    }

    // =============== operations ===============

    /**
     * @brief Calculates the determinant of the 3x3 matrix.
     *
     * This function computes the determinant of the matrix using the expansion by minors method.
     *
     * @return The determinant of the matrix.
     */
    inline constexpr T determinant() const
    {
        return +columns[0].x * ((columns[1].y * columns[2].z) - (columns[2].y * columns[1].z))
               -columns[1].x * ((columns[0].y * columns[2].z) - (columns[2].y * columns[0].z))
               +columns[2].x * ((columns[0].y * columns[1].z) - (columns[1].y * columns[0].z));
    }

    /**
     * @brief Computes the transpose of the 3x3 matrix.
     *
     * This function returns a new matrix where the rows become columns and vice versa.
     *
     * @return The transposed matrix.
     */
    inline constexpr type transpose() const
    {
        return type(
            columns[0].x, columns[1].x, columns[2].x,
            columns[0].y, columns[1].y, columns[2].y,
            columns[0].z, columns[1].z, columns[2].z
        );
    }

    /**
     * @brief Computes the inverse of the 3x3 matrix.
     *
     * This function calculates the inverse matrix if it exists, otherwise, it returns a matrix with zeros.
     * The inverse is computed using the determinant and adjugate matrix.
     *
     * @return The inverted matrix if the determinant is non-zero, otherwise a matrix with zeros.
     */
    inline constexpr type invert() const
    {
        const T det = determinant();

        if (math::is_zero_approx(det))
        {
            return type(static_cast<T>(0));
        }

        const T idet = static_cast<T>(1) / det;

        return type(
            +((columns[1].y * columns[2].z) - (columns[2].y * columns[1].z)) * idet,
            -((columns[0].y * columns[2].z) - (columns[2].y * columns[0].z)) * idet,
            +((columns[0].y * columns[1].z) - (columns[1].y * columns[0].z)) * idet,

            -((columns[1].x * columns[2].z) - (columns[2].x * columns[1].z)) * idet,
            +((columns[0].x * columns[2].z) - (columns[2].x * columns[0].z)) * idet,
            -((columns[0].x * columns[1].z) - (columns[1].x * columns[0].z)) * idet,

            +((columns[1].x * columns[2].y) - (columns[2].x * columns[1].y)) * idet,
            -((columns[0].x * columns[2].y) - (columns[2].x * columns[0].y)) * idet,
            +((columns[0].x * columns[1].y) - (columns[1].x * columns[0].y)) * idet
        );
    }

    // =============== comparison and testing ===============

    inline constexpr void set(
        T x1, T y1, T z1,
        T x2, T y2, T z2,
        T x3, T y3, T z3
    )
    {
        columns[0].x = x1;
        columns[0].y = y1;
        columns[0].z = z1;

        columns[1].x = x2;
        columns[1].y = y2;
        columns[1].z = z2;

        columns[2].x = x3;
        columns[2].y = y3;
        columns[2].z = z3;
    }

    inline constexpr row_type get_row(size_type i) const
    {
        assert(i < 3);
        return row_type(columns[0][i], columns[1][i], columns[2][i]);
    }

    inline constexpr void set_row(size_type i, const row_type& row)
    {
        assert(i < 3);
        columns[0][i] = row.x;
        columns[1][i] = row.y;
        columns[2][i] = row.z;
    }

    inline constexpr col_type get_column(size_type i) const
    {
        return operator[](i);
    }

    inline constexpr void set_column(size_type i, const col_type& column)
    {
        operator[](i) = column;
    }

    // =============== rotation ===============

    /**
     * @brief Creates a 3x3 matrix from an axis and an angle.
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

            (t.y * naxis.x) - (sina * naxis.z),
            cosa + (t.y * naxis.y),
            (t.y * naxis.z) + (sina * naxis.x),

            (t.z * naxis.x) + (sina * naxis.y),
            (t.z * naxis.y) - (sina * naxis.x),
            cosa + (t.z * naxis.z)
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
            scale.y,
            static_cast<T>(0),

            static_cast<T>(0),
            static_cast<T>(0),
            scale.z
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

    // =============== look at ===============

    // https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml

    /**
     * @brief Creates a left-handed view matrix for a given camera transformation.
     *
     * This function generates a left-handed view matrix based on the camera's eye, target, and up vectors.
     *
     * @param eye The position of the camera.
     * @param target The target point the camera is looking at.
     * @param up The up vector for the camera (default is the positive y-axis).
     * @return The left-handed view matrix.
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

        return type(x, y, z);
    }

    /**
     * @brief Creates a right-handed view matrix for a given camera transformation.
     *
     * This function generates a right-handed view matrix based on the camera's eye, target, and up vectors.
     *
     * @param eye The position of the camera.
     * @param target The target point the camera is looking at.
     * @param up The up vector for the camera (default is the positive y-axis).
     * @return The right-handed view matrix.
     */
    static inline constexpr type make_look_at_rh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        const vec3_type z(normalize(eye - target));
        const vec3_type x(normalize(cross(normalize(up), z)));
        const vec3_type y(cross(z, x));

        return type(x, y, z);
    }

    /**
     * @brief Creates a view matrix for a given camera transformation.
     *
     * This function automatically selects between left-handed and right-handed view matrix creation based on configuration.
     *
     * @param eye The position of the camera.
     * @param target The target point the camera is looking at.
     * @param up The up vector for the camera (default is the positive y-axis).
     * @return The view matrix.
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

    // =============== affine ===============

    /**
     * @brief Creates a 2D translation matrix.
     *
     * This function generates a 2D translation matrix based on the specified 2D translation vector.
     *
     * @param translation The 2D translation vector.
     * @return The translation matrix.
     */
    static inline constexpr type make_affine_translation(const vec2_type& translation)
    {
        return type(
            static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
            translation.x,	   translation.y,	  static_cast<T>(1)
        );
    }

    /**
     * @brief Retrieves the 2D translation vector from the matrix.
     *
     * This function extracts the translation vector from the matrix.
     *
     * @return The 2D translation vector.
     */
    inline constexpr vec2_type get_affine_translation() const
    {
        return vec2_type(columns[2].x, columns[2].y);
    }

    /**
     * @brief Creates a 2D rotation matrix.
     *
     * This function generates a 2D rotation matrix based on the specified angle (in radians).
     *
     * @param angle The angle of rotation in radians.
     * @return The rotation matrix.
     */
    static inline constexpr type make_affine_rotation(T angle)
    {
        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);

        return type(
             cosa,			    sina,			   static_cast<T>(0),
            -sina,			    cosa,			   static_cast<T>(0),
             static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
        );
    }

    /**
     * @brief Retrieves the 2D rotation angle from the matrix.
     *
     * This function calculates the rotation angle (in radians) from the 2D rotation matrix.
     *
     * @return The rotation angle.
     */
    inline constexpr T get_affine_rotation() const
    {
        return math::atan2(columns[0].y, columns[0].x);
    }

    /**
     * @brief Creates a 2D scaling matrix.
     *
     * This function generates a 2D scaling matrix based on the specified 2D scaling factors.
     *
     * @param scale The 2D scaling factors along the x and y axes.
     * @return The scaling matrix.
     */
    static inline constexpr type make_affine_scale(const vec2_type& scale)
    {
        return type(
            scale.x,		   static_cast<T>(0), static_cast<T>(0),
            static_cast<T>(0), scale.y,			  static_cast<T>(0),
            static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
        );
    }

    /**
     * @brief Retrieves the 2D scaling factors from the matrix.
     *
     * This function calculates the scaling factors along the x and y axes from the matrix.
     *
     * @return The 2D scaling factors.
     */
    inline constexpr vec2_type get_affine_scale() const
    {
        return vec2_type(
            math::sqrt(columns[0].x * columns[0].x + columns[0].y * columns[0].y),
            math::sqrt(columns[1].x * columns[1].x + columns[1].y * columns[1].y)
        );
    }

    /**
     * @brief Creates a 2D transformation matrix combining translation, rotation, and scaling.
     *
     * This function generates a 2D transformation matrix combining translation, rotation, and scaling.
     *
     * @param translation The 2D translation vector.
     * @param angle The angle of rotation in radians.
     * @param scale The 2D scaling factors along the x and y axes.
     * @return The combined transformation matrix.
     */
    static inline constexpr type make_affine_trs(const vec2_type& translation, T angle, vec2_type& scale)
    {
        const T cosa = math::cos(angle);
        const T sina = math::sin(angle);

        return type(
             cosa * scale.x, sina * scale.x, static_cast<T>(0),
            -sina * scale.y, cosa * scale.y, static_cast<T>(0),
             translation.x,  translation.y,  static_cast<T>(1)
        );
    }

    /**
     * @brief Computes the inverse of the 2D transformation matrix, effectively reversing applied transformations.
     *
     * This function calculates the inverse matrix for an 2D transformation matrix, providing a matrix that undoes
     * the original transformations.
     *
     * @return The inverted 2D transformation matrix.
     */
    inline constexpr type affine_invert() const
    {
        const mat2_type ibasis(mat2_type(*this).invert());

        return type(
            col_type(ibasis.columns[0],				  static_cast<T>(0)),
            col_type(ibasis.columns[1],				  static_cast<T>(0)),
            col_type(ibasis * vec2_type(-columns[2]), static_cast<T>(1))
        );
    }

    /**
     * @brief Transforms a 2D vector using the matrix.
     *
     * This function applies the matrix transformation to a 2D vector.
     *
     * @param v The 2D vector to be transformed.
     * @return The transformed 2D vector.
     */
    inline constexpr vec2_type transform(const vec2_type& v) const
    {
        return (*this) * v;
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(); }
    static inline constexpr type ZERO() { return type(static_cast<T>(0)); }

};

}

using mat3  = detail::mat<3, 3,  float>;
using mat3f = detail::mat<3, 3,  float>;
using mat3d = detail::mat<3, 3, double>;

}
}