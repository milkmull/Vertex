#pragma once

#include "./base.hpp"

namespace vx {
namespace math {
namespace geometry2d {

///////////////////////////////////////////////////////////////////////////////
// is_shape
///////////////////////////////////////////////////////////////////////////////

template <typename T> struct is_shape : false_t {};

template <typename T> struct is_shape<point<T>> : true_t {};
template <typename T> struct is_shape<line<T>> : true_t {};
template <typename T> struct is_shape<rect<T>> : true_t {};
template <typename T> struct is_shape<circle<T>> : true_t {};
template <typename T> struct is_shape<triangle<T>> : true_t {};

#define VXM_REQ_SHAPE(T) VXM_REQ((is_shape<T>::value))

} // namespace geometry2d
} // namespace math
} // namespace vx