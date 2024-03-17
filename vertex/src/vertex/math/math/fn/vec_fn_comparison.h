#pragma once

#include "fn_comparison.h"

#include "../type/vec2b.h"
#include "../type/vec3b.h"
#include "../type/vec4b.h"

namespace vx {
namespace math {

// =============== any ===============

/**
 * @brief Checks if any component of a vector is true.
 *
 * This function checks if any component of a vector is true (non-zero).
 *
 * @tparam T The type of the vector components.
 * @param x The vector.
 * @return True if any component of the vector is true (non-zero), false otherwise.
 */
template <typename T>
inline constexpr bool any(const vec<2, T>& x)
{
    return x.x || x.y;
}

template <typename T>
inline constexpr bool any(const vec<3, T>& x)
{
    return x.x || x.y || x.z;
}

template <typename T>
inline constexpr bool any(const vec<4, T>& x)
{
    return x.x || x.y || x.z || x.w;
}

// =============== all ===============

/**
 * @brief Checks if all components of a vector are true.
 *
 * @tparam T The type of the vector components.
 * @param x The vector.
 * @return True if all components of the vector are true (non-zero), false otherwise.
 */
template <typename T>
inline constexpr bool all(const vec<2, T>& x)
{
    return x.x && x.y;
}

template <typename T>
inline constexpr bool all(const vec<3, T>& x)
{
    return x.x && x.y && x.z;
}

template <typename T>
inline constexpr bool all(const vec<4, T>& x)
{
    return x.x && x.y && x.z && x.w;
}

// =============== equal ===============

/**
 * @brief Checks if components of two vectors are equal.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components are equal.
 */
template <typename T>
inline constexpr vec<2, bool> equal(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> equal(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> equal(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        equal(x.x, y.x),
        equal(x.y, y.y),
        equal(x.z, y.z),
        equal(x.w, y.w)
    );
}

// =============== not_equal ===============

/**
 * @brief Checks if components of two vectors are not equal.
 *
 * This function checks if components of two vectors are not equal.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components are not equal.
 */
template <typename T>
inline constexpr vec<2, bool> not_equal(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> not_equal(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> not_equal(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        not_equal(x.x, y.x),
        not_equal(x.y, y.y),
        not_equal(x.z, y.z),
        not_equal(x.w, y.w)
    );
}

// =============== greater_than ===============

/**
 * @brief Checks if components of the first vector are greater than the components of the second vector.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components of the first vector are greater than the components of the second vector.
 */
template <typename T>
inline constexpr vec<2, bool> greater_than(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> greater_than(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y),
        greater_than(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> greater_than(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        greater_than(x.x, y.x),
        greater_than(x.y, y.y),
        greater_than(x.z, y.z),
        greater_than(x.w, y.w)
    );
}

// =============== greater_than_equal ===============

/**
 * @brief Checks if components of the first vector are greater than or equal to the components of the second vector.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components of the first vector are greater than or equal to the components of the second vector.
 */
template <typename T>
inline constexpr vec<2, bool> greater_than_equal(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        greater_than_equal(x.x, y.x),
        greater_than_equal(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> greater_than_equal(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        greater_than_equal(x.x, y.x),
        greater_than_equal(x.y, y.y),
        greater_than_equal(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> greater_than_equal(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        greater_than_equal(x.x, y.x),
        greater_than_equal(x.y, y.y),
        greater_than_equal(x.z, y.z),
        greater_than_equal(x.w, y.w)
    );
}

// =============== less_than ===============

/**
 * @brief Checks if components of the first vector are less than the components of the second vector.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components of the first vector are less than the components of the second vector.
 */
template <typename T>
inline constexpr vec<2, bool> less_than(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        less_than(x.x, y.x),
        less_than(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> less_than(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        less_than(x.x, y.x),
        less_than(x.y, y.y),
        less_than(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> less_than(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        less_than(x.x, y.x),
        less_than(x.y, y.y),
        less_than(x.z, y.z),
        less_than(x.w, y.w)
    );
}

// =============== less_than_equal ===============

/**
 * @brief Checks if components of the first vector are less than or equal to the components of the second vector.
 *
 * @tparam T The type of the vector components.
 * @param x The first vector.
 * @param y The second vector.
 * @return A vector of boolean values indicating whether components of the first vector are less than or equal to the components of the second vector.
 */
template <typename T>
inline constexpr vec<2, bool> less_than_equal(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        less_than_equal(x.x, y.x),
        less_than_equal(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> less_than_equal(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        less_than_equal(x.x, y.x),
        less_than_equal(x.y, y.y),
        less_than_equal(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> less_than_equal(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        less_than_equal(x.x, y.x),
        less_than_equal(x.y, y.y),
        less_than_equal(x.z, y.z),
        less_than_equal(x.w, y.w)
    );
}

// =============== is_inf ===============

/**
 * @brief Checks if components of the vector are infinite.
 *
 * @tparam T The type of the vector components.
 * @param x The vector.
 * @return A vector of boolean values indicating whether components of the vector are infinite.
 */
template <typename T>
inline constexpr vec<2, bool> is_inf(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        is_inf(x.x, y.x),
        is_inf(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> is_inf(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        is_inf(x.x, y.x),
        is_inf(x.y, y.y),
        is_inf(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> is_inf(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        is_inf(x.x, y.x),
        is_inf(x.y, y.y),
        is_inf(x.z, y.z),
        is_inf(x.w, y.w)
    );
}

// =============== is_nan ===============

/**
 * @brief Checks if components of the vector are NaN (Not-a-Number).
 *
 * @tparam T The type of the vector components.
 * @param x The vector.
 * @return A vector of boolean values indicating whether components of the vector are NaN.
 */
template <typename T>
inline constexpr vec<2, bool> is_nan(const vec<2, T>& x, vec<2, T>& y)
{
    return vec<2, bool>(
        is_nan(x.x, y.x),
        is_nan(x.y, y.y)
    );
}

template <typename T>
inline constexpr vec<3, bool> is_nan(const vec<3, T>& x, vec<3, T>& y)
{
    return vec<3, bool>(
        is_nan(x.x, y.x),
        is_nan(x.y, y.y),
        is_nan(x.z, y.z)
    );
}

template <typename T>
inline constexpr vec<4, bool> is_nan(const vec<4, T>& x, vec<4, T>& y)
{
    return vec<4, bool>(
        is_nan(x.x, y.x),
        is_nan(x.y, y.y),
        is_nan(x.z, y.z),
        is_nan(x.w, y.w)
    );
}

// =============== is_zero_approx ===============

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<2, T>& x,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<3, T>& x,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon);
}

template <typename T>
inline constexpr bool is_zero_approx(
    const vec<4, T>& x,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_zero_approx(x.x, epsilon)
        && math::is_zero_approx(x.y, epsilon)
        && math::is_zero_approx(x.z, epsilon)
        && math::is_zero_approx(x.w, epsilon);
}

// =============== is_equal_approx ===============

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<2, T>& a,
    const vec<2, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<3, T>& a,
    const vec<3, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon);
}

template <typename T>
inline constexpr bool is_equal_approx(
    const vec<4, T>& a,
    const vec<4, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_equal_approx(a.x, b.x, epsilon)
        && math::is_equal_approx(a.y, b.y, epsilon)
        && math::is_equal_approx(a.z, b.z, epsilon)
        && math::is_equal_approx(a.w, b.w, epsilon);
}

// =============== is_not_equal_approx ===============

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<2, T>& a,
    const vec<2, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon);
}

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<3, T>& a,
    const vec<3, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon)
        && math::is_not_equal_approx(a.z, b.z, epsilon);
}

template <typename T>
inline constexpr bool is_not_equal_approx(
    const vec<4, T>& a,
    const vec<4, T>& b,
    const T epsilon = math::epsilon<T>()
)
{
    return math::is_not_equal_approx(a.x, b.x, epsilon)
        && math::is_not_equal_approx(a.y, b.y, epsilon)
        && math::is_not_equal_approx(a.z, b.z, epsilon)
        && math::is_not_equal_approx(a.w, b.w, epsilon);
}

}
}