#pragma once

#include "../../_priv/common.h"

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

    ///////////////////////////////////////////////////////////////////////////////
    // meta
    ///////////////////////////////////////////////////////////////////////////////

    using scaler_type = T;
    using type = quat_t<T>;

    static VX_FORCE_INLINE constexpr size_t size() noexcept { return static_cast<size_t>(4); }

    ///////////////////////////////////////////////////////////////////////////////
    // data
    ///////////////////////////////////////////////////////////////////////////////

    T w, x, y, z;

    ///////////////////////////////////////////////////////////////////////////////
    // implicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr quat_t() noexcept
        : w(static_cast<T>(1))
        , x(static_cast<T>(0))
        , y(static_cast<T>(0))
        , z(static_cast<T>(0)) {}

    VX_FORCE_INLINE constexpr quat_t(const type& q) noexcept
        : w(q.w), x(q.x), y(q.y), z(q.z) {}

    VX_FORCE_INLINE constexpr quat_t(type&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // explicit constructors
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr quat_t(T w, const vec<3, T>& v) noexcept
        : w(w), x(v.x), y(v.y), z(v.z) {}

    VX_FORCE_INLINE constexpr quat_t(T w, T x, T y, T z) noexcept
        : w(w), x(x), y(y), z(z) {}

    ///////////////////////////////////////////////////////////////////////////////
    // conversion constructors
    ///////////////////////////////////////////////////////////////////////////////
    
    template <typename W, typename XYZ, typename std::enable_if<std::is_arithmetic<W>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr quat_t(W w, const vec<3, XYZ>& v)
        : w(static_cast<T>(w))
        , x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    template <typename W, typename X, typename Y, typename Z, typename std::enable_if<
        std::is_arithmetic<W>::value &&
        std::is_arithmetic<X>::value &&
        std::is_arithmetic<Y>::value &&
        std::is_arithmetic<Z>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr quat_t(W w, X x, Y y, Z z)
        : w(static_cast<T>(w))
        , x(static_cast<T>(x))
        , y(static_cast<T>(y))
        , z(static_cast<T>(z)) {}

    template <typename U>
    VX_FORCE_INLINE constexpr explicit quat_t(const quat_t<U>& q)
        : w(static_cast<T>(q.w))
        , x(static_cast<T>(q.x))
        , y(static_cast<T>(q.y))
        , z(static_cast<T>(q.z)) {}

    template <typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit quat_t(const vec<4, U>& v)
        : w(static_cast<T>(v.w))
        , x(static_cast<T>(v.x))
        , y(static_cast<T>(v.y))
        , z(static_cast<T>(v.z)) {}

    ///////////////////////////////////////////////////////////////////////////////
    // destructor
    ///////////////////////////////////////////////////////////////////////////////

    ~quat_t() noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////
    // assignment operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type& operator=(const type& q) noexcept
    {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator=(type&&) noexcept = default;

    template <typename U>
    VX_FORCE_INLINE constexpr type& operator=(const quat_t<U>& q)
    {
        w = static_cast<T>(q.w);
        x = static_cast<T>(q.x);
        y = static_cast<T>(q.y);
        z = static_cast<T>(q.z);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // index operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr T& operator[](size_t i)
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
    }

    VX_FORCE_INLINE constexpr const T& operator[](size_t i) const
    {
        assert(i < 4);

        switch (i)
        {
            default:
            case 0: return w;
            case 1: return x;
            case 2: return y;
            case 3: return z;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // conversion operators
    ///////////////////////////////////////////////////////////////////////////////

    template <typename U, typename std::enable_if<type_traits::is_floating_point<U>::value, bool>::type = true>
    VX_FORCE_INLINE constexpr explicit operator vec<4, U>() const
    {
        return vec<4, U>(
            static_cast<U>(x),
            static_cast<U>(y),
            static_cast<U>(z),
            static_cast<U>(w)
        );
    }

    ///////////////////////////////////////////////////////////////////////////////
    // comparison operators
    ///////////////////////////////////////////////////////////////////////////////

    friend VX_FORCE_INLINE constexpr bool operator==(const type& q1, const type& q2)
    {
        return q1.w == q2.w && q1.x == q2.x && q1.y == q2.y && q1.z == q2.z;
    }

    friend VX_FORCE_INLINE constexpr bool operator!=(const type& q1, const type& q2)
    {
        return !(q1 == q2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary const operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_FORCE_INLINE constexpr type operator+() const
    {
        return type(+w, +x, +y, +z);
    }

    VX_FORCE_INLINE constexpr type operator-() const
    {
        return type(-w, -x, -y, -z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // binary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+)

    friend VX_FORCE_INLINE constexpr type operator+(const type& q1, const type& q2)
    {
        return type(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
    }

    // subtraction (-)

    friend VX_FORCE_INLINE constexpr type operator-(const type& q1, const type& q2)
    {
        return type(q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z);
    }

    // multiplication (*)

    friend VX_FORCE_INLINE constexpr type operator*(const type& q, T scaler)
    {
        return type(q.w * scaler, q.x * scaler, q.y * scaler, q.z * scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator*(T scaler, const type& q)
    {
        return type(scaler * q.w, scaler * q.x, scaler * q.y, scaler * q.z);
    }

    // https://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/arithmetic/index.htm

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Multiplies this quaternion by another quaternion in-place.
    /// 
    /// Quaternion multiplication is used to compose multiple rotations together.
    /// The operation follows the Hamilton product formula and updates the components
    /// of this quaternion based on the components of the current quaternion and the
    /// specified quaternion.
    ///
    /// @param q The quaternion to multiply with.
    /// 
    /// @return A reference to the updated quaternion after multiplication.
    ///////////////////////////////////////////////////////////////////////////////
    friend VX_FORCE_INLINE constexpr type operator*(const type& q1, const type& q2)
    {
        return type(
            (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z),
            (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
            (q1.w * q2.y) + (q1.y * q2.w) + (q1.z * q2.x) - (q1.x * q2.z),
            (q1.w * q2.z) + (q1.z * q2.w) + (q1.x * q2.y) - (q1.y * q2.x)
        );
    }

    // https://en.m.wikipedia.org/wiki/Euler%E2%80%93Rodrigues_formula

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Performs quaternion-vector multiplication to rotate a 3D vector.
    ///
    /// The result is a new vector representing the input vector after rotation by
    /// the quaternion.
    ///
    /// @param q The quaternion.
    /// @param v The 3D vector to be rotated.
    /// 
    /// @return The resulting 3D vector after rotation.
    ///////////////////////////////////////////////////////////////////////////////
    friend VX_FORCE_INLINE constexpr vec<3, T> operator*(const type& q, const vec<3, T>& v)
    {
        const vec<3, T> qv = q.vector();
        const vec<3, T> uv = math::cross(qv, v);
        const vec<3, T> uuv = math::cross(qv, uv);

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    
    friend VX_FORCE_INLINE constexpr vec<3, T> operator*(const vec<3, T>& v, const type& q)
    {
        return inverse(q) * v;
    }

    // division (/)

    friend VX_FORCE_INLINE constexpr type operator/(const type& q, T scaler)
    {
        return type(q.w / scaler, q.x / scaler, q.y / scaler, q.z / scaler);
    }

    friend VX_FORCE_INLINE constexpr type operator/(T scaler, const type& q)
    {
        return scaler * inverse(q);
    }

    friend VX_FORCE_INLINE constexpr type operator/(const type& q1, const type& q2)
    {
        return q1 * inverse(q2);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // unary arithmetic operators
    ///////////////////////////////////////////////////////////////////////////////

    // addition (+=)

    VX_FORCE_INLINE constexpr type& operator+=(const type& q)
    {
        w += q.w;
        x += q.x;
        y += q.y;
        z += q.z;
        return *this;
    }

    // subtraction (-=)

    VX_FORCE_INLINE constexpr type& operator-=(const type& q)
    {
        w -= q.w;
        x -= q.x;
        y -= q.y;
        z -= q.z;
        return *this;
    }

    // multiplication (*=)

    VX_FORCE_INLINE constexpr type& operator*=(T scaler)
    {
        w *= scaler;
        x *= scaler;
        y *= scaler;
        z *= scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator*=(const type& q)
    {
        (*this) = (*this) * q;
        return *this;
    }

    // division (/=)

    VX_FORCE_INLINE constexpr type& operator/=(T scaler)
    {
        w /= scaler;
        x /= scaler;
        y /= scaler;
        z /= scaler;
        return *this;
    }

    VX_FORCE_INLINE constexpr type& operator/=(const type& q)
    {
        (*this) = (*this) / q;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // operations
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Extracts the vector part of this quaternion.
    ///
    /// The vector part of a quaternion represents the imaginary
    /// components (x, y, z) of the quaternion. This method returns
    /// a 3D vector containing these components.
    ///
    /// @return The vector part of this quaternion.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr vec<3, T> vector() const { return vec<3, T>(x, y, z); }

    ///////////////////////////////////////////////////////////////////////////////
    // direction and orientation
    ///////////////////////////////////////////////////////////////////////////////

    // https://github.com/g-truc/glm/blob/586a402397dd35d66d7a079049856d1e2cbab300/glm/gtx/quaternion.inl
    // https://www.cesarkallas.net/arquivos/livros/informatica/game/Game%20Programming%20Gems%201.pdf (page 215 (pdf page 211))

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a quaternion representing rotation from one vector to another.
    ///
    /// This function calculates a quaternion representing the rotation from the
    /// "from" vector to the "to" vector. If both vectors point in the same direction,
    /// it returns the identity quaternion (no rotation). If the vectors point in
    /// opposite directions, it chooses an arbitrary axis perpendicular to both
    /// vectors and rotates around that axis by pi radians.
    ///
    /// @param from The initial vector.
    /// @param to The target vector.
    /// 
    /// @return The quaternion representing the rotation from the "from" vector to
    /// the "to" vector.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type from_to(const vec<3, T>& from, const vec<3, T>& to)
    {
        const vec<3, T> fn = math::normalize(from);
        const vec<3, T> tn = math::normalize(to);

        const T cosa = math::dot(fn, tn);

        if (cosa > static_cast<T>(1) - math::epsilon<T>())
        {
            // In this case, both vectors are pointing in the
            // same direction. We can return the identity
            // quaternion (no rotation).
            return type(1, 0, 0, 0);
        }

        const vec<3, T> axis = math::normalize(math::cross(fn, tn));

        if (cosa < static_cast<T>(-1) + math::epsilon<T>())
        {
            // In this case, the vectors are pointing in opposite
            // directions. There is no ideal axis to rotate around,
            // so we choose one. In this case we just use the cross
            // product of the 2 input vectors and a rotation angle
            // of pi.
            return from_axis_angle(axis, math::pi<T>());
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

    ///////////////////////////////////////////////////////////////////////////////
    // axis angle
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a quaternion representing rotation around an axis by a given
    /// angle.
    ///
    /// @param axis The axis of rotation.
    /// @param angle The angle of rotation in radians.
    /// 
    /// @return The quaternion representing the rotation around the specified axis
    /// by the given angle.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type from_axis_angle(const vec<3, T>& axis, T angle)
    {
        const vec<3, T> naxis = math::normalize(axis);

        const T sina2 = math::sin(angle * static_cast<T>(0.5));
        const T cosa2 = math::cos(angle * static_cast<T>(0.5));

        return type(cosa2, naxis * sina2);
    }

    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
    // https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation#Unit_quaternions

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the axis of rotation represented by the quaternion.
    ///
    /// If the quaternion represents zero rotation (i.e., its magnitude is close
    /// to zero), it returns the positive y-axis as the default axis.
    ///
    /// @return The axis of rotation represented by the quaternion.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr vec<3, T> axis() const
    {
        const T nw = math::normalize(*this).w;
        const T s2 = static_cast<T>(1) - (nw * nw);

        if (s2 < math::epsilon<T>())
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Retrieves the angle of rotation represented by the quaternion.
    ///
    /// @return The angle of rotation represented by the quaternion.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr T angle() const
    {
        return static_cast<T>(2) * math::acos_safe(math::normalize(*this).w);
    }

    ///////////////////////////////////////////////////////////////////////////////
    // euler angles
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a quaternion from Euler angles.
    ///
    /// The Euler angles are specified in radians and represent rotations around
    /// the x, y, and z axes, respectively.
    ///
    /// @param x The rotation angle around the x-axis.
    /// @param y The rotation angle around the y-axis.
    /// @param z The rotation angle around the z-axis.
    /// 
    /// @return The quaternion representing the specified Euler angles.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type from_euler_angles(T x, T y, T z)
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Converts the quaternion to Euler angles.
    ///
    /// The resulting angles represent rotations around the x, y, and z axes,
    /// respectively. The angles are returned in radians.
    ///
    /// @return The Euler angles corresponding to the quaternion.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr vec<3, T> to_euler_angles() const
    {
        const type qn = math::normalize(*this);

        const T qxy = qn.x * qn.y;
        const T qwz = qn.w * qn.z;

        const T test = qxy + qwz;

        if (test > static_cast<T>(0.5) - math::epsilon<T>())
        {
            // singularity at north pole
            return vec<3, T>(
                static_cast<T>(2) * math::atan2(qn.x, qn.w),
                math::half_pi<T>,
                static_cast<T>(0)
            );
        }
        if (test < static_cast<T>(-0.5) + math::epsilon<T>())
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

    ///////////////////////////////////////////////////////////////////////////////
    // matrix conversions
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Creates a quaternion from a 3x3 rotation matrix.
    ///
    /// @param m The 3x3 rotation matrix.
    /// 
    /// @return The quaternion representing the rotation.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type from_mat3(const mat<3, 3, T>& m)
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Converts the quaternion to a 3x3 rotation matrix.
    /// 
    /// @return The 3x3 rotation matrix represented by the quaternion.
    ///////////////////////////////////////////////////////////////////////////////
    VX_FORCE_INLINE constexpr mat<3, 3, T> to_mat3() const
    {
        const type qn = math::normalize(*this);

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

    ///////////////////////////////////////////////////////////////////////////////
    // look at
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Constructs a quaternion representing a left-handed look-at
    /// transformation.
    ///
    /// The resulting quaternion represents the orientation required for an object
    /// to face the target from the specified eye position, assuming a left-handed
    /// coordinate system.
    ///
    /// @param eye The position of the observer.
    /// @param target The position of the target.
    /// @param up The up direction (default is the positive y-axis).
    /// 
    /// @return The quaternion representing the left-handed look-at transformation.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type make_look_at_lh(
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Constructs a quaternion representing a right-handed look-at
    /// transformation.
    ///
    /// The resulting quaternion represents the orientation required for an object
    /// to face the target from the specified eye position, assuming a right-handed
    /// coordinate system.
    ///
    /// @param eye The position of the observer.
    /// @param target The position of the target.
    /// @param up The up direction (default is the positive y-axis).
    /// 
    /// @return The quaternion representing the right-handed look-at transformation.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type make_look_at_rh(
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

    ///////////////////////////////////////////////////////////////////////////////
    /// @brief Constructs a quaternion representing a look-at transformation.
    ///
    /// The resulting quaternion represents the orientation required for an object
    /// to face the target from the specified eye position. The handedness of the
    /// resulting transformation depends on the configuration defined in
    /// VX_CONFIG_CLIP_CONTROL.
    ///
    /// @param eye The position of the observer.
    /// @param target The position of the target.
    /// @param up The up direction (default is the positive y-axis).
    /// 
    /// @return The quaternion representing the look-at transformation.
    ///////////////////////////////////////////////////////////////////////////////
    static VX_FORCE_INLINE constexpr type make_look_at(
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

    ///////////////////////////////////////////////////////////////////////////////
    // constants
    ///////////////////////////////////////////////////////////////////////////////

    static VX_FORCE_INLINE constexpr type IDENTITY() { return type(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }
    static VX_FORCE_INLINE constexpr type ZERO() { return type(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }

};

VX_PACK_POP()

}
}