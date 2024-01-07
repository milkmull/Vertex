#pragma once

namespace vx {
namespace math {

// =============== radians ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T radians(T deg) noexcept
{
    constexpr T r = math::two_pi<T> / static_cast<T>(360);
    return deg * r;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr auto radians(T deg)
{
    using FT = typename detail::to_float_type<T>::type;
    return math::radians(static_cast<FT>(deg));
}

// =============== degrees ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T degrees(T rad) noexcept
{
    constexpr T r = static_cast<T>(360) / math::two_pi<T>;
    return rad * r;
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr auto degrees(T rad)
{
    using FT = typename detail::to_float_type<T>::type;
    return math::degrees(static_cast<FT>(rad));
}

// =============== sin ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T sin(T x)
{
    return std::sin(x);
}

// =============== cos ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T cos(T x)
{
    return std::cos(x);
}


// =============== tan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T tan(T x)
{
    return std::tan(x);
}

// =============== asin ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T asin(T x)
{
    return std::asin(x);
}

// =============== asin clamped ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T asin_clamped(T x)
{
    return math::asin(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== acos ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T acos(T x)
{
    return std::acos(x);
}

// =============== acos clamped ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T acos_clamped(T x)
{
    return math::acos(std::clamp(x, static_cast<T>(-1), static_cast<T>(1)));
}

// =============== atan ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T atan(T x)
{
    return std::atan(x);
}

// =============== atan2 ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T atan2(T x, T y)
{
    return std::atan2(x, y);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool>>
inline constexpr auto atan2(T x, T y)
{
    using FT = typename detail::to_float_type<T>::type;
    return std::atan2(static_cast<FT>(x), static_cast<FT>(y));
}

// =============== sinh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T sinh(T x)
{
    return std::sinh(x);
}

// =============== cosh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T cosh(T x)
{
    return std::cosh(x);
}

// =============== tanh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T tanh(T x)
{
    return std::tanh(x);
}

// =============== asinh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T asinh(T x)
{
    return std::asinh(x);
}

// =============== acosh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T acosh(T x)
{
    return std::acosh(x);
}

// =============== atanh ===============

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool>>
inline constexpr T atanh(T x)
{
    return std::atanh(x);
}

}
}