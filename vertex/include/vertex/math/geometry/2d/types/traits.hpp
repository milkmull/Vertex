#pragma once

#include "vertex/math/geometry/2d/types/base.hpp"

namespace vx {
namespace math {
namespace g2 {

///////////////////////////////////////////////////////////////////////////////
// is_shape
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_shape : false_t {};

template <typename T> struct is_shape<point_t<T>> : true_t {};
template <typename T> struct is_shape<line_t<T>> : true_t {};
template <typename T> struct is_shape<rect_t<T>> : true_t {};
template <typename T> struct is_shape<circle_t<T>> : true_t {};
template <typename T> struct is_shape<triangle_t<T>> : true_t {};

#define VXM_REQ_SHAPE(T) VXM_REQ((is_shape<T>::value))

} // namespace g2
} // namespace math
} // namespace vx