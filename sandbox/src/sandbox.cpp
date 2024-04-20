#include "sandbox/sandbox.h"

#include "vertex/math/math/util/to_string.h"
#include "vertex/image/io_load.h"
#include "vertex/app/display/window.h"
#include "vertex/system/error.h"

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
                //window.minimize();
                VX_LOG_INFO << "mouse click: " << math::vec2i(e.mouse_button.x, e.mouse_button.y);
                
                if (window.get_cursor().shape() == app::cursor::SHAPE_ARROW)
                {
                    window.set_cursor(cursor);
                }
                else
                {
                    window.set_cursor(app::cursor::SHAPE_ARROW);
                }
            }
            if (e.type == app::event_type::MOUSE_MOVE)
            {
                //window.set_min_size(window.get_size());
                //VX_LOG_INFO << "mouse move: " << math::vec2i(e.mouse_move.x, e.mouse_move.y);
            }
            if (e.type == app::event_type::MOUSE_HOVER)
            {
                VX_LOG_INFO << "mouse enter/exit: " << e.mouse_hover.value;
            }
            if (e.type == app::event_type::MOUSE_SCROLL)
            {
                VX_LOG_INFO << "mouse scroll: " << e.mouse_scroll.delta;
            }
        }
    
        //VX_LOG_INFO << "hovered: " << window.is_hovered();
    }

    return 0;
}
