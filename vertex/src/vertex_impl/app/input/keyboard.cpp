#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/input/keyboard_internal.hpp"

namespace vx {
namespace app {
namespace keyboard {

//-------------------------------------------------------------------------
// Initialization
//-------------------------------------------------------------------------

bool keyboard_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    return true;
}

void keyboard_instance::quit()
{
}

} // namespace keyboard
} // namespace app
} // namespace vx