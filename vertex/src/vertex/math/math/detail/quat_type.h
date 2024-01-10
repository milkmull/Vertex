#pragma once

#include <sstream>

#include "../fn_common.h"
#include "../vec_fn_geometric.h"

namespace vx {
namespace math {
namespace detail {

// key formuals:
// q = s + v = cos(t / 2) + sin(t / 2) * v
// acos(s) * 2 = t
// 1 - (s * s) = sin2(t / 2)
// |cross(u, v)| = |u||v|sin(t)

template <typename T>
struct quat
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using type = quat<T>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::floating_point>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::floating_point>;

    using mat2_type = mat<2, 2, T>;
    using mat3_type = mat<3, 3, T>;
    using mat4_type = mat<4, 4, T>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(4); }

    // =============== data ===============

    T w, x, y, z;

    // =============== implicit constructors ===============

    inline constexpr quat() noexcept
        : w(static_cast<T>(1))
        , x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    inline constexpr quat(const type& q) noexcept
        : w(q.w), x(q.x), y(q.y), z(q.z) {}

    inline constexpr quat(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr quat(T w, const vec3_type& v) noexcept
        : w(w), x(v.x), y(v.y), z(v.z) {}

    inline constexpr quat(T w, T x, T y, T z) noexcept
        : w(w), x(x), y(y), z(z) {}

    // =============== conversion constructors ===============

    template <typename U>
    inline constexpr explicit quat(const quat<U>& q)
        : w(static_cast<T>(q.w))
        , x(static_cast<T>(q.x))
        , y(static_cast<T>(q.y))
        , z(static_cast<T>(q.z)) {}

    template <typename U>
    inline constexpr explicit quat(const vecf<4, U>& v)
        : w(static_cast<T>(v.w))
        , x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    // =============== conversion matrix constructors ===============

    /**
     * @brief Constructs a quaternion from a 3x3 matrix.
     *
     * This constructor initializes a quaternion from the rotational part of the specified 3x3 matrix @p m.
     * The matrix is assumed to be a rotation matrix, and the quaternion represents the same rotation.
     *
     * @param[in] m The 3x3 matrix containing the rotational part.
     *
     * @ref https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
     */
    inline constexpr explicit quat(const mat3_type& m)
    {
        const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

        if (trace > static_cast<T>(0))
        {
            const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            w = s * static_cast<T>(0.25);
            x = (m.columns[2].y - m.columns[1].z) * invs;
            y = (m.columns[0].z - m.columns[2].x) * invs;
            z = (m.columns[1].x - m.columns[0].y) * invs;
        }
        else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            w = (m.columns[2].y - m.columns[1].z) * invs;
            x = s * static_cast<T>(0.25);
            y = (m.columns[0].y + m.columns[1].x) * invs;
            z = (m.columns[0].z + m.columns[2].x) * invs;
        }
        else if (m.columns[1].y > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            w = (m.columns[2].x - m.columns[0].z) * invs;
            x = (m.columns[0].y + m.columns[1].x) * invs;
            y = s * static_cast<T>(0.25);
            z = (m.columns[1].z + m.columns[2].y) * invs;
        }
        else
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            w = (m.columns[1].x - m.columns[0].y) * invs;
            x = (m.columns[0].z + m.columns[2].x) * invs;
            y = (m.columns[1].z + m.columns[2].y) * invs;
            z = s * static_cast<T>(0.25);
        }
    }

    /**
     * @brief Constructs a quaternion from a 4x4 matrix.
     *
     * This constructor initializes a quaternion from the rotational part of the specified 4x4 matrix @p m.
     * The matrix is assumed to be a rotation matrix, and the quaternion represents the same rotation.
     *
     * @param[in] m The 4x4 matrix containing the rotational part.
     */
    inline constexpr explicit quat(const mat4_type& m)
        : quat(mat3_type(m)) {}

    // =============== destructor ===============

    ~quat() noexcept = default;

    // =============== assignment operators ===============

    inline constexpr type& operator=(const type& q) noexcept
    {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        return *this;
    }

    inline constexpr type& operator=(type&&) noexcept = default;

    // =============== accessors ===============

    inline constexpr T& operator[](size_type i)
    {
        assert(i < 4);
        return (&w)[i];
    }

    inline constexpr const T& operator[](size_type i) const
    {
        assert(i < 4);
        return (&w)[i];
    }

    // =============== boolean operators ===============

    friend inline constexpr bool operator==(const type& q1, const type& q2)
    {
        return q1.w == q2.w && q1.x == q2.x && q1.y == q2.y && q1.z == q2.z;
    }

    friend inline constexpr bool operator!=(const type& q1, const type& q2)
    {
        return !(q1 == q2);
    }

    // =============== unary constant operators ===============

    inline constexpr type operator+() const
    {
        return *this;
    }

    inline constexpr type operator-() const
    {
        return type(-w, -x, -y, -z);
    }

    // =============== binary arithmetic operators ===============

    // addition (+)

    friend inline constexpr type operator+(const type& q1, const type& q2)
    {
        return type(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
    }

    // subtraction (-)

    friend inline constexpr type operator-(const type& q1, const type& q2)
    {
        return type(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
    }

    // multiplication (*)

    friend inline constexpr type operator*(const type& q, T scaler)
    {
        return type(q.w * scaler, q.x * scaler, q.y * scaler, q.z * scaler);
    }

    friend inline constexpr type operator*(T scaler, const type& q)
    {
        return type(scaler * q.w, scaler * q.x, scaler * q.y, scaler * q.z);
    }

    friend inline constexpr type operator*(const type& q1, const type& q2)
    {
        return type(q1) *= q2;
    }

    /**
     * @brief Performs quaternion-vector multiplication to rotate a 3D vector.
     *
     * This function represents the operation of rotating a 3D vector @p v by the quaternion @p q.
     * The resulting vector is obtained by applying the quaternion rotation to the original 3D vector.
     *
     * @param[in] q The quaternion representing the rotation.
     * @param[in] v The 3D vector to be rotated.
     * @return The rotated 3D vector after quaternion multiplication.
     *
     * https://en.m.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula
     */
    friend inline constexpr vec3_type operator*(const type& q, const vec3_type& v)
    {
        const vec3_type qv(q.x, q.y, q.z);
        const vec3_type uv(cross(qv, v));
        const vec3_type uuv(cross(qv, uv));

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    /**
     * @brief Performs quaternion-vector multiplication to rotate a 3D vector.
     *
     * This function represents the operation of rotating a 3D vector @p v by the invert of the quaternion @p q.
     * The resulting vector is obtained by applying the invert quaternion rotation to the original 3D vector.
     *
     * @param[in] v The 3D vector to be rotated.
     * @param[in] q The quaternion representing the invert rotation.
     * @return The rotated 3D vector after invert quaternion multiplication.
     */
    friend inline constexpr vec3_type operator*(const vec3_type& v, const type& q)
    {
        return q.invert() * v;
    }

    // division (/)

    friend inline constexpr type operator/(const type& q, T scaler)
    {
        return type(q.w / scaler, q.x / scaler, q.y / scaler, q.z / scaler);
    }

    // =============== unary arithmetic operators ===============

    // addition (+=)

    inline constexpr type& operator+=(const type& q)
    {
        w += q.w;
        x += q.x;
        y += q.y;
        z += q.z;
        return *this;
    }

    // subtraction (-=)

    inline constexpr type& operator-=(const type& q)
    {
        w -= q.w;
        x -= q.x;
        y -= q.y;
        z -= q.z;
        return *this;
    }

    // multiplication (*=)

    inline constexpr type& operator*=(T scaler)
    {
        w *= scaler;
        x *= scaler;
        y *= scaler;
        z *= scaler;
        return *this;
    }

    /**
     * @brief Multiplies this quaternion by another quaternion in place.
     *
     * This method performs quaternion multiplication in place by multiplying this quaternion
     * by the quaternion @p q. The result is stored in this quaternion.
     *
     * @param[in] q The quaternion to multiply with.
     * @return A reference to this quaternion after multiplication.
     *
     * @note Quaternion multiplication is used to compose multiple rotations together.
     * The operation follows the Hamilton product formula and updates the components of this quaternion
     * based on the components of the current quaternion and the specified quaternion.
     *
     * @ref https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm
     */
    inline constexpr type& operator*=(const type& q)
    {
        const type p(*this);

        w = (p.w * q.w) - (p.x * q.x) - (p.y * q.y) - (p.z * q.z);
        x = (p.w * q.x) + (p.x * q.w) + (p.y * q.z) - (p.z * q.y);
        y = (p.w * q.y) + (p.y * q.w) + (p.z * q.x) - (p.x * q.z);
        z = (p.w * q.z) + (p.z * q.w) + (p.x * q.y) - (p.y * q.x);
        return *this;
    }

    // division (/=)

    inline constexpr type& operator/=(T scaler)
    {
        w /= scaler;
        x /= scaler;
        y /= scaler;
        z /= scaler;
        return *this;
    }

    inline constexpr type& operator/=(const type& q)
    {
        w /= q.w;
        x /= q.x;
        y /= q.y;
        z /= q.z;
        return *this;
    }

    // =============== iterator ===============

    inline T* begin() noexcept
    {
        return &w;
    }

    inline const T* begin() const noexcept
    {
        return cbegin();
    }

    inline T* end() noexcept
    {
        return &z + 1;
    }

    inline const T* end() const noexcept
    {
        return cend();
    }

    inline const T* cbegin() const noexcept
    {
        return &w;
    }

    inline const T* cend() const noexcept
    {
        return &z + 1;
    }

    inline auto rbegin() noexcept
    {
        return std::reverse_iterator<T*>(&z + 1);
    }

    inline const auto rbegin() const noexcept
    {
        return crbegin();
    }

    inline auto rend() noexcept
    {
        return std::reverse_iterator<T*>(&w);
    }

    inline const auto rend() const noexcept
    {
        return crend();
    }

    inline auto crbegin() const noexcept
    {
        return std::reverse_iterator<const T*>(&z + 1);
    }

    inline auto crend() const noexcept
    {
        return std::reverse_iterator<const T*>(&w);
    }

    // =============== string ===============

    inline constexpr std::string to_string(bool pretty_print = false) const
    {
        if (pretty_print)
        {
            return type(
                math::make_pretty(w),
                math::make_pretty(x),
                math::make_pretty(y),
                math::make_pretty(z)
            ).to_string();
        }

        std::ostringstream oss;
        oss << "{ " << w << ", " << x << ", " << y << ", " << z << " }";
        return oss.str();
    }

    // =============== operations ===============

    inline constexpr type conjugate() const
    {
        return type(w, -x, -y, -z);
    }

    inline constexpr type invert() const
    {
        return conjugate() / magnitude_squared();
    }

    inline constexpr vec3_type vec() const { return vec3_type(x, y, z); }

    // =============== comparison and testing ===============

    inline constexpr void set(T nwxyz) { w = x = y = z = nwxyz; }
    inline constexpr void set(T nw, T nx, T ny, T nz) { w = nw;  x = nx; y = ny; z = nz; }

    inline constexpr T min() const { return math::min({ w, x, y, z }); }
    inline constexpr T max() const { return math::max({ w, x, y, z }); }

    // =============== magnitude ===============

    inline constexpr T magnitude_squared() const { return (w * w) + (x * x) + (y * y) + (z * z); }
    inline constexpr T magnitude() const { return math::sqrt((w * w) + (x * x) + (y * y) + (z * z)); }

    inline constexpr type normalize() const
    {
        const T magsq = magnitude_squared();
        if (magsq < math::epsilon<T>) return type();
        return (*this) * math::inverse_sqrt(magsq);
    }

    // =============== direction and orientation ===============

    // https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/quaternion.inl
    // https://www.cesarkallas.net/arquivos/livros/informatica/game/Game%20Programming%20Gems%201.pdf (page 215 (pdf page 211))

    static inline constexpr type from_to(const vec3_type& from, const vec3_type& to)
    {
        const vec3_type fn = from.normalize();
        const vec3_type tn = to.normalize();

        const T cosa = dot(fn, tn);

        if (cosa > static_cast<T>(1) - math::epsilon<T>)
        {
            // In this case, both vectors are pointing in the
            // same direction. We can return the identity
            // quaternion (no rotation).
            return type(1, 0, 0, 0);
        }

        const vec3_type axis = math::cross(fn, tn).normalize();

        if (cosa < static_cast<T>(-1) + math::epsilon<T>)
        {
            // In this case, the vectors are pointing in opposite
            // directions. There is no ideal axis to rotate around,
            // so we choose one. In this case we just use the cross
            // product of the 2 input vectors and a rotation angle
            // of pi.
            return from_axis_angle(axis, math::pi<T>);
        }

        const T s = math::sqrt((static_cast<T>(1) + cosa) * static_cast<T>(2));
        const T invs = static_cast<T>(1) / s;

        return type(
            s * static_cast<T>(0.5),
            axis.x * invs,
            axis.y * invs,
            axis.z * invs
        );
    }

    // =============== look at ===============

    static inline constexpr type make_look_at_rotation_lh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        return type(mat3::make_look_at_lh(eye, target, up));
    }

    static inline constexpr type make_look_at_rotation_rh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        return type(mat3::make_look_at_rh(eye, target, up));
    }

    static inline constexpr type make_look_at_rotation(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return make_look_at_rotation_lh(eye, target, up);
#	else
        return make_look_at_rotation_rh(eye, target, up);
#	endif
    }

    // =============== axis angle ===============

    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
    // https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions

    inline constexpr vec3_type axis() const
    {
        const type qn(normalize());
        const T s2 = static_cast<T>(1) - (qn.w * qn.w);

        if (s2 < math::epsilon<T>)
        {
            // This indicates that the angle is 0 degrees and thus,
            // the axis does not matter. We choose the +y axis.
            return vec3_type::UP();
        }

        const T invs = math::inverse_sqrt(s2);
        return vec3_type(
            x * invs,
            y * invs,
            z * invs
        );
    }

    inline constexpr T angle() const
    {
        return static_cast<T>(2) * math::acos_clamped(normalize().w);
    }

    static inline constexpr type from_axis_angle(const vec3_type& axis, T angle)
    {
        const vec3_type naxis = axis.normalize();

        const T sina2 = math::sin(angle * static_cast<T>(0.5));
        const T cosa2 = math::cos(angle * static_cast<T>(0.5));

        return type(cosa2, naxis * sina2);
    }

    // =============== vector transformations ===============

    inline constexpr vec3_type rotate(const vec3_type& v) const
    {
        return (*this) * v;
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(); }

};

}
}
}