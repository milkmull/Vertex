#pragma once

#include <sstream>
#include <iomanip>

#include "../type/base/base_types.h"

namespace vx {

template <typename T>
inline std::string to_string(const math::rect_t<T>& r)
{
    std::ostringstream oss;
    oss << "rect(" << r.position.x << ", " << r.position.y << ", " << r.size.x << ", " << r.size.y << ')';
    return oss.str();
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const math::rect_t<T>& c)
{
    os << to_string(c);
    return os;
}

}