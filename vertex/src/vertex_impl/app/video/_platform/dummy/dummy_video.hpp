#pragma once

#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace video {

//=============================================================================
// video_impl
//=============================================================================

class video_instance_impl
{
public:

    video_instance_impl() = default;
    ~video_instance_impl() = default;

    video_instance_impl(const video_instance_impl&) = delete;
    video_instance_impl& operator=(const video_instance_impl&) = delete;

    video_instance_impl(video_instance_impl&&) noexcept = delete;
    video_instance_impl& operator=(video_instance_impl&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(video_instance* owner)
    {
        if (video)
        {
            quit();
        }
        video = owner;

        if (!add_dummy_display())
        {
            goto failed;
        }

        return true;

        failed:
        {
            quit();
            return false;
        }
    }

    void quit()
    {
        video = nullptr;
    }

    //=============================================================================
    // displays
    //=============================================================================

    bool add_dummy_display()
    {
        // add fake 32-bpp desktop mode
        display_mode mode;
        mode.pixel_format = pixel::pixel_format::xrgb_8888;
        mode.resolution.x = 1024;
        mode.resolution.y = 768;

        return video->add_basic_display(mode);
    }

    //=============================================================================
    // data
    //=============================================================================

    video_instance* video = nullptr;
};

//=============================================================================
// display_mode_instance_impl
//=============================================================================

class display_mode_instance_impl {};

//=============================================================================
// display_impl
//=============================================================================

class display_instance_impl {};

} // namespace video
} // namespace app
} // namespace vx