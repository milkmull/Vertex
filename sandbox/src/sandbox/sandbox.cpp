#include "sandbox/sandbox.h"

#include "vertex/app/window/window.h"
#include "vertex/image/io_load.h"

static bool open = true;

static void callback(vx::app::event& e)
{
    switch (e.type)
    {
        case vx::app::event_type::WINDOW_CLOSE:
            VX_LOG_INFO << "close event" << std::endl;
            open = false;
            break;

        case vx::app::event_type::WINDOW_RESIZE:
            VX_LOG_INFO << "resize event: { " << e.window_resize.width << ", " << e.window_resize.height << " }" << std::endl;
            break;

        case vx::app::event_type::WINDOW_MOVE:
            VX_LOG_INFO << "move event: { " << e.window_move.x << ", " << e.window_move.y << " }" << std::endl;
            break;

        case vx::app::event_type::WINDOW_ICONIFY:
            VX_LOG_INFO << "iconify event: " << e.window_iconify.value << std::endl;
            break;

        case vx::app::event_type::WINDOW_MAXIMIZE:
            VX_LOG_INFO << "maximize event: " << e.window_maximize.value << std::endl;
            break;

        case vx::app::event_type::WINDOW_FOCUS:
            VX_LOG_INFO << "focus event: " << e.window_focus.value << std::endl;
            break;

        default:
            break;
    }
}

int main()
{
    using namespace vx;

    app::window::window_specs specs;
    specs.size = { 640, 480 };
    
    img::error_code err;
    img::image i = img::load("../../assets/michael.png", err);
    
    app::window w(specs);
    w.set_icon(i);
    w.set_min_size({ 100, 100 });

    w.set_event_callback(callback);
    
    while (!w.should_close())
    {
        w.poll_events();
    }

    return 0;
}