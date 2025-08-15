#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/video/window.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/math/core/util.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    if (app::video::init())
    {
        app::video::window_config wcfg;
        wcfg.size = { 500, 500 };
        wcfg.position = { 100, 100 };
        wcfg.resizable = true;

        auto* window = app::video::create_window(wcfg);

        if (window)
        {
            bool running = true;

            while (running)
            { 
                app::event::event e;
                while (app::event::poll_event(e))
                {
                    if (e.type == app::event::WINDOW_CLOSE_REQUESTED)
                    {
                        running = false;
                        break;
                    }
                }
            }

            app::video::destroy_window(*window);
        }
    }

    app::video::quit();
}
