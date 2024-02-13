#pragma once

#include <type_traits>

#include "base_types.h"

namespace vx {
namespace math {
namespace detail {

// =============== vector ===============

template <typename T>
struct is_vector : std::false_type {};

template <typename T>
struct is_vector<vec2_t<T>> : std::true_type {};
template <typename T>
struct is_vector<vec3_t<T>> : std::true_type {};
template <typename T>
struct is_vector<vec4_t<T>> : std::true_type {};

template <typename T>
struct vector_size : std::integral_constant<size_type, 0> {};

template <typename T>
struct vector_size<vec2_t<T>> : std::integral_constant<size_type, 2> {};
template <typename T>
struct vector_size<vec3_t<T>> : std::integral_constant<size_type, 3> {};
template <typename T>
struct vector_size<vec4_t<T>> : std::integral_constant<size_type, 4> {};

template <typename T>
struct is_floating_point_vector : std::false_type {};

template <typename T>
struct is_floating_point_vector<vec2_t<T>> : std::is_floating_point<T> {};
template <typename T>
struct is_floating_point_vector<vec3_t<T>> : std::is_floating_point<T> {};
template <typename T>
struct is_floating_point_vector<vec4_t<T>> : std::is_floating_point<T> {};

template <typename T>
struct is_integral_vector : std::false_type {};

template <typename T>
struct is_integral_vector<vec2_t<T>> : std::is_integral<T> {};
template <typename T>
struct is_integral_vector<vec3_t<T>> : std::is_integral<T> {};
template <typename T>
struct is_integral_vector<vec4_t<T>> : std::is_integral<T> {};

// =============== matrix ===============

template <typename T>
struct is_matrix : std::false_type {};

template <typename T>
struct is_matrix<mat2_t<T>> : std::true_type {};
template <typename T>
struct is_matrix<mat3_t<T>> : std::true_type {};
template <typename T>
struct is_matrix<mat4_t<T>> : std::true_type {};

template <typename T>
struct matrix_width : std::integral_constant<size_type, 0> {};

template <typename T>
struct matrix_width<mat2_t<T>> : std::integral_constant<size_type, 2> {};
template <typename T>
struct matrix_width<mat3_t<T>> : std::integral_constant<size_type, 3> {};
template <typename T>
struct matrix_width<mat4_t<T>> : std::integral_constant<size_type, 4> {};

template <typename T>
struct matrix_height : std::integral_constant<size_type, 0> {};

template <typename T>
struct matrix_height<mat2_t<T>> : std::integral_constant<size_type, 2> {};
template <typename T>
struct matrix_height<mat3_t<T>> : std::integral_constant<size_type, 3> {};
template <typename T>
struct matrix_height<mat4_t<T>> : std::integral_constant<size_type, 4> {};

}
}
}