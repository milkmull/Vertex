#include "sandbox/sandbox.h"

#include "vertex/system/error.h"

#include "vertex/app/video/window.h"

int main()
{
    using namespace vx;

    vx::app::video_device::init();

    auto displays = app::video_device::list_displays();
    auto video_modes = app::video_device::list_display_modes(displays.front());

    app::window::config wconfig;
    //wconfig.hint.maximized = true;
    //wconfig.hint.scale_to_monitor = true;
    wconfig.hint.borderless = true;
    wconfig.size = { 1024, 576 };

    app::window w(wconfig);// , displays.front());

    std::this_thread::sleep_for(std::chrono::seconds(5));

    vx::app::video_device::quit();

    return 0;
}
