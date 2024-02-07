#pragma once

#include "type_traits.h"
#include "vertex/tools/detail/iterator.h"

namespace vx {
namespace math {
namespace detail {

using length_type = size_t;

enum class vec_t
{
    vec,	// vector
    col,	// color
};

// =============== types ===============

template <length_type L, typename T, vec_t Q, val_t V = value_type_selector<T>::value> struct vec;

template <length_type M, length_type N, typename T> struct mat;

template <typename T> struct quat;

// =============== vector alias ===============

template <length_type L, typename T>
using vecx = vec<L, T, vec_t::vec>;

template <length_type L, typename T, vec_t Q = vec_t::vec>
using veci = vec<L, T, Q, val_t::integral>;

template <length_type L, typename T, vec_t Q = vec_t::vec>
using vecf = vec<L, T, Q, val_t::floating_point>;

// =============== color alias ===============

template <typename T>
using colx = vec<4, T, vec_t::col>;

template <typename T>
using coli = vec<4, T, vec_t::col, val_t::integral>;

template <typename T>
using colf = vec<4, T, vec_t::col, val_t::floating_point>;

// =============== matrix alias ===============

template <typename T>
using mat2x = mat<2, 2, T>;

template <typename T>
using mat3x = mat<3, 3, T>;

template <typename T>
using mat4x = mat<4, 4, T>;

}
}
}