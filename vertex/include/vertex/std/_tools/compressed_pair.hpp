#pragma once

#include "vertex/config/type_traits.hpp"

namespace vx {
namespace _compressed_pair_priv {

// tag type for value-initializing first, constructing second from remaining args
struct zero_then_variadic_args_tag
{
    explicit zero_then_variadic_args_tag() = default;
};

// tag type for constructing first from one arg, constructing second from remaining args
struct one_then_variadic_args_tag
{
    explicit one_then_variadic_args_tag() = default;
};

// store a pair of values, deriving from empty first
template <typename T1, typename T2, bool = std::is_empty<T1>::value && !std::is_final<T1>::value>
class compressed_pair final : private T1
{
public:

    using base = T1;
    T2 second;

    template <typename... Args>
    constexpr explicit compressed_pair(zero_then_variadic_args_tag, Args&&... val2) noexcept(
        std::is_nothrow_default_constructible<T1>::value && std::is_nothrow_constructible<T2, Args...>::value)
        : T1(), second(std::forward<Args>(val2)...)
    {}

    template <typename Arg1, typename... Args>
    constexpr compressed_pair(one_then_variadic_args_tag, Arg1&& val1, Args&&... val2) noexcept(
        std::is_nothrow_constructible<T1, Arg1>::value && std::is_nothrow_constructible<T2, Args...>::value)
        : T1(std::forward<Arg1>(val1)), second(std::forward<Args>(val2)...)
    {}

    constexpr T1& first() noexcept
    {
        return *this;
    }

    constexpr const T1& first() const noexcept
    {
        return *this;
    }
};

// store a pair of values, not deriving from first
template <typename T1, typename T2>
class compressed_pair<T1, T2, false> final
{
public:

    T1 _first;
    T2 second;

    template <typename... Args>
    constexpr explicit compressed_pair(zero_then_variadic_args_tag, Args&&... val2) noexcept(
        std::is_nothrow_default_constructible<T1>::value && std::is_nothrow_constructible<T2, Args...>::value)
        : _first(), second(std::forward<Args>(val2)...)
    {}

    template <typename Arg1, typename... Args>
    constexpr compressed_pair(one_then_variadic_args_tag, Arg1&& val1, Args&&... val2) noexcept(
        std::is_nothrow_constructible<T1, Arg1>::value && std::is_nothrow_constructible<T2, Args...>::value)
        : _first(std::forward<Arg1>(val1)), second(std::forward<Args>(val2)...)
    {}

    constexpr T1& first() noexcept
    {
        return _first;
    }

    constexpr const T1& first() const noexcept
    {
        return _first;
    }
};

} // namespace _compressed_pair_priv
} // namespace vx
