#pragma once

#include <type_traits>

namespace vx {

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T operator&(T x, T y)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(x) & static_cast<U>(y));
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T operator|(T x, T y)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(x) | static_cast<U>(y));
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T operator^(T x, T y)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(x) ^ static_cast<U>(y));
}

template <typename T, typename V, typename std::enable_if<std::is_enum<T>::value && std::is_integral<V>::value, bool>::type = true>
inline constexpr T operator<<(T x, V y)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(x) << static_cast<U>(y));
}

template <typename T, typename V, typename std::enable_if<std::is_enum<T>::value&& std::is_integral<V>::value, bool>::type = true>
inline constexpr T operator>>(T x, V y)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<U>(x) >> static_cast<U>(y));
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T operator~(T x)
{
    using U = typename std::underlying_type<T>::type;
    return static_cast<T>(~static_cast<U>(x));
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T& operator&=(T& x, T y)
{
    x = x & y;
    return x;
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T& operator|=(T& x, T y)
{
    x = x | y;
    return x;
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
inline constexpr T& operator^=(T& x, T y)
{
    x = x ^ y;
    return x;
}

template <typename T, typename V, typename std::enable_if<std::is_enum<T>::value && std::is_integral<V>::value, bool>::type = true>
inline constexpr T& operator<<=(T& x, V y)
{
    x = x << y;
    return x;
}

template <typename T, typename V, typename std::enable_if<std::is_enum<T>::value&& std::is_integral<V>::value, bool>::type = true>
inline constexpr T& operator>>=(T& x, V y)
{
    x = x >> y;
    return x;
}

}                                                                                        