#pragma once

#include <type_traits>

#include "../type/base/base_types.h"

namespace vx {
namespace math {
namespace type_traits {

// =============== scaler ===============

template <typename T> struct is_scaler : std::false_type {};

template <> struct is_scaler<bool> : std::true_type {};
template <> struct is_scaler<double> : std::true_type {};
template <> struct is_scaler<float> : std::true_type {};
template <> struct is_scaler<int32_t> : std::true_type {};
template <> struct is_scaler<uint32_t> : std::true_type {};

template <typename T> struct is_numeric : std::false_type {};

template <> struct is_numeric<double> : std::true_type {};
template <> struct is_numeric<float> : std::true_type {};
template <> struct is_numeric<int32_t> : std::true_type {};
template <> struct is_numeric<uint32_t> : std::true_type {};

template <typename T> struct is_integral : std::false_type {};

template <> struct is_integral<int32_t> : std::true_type {};
template <> struct is_integral<uint32_t> : std::true_type {};

template <typename T> struct is_floating_point : std::false_type {};

template <> struct is_floating_point<double> : std::true_type {};
template <> struct is_floating_point<float> : std::true_type {};

// =============== vector ===============

template <typename T>
struct is_vector : std::false_type {};

template <size_t L, typename T>
struct is_vector<vec<L, T>> : std::true_type {};

template <typename T>
struct vector_size : std::integral_constant<size_t, 0> {};

template <size_t L, typename T>
struct vector_size<vec<L, T>> : std::integral_constant<size_t, L> {};

template <size_t L, typename T>
struct is_numeric<vec<L, T>> : is_numeric<T> {};

template <size_t L, typename T>
struct is_floating_point<vec<L, T>> : is_floating_point<T> {};

template <size_t L, typename T>
struct is_integral<vec<L, T>> : is_integral<T> {};

// =============== matrix ===============

template <typename T>
struct is_matrix : std::false_type {};

template <size_t M, size_t N, typename T>
struct is_matrix<mat<M, N, T>> : std::true_type {};

template <typename T>
struct matrix_width : std::integral_constant<size_t, 0> {};

template <size_t M, size_t N, typename T>
struct matrix_width<mat<M, N, T>> : std::integral_constant<size_t, M> {};

template <typename T>
struct matrix_height : std::integral_constant<size_t, 0> {};

template <size_t M, size_t N, typename T>
struct matrix_height<mat<M, N, T>> : std::integral_constant<size_t, N> {};

template <typename T>
struct is_square_matrix : std::false_type {};

template <size_t M, size_t N, typename T>
struct is_square_matrix<mat<M, N, T>> : std::bool_constant<(M == N)> {};

template <size_t M, size_t N, typename T>
struct is_numeric<mat<M, N, T>> : is_numeric<T> {};

template <size_t M, size_t N, typename T>
struct is_floating_point<mat<M, N, T>> : is_floating_point<T> {};

template <size_t M, size_t N, typename T>
struct is_integral<mat<M, N, T>> : is_integral<T> {};

}
}
}