#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/app.hpp"
#include "vertex/app/hints/hints.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/math/core/util.hpp"
#include "vertex/math/geometry/2d/types/circle.hpp"

using namespace vx;

static bool display_added_event_watcher(const app::event::event& e, void*)
{
    if (e.type != app::event::display_added)
    {
        return false;
    }

    std::cout << (int)(e.type) << std::endl;

    const app::video::display d = e.display_event.comon.display_id;

    if (d.is_connected())
    {
        std::cout << "display " << d.id() << std::endl;

        const auto modes = d.list_modes();

        for (size_t i = 0; i < modes.size(); ++i)
        {
            std::cout << "mode " << i << ": " << (int)modes[i].pixel_format << std::endl;
        }

        std::cout << std::endl;
    }

    app::event::remove_event_watch(display_added_event_watcher, nullptr);
    return false;
}

static void run_app()
{
    app::hint::set_hint(app::hint::HINT_VIDEO_ALLOW_SCREEN_SAVER, "true");
    const bool allow_screen_saver = app::hint::get_hint_boolean(app::hint::HINT_VIDEO_ALLOW_SCREEN_SAVER, false);

    //while (true)
    {
        app::event::pump_events(true);
        os::sleep(time::milliseconds(100));
    }
}

int main(int argc, char* argv[])
{
    constexpr auto x = math::g2::circle_t<int>().radius;


    if (app::init() && app::init_subsystem(app::init_flags::events))
    {
        app::event::add_event_watch(display_added_event_watcher, nullptr);

        if (app::init_subsystem(app::init_flags::video))
        {
            run_app();
        }
    }

    app::quit();
}