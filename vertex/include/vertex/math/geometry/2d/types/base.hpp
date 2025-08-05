#pragma once

#include "vertex/math/core/functions/geometric.hpp"

namespace vx {
namespace math {
namespace g2 {

///////////////////////////////////////////////////////////////////////////////
// shape types
///////////////////////////////////////////////////////////////////////////////

template <typename T>
using point_t = vec<2, T>;

template <typename T>
struct line_t;

template <typename T>
struct rect_t;

template <typename T>
struct circle_t;

template <typename T>
struct triangle_t;

template <typename T>
struct polygon_t;

template <typename T>
struct spline_t;

template <typename T>
struct ray_t;

///////////////////////////////////////////////////////////////////////////////
// helper types
///////////////////////////////////////////////////////////////////////////////

// point
using point_i = point_t<i32>;
using point_u = point_t<u32>;
using point_f = point_t<f32>;
using point = point_f;

// line
using line_i = line_t<i32>;
using line_u = line_t<u32>;
using line_f = line_t<f32>;
using line = line_f;

// rect
using rect_i = rect_t<i32>;
using rect_u = rect_t<u32>;
using rect_f = rect_t<f32>;
using rect = rect_f;

} // namespace g2
} // namespace math
} // namespace vx