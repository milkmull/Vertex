#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/app.hpp"
#include "vertex/app/hints/hints.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/math/core/util.hpp"
#include "vertex/math/geometry/2d/types/circle.hpp"

using namespace vx;

static void run_app()
{
    app::hint::set_hint(app::hint::video_allow_screen_saver, "true");
    const bool allow_screen_saver = app::hint::get_hint_boolean(app::hint::video_allow_screen_saver, false);

    bool running = true;

    while (running)
    {
        app::event::pump_events();
        os::sleep(time::milliseconds(100));
    
        app::event::event e;
        while (app::event::poll_event(e))
        {
            if (e.type == app::event::app_quit)
            {
                std::cout << "quit detected" << std::endl;
                running = false;
                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    if (!app::init())
    {
        return 1;
    }

    if (!app::init_subsystem(app::init_flags::events))
    {
        return 1;
    }

    if (!app::init_subsystem(app::init_flags::video))
    {
        return 1;
    }

    app::quit();
}