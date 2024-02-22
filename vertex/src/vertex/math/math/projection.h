#pragma once

#include "math.h"

namespace vx {
namespace math {
namespace project {

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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_lh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_lh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_rh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_rh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_lh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho_rh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#else
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> ortho(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return ortho_lh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return ortho_lh_no(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return ortho_rh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return ortho_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_lh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_lh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_rh_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_rh_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_zo(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_no(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#else
    return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_lh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum_rh(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#else
    return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> frustrum(
    T left, T right,
    T top, T bottom,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return frustrum_lh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return frustrum_lh_no(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return frustrum_rh_zo(left, right, bottom, top, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return frustrum_rh_no(left, right, bottom, top, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_lh_zo(
    T fovy, T aspect,
    T znear, T zfar
)
{
    assert(!math::is_zero_approx(aspect));

    const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_lh_no(
    T fovy, T aspect,
    T znear, T zfar
)
{
    assert(!math::is_zero_approx(aspect));

    const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_rh_zo(
    T fovy, T aspect,
    T znear, T zfar
)
{
    assert(!math::is_zero_approx(aspect));

    const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_rh_no(
    T fovy, T aspect,
    T znear, T zfar
)
{
    assert(!math::is_zero_approx(aspect));

    const T tanfovy2 = math::tan(fovy * static_cast<T>(0.5));

    return mat<4, 4, T>(
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_zo(
    T fovy, T aspect,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
static inline constexpr mat<4, 4, T> perspective_no(
    T fovy, T aspect,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_LH_BIT)
    return perspective_lh_no(fovy, aspect, znear, zfar);
#else
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> perspective_lh(
    T fovy, T aspect,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_lh_no(fovy, aspect, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> perspective_rh(
    T fovy, T aspect,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#else
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr mat<4, 4, T> perspective(
    T fovy, T aspect,
    T znear, T zfar
)
{
#if (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_ZO)
    return perspective_lh_zo(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_LH_NO)
    return perspective_lh_no(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_ZO)
    return perspective_rh_zo(fovy, aspect, znear, zfar);
#elif (VX_CONFIG_CLIP_CONTROL == VX_CLIP_CONTROL_RH_NO)
    return perspective_rh_no(fovy, aspect, znear, zfar);
#endif
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> project(const mat<4, 4, T>& m, const vec<3, T>& v)
{
    vec<4, T> v4(v, static_cast<T>(1));
    v4 = m * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
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
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> unproject(const mat<4, 4, T>& m, const vec<3, T>& v)
{
    vec<4, T> v4(v, static_cast<T>(1));
    v4 = math::invert(m) * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

/**
 * @brief Projects a 3D vector in object space to screen space with a depth range of 0 to 1.
 *
 * This function takes a 3D vector in object space, transforms it using the provided model and projection matrices,
 * and maps it to screen space with a depth range of 0 to 1. The resulting vector is adjusted based on the viewport parameters.
 *
 * @param object The 3D vector in object space to be projected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the result to screen coordinates.
 * @return The resulting 3D vector in screen space with a depth range of 0 to 1.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> project_zo(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
    vec<4, T> v4(object, static_cast<T>(1));
    v4 = projection * (model * v4);

    const T invw = static_cast<T>(1) / v4.w;
    v4 *= invw;

    v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);

    v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
    v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

    return vec<3, T>(v4);
}

/**
 * @brief Projects a 3D vector in object space to screen space with a depth range of -1 to 1.
 *
 * This function takes a 3D vector in object space, transforms it using the provided model and projection matrices,
 * and maps it to screen space with a depth range of -1 to 1. The resulting vector is adjusted based on the viewport parameters.
 *
 * @param object The 3D vector in object space to be projected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the result to screen coordinates.
 * @return The resulting 3D vector in screen space with a depth range of -1 to 1.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> project_no(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
    vec<4, T> v4(object, static_cast<T>(1));
    v4 = projection * (model * v4);

    const T invw = static_cast<T>(1) / v4.w;
    v4 *= invw;

    v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);
    v4.z = v4.z * static_cast<T>(0.5) + static_cast<T>(0.5);

    v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
    v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

    return vec<3, T>(v4);
}

/**
 * @brief Projects a 3D vector in object space to screen space based on the configured clip control.
 *
 * This function takes a 3D vector in object space, transforms it using the provided model and projection matrices,
 * and maps it to screen space based on the configured clip control (ZO or NO). The resulting vector is adjusted
 * based on the viewport parameters.
 *
 * @param object The 3D vector in object space to be projected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the result to screen coordinates.
 * @return The resulting 3D vector in screen space based on the configured clip control.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> project(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return project_zo(object, model, projection, viewport);
#else
    return project_no(object, model, projection, viewport);
#endif
}

// =============== unproject ===============

/**
 * @brief Unprojects a 3D vector in screen space with a depth range of 0 to 1 to object space.
 *
 * This function takes a 3D vector in screen space with a depth range of 0 to 1, transforms it using the inverse
 * of the provided model and projection matrices, and maps it back to object space. The resulting vector is adjusted
 * based on the viewport parameters.
 *
 * @param object The 3D vector in screen space to be unprojected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the input to screen coordinates.
 * @return The resulting 3D vector in object space.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> unproject_zo(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
    const mat<4, 4, T> inv(projection * model);
    vec<4, T> v4(object, static_cast<T>(1));

    v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
    v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

    v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
    v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);

    v4 = inv * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

/**
 * @brief Unprojects a 3D vector in screen space with a depth range of -1 to 1 to object space.
 *
 * This function takes a 3D vector in screen space with a depth range of -1 to 1, transforms it using the inverse
 * of the provided model and projection matrices, and maps it back to object space. The resulting vector is adjusted
 * based on the viewport parameters.
 *
 * @param object The 3D vector in screen space to be unprojected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the input to screen coordinates.
 * @return The resulting 3D vector in object space.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> unproject_no(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
    const mat<4, 4, T> inv(projection * model);
    vec<4, T> v4(object, static_cast<T>(1));

    v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
    v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

    v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
    v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);
    v4.z = v4.z * static_cast<T>(2) - static_cast<T>(1);

    v4 = inv * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

/**
 * @brief Unprojects a 3D vector in screen space to object space based on the configured clip control.
 *
 * This function takes a 3D vector in screen space, transforms it using the inverse of the provided model
 * and projection matrices, and maps it back to object space based on the configured clip control (ZO or NO).
 * The resulting vector is adjusted based on the viewport parameters.
 *
 * @param object The 3D vector in screen space to be unprojected.
 * @param model The model matrix for transforming the object space vector.
 * @param projection The projection matrix for the perspective transformation.
 * @param viewport The viewport parameters specifying the mapping of the input to screen coordinates.
 * @return The resulting 3D vector in object space based on the configured clip control.
 */
template <typename T, typename U, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> unproject(
    const vec<3, T>& object,
    const mat<4, 4, T>& model,
    const mat<4, 4, T>& projection,
    const vec<4, U>& viewport
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
    return unproject_zo(object, model, projection, viewport);
#else
    return unproject_no(object, model, projection, viewport);
#endif
}

}
}
}