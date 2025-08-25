#include <iostream>

#include "vertex/app/main.hpp"
#include "vertex/app/app.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    if (app::init(app::NONE))
    {
        app::quit();
    }
}
