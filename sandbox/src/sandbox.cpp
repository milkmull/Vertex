#include "sandbox/sandbox.h"

#include "vertex/system/error.h"

#include "vertex/app/video/window.h"

#include "vertex/math/random/rng.h"

int main()
{
    using namespace vx::app;

    vx::math::rng rng;

    video::init();

    vx::app::event e;

    video::display* d = video::get_primary_display();

    auto& modes = d->list_modes();
    video::display_mode mode = rng.choice(modes.begin(), modes.end());

    d->set_mode(mode);

    while (true)
    {
        vx::app::video::update_displays();

        if (vx::app::event::poll_event(e))
        {
            switch (e.type)
            {
                case event_type::DISPLAY_ADDED:
                {
                    std::cout << "Display Added: " << e.display_event.display_id << std::endl;
                    break;
                }
                case event_type::DISPLAY_REMOVED:
                {
                    std::cout << "Display Removed: " << e.display_event.display_id << std::endl;
                    break;
                }
                case event_type::DISPLAY_MOVED:
                {
                    std::cout << "Display Moved: " << e.display_event.display_id << std::endl;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
    }

    while (vx::app::event::poll_event(e))
    {
        std::cout << e.type << std::endl;
    }

    vx::app::video::quit();

    return 0;
}
