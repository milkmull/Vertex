#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/math/core/util.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    if (app::video::init())
    {
        std::cout << (math::vec4)app::video::get_desktop_area() << std::endl;
    }

    app::video::quit();
}
