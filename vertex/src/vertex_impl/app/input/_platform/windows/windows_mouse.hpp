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

    bool init(mouse_instance* owner);
    void quit();

    ~mouse_instance_impl() { quit(); }

    //=============================================================================
    // 
    //=============================================================================

    buttons get_global_state(float* x, float* y) const;

    time::time_point get_double_click_time() const;
    int32_t get_double_click_radius() const;

    //=============================================================================
    // data
    //=============================================================================

    mouse_instance* mouse;
    mouse_impl_data data;
};

} // namespace mouse
} // namespace app
} // namespace vx