#pragma once

#include <type_traits>
#include <iterator>

namespace vx {
namespace detail {

// =============== iterators ===============

template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

// =============== containers ===============

template <typename T, typename = void>
struct is_stl_container : std::false_type {};

template <typename T>
struct is_stl_container<
    T,
    std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    typename T::value_type,
    typename T::size_type,
    typename T::iterator,
    typename T::const_iterator
    >
> : std::true_type {};

}
}