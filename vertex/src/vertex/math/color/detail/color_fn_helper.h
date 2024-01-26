#pragma once

namespace vx {
namespace math {
namespace detail {

inline constexpr char helper_rgba_swizzle(char c)
{
    switch (c)
    {
        case 'r': return static_cast<char>(0);
        case 'g': return static_cast<char>(1);
        case 'b': return static_cast<char>(2);
        case 'a': return static_cast<char>(4);
        default:  return c;
    }
}

}
}
}