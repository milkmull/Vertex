#pragma once

#include "../../detail/size_type.h"

namespace vx {
namespace math {

template <size_type L, typename T> struct vec;

template <typename T> using vec2_t = vec<2, T>;
template <typename T> using vec3_t = vec<3, T>;
template <typename T> using vec4_t = vec<4, T>;

template <typename T> struct quat_t;

template <typename T> struct mat2_t;
template <typename T> struct mat3_t;
template <typename T> struct mat4_t;

}
}