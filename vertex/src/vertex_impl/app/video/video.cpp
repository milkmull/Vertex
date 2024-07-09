#include "vertex/config.h"
#include "vertex_impl/app/video/video.h"
#include "vertex/app/video/window.h"

#if defined(VX_SYSTEM_WINDOWS)

#include "win32_impl/video/win32_video.h"

#endif

namespace vx {
namespace app {
namespace video {

display_mode::display_mode()
    : bpp(0)
    , pixel_format(pixel::pixel_format::PIXEL_FORMAT_UNKNOWN)
    , pixel_density(0.0f)
    , refresh_rate(0.0f) {}

display_mode::display_mode(const display_mode& dm)
    : resolution(dm.resolution)
    , bpp(dm.bpp)
    , pixel_format(dm.pixel_format)
    , pixel_density(dm.pixel_density)
    , refresh_rate(dm.refresh_rate)
    , m_impl(std::make_unique<display_mode_impl>(*dm.m_impl)) {}

display_mode::display_mode(display_mode&& dm) noexcept
    : resolution(dm.resolution)
    , bpp(dm.bpp)
    , pixel_format(dm.pixel_format)
    , pixel_density(dm.pixel_density)
    , refresh_rate(dm.refresh_rate)
    , m_impl(std::move(dm.m_impl)) {}

display_mode::~display_mode() {}

display_mode& display_mode::operator=(const display_mode& dm)
{
    resolution = dm.resolution;
    bpp = dm.bpp;
    pixel_format = dm.pixel_format;
    pixel_density = dm.pixel_density;
    refresh_rate = dm.refresh_rate;
    m_impl = std::make_unique<display_mode_impl>(*dm.m_impl);
    return *this;
}

display_mode& display_mode::operator=(display_mode&& dm) noexcept
{
    resolution = dm.resolution;
    bpp = dm.bpp;
    pixel_format = dm.pixel_format;
    pixel_density = dm.pixel_density;
    refresh_rate = dm.refresh_rate;
    m_impl = std::move(dm.m_impl);
    return *this;
}

bool display_mode::operator==(const display_mode& dm) const
{
    return resolution == dm.resolution
        && bpp == dm.bpp
        && pixel_format == dm.pixel_format
        && refresh_rate == dm.refresh_rate;
}

bool display_mode::operator!=(const display_mode& dm) const
{
    return !((*this) == dm);
}

//===========================================================================

display::display() {}

display::display(const display& d)
    : m_impl(std::make_unique<display_impl>(*d.m_impl)) {}

display::display(display&& d) noexcept
    : m_impl(std::move(d.m_impl)) {}

display::~display() {}

display& display::operator=(const display& d)
{
    m_impl = std::make_unique<display_impl>(*d.m_impl);
    return *this;
}

display& display::operator=(display&& d) noexcept
{
    m_impl = std::move(d.m_impl);
    return *this;
}

device_id display::id() const
{
    return m_impl->data.id;
}

const std::string& display::name() const
{
    return m_impl->data.name;
}

const display_mode& display::get_desktop_mode() const
{
    return m_impl->data.desktop_mode;
}

const display_mode& display::get_mode() const
{
    return m_impl->data.current_mode;
}

bool display::set_mode(display_mode& mode) const
{
    bool found = false;

    for (const display_mode& m : m_impl->data.modes)
    {
        if (m == mode)
        {
            mode = m;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // requested mode is not supported by this display
        return false;
    }

    if (m_impl->data.current_mode == mode)
    {
        // already set
        return true;
    }

    bool set = m_impl->set_display_mode(mode);
    if (set)
    {
        m_impl->data.current_mode = mode;
    }

    return set;
}

const std::vector<display_mode>& display::list_modes() const
{
    if (m_impl->data.modes.empty())
    {
        m_impl->list_display_modes();
    }

    return m_impl->data.modes;
}

bool display::has_mode(const display_mode& mode) const
{
    for (const display_mode& m : m_impl->data.modes)
    {
        if (m == mode) return true;
    }

    return false;
}

display_orientation display::get_orientation() const
{
    return m_impl->data.orientation;
}

const math::vec2& display::get_content_scale() const
{
    return m_impl->data.content_scale;
}

math::recti display::get_bounds() const
{
    math::recti bounds;
    m_impl->get_bounds(bounds);
    return bounds;
}

math::recti display::get_work_area() const
{
    math::recti work_area;
    m_impl->get_work_area(work_area);
    return work_area;
}

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

bool init()
{
    if (!video_impl::init())
    {
        return false;
    }

    update_displays();
    if (s_video_data.displays.empty())
    {
        return false;
    }

    return true;
}

void quit()
{
    video_impl::quit();
}

process_dpi_awareness get_dpi_awareness()
{
    return video_impl::get_dpi_awareness();
}

void update_displays()
{
    display::display_impl::update_displays(s_video_data.displays);
}

const display* get_display(device_id id)
{
    for (const auto& d : s_video_data.displays)
    {
        if (d->id() == id) return d.get();
    }

    return nullptr;
}

display* get_primary_display()
{
    return !s_video_data.displays.empty() ? s_video_data.displays.front().get() : nullptr;
}

std::vector<const display*> list_displays()
{
    std::vector<const display*> displays(s_video_data.displays.size());

    for (size_t i = 0; i < s_video_data.displays.size(); ++i)
    {
        displays[i] = s_video_data.displays[i].get();
    }

    return displays;
}

const display* get_display_for_point(const math::vec2i& p)
{
    const display* closest = nullptr;
    int32_t closest_dist = 0x7FFFFFFF;

    for (const auto& d : s_video_data.displays)
    {
        math::recti bounds(d->get_bounds());
        if (bounds.empty())
        {
            continue;
        }

        if (math::contains(bounds, p))
        {
            return d.get();
        }

        math::vec2i c = bounds.clip(p);
        math::vec2i pc = c - p;

        int32_t dist = (pc.x * pc.x) + (pc.y * pc.y);
        if (dist < closest_dist)
        {
            closest = d.get();
            closest_dist = dist;
        }
    }

    return closest;
}

const display* get_display_for_rect(const math::recti& rect)
{
    return get_display_for_point(rect.center());
}

const display* get_display_for_window_position(const window* window)
{
    /* Fullscreen windows may be larger than the display if they were moved between differently sized
     * displays and the new position was received before the new size or vice versa. Using the center
     * of the window rect in this case can report the wrong display, so use the origin.
     */

    const display* d = nullptr;

    // In some cases, fullscreen windows may be larger than the display if they
    // were moved between differently sized displays. In this case getting the
    // display from the center of the image may give the incorrect display, so
    // we use the topleft of the window instead.

    if (window->is_fullscreen())
    {
        d = get_display_for_point(window->get_position());
    }
    else
    {
        d = get_display_for_rect(window->get_rect());
    }

    // The primary display is a good default

    if (!d)
    {
        d = get_primary_display();
    }

    return d;
}

const display* get_display_for_window(const window* window)
{
    return nullptr;
}

math::recti get_desktop_area()
{
    math::recti area;

    for (const auto& d : s_video_data.displays)
    {
        area = math::recti::merge(area, d->get_bounds());
    }

    return area;
}

///////////////////////////////////////////////////////////////////////////////
// video_data
///////////////////////////////////////////////////////////////////////////////

video_data s_video_data = video_data{};

}
}
}