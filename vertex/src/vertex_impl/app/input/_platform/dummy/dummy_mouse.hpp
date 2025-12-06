#pragma once

#include "vertex_impl/app/input/mouse_internal.hpp"

namespace vx {
namespace app {
namespace mouse {

//=============================================================================
// mouse_instance_impl
//=============================================================================

class mouse_instance_impl
{
public:

    mouse_instance_impl() = default;
    ~mouse_instance_impl() = default;

    mouse_instance_impl(const mouse_instance_impl&) = delete;
    mouse_instance_impl& operator=(const mouse_instance_impl&) = delete;

    mouse_instance_impl(mouse_instance_impl&&) noexcept = delete;
    mouse_instance_impl& operator=(mouse_instance_impl&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(mouse_manager* owner)
    {
        if (mouse)
        {
            quit();
        }
        mouse = owner;
        return true;
    }

    void quit()
    {
        mouse = nullptr;
    }

    //=============================================================================
    // Relative Mode
    //=============================================================================

    bool set_relative_mode(bool) { return true; }

    //=============================================================================
    // data
    //=============================================================================

    mouse_manager* mouse = nullptr;
};

} // namespace mouse
} // namespace app
} // namespace vx