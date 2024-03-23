#include "sandbox/sandbox.h"

#include "vertex/app/window/window.h"
#include "vertex/image/io_load.h"

int main()
{
    using namespace vx;

    app::window::window_specs specs;
    specs.size = { 640, 480 };

    img::error_code err;
    img::image i = img::load("../../assets/michael.png", err);

    app::window w(specs);
    w.set_icon(i);
    w.set_min_size({ 100, 100 });
    
    while (w.is_open())
    {
        w.poll_events();
    }

    return 0;
}