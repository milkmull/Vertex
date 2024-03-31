#include "sandbox/sandbox.h"

#include "vertex/math/math/util/to_string.h"
#include "vertex/tools/string/string_fn.h"
#include "vertex/image/io_load.h"
#include "vertex/app/display/window.h"

static void error_callback(vx::app::error_code error, const char* message)
{
    VX_LOG_ERROR << message;
}

int main()
{
    using namespace vx;

    vx::app::set_error_callback(error_callback);

    img::error_code err;
    img::image icon = img::load("../../assets/michael.png", err);

    math::vec2i position = { 200, 200 };
    math::vec2i size = { 800, 600 };
    std::string title = u8"English 日本語 русский язык 官話";
    auto window = vx::app::window(title, size, position);

    app::event e;
    bool running = window.is_open();

    window.set_icon(icon);

    window.set_resizable(false);

    while (running)
    {
        while (window.pop_event(e))
        {
            if (e.type == app::event_type::WINDOW_CLOSE)
            {
                running = false;
                break;
            }
            if (e.type == app::event_type::WINDOW_RESIZE)
            {
                VX_LOG_INFO << "resize: " << math::vec2i(e.window_resize.width, e.window_resize.height);
                break;
            }
            if (e.type == app::event_type::WINDOW_MOVE)
            {
                VX_LOG_INFO << "move: " << math::vec2i(e.window_move.x, e.window_move.y);
                break;
            }
            if (e.type == app::event_type::WINDOW_FOCUS)
            {
                VX_LOG_INFO << "focus: " << e.window_focus.value;
                break;
            }

            if (e.type == app::event_type::MOUSE_BUTTON_DOWN)
            {
                window.set_min_size(window.get_size());
                VX_LOG_INFO << "size: " << math::vec2i(window.get_size().x, window.get_size().y);
            }

            if (e.type == app::event_type::WINDOW_SHOW)
            {
                VX_LOG_INFO << "show: " << e.window_show.value;
                break;
            }
        }
    }

    return 0;
}
