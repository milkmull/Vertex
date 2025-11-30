#pragma once

#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace video {

//=============================================================================
// window_instance_impl
//=============================================================================

class window_instance_impl
{
public:

    window_instance_impl() = default;
    ~window_instance_impl() { destroy(); }

public:

    //=============================================================================
    // creation
    //=============================================================================

    bool create(window_instance* owner)
    {
        if (window)
        {
            destroy();
        }
        window = owner;
        return true;
    }

    void destroy()
    {
        window = nullptr;
    }

    //=============================================================================
    // position and size
    //=============================================================================

    bool set_position()
    {
        window->send_moved(
            window->data.pending_rect.position.x,
            window->data.pending_rect.position.y
        );
        return true;
    }

    bool set_size()
    {
        window->send_resized(
            window->data.pending_rect.size.x,
            window->data.pending_rect.size.y
        );
        return true;
    }

    //=============================================================================
    // data
    //=============================================================================

    window_instance* window = nullptr;
};

} // namespace video
} // namespace app
} // namespace vx