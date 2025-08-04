#pragma once

#include <sstream>
#include <iomanip>

#include "../type/base/base_types.h"

namespace vx {

template <typename T>
inline std::string to_string(const math::color_t<T>& c)
{
    std::ostringstream oss;
    oss << "rgba(" << +c.r << ", " << +c.g << ", " << +c.b << ", " << +c.a << ')';
    return oss.str();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const math::color_t<T>& c)
{
    os << to_string(c);
    return os;
}

}