#pragma once

#include <type_traits>

namespace vx {
namespace type_traits {

template <typename...>
using void_t = void;

///////////////////////////////////////////////////////////////////////////////
// enable if
///////////////////////////////////////////////////////////////////////////////

template <bool Test, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> { using type = T; };

#define VX_REQUIRES(condition) typename ::vx::type_traits::enable_if<condition, int>::type = 0

///////////////////////////////////////////////////////////////////////////////
// conjunction disjunction
///////////////////////////////////////////////////////////////////////////////

template <typename... Traits>
struct conjunction : std::true_type {};

template <typename First, typename... Rest>
struct conjunction<First, Rest...> : std::bool_constant<First::value && conjunction<Rest...>::value> {};

template <typename... Traits>
struct disjunction : std::false_type {};

template <typename First, typename... Rest>
struct disjunction<First, Rest...> : std::bool_constant<First::value || disjunction<Rest...>::value> {};

///////////////////////////////////////////////////////////////////////////////
// is any of
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Types>
struct is_any_of : disjunction<std::is_same<T, Types>...> {};

///////////////////////////////////////////////////////////////////////////////
// is iterator
///////////////////////////////////////////////////////////////////////////////

template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<T, void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

///////////////////////////////////////////////////////////////////////////////
// element type
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <typename T, typename = void>
struct value_type_impl { using type = void; };

template <typename T>
struct value_type_impl<T, void_t<typename T::value_type>>
{
    using type = typename T::value_type;
};

template <typename T>
struct value_type_impl<T*> { using type = T; };

template <typename T, size_t N>
struct value_type_impl<T[N]> { using type = T; };

} // namespace __detail

template <typename T>
struct value_type
{
    using type = typename __detail::value_type_impl<typename std::remove_cv<T>::type>::type;
};

} // namespace type_traits
} // namespace vx