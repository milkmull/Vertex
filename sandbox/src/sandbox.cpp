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
        std::cout << (int)app::video::get_dpi_awareness() << std::endl;
        std::cout << (int)app::video::get_system_theme() << std::endl;
    }

    app::video::quit();
}
