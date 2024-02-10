#pragma once

#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {

using size_type = size_t;

template <size_type L, typename T> struct vec;

template <typename T> using vec2_t = vec<2, T>;
template <typename T> using vec3_t = vec<3, T>;
template <typename T> using vec4_t = vec<4, T>;

template <typename T> struct color_t;

template <typename T> struct quat_t;

template <typename T> struct mat2_t;
template <typename T> struct mat3_t;
template <typename T> struct mat4_t;

}
}