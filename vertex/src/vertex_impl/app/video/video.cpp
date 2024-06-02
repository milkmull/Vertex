#include "vertex/config.h"
#include "vertex/app/video/video.h"
#include "vertex/app/video/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/video/win32_video.h"

#endif

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// display
///////////////////////////////////////////////////////////////////////////////

bool video::display::get_mode(display_mode& mode) const
{
    return m_impl->get_mode(mode);
}

std::vector<video::display_mode> video::display::list_modes() const
{
    return m_impl->list_modes();
}

video::display_orientation video::display::get_orientation() const
{
    return m_impl->get_orientation();
}

math::vec2 video::display::get_content_scale() const
{
    return m_impl->get_content_scale();
}

bool video::display::get_bounds(math::recti& bounds) const
{
    return m_impl->get_bounds(bounds);
}

bool video::display::get_work_area(math::recti& work_area) const
{
    return m_impl->get_work_area(work_area);
}

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

bool video::init()
{
    return video_impl::init();
}

void video::quit()
{
    video_impl::quit();
}

video::process_dpi_awareness video::get_dpi_awareness()
{
    return video_impl::get_dpi_awareness();
}

void video::update_displays()
{
    video_impl::update_displays();
}

const video::display* video::get_primary_display()
{
    return video_impl::get_primary_display();
}

const std::vector<const video::display*> video::list_displays()
{
    return video_impl::list_displays();
}

const video::display* video::get_display_for_point(const math::vec2i& p)
{
    const display* closest = nullptr;
    int32_t closest_dist = 0x7FFFFFFF;

    for (const display* display : list_displays())
    {
        math::recti bounds;
        if (!display->get_bounds(bounds))
        {
            continue;
        }

        if (math::contains(bounds, p))
        {
            return display;
        }

        math::vec2i c = bounds.clip(p);
        math::vec2i pc = c - p;

        int32_t dist = (pc.x * pc.x) + (pc.y * pc.y);
        if (dist < closest_dist)
        {
            closest = display;
            closest_dist = dist;
        }
    }

    return closest;
}

const video::display* video::get_display_for_rect(const math::recti& rect)
{
    return get_display_for_point(rect.center());
}

const video::display* video::get_display_for_window_position(const window* window)
{
    /* Fullscreen windows may be larger than the display if they were moved between differently sized
     * displays and the new position was received before the new size or vice versa. Using the center
     * of the window rect in this case can report the wrong display, so use the origin.
     */

    const display* display = nullptr;

    if (window->is_fullscreen())
    {
        display = get_display_for_point(window->get_position());
    }
    else
    {
        display = get_display_for_rect(window->get_rect());
    }

    if (!display)
    {
        display = get_primary_display();
    }

    return display;
}

const video::display* video::get_display_for_window(const window* window)
{

}

}
}