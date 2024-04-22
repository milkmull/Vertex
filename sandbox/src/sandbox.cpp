#include "sandbox/sandbox.h"

#include "vertex/math/math/util/to_string.h"
#include "vertex/image/io_load.h"
#include "vertex/app/display/window.h"
#include "vertex/system/error.h"

static void print_event(const vx::app::event& e)
{
    switch (e.type)
    {
        case vx::app::event_type::WINDOW_CLOSE:
        {
            VX_LOG_INFO << "WINDOW_CLOSE";
            return;
        }
        case vx::app::event_type::WINDOW_RESIZE:
        {
            VX_LOG_INFO << "WINDOW_RESIZE { " << e.window_resize.width << ", " << e.window_resize.height << " }";
            return;
        }
        case vx::app::event_type::WINDOW_MINIMIZE:
        {
            VX_LOG_INFO << "WINDOW_MINIMIZE";
            return;
        }
        case vx::app::event_type::WINDOW_MAXIMIZE:
        {
            VX_LOG_INFO << "WINDOW_MAXIMIZE";
            return;
        }
        case vx::app::event_type::WINDOW_MOVE:
        {
            VX_LOG_INFO << "WINDOW_MOVE { " << e.window_move.x << ", " << e.window_move.y << " }";
            return;
        }
        case vx::app::event_type::WINDOW_FOCUS:
        {
            VX_LOG_INFO << "WINDOW_FOCUS " << e.window_focus.value;
            return;
        }
        case vx::app::event_type::WINDOW_SHOW:
        {
            VX_LOG_INFO << "WINDOW_SHOW " << e.window_show.value;
            return;
        }
        case vx::app::event_type::KEY_DOWN:
        case vx::app::event_type::KEY_REPEAT:
        case vx::app::event_type::KEY_UP:
        case vx::app::event_type::TEXT_INPUT:
        {
            return;
        }
        case vx::app::event_type::MOUSE_MOVE:
        {
            VX_LOG_INFO << "MOUSE_MOVE { " << e.mouse_move.x << ", " << e.mouse_move.y << " }";
            return;
        }
        case vx::app::event_type::MOUSE_HOVER:
        {
            VX_LOG_INFO << "MOUSE_HOVER " << e.mouse_hover.value;
            return;
        }
        case vx::app::event_type::MOUSE_BUTTON_DOWN:
        {
            VX_LOG_INFO << "MOUSE_BUTTON_DOWN " << e.mouse_button.button << " { " << e.mouse_button.x << ", " << e.mouse_button.y << " }";
            return;
        }
        case vx::app::event_type::MOUSE_BUTTON_UP:
        {
            VX_LOG_INFO << "MOUSE_BUTTON_UP " << e.mouse_button.button << " { " << e.mouse_button.x << ", " << e.mouse_button.y << " }";
            return;
        }
        case vx::app::event_type::MOUSE_SCROLL:
        {
            VX_LOG_INFO << "MOUSE_SCROLL " << e.mouse_scroll.wheel << " { " << e.mouse_scroll.x << ", " << e.mouse_scroll.y << " }";
            return;
        }
        case vx::app::event_type::JOYSTICK_CONNECTION:
        {
            return;
        }
    }
}

int main()
{
    using namespace vx;

    bool err;
    img::image icon = img::load("../../assets/michael.png", err);
    
    math::vec2i position = { 200, 200 };
    math::vec2i size = { 800, 600 };
    std::string title = u8"English 日本語 русский язык 官話";
    auto window = vx::app::window(title, size, position);
    
    app::event e;
    bool running = window.is_open();
    
    window.set_icon(icon.data(), icon.size());
    //window.set_cursor_visibility(false);
    
    app::cursor cursor = app::cursor::make_custom(icon.data(), icon.size());
    //window.set_cursor(cursor);
    
    //window.set_resizable(false);

    auto last = std::chrono::system_clock::now();
    bool timer_set = false;
    
    while (running)
    {
        if (timer_set && (std::chrono::system_clock::now() - last).count() > 50000000)
        {
            window.maximize();
            timer_set = false;
        }

        while (window.pop_event(e))
        {
            print_event(e);

            if (e.type == app::event_type::WINDOW_CLOSE)
            {
                running = false;
                break;
            }
            if (e.type == app::event_type::MOUSE_BUTTON_DOWN)
            {
                if (!window.is_maximized())
                {
                    last = std::chrono::system_clock::now();
                    timer_set = true;
                    window.minimize();
                }
                else
                {
                    running = false;
                    break;
                }
            }
        }
    }

    return 0;
}
