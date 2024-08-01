#include "sandbox/sandbox.h"

#include "vertex/system/error.h"

#include "vertex/app/event/event.h"
#include "vertex/app/video/window.h"

int main()
{
    using namespace vx::app;

    vx::app::event e;
    video::init();

    const video::display* d = video::get_primary_display();

    video::window_config config;
    config.size = vx::math::vec2i(500);
    config.center_on_display = true;
    config.title = "window";
    config.resizable = true;
    //config.borderless = true;
    video::window* w = video::create_window(config);
    //w->set_min_size({ 10, 10 });
    //w->set_max_size({ 300, 300 });
    w->lock_aspect_ratio(1.0f);

    //w->set_size({ 300, 300 });

    bool running = true;
    while (running)
    {
        if (vx::app::event::poll_event(e))
        {
            switch (e.type)
            {
                case event_type::WINDOW_CLOSE_REQUESTED:
                {
                    int left, right, bottom, top;
                    w->get_border_size(left, right, bottom, top);
                    std::cout << "{ " << left << ' ' << right << ' ' << bottom << ' ' << top << " }" << std::endl;

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
