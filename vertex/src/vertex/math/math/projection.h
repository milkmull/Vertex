#pragma once

#include "math.h"

namespace vx {
namespace math {

// =============== ortho ===============

/**
 * @brief Constructs a left-handed orthographic projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 left-handed orthographic projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed orthographic projection matrix.
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
 * @brief Constructs a left-handed orthographic projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 left-handed orthographic projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed orthographic projection matrix.
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
 * @brief Constructs a right-handed orthographic projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 right-handed orthographic projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed orthographic projection matrix.
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
 * @brief Constructs a right-handed orthographic projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 right-handed orthographic projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed orthographic projection matrix.
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
 * @brief Constructs an orthographic projection matrix with depth range [0, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 orthographic projection matrix with a depth range of [0, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed orthographic projection matrix with depth range [0, 1].
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
 * @brief Constructs an orthographic projection matrix with depth range [-1, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 orthographic projection matrix with a depth range of [-1, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed orthographic projection matrix with depth range [-1, 1].
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
 * @brief Constructs a left-handed orthographic projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 left-handed orthographic projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed orthographic projection matrix with appropriate depth range.
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
 * @brief Constructs a right-handed orthographic projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 right-handed orthographic projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed orthographic projection matrix with appropriate depth range.
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
 * @brief Constructs an orthographic projection matrix using the appropriate handedness and depth range.
 *
 * This function constructs a 4x4 orthographic projection matrix using either left-handed or right-handed convention and depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed orthographic projection matrix with appropriate handedness and depth range.
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
 * @brief Constructs a left-handed frustum projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 left-handed frustum projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed frustum projection matrix.
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
 * @brief Constructs a left-handed frustum projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 left-handed frustum projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed frustum projection matrix.
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
 * @brief Constructs a right-handed frustum projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 right-handed frustum projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed frustum projection matrix.
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
 * @brief Constructs a right-handed frustum projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 right-handed frustum projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed frustum projection matrix.
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
 * @brief Constructs a frustum projection matrix with depth range [0, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 frustum projection matrix with a depth range of [0, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed frustum projection matrix with depth range [0, 1].
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
 * @brief Constructs a frustum projection matrix with depth range [-1, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 frustum projection matrix with a depth range of [-1, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed frustum projection matrix with depth range [-1, 1].
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
 * @brief Constructs a left-handed frustum projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 left-handed frustum projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed frustum projection matrix with appropriate depth range.
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
 * @brief Constructs a right-handed frustum projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 right-handed frustum projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed frustum projection matrix with appropriate depth range.
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
 * @brief Constructs a frustum projection matrix using the appropriate handedness and depth range.
 *
 * This function constructs a 4x4 frustum projection matrix using either left-handed or right-handed convention and depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param left The minimum x-coordinate of the view volume.
 * @param right The maximum x-coordinate of the view volume.
 * @param top The maximum y-coordinate of the view volume.
 * @param bottom The minimum y-coordinate of the view volume.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed frustum projection matrix with appropriate handedness and depth range.
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
 * @brief Constructs a left-handed perspective projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 left-handed perspective projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed perspective projection matrix.
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
 * @brief Constructs a left-handed perspective projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 left-handed perspective projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed perspective projection matrix.
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
 * @brief Constructs a right-handed perspective projection matrix with depth range [0, 1].
 *
 * This function constructs a 4x4 right-handed perspective projection matrix suitable for rendering with a depth range of [0, 1].
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed perspective projection matrix.
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
 * @brief Constructs a right-handed perspective projection matrix with depth range [-1, 1].
 *
 * This function constructs a 4x4 right-handed perspective projection matrix suitable for rendering with a depth range of [-1, 1].
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed perspective projection matrix.
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
 * @brief Constructs a perspective projection matrix with depth range [0, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 perspective projection matrix with a depth range of [0, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed perspective projection matrix with depth range [0, 1].
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
 * @brief Constructs a perspective projection matrix with depth range [-1, 1] using the appropriate handedness.
 *
 * This function constructs a 4x4 perspective projection matrix with a depth range of [-1, 1] using either left-handed or right-handed convention based on configuration.
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed perspective projection matrix with depth range [-1, 1].
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
 * @brief Constructs a left-handed perspective projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 left-handed perspective projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param fovy The field of view angle in the y-direction, in radians.
 * @param aspect The aspect ratio, defined as width divided by height.
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed left-handed perspective projection matrix with appropriate depth range.
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
 * @brief Constructs a right-handed perspective projection matrix using the appropriate depth range.
 *
 * This function constructs a 4x4 right-handed perspective projection matrix using either depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param fovy The vertical field of view angle in radians.
 * @param aspect The aspect ratio of the viewport (width / height).
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed right-handed perspective projection matrix with appropriate depth range.
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
 * @brief Constructs a perspective projection matrix using the appropriate handedness and depth range.
 *
 * This function constructs a 4x4 perspective projection matrix using either left-handed or right-handed convention and depth range [0, 1] or [-1, 1] based on configuration.
 *
 * @param fovy The vertical field of view angle in radians.
 * @param aspect The aspect ratio of the viewport (width / height).
 * @param znear The minimum z-coordinate of the view volume (near clipping plane).
 * @param zfar The maximum z-coordinate of the view volume (far clipping plane).
 *
 * @return The constructed perspective projection matrix with appropriate handedness and depth range.
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

// =============== project ===============

/**
 * @brief Projects a 3D object onto the screen in normalized device coordinates with a depth range of [0, 1].
 *
 * This function takes a 3D object coordinate, applies model and projection transformations,
 * and maps the resulting position to the normalized device coordinates with a depth range of [0, 1].
 *
 * @param object The 3D object coordinate to be projected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of normalized device coordinates to screen coordinates.
 * It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the projected 3D object coordinate in normalized device coordinates with a depth range of [0, 1].
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
 * @brief Projects a 3D object onto the screen in clip space with a depth range of [-1, 1].
 *
 * This function takes a 3D object coordinate, applies model and projection transformations,
 * and maps the resulting position to the clip space coordinates with a depth range of [-1, 1].
 *
 * @param object The 3D object coordinate to be projected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of normalized device coordinates to screen coordinates.
 *                 It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the projected 3D object coordinate in clip space coordinates with a depth range of [-1, 1].
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
 * @brief Projects a 3D object onto the screen in clip space with configurable depth range.
 *
 * This function takes a 3D object coordinate, applies model and projection transformations,
 * and maps the resulting position to the clip space coordinates. The behavior depends on compile-time
 * configuration for the depth range.
 *
 * @param object The 3D object coordinate to be projected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of normalized device coordinates to screen coordinates.
 * It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the projected 3D object coordinate in clip space coordinates.
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
 * @brief Unprojects a 3D point from screen space with a depth range of [0, 1] to world space.
 *
 * This function takes a 3D point in screen space with a depth range of [0, 1], applies
 * the inverse model-view-projection transformation, and returns the corresponding point
 * in world space.
 *
 * @param object The 3D point in screen space to be unprojected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of screen coordinates to normalized device coordinates.
 * It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the unprojected 3D point in world space.
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
 * @brief Unprojects a 3D point from screen space with a depth range of [-1, 1] to world space.
 *
 * This function takes a 3D point in screen space with a depth range of [-1, 1], applies
 * the inverse model-view-projection transformation, and returns the corresponding point
 * in world space.
 *
 * @param object The 3D point in screen space to be unprojected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of screen coordinates to normalized device coordinates.
 * It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the unprojected 3D point in world space.
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
 * @brief Unprojects a 3D point from screen space with configurable depth range to world space.
 *
 * This function takes a 3D point in screen space with a configurable depth range, applies
 * the inverse model-view-projection transformation, and returns the corresponding point
 * in world space. The behavior depends on compile-time configuration for the depth range.
 *
 * @param object The 3D point in screen space to be unprojected.
 * @param model The model matrix representing the object's transformation.
 * @param projection The projection matrix representing the camera's projection.
 * @param viewport The viewport defining the mapping of screen coordinates to normalized device coordinates.
 * It's a 4D vector representing (x, y, width, height) of the viewport.
 *
 * @return Returns the unprojected 3D point in world space.
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

// =============== projection ===============

/**
 * @brief Projects a 3D vector using a 4x4 transformation matrix.
 *
 * This function applies the transformation represented by the provided 4x4 matrix to the
 * input 3D vector, effectively projecting it onto another space.
 *
 * @param m The 4x4 transformation matrix representing the projection.
 * @param v The 3D vector to be projected.
 *
 * @return Returns the projected 3D vector.
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
 * @brief Unprojects a 3D vector using the inverse of a 4x4 transformation matrix.
 *
 * This function applies the inverse transformation of the provided 4x4 matrix to the
 * input 3D vector, effectively unprojecting it from one space to another.
 *
 * @param m The 4x4 transformation matrix representing the inverse projection.
 * @param v The 3D vector to be unprojected.
 *
 * @return Returns the unprojected 3D vector.
 */
template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
inline constexpr vec<3, T> unproject(const mat<4, 4, T>& m, const vec<3, T>& v)
{
    vec<4, T> v4(v, static_cast<T>(1));
    v4 = math::invert(m) * v4;
    const T invw = static_cast<T>(1) / v4.w;
    return vec<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
}

}
}