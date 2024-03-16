#pragma once

#include <initializer_list>

#include "../detail/common.h"

namespace vx {
namespace math {

// =============== bit_count ===============

template <typename T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value, bool>::type = true>
inline constexpr T bit_count(T x)
{
    return 0;
}



}
}
