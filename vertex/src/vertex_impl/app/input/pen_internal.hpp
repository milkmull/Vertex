#pragma once

#include "vertex/app/input/pen.hpp"

namespace vx {
namespace app {
namespace pen {

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_pen_c.h

///////////////////////////////////////////////////////////////////////////////
// pen
///////////////////////////////////////////////////////////////////////////////

class pen_instance
{
private:

    struct pen_data
    {
    };

    pen_data data;

public:

    bool init();
    void quit();

public:

};

} // namespace pen
} // namespace app
} // namespace vx