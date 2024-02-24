#pragma once

#include <type_traits>
#include <iterator>

namespace vx {
namespace tools {

// =============== iterators ===============

template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<T, std::void_t<typename std::iterator_traits<T>::iterator_category>> : std::true_type {};

}
}