#pragma once

#include "../../detail/size_type.h"

namespace vx {
namespace math {

template <size_type L, typename T> struct vec;
template <typename T> using vec2_t = vec<2, T>;
template <typename T> using vec3_t = vec<3, T>;
template <typename T> using vec4_t = vec<4, T>;

template <typename T> struct quat_t;

template <size_type M, size_type N, typename T> struct mat;
template <typename T> using mat2_t = mat<2, 2, T>;
template <typename T> using mat3_t = mat<3, 3, T>;
template <typename T> using mat4_t = mat<4, 4, T>;

}
}