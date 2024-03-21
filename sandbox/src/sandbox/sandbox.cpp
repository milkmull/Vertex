#include "sandbox/sandbox.h"

#include "vertex/app/window/window.h"

int main()
{
    using namespace vx;

    app::window w;

    while (w.is_open())
    {
        w.poll_events();
    }

    return 0;
}