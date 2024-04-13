#include "../../win32_header.h"
#include "vertex/app/input/mouse.h"

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

bool is_mouse_button_down(button button)
{
    int virtual_key = 0;

    switch (button)
    {
        case button::BUTTON_LEFT:
            virtual_key = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
            break;
        case button::BUTTON_RIGHT:
            virtual_key = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON;
            break;
        case button::BUTTON_MIDDLE:
            virtual_key = VK_MBUTTON;
            break;
        case button::BUTTON_EXTRA_1:
            virtual_key = VK_XBUTTON1;
            break;
        case button::BUTTON_EXTRA_2:
            virtual_key = VK_XBUTTON2;
            break;
        default:
            break;
    }

    return GetAsyncKeyState(virtual_key) & 0x8000;
}

}
}
}