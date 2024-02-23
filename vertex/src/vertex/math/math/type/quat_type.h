#pragma once

#include <sstream>

#include "../math.h"

namespace vx {
namespace math {

// key formuals:
// q = s + v = cos(t / 2) + sin(t / 2) * v
// acos(s) * 2 = t
// 1 - (s * s) = sin2(t / 2)
// |cross(u, v)| = |u||v|sin(t)

VX_PACK_PUSH()

template <typename T>
struct quat_t
{
    static_assert(std::is_floating_point<T>::value, "type T must be floating point type");

    // =============== meta ===============

    using value_type = T;
    using type = quat_t<T>;

    using size_type = math::size_type;
    static inline constexpr size_type size() noexcept { return static_cast<size_type>(4); }

    using iterator = ::vx::detail::iterator<T>;
    using const_iterator = ::vx::detail::iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // =============== data ===============

    T w, x, y, z;

    // =============== implicit constructors ===============

    inline constexpr quat_t() noexcept
        : w(static_cast<T>(1))
        , x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    inline constexpr quat_t(const type& q) noexcept
        : w(q.w), x(q.x), y(q.y), z(q.z) {}

    inline constexpr quat_t(type&&) noexcept = default;

    // =============== explicit constructors ===============

    inline constexpr quat_t(T w, const vec<3, T>& v) noexcept
        : w(w), x(v.x), y(v.y), z(v.z) {}

    inline constexpr quat_t(T w, T x, T y, T z) noexcept
        : w(w), x(x), y(y), z(z) {}

    // =============== conversion constructors ===============
    
    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr quat_t(U w, const vec<3, U>& v)
        : w(static_cast<T>(w))
        , x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    template <typename U, typename std::enable_if<std::is_arithmetic<U>::value, bool>::type = true>
    inline constexpr quat_t(U w, U x, U y, U z)
        : w(static_cast<T>(w))
        , x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename U>
    inline constexpr explicit quat_t(const quat_t<U>& q)
        : w(static_cast<T>(q.w))
        , x(static_cast<T>(q.x))
        , y(static_cast<T>(q.y))
        , z(static_cast<T>(q.z)) {}

    template <typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
    inline constexpr explicit quat_t(const vec<4, U>& v)
        : w(static_cast<T>(v.w))
        , x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    // =============== destructor ===============

    ~quat_t() noexcept = default;

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
        return type(+w, +x, +y, +z);
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
    friend inline constexpr vec<3, T> operator*(const type& q, const vec<3, T>& v)
    {
        const vec<3, T> qv = q.vector();
        const vec<3, T> uv = math::cross(qv, v);
        const vec<3, T> uuv = math::cross(qv, uv);

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    
    friend inline constexpr vec<3, T> operator*(const vec<3, T>& v, const type& q)
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
        const type p = *this;

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

    inline constexpr std::string to_string() const
    {
        std::ostringstream oss;
        oss << "{ " << +w << ", " << +x << ", " << +y << ", " << +z << " }";
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
    inline constexpr vec<3, T> vector() const { return vec<3, T>(x, y, z); }

    // =============== comparison and testing ===============

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
    inline constexpr T magnitude_squared() const
    {
        return (w * w) + (x * x) + (y * y) + (z * z);
    }

    /**
     * @brief Calculates the magnitude of the quaternion.
     *
     * This function computes the magnitude of the quaternion.
     *
     * @return The magnitude of the quaternion.
     */
    inline constexpr T magnitude() const
    {
        return math::sqrt(magnitude_squared());
    }

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

        if VX_UNLIKELY(magsq < math::epsilon<T>)
        {
            return quat_t<T>();
        }

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
    static inline constexpr type from_to(const vec<3, T>& from, const vec<3, T>& to)
    {
        const vec<3, T> fn = math::normalize(from);
        const vec<3, T> tn = math::normalize(to);

        const T cosa = math::dot(fn, tn);

        if (cosa > static_cast<T>(1) - math::epsilon<T>)
        {
            // In this case, both vectors are pointing in the
            // same direction. We can return the identity
            // quaternion (no rotation).
            return type(1, 0, 0, 0);
        }

        const vec<3, T> axis = math::normalize(math::cross(fn, tn));

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

    // =============== axis angle ===============

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
    static inline constexpr type from_axis_angle(const vec<3, T>& axis, T angle)
    {
        const vec<3, T> naxis = math::normalize(axis);

        const T sina2 = math::sin(angle * static_cast<T>(0.5));
        const T cosa2 = math::cos(angle * static_cast<T>(0.5));

        return type(cosa2, naxis * sina2);
    }

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
    inline constexpr vec<3, T> axis() const
    {
        const T nw = normalize().w;
        const T s2 = static_cast<T>(1) - (nw * nw);

        if (s2 < math::epsilon<T>)
        {
            // This indicates that the angle is 0 degrees and thus,
            // the axis does not matter. We choose the +y axis.
            return vec<3, T>::UP();
        }

        const T invs = math::inverse_sqrt(s2);
        return vec<3, T>(
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

    // =============== euler angles ===============

    static inline constexpr type from_euler_angles(T x, T y, T z)
    {
        x *= static_cast<T>(0.5);
        y *= static_cast<T>(0.5);
        z *= static_cast<T>(0.5);

        const T cx = math::cos(x);
        const T cy = math::cos(y);
        const T cz = math::cos(z);

        const T sx = math::sin(x);
        const T sy = math::sin(y);
        const T sz = math::sin(z);

        return type(
            -(sx * sy * sz) + (cx * cy * cz),
            +(sx * cy * cz) + (sy * sz * cx),
            -(sx * sz * cy) + (sy * cx * cz),
            +(sx * sy * cz) + (sz * cx * cy)
        );
    }

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_(in_3-2-1_sequence)_conversion

    inline constexpr vec<3, T> to_euler_angles() const
    {
        const type qn = normalize();

        const T qxy = qn.x * qn.y;
        const T qwz = qn.w * qn.z;

        const T test = qxy + qwz;

        if (test > static_cast<T>(0.5) - math::epsilon<T>)
        {
            // singularity at north pole
            return vec<3, T>(
                static_cast<T>(2) * math::atan2(qn.x, qn.w),
                math::half_pi<T>,
                static_cast<T>(0)
            );
        }
        if (test < static_cast<T>(-0.5) + math::epsilon<T>)
        {
            // singularity at south pole
            return vec<3, T>(
                static_cast<T>(-2) * math::atan2(qn.x, qn.w),
                -math::half_pi<T>,
                static_cast<T>(0)
            );
        }

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qww = qn.w * qn.w;
        const T qxz = qn.x * qn.z;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;

        return vec<3, T>(
            math::atan2(static_cast<T>(2) * (qwy - qxz), qxx - qyy - qzz + qww),
            math::asin (static_cast<T>(2) * test),
            math::atan2(static_cast<T>(2) * (qwx - qyz), qyy - qxx - qzz + qww)
        );
    }

    // =============== matrix conversions ===============

    static inline constexpr type from_mat3(const mat<3, 3, T>& m)
    {
        const T trace = m.columns[0].x + m.columns[1].y + m.columns[2].z;

        if (trace > static_cast<T>(0))
        {
            const T s = math::sqrt(static_cast<T>(1) + trace) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return type(
                s * static_cast<T>(0.25),
                (m.columns[2].y - m.columns[1].z) * invs,
                (m.columns[0].z - m.columns[2].x) * invs,
                (m.columns[1].x - m.columns[0].y) * invs
            );
        }
        else if (m.columns[0].x > m.columns[1].y && m.columns[0].x > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[0].x - m.columns[1].y - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return type(
                (m.columns[2].y - m.columns[1].z) * invs,
                s * static_cast<T>(0.25),
                (m.columns[0].y + m.columns[1].x) * invs,
                (m.columns[0].z + m.columns[2].x) * invs
            );
        }
        else if (m.columns[1].y > m.columns[2].z)
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[1].y - m.columns[0].x - m.columns[2].z) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return type(
                (m.columns[2].x - m.columns[0].z) * invs,
                (m.columns[0].y + m.columns[1].x) * invs,
                s * static_cast<T>(0.25),
                (m.columns[1].z + m.columns[2].y) * invs
            );
        }
        else
        {
            const T s = math::sqrt(static_cast<T>(1) + m.columns[2].z - m.columns[0].x - m.columns[1].y) * static_cast<T>(2);
            const T invs = static_cast<T>(1) / s;

            return type(
                (m.columns[1].x - m.columns[0].y) * invs,
                (m.columns[0].z + m.columns[2].x) * invs,
                (m.columns[1].z + m.columns[2].y) * invs,
                s * static_cast<T>(0.25)
            );
        }
    }

    inline constexpr mat<3, 3, T> to_mat3() const
    {
        const type qn = normalize();

        const T qxx = qn.x * qn.x;
        const T qyy = qn.y * qn.y;
        const T qzz = qn.z * qn.z;
        const T qxz = qn.x * qn.z;
        const T qxy = qn.x * qn.y;
        const T qyz = qn.y * qn.z;
        const T qwx = qn.w * qn.x;
        const T qwy = qn.w * qn.y;
        const T qwz = qn.w * qn.z;

        return mat<3, 3, T>(
            static_cast<T>(1) - static_cast<T>(2) * (qyy + qzz),
            static_cast<T>(2) * (qxy + qwz),
            static_cast<T>(2) * (qxz - qwy),

            static_cast<T>(2) * (qxy - qwz),
            static_cast<T>(1) - static_cast<T>(2) * (qxx + qzz),
            static_cast<T>(2) * (qyz + qwx),

            static_cast<T>(2) * (qxz + qwy),
            static_cast<T>(2) * (qyz - qwx),
            static_cast<T>(1) - static_cast<T>(2) * (qxx + qyy)
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
    static inline constexpr type make_look_at_lh(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
        const vec<3, T> z = math::normalize(target - eye);
        const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
        const vec<3, T> y = math::cross(z, x);

        return from_mat3(mat<3, 3, T>(x, y, z));
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
    static inline constexpr type make_look_at_rh(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
        const vec<3, T> z = math::normalize(eye - target);
        const vec<3, T> x = math::normalize(math::cross(math::normalize(up), z));
        const vec<3, T> y = math::cross(z, x);

        return from_mat3(mat<3, 3, T>(x, y, z));
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
    static inline constexpr type make_look_at(
        const vec<3, T>& eye,
        const vec<3, T>& target,
        const vec<3, T>& up = vec<3, T>::UP()
    )
    {
#	if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
        return make_look_at_rotation_lh(eye, target, up);
#	else
        return make_look_at_rotation_rh(eye, target, up);
#	endif
    }

    // =============== constants ===============

    static inline constexpr type IDENTITY() { return type(1, 0, 0, 0); }
    static inline constexpr type ZERO() { return type(0, 0, 0, 0); }

};

VX_PACK_POP()

using quat  = quat_t<float>;
using quatd = quat_t<double>;

}
}