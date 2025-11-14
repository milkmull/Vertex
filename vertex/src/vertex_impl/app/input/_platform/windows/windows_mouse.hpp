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

    void apply_system_scale(time::time_point t, const video::window_instance* w, mouse_id id, float* x, float* y) const {}

    void warp(const video::window_instance* w, float x, float y) {}
    bool warp_global(float x, float y) { return false; }

    bool set_relative_mode(bool enabled) { return false; }

    cursor_id create_cursor(const argb_surface& surf, int hot_x, int hot_y) { return invalid_id; }
    cursor_id create_system_cursor(cursor_shape shape) { return invalid_id; }
    void show_cursor(cursor_id id) {}

    bool capture_mouse(video::window_instance* capture_window) { return false; }

    //=============================================================================
    // data
    //=============================================================================

    mouse_instance* mouse;
    mouse_impl_data data;
};

} // namespace mouse
} // namespace app
} // namespace vx