#pragma once

#include "detail/mat4x4_type.h"

#include "mat_fn_comparison.h"

namespace vx {
namespace math {

template <typename T>
using matrix4 = detail::mat4x<T>;

}
}