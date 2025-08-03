#pragma once

#include "vertex/math/color/types.hpp"
#include "vertex/math/core/util/to_string.hpp"

namespace vx {
namespace math {

template <typename T>
VX_FORCE_INLINE std::string to_string(const color_t<T>& c)
{
    char const* fmt = __detail::literal_format<T>();

    constexpr size_t buffer_size_1 = __detail::vec_calculate_buffer_size_1<4, T>();
    constexpr size_t buffer_size_2 = __detail::vec_calculate_buffer_size_2<4, T>();

    char buffer_1[buffer_size_1]{};
    char buffer_2[buffer_size_2]{};

    const char* f = "color{ %s, %s, %s, %s }";
    __detail::format<buffer_size_1>(buffer_1, f, fmt, fmt, fmt, fmt);
    __detail::format<buffer_size_2>(buffer_2, buffer_1, c.r, c.g, c.b, c.a);

    return buffer_2;
}

///////////////////////////////////////////////////////////////////////////////
// ostream operators
///////////////////////////////////////////////////////////////////////////////

template <typename Elem, typename Traits, typename T>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& os, const color_t<T>& c)
{
    return (os << to_string(c));
}

} // namespace math
} // namespace vx