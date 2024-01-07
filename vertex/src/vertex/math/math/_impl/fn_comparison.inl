#pragma once

namespace vx {
namespace math {

// =============== is_zero_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_zero_approx(T x, const T epsilon)
{
    return ((x < static_cast<T>(0)) ? -x : x) < epsilon;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_zero_approx(T x, const T epsilon)
{
    return x == static_cast<T>(0);
}

// =============== is_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon)
{
    return math::is_zero_approx(a - b, epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_equal_approx(T a, T b, const T epsilon)
{
    return a == b;
}

// =============== is_not_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon)
{
    return !math::is_zero_approx(a - b, epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_not_equal_approx(T a, T b, const T epsilon)
{
    return a != b;
}

// =============== is_greater_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon)
{
    return a > (b + epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_greater_approx(T a, T b, const T epsilon)
{
    return a > b;
}

// =============== is_greater_or_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon)
{
    return a > (b - epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_greater_or_equal_approx(T a, T b, const T epsilon)
{
    return a >= b;
}

// =============== is_less_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_less_approx(T a, T b, const T epsilon)
{
    return a < (b - epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_less_approx(T a, T b, const T epsilon)
{
    return a < b;
}

// =============== is_less_or_equal_approx ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon)
{
    return a < (b + epsilon);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_less_or_equal_approx(T a, T b, const T epsilon)
{
    return a <= b;
}

// =============== is_finite ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_finite(T x)
{
    return std::isfinite(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_finite(T x)
{
    return true;
}

// =============== is_infinite ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_infinite(T x)
{
    return std::isinf(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_infinite(T x)
{
    return false;
}

// =============== is_nan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr bool is_nan(T x)
{
    return std::isnan(x);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr bool is_nan(T x)
{
    return false;
}

// =============== round_if_close ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T round_if_close(T x, const T epsilon)
{
    const T xr = std::round(x);
    return math::is_equal_approx(x, xr, epsilon) ? xr : x;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr T round_if_close(T x, const T epsilon)
{
    return x;
}

// =============== make_pretty ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T make_pretty(T x, const T epsilon)
{
    const T r = math::round_if_close(x, epsilon);
    return (r == static_cast<T>(0)) ? std::abs(r) : r;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr T make_pretty(T x, const T epsilon)
{
    return x;
}

}
}