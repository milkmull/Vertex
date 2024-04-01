#include "../display/win32_window.h"

namespace vx {
namespace app {
namespace mouse {

math::vec2i get_mouse_position()
{
    POINT point;
    GetCursorPos(&point);
    return math::vec2i(point.x, point.y);
}

void set_mouse_position(const math::vec2i& position)
{
    SetCursorPos(position.x, position.y);
}

}
}
}