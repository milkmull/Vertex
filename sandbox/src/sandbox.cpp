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
            if (e.type == app::event_type::MOUSE_BUTTON_DOWN)
            {
                window.set_max_size(window.get_size());
            }
            if (e.type == app::event_type::MOUSE_MOVE)
            {
                //window.set_min_size(window.get_size());
                VX_LOG_INFO << "mouse move: " << math::vec2i(e.mouse_move.x, e.mouse_move.y);
            }
        }
    }

    return 0;
}
