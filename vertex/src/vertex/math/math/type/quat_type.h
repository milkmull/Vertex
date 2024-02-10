#pragma once

#include <sstream>

#include "../math.h"

namespace vx {
namespace math {
namespace detail {

// key formuals:
// q = s + v = cos(t / 2) + sin(t / 2) * v
// acos(s) * 2 = t
// 1 - (s * s) = sin2(t / 2)
// |cross(u, v)| = |u||v|sin(t)

VX_PACK_PUSH()

template <typename T>
struct quat
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using type = quat<T>;

    using size_type = length_type;
    static inline constexpr size_type size() noexcept { return static_cast<T>(4); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using vec2_type = vec<2, T, vec_t::vec, val_t::floating_point>;
    using vec3_type = vec<3, T, vec_t::vec, val_t::floating_point>;
    using vec4_type = vec<4, T, vec_t::vec, val_t::floating_point>;

    using mat2_type = mat<2, 2, T>;
    using mat3_type = mat<3, 3, T>;
    using mat4_type = mat<4, 4, T>;

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
      * @brief Explicit constructor from a 3x3 matrix.
      * @param m The 3x3 matrix to convert into a quaternion.
      *
      * This constructor creates a quaternion from a 3x3 matrix. The matrix is assumed to be a rotation matrix,
      * and the quaternion represents the rotation. The matrix is used to determine the rotation axis and angle.
      * The resulting quaternion is normalized.
      *
      * @param m The 3x3 matrix representing a rotation.
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
     * @brief Explicit constructor from a 4x4 matrix.
     *
     * This constructor creates a quaternion from a 4x4 matrix. The matrix is assumed to be a rotation matrix,
     * and the quaternion represents the rotation. The rotation is extracted from the upper-left 3x3 submatrix of the
     * 4x4 matrix, and the resulting quaternion is normalized.
     *
     * @param m The 4x4 matrix representing a rotation.
     */
    inline constexpr explicit quat(const mat4_type& m)
        : type(mat3_type(m)) {}

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
        return q * scaler;
    }

    friend inline constexpr type operator*(const type& q1, const type& q2)
    {
        return type(q1) *= q2;
    }

     /**
      * @brief Performs quaternion-vector multiplication to rotate a 3D vector.
      *
      * This function performs the multiplication of a quaternion and a 3D vector. The result is a new
      * vector representing the input vector after rotation by the quaternion.
      *
      * @param q The quaternion.
      * @param v The 3D vector to be rotated.
      * @return The resulting 3D vector after rotation.
      * 
      * @ref https://en.m.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula
      */
    friend inline constexpr vec3_type operator*(const type& q, const vec3_type& v)
    {
        const vec3_type qv(q.x, q.y, q.z);
        const vec3_type uv(cross(qv, v));
        const vec3_type uuv(cross(qv, uv));

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    
    friend inline constexpr vec3_type operator*(const vec3_type& v, const type& q)
    {
        return q.invert() * v;
    }

    // division (/)

    friend inline constexpr type operator/(const type& q, T scaler)
    {
        return type(q.w / scaler, q.x / scaler, q.y / scaler, q.z / scaler);
    }

    friend inline constexpr type operator/(T scaler, const type& q)
    {
        return scaler * q.invert();
    }

    friend inline constexpr type operator/(const type& q1, const type& q2)
    {
        return q1 * q2.invert();
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
      * @brief Multiplies this quaternion by another quaternion in-place.
      *
      * This operator performs quaternion multiplication and updates the current quaternion
      * with the result.
      *
      * @param q The quaternion to multiply with.
      * @return A reference to the updated quaternion after multiplication.
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
        return ((*this) = (*this) / q);
    }

    // =============== iterator ===============

    inline constexpr iterator begin() noexcept
    {
        return iterator(&w);
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    inline constexpr iterator end() noexcept
    {
        return iterator(&z + 1);
    }

    inline constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    inline constexpr const_iterator cbegin() const noexcept
    {
        return const_iterator(&w);
    }

    inline constexpr const_iterator cend() const noexcept
    {
        return const_iterator(&z + 1);
    }

    inline constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(&z + 1);
    }

    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    inline constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(&w);
    }

    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    inline constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(&z + 1);
    }

    inline constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(&w);
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

    /**
     * @brief Calculates the conjugate of this quaternion.
     *
     * The conjugate of a quaternion is obtained by negating the imaginary components
     * (x, y, z) while keeping the real component (w) unchanged.
     *
     * @return The conjugate of this quaternion.
     */
    inline constexpr type conjugate() const
    {
        return type(w, -x, -y, -z);
    }

    /**
     * @brief Calculates the inverse of this quaternion.
     * 
     * This function returns the inverse of this quaternion, effectively
     * reversing the encoded rotation.
     *
     * @return The inverse of this quaternion.
     */
    inline constexpr type invert() const
    {
        return conjugate() / magnitude_squared();
    }

    /**
     * @brief Retrieves the vector part of this quaternion.
     *
     * This function returns a 3D vector containing the x, y, and z components
     * of this quaternion. It discards the scalar (w) component.
     *
     * @return A 3D vector representing the vector part of this quaternion.
     */
    inline constexpr vec3_type vector() const { return vec3_type(x, y, z); }

    // =============== comparison and testing ===============

    inline constexpr void set(T nwxyz) { w = x = y = z = nwxyz; }
    inline constexpr void set(T nw, T nx, T ny, T nz) { w = nw;  x = nx; y = ny; z = nz; }

    /**
     * @brief Get the minimum component value of the quaternion.
     *
     * This function returns the minimum value between the components of the quaternion.
     *
     * @return The minimum component value.
     */
    inline constexpr T min() const { return math::min({ w, x, y, z }); }

    /**
     * @brief Get the maximum component value of the quaternion.
     *
     * This function returns the maximum value between the components of the quaternion.
     *
     * @return The maximum component value.
     */
    inline constexpr T max() const { return math::max({ w, x, y, z }); }

    // =============== magnitude ===============

    /**
     * @brief Calculates the squared magnitude of the quaternion.
     *
     * This function computes the squared magnitude of the quaternion.
     *
     * @return The squared length of the quaternion.
     */
    inline constexpr T magnitude_squared() const { return (w * w) + (x * x) + (y * y) + (z * z); }

    /**
     * @brief Calculates the magnitude of the quaternion.
     *
     * This function computes the magnitude of the quaternion.
     *
     * @return The magnitude of the quaternion.
     */
    inline constexpr T magnitude() const { return math::sqrt((w * w) + (x * x) + (y * y) + (z * z)); }

    /**
     * @brief Normalizes the quaternion.
     *
     * This function normalizes the quaternion.
     *
     * @return The normalized quaternion. If the length of the quaternion is 0,
     * a zero quaternion will be returned.
     */
    inline constexpr type normalize() const
    {
        const T magsq = magnitude_squared();
        if (magsq < math::epsilon<T>)
            return type();
        return (*this) * math::inverse_sqrt(magsq);
    }

    // =============== direction and orientation ===============

    /**
     * @brief Creates a quaternion representing the rotation from one vector to another.
     *
     * Given two normalized 3D vectors 'from' and 'to', this function calculates a
     * quaternion that represents the rotation needed to align the 'from' vector with
     * the 'to' vector.
     *
     * If the vectors are nearly parallel, it returns the identity quaternion (no rotation).
     * If the vectors are nearly antiparallel, it chooses an arbitrary axis and rotates by pi radians.
     * Otherwise, it calculates the rotation axis and angle to create the quaternion.
     *
     * @param from The normalized starting vector.
     * @param to The normalized target vector.
     * @return The quaternion representing the rotation from the 'from' vector to the 'to' vector.
     * 
     * @ref https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/quaternion.inl
     * @ref https://www.cesarkallas.net/arquivos/livros/informatica/game/Game%20Programming%20Gems%201.pdf (page 215 (pdf page 211))
     */
    static inline constexpr type from_to(const vec3_type& from, const vec3_type& to)
    {
        const vec3_type fn = math::normalize(from);
        const vec3_type tn = math::normalize(to);

        const T cosa = dot(fn, tn);

        if (cosa > static_cast<T>(1) - math::epsilon<T>)
        {
            // In this case, both vectors are pointing in the
            // same direction. We can return the identity
            // quaternion (no rotation).
            return type(1, 0, 0, 0);
        }

        const vec3_type axis = math::normalize(math::cross(fn, tn));

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

    /**
     * @brief Creates a quaternion representing a left-handed look-at rotation.
     *
     * Given an 'eye' position, a 'target' position, and an optional 'up' vector,
     * this function computes a quaternion representing a left-handed look-at rotation.
     *
     * @param eye The position of the viewer's eye.
     * @param target The position the viewer is looking at.
     * @param up The up vector (default is positive y-axis).
     * @return The quaternion representing the left-handed look-at rotation.
     */
    static inline constexpr type make_look_at_rotation_lh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        return type(mat3_type::make_look_at_lh(eye, target, up));
    }

    /**
     * @brief Creates a quaternion representing a right-handed look-at rotation.
     *
     * Given an 'eye' position, a 'target' position, and an optional 'up' vector,
     * this function computes a quaternion representing a right-handed look-at rotation.
     *
     * @param eye The position of the viewer's eye.
     * @param target The position the viewer is looking at.
     * @param up The up vector (default is positive y-axis).
     * @return The quaternion representing the right-handed look-at rotation.
     */
    static inline constexpr type make_look_at_rotation_rh(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
        return type(mat3_type::make_look_at_rh(eye, target, up));
    }

    /**
     * @brief Creates a quaternion representing a look-at rotation.
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
    static inline constexpr type make_look_at_rotation(
        const vec3_type& eye,
        const vec3_type& target,
        const vec3_type& up = vec3_type::UP()
    )
    {
#	if VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT
        return make_look_at_rotation_lh(eye, target, up);
#	else
        return make_look_at_rotation_rh(eye, target, up);
#	endif
    }

    // =============== axis angle ===============

    /**
     * @brief Retrieves the rotation axis of this quaternion.
     *
     * This function calculates and returns the 3D vector representing the rotation axis
     * of this normalized quaternion. If the quaternion represents no rotation (angle is 0),
     * it returns the positive y-axis.
     *
     * @return The rotation axis of this quaternion.
     * 
     * @ref https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
     * @ref https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions
     */
    inline constexpr vec3_type axis() const
    {
        const T nw = normalize().w;
        const T s2 = static_cast<T>(1) - (nw * nw);

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

    /**
     * @brief Retrieves the rotation angle of this quaternion.
     *
     * This function calculates and returns the rotation angle (in radians) represented
     * by this normalized quaternion. The angle is twice the arccosine of the scalar (w)
     * component of the quaternion.
     *
     * @return The rotation angle of this quaternion in radians.
     */
    inline constexpr T angle() const
    {
        return static_cast<T>(2) * math::acos_clamped(normalize().w);
    }

    /**
     * @brief Creates a quaternion from an axis and an angle.
     *
     * Given a normalized 3D vector 'axis' and an angle in radians, this function
     * computes a quaternion representing the rotation around the specified axis.
     *
     * @param axis The normalized rotation axis.
     * @param angle The rotation angle in radians.
     * @return The quaternion representing the rotation around the specified axis.
     */
    static inline constexpr type from_axis_angle(const vec3_type& axis, T angle)
    {
        const vec3_type naxis = math::normalize(axis);

        const T sina2 = math::sin(angle * static_cast<T>(0.5));
        const T cosa2 = math::cos(angle * static_cast<T>(0.5));

        return type(cosa2, naxis * sina2);
    }

    // =============== vector transformations ===============

    /**
     * @brief Rotates a 3D vector using this quaternion.
     *
     * Given a 3D vector 'v', this function rotates the vector using this quaternion.
     * The rotation is performed by quaternion multiplication.
     *
     * @param v The 3D vector to be rotated.
     * @return The rotated 3D vector.
     */
    inline constexpr vec3_type rotate(const vec3_type& v) const
    {
        return (*this) * v;
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(); }
    static inline constexpr type ZERO() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }

};

VX_PACK_POP()

}

using quat  = detail::quat< float>;
using quatf = detail::quat< float>;
using quatd = detail::quat<double>;

}
}