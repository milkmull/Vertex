#pragma once

#include "detail/base_type_defs.h"

namespace vx {
namespace math {

// =============== project ===============

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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> project_zo(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
)
{
	detail::vecf<4, T> v4(object, static_cast<T>(1));
	v4 = projection * (model * v4);

	const T invw = static_cast<T>(1) / v4.w;
	v4 *= invw;

	v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);

	v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
	v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

	return detail::vecf<3, T>(v4);
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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> project_no(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
)
{
	detail::vecf<4, T> v4(object, static_cast<T>(1));
	v4 = projection * (model * v4);

	const T invw = static_cast<T>(1) / v4.w;
	v4 *= invw;

	v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.z = v4.z * static_cast<T>(0.5) + static_cast<T>(0.5);

	v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
	v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

	return detail::vecf<3, T>(v4);
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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> project(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> unproject_zo(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
)
{
	const detail::mat<4, 4, T> inv(projection * model);
	detail::vecf<4, T> v4(object, static_cast<T>(1));

	v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
	v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

	v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
	v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);

	v4 = inv * v4;
	const T invw = static_cast<T>(1) / v4.w;
	return detail::vecf<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> unproject_no(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
)
{
	const detail::mat<4, 4, T> inv(projection * model);
	detail::vecf<4, T> v4(object, static_cast<T>(1));

	v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
	v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

	v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
	v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);
	v4.z = v4.z * static_cast<T>(2) - static_cast<T>(1);

	v4 = inv * v4;
	const T invw = static_cast<T>(1) / v4.w;
	return detail::vecf<3, T>(v4.x * invw, v4.y * invw, v4.z * invw);
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
template <typename T, typename U>
inline constexpr detail::vecf<3, T> unproject(
	const detail::vecf<3, T>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vecx<4, U>& viewport
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