#pragma once

#include "detail/base_type_defs.h"

namespace vx {
namespace math {

// =============== project ===============

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> project_zo(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
)
{
	detail::vec<4, T, detail::vec_t::vec> v4(object, static_cast<T>(1));
	v4 = projection * (model * v4);

	const T invw = static_cast<T>(1) / v4.w;
	v4 *= invw;

	v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);

	v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
	v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

	return detail::vec<3, T, detail::vec_t::vec>(v4);
}

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> project_no(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
)
{
	detail::vec<4, T, detail::vec_t::vec> v4(object, static_cast<T>(1));
	v4 = projection * (model * v4);

	const T invw = static_cast<T>(1) / v4.w;
	v4 *= invw;

	v4.x = v4.x * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.y = v4.y * static_cast<T>(0.5) + static_cast<T>(0.5);
	v4.z = v4.z * static_cast<T>(0.5) + static_cast<T>(0.5);

	v4.x = v4.x * static_cast<T>(viewport.z) + static_cast<T>(viewport.x);
	v4.y = v4.y * static_cast<T>(viewport.w) + static_cast<T>(viewport.y);

	return detail::vec<3, T, detail::vec_t::vec>(v4);
}

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> project(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
)
{
#if (VX_CONFIG_CLIP_CONTROL & VX_CLIP_CONTROL_ZO_BIT)
	return project_zo(object, model, projection, viewport);
#else
	return project_no(object, model, projection, viewport);
#endif
}

// =============== unproject ===============

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> unproject_zo(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
)
{
	const detail::mat<4, 4, T> inv(projection * model);
	detail::vec<4, T, detail::vec_t::vec> v4(object, static_cast<T>(1));

	v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
	v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

	v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
	v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);

	v4 = inv * v4;
	const T invw = static_cast<T>(1) / v4.w;
	return detail::vec<3, T, detail::vec_t::vec>(v4.x * invw, v4.y * invw, v4.z * invw);
}

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> unproject_no(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
)
{
	const detail::mat<4, 4, T> inv(projection * model);
	detail::vec<4, T, detail::vec_t::vec> v4(object, static_cast<T>(1));

	v4.x = (v4.x - static_cast<T>(viewport.x)) / static_cast<T>(viewport.y);
	v4.y = (v4.y - static_cast<T>(viewport.z)) / static_cast<T>(viewport.w);

	v4.x = v4.x * static_cast<T>(2) - static_cast<T>(1);
	v4.y = v4.y * static_cast<T>(2) - static_cast<T>(1);
	v4.z = v4.z * static_cast<T>(2) - static_cast<T>(1);

	v4 = inv * v4;
	const T invw = static_cast<T>(1) / v4.w;
	return detail::vec<3, T, detail::vec_t::vec>(v4.x * invw, v4.y * invw, v4.z * invw);
}

template <typename T, typename U>
inline constexpr detail::vec<3, T, detail::vec_t::vec> unproject(
	const detail::vec<3, T, detail::vec_t::vec>& object,
	const detail::mat<4, 4, T>& model,
	const detail::mat<4, 4, T>& projection,
	const detail::vec<4, U, detail::vec_t::vec>& viewport
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