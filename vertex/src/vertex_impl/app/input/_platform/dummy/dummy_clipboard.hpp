#pragma once

#include "vertex_impl/app/input/clipboard_internal.hpp"

namespace vx {
namespace app {
namespace clipboard {

//=============================================================================
// clipboard_instance_impl
//=============================================================================

class clipboard_instance_impl
{
public:

    clipboard_instance_impl() = default;
    ~clipboard_instance_impl() = default;

    clipboard_instance_impl(const clipboard_instance_impl&) = delete;
    clipboard_instance_impl& operator=(const clipboard_instance_impl&) = delete;

    clipboard_instance_impl(clipboard_instance_impl&&) noexcept = delete;
    clipboard_instance_impl& operator=(clipboard_instance_impl&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(clipboard_instance* owner)
    {
        if (clipboard)
        {
            quit();
        }
        clipboard = owner;
        return true;
    }

    void quit()
    {
        clipboard = nullptr;
    }

    //=============================================================================
    // data
    //=============================================================================

    clipboard_instance* clipboard = nullptr;
};

} // namespace clipboard
} // namespace app
} // namespace vx