#include "sandbox/sandbox.h"

#include "vertex/app/init.h"
#include "vertex/app/display/window.h"
#include "vertex/image/io_load.h"

#include "vertex/app/display/monitor.h"

static bool open = true;

//static void callback(vx::app::event& e)
//{
//    switch (e.type)
//    {
//        case vx::app::event_type::WINDOW_CLOSE:
//            VX_LOG_INFO << "close event" << std::endl;
//            open = false;
//            break;
//
//        case vx::app::event_type::WINDOW_RESIZE:
//            VX_LOG_INFO << "resize event: { " << e.window_resize.width << ", " << e.window_resize.height << " }" << std::endl;
//            break;
//
//        case vx::app::event_type::WINDOW_MOVE:
//            VX_LOG_INFO << "move event: { " << e.window_move.x << ", " << e.window_move.y << " }" << std::endl;
//            break;
//
//        case vx::app::event_type::WINDOW_ICONIFY:
//            VX_LOG_INFO << "iconify event: " << e.window_iconify.value << std::endl;
//            break;
//
//        case vx::app::event_type::WINDOW_MAXIMIZE:
//            VX_LOG_INFO << "maximize event: " << e.window_maximize.value << std::endl;
//            break;
//
//        case vx::app::event_type::WINDOW_FOCUS:
//            VX_LOG_INFO << "focus event: " << e.window_focus.value << std::endl;
//            break;
//
//        case vx::app::event_type::KEY_DOWN:
//            VX_LOG_INFO << "key down event: " << (char)e.key.key << std::endl;
//            break;
//
//        case vx::app::event_type::KEY_UP:
//            VX_LOG_INFO << "key up event: " << (char)e.key.key << std::endl;
//            break;
//
//        case vx::app::event_type::TEXT_INPUT:
//            VX_LOG_INFO << "text event: " << e.text_input.codepoint;
//            break;
//
//        default:
//            break;
//    }
//}

int main()
{
    using namespace vx;

    img::error_code err;

    img::image i;
    i = img::load("../../assets/michael.png", err);

    //vx::app::init();
    //
    //vx::app::joystick::joystick stick = vx::app::joystick::JOYSTICK_1;
    //
    //while (vx::app::joystick::is_present(stick))
    //{
    //    vx::app::joystick::state state;
    //    state = vx::app::joystick::get_state(stick);
    //
    //    for (int i = 0; i < state.axes.size(); ++i)
    //    {
    //        std::cout << i << ": " << state.axes[i] << ' ';
    //    }
    //
    //    if (state.buttons[vx::app::joystick::button::A])
    //    {
    //        break;
    //    }
    //
    //    std::cout << std::endl;
    //}
    //
    //vx::app::terminate();

    return 0;
}