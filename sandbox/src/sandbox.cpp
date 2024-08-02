#include "sandbox/sandbox.h"

#include "vertex/system/error.h"

#include "vertex/app/event/event.h"
#include "vertex/app/video/window.h"

#include "vertex/image/io_load.h"

int main()
{
    using namespace vx::app;

    vx::app::event e;
    video::init();

    bool status;
    vx::img::image i = vx::img::load("../../assets/michael.png", status);

    VX_LOG_INFO << "status: " << status;

    const video::display* d = video::get_primary_display();

    video::window_config config;
    config.size = vx::math::vec2i(500);
    config.center_on_display = true;
    config.title = "window";
    config.resizable = true;
    config.borderless = false;
    video::window* w = video::create_window(config);
    w->set_icon(i.data(), i.size());
    //w->set_min_size({ 100, 100 });
    //w->set_max_size({ 300, 300 });
    w->lock_aspect_ratio(16.0f / 9.0f);

    //w->set_size({ 300, 700 });

    bool running = true;
    while (running)
    {
        if (vx::app::event::poll_event(e))
        {
            switch (e.type)
            {
                case event_type::WINDOW_MOVED:
                {
                    w->set_fullscreen(true);
                    break;
                }
                case event_type::WINDOW_CLOSE_REQUESTED:
                {
                    running = false;
                    VX_FALLTHROUGH;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    vx::app::video::quit();

    return 0;
}
