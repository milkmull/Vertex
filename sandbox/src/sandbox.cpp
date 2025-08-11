#include <iostream>

#include "vertex/app/video/video.hpp"
#include "vertex/math/core/util.hpp"
#include "vertex/os/time.hpp"
#include "vertex/util/random.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    if (app::video::init())
    {
        app::video::update_displays();
    }

    app::video::quit();
}
