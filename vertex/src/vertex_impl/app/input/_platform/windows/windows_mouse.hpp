#pragma once

#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace mouse {

//=============================================================================
// data
//=============================================================================

struct mouse_impl_data
{

};

//=============================================================================
// mouse_instance_impl
//=============================================================================

class mouse_instance_impl
{
public:

    //=============================================================================
    // lifetime
    //=============================================================================

    bool init(mouse_instance* owner) { return true; }
    void quit() {}

    ~mouse_instance_impl() { quit(); }

    //=============================================================================
    // 
    //=============================================================================

    button get_global_state(float* x, float* y) const { return button::none; }

    time::time_point get_double_click_time() const { return time::zero(); }

    //=============================================================================
    // data
    //=============================================================================

    mouse_instance* mouse;
    mouse_impl_data data;
};

} // namespace mouse
} // namespace app
} // namespace vx