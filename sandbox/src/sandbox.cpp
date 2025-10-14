#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/app.hpp"
#include "vertex/app/hints/hints.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/math/core/util.hpp"

using namespace vx;

static void run_app()
{
    app::hint::set_hint(app::hint::HINT_VIDEO_ALLOW_SCREEN_SAVER, "true");
    const bool allow_screen_saver = app::hint::get_hint_boolean(app::hint::HINT_VIDEO_ALLOW_SCREEN_SAVER, false);

    while (true)
    {
        app::event::pump_events(true);
        os::sleep(time::milliseconds(100));
    }
}

int main(int argc, char* argv[])
{
    if (app::init() && app::init_subsystem(app::INIT_VIDEO))
    {
        run_app();
    }

    app::quit();
}
