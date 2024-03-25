#pragma once

#if defined(VX_APPLICATION)

#include <vector>

#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace app {
namespace monitor {

using monitor = void*;

monitor get_primary_monitor();
std::vector<monitor> list_monitors();

const char* get_monitor_name(monitor monitor);
math::vec2i get_monitor_position(monitor monitor);
math::vec2i get_monitor_physical_size(monitor monitor);

}
}
}

#endif