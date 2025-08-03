#pragma once

#include "vertex/config/type_traits.hpp"
#include "vertex/util/string/string_type.hpp"

namespace vx {
namespace type_traits {

///////////////////////////////////////////////////////////////////////////////
// is char iterator
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <typename T, typename = void>
struct is_char_iterator_impl : std::false_type {};

template <typename T>
struct is_char_iterator_impl<T, void_t<typename std::iterator_traits<T>::value_type>>
    : is_char<typename std::iterator_traits<T>::value_type> {};

template <typename T, size_t N>
struct is_char_iterator_impl<T[N]> : is_char<T> {};
 
} // namespace __detail

template <typename T>
struct is_char_iterator : __detail::is_char_iterator_impl<typename std::remove_cv<T>::type> {};

///////////////////////////////////////////////////////////////////////////////
// string type
///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct is_string : std::false_type {};

template <typename Elem, typename Traits, typename Alloc>
struct is_string<std::basic_string<Elem, Traits, Alloc>> : std::true_type {};

#if defined(__cpp_lib_string_view)

template <typename Elem, typename Traits>
struct is_string<std::basic_string_view<Elem, Traits>> : std::true_type {};

#endif // __cpp_lib_string_view

///////////////////////////////////////////////////////////////////////////////
// string-like
///////////////////////////////////////////////////////////////////////////////

namespace __detail {

template <typename T>
struct is_string_like_impl : is_char_iterator<typename std::decay<T>::type> {};

template <typename Elem, typename Traits, typename Alloc>
struct is_string_like_impl<std::basic_string<Elem, Traits, Alloc>> : std::true_type {};

#if defined(__cpp_lib_string_view)

template <typename Elem, typename Traits>
struct is_string_like_impl<std::basic_string_view<Elem, Traits>> : std::true_type {};

#endif // __cpp_lib_string_view

} // namespace __detail

template <typename T>
struct is_string_like : __detail::is_string_like_impl<typename std::remove_cv<T>::type> {};

} // namespace type_traits
} // namespace vx