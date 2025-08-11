#include <vector>

#include "vertex_impl/app/video/_platform/video.hpp"
#include "vertex/app/event/event.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

video_data s_video_data;

VX_API bool init()
{
    if (s_video_data.is_init)
    {
        quit();
    }

    if (!init_impl())
    {
        return false;
    }

    s_video_data.is_init = true;
    return true;
}

VX_API bool is_init()
{
    return s_video_data.is_init;
}

VX_API void quit()
{
    quit_impl();
    s_video_data.is_init = false;
}

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

VX_API process_dpi_awareness get_dpi_awareness()
{
    return get_dpi_awareness_impl();
}

VX_API system_theme get_system_theme()
{
    return get_system_theme_impl();
}

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

VX_API display_mode::display_mode()
    : bpp(0)
    , pixel_format(pixel::pixel_format::UNKNOWN)
    , pixel_density(0.0f)
    , refresh_rate(0.0f)
    , m_display_id(INVALID_DEVICE_ID)
    , m_impl(new _priv::display_mode_impl) {}

VX_API display_mode::display_mode(const display_mode& other)
    : resolution(other.resolution)
    , bpp(other.bpp)
    , pixel_format(other.pixel_format)
    , pixel_density(other.pixel_density)
    , refresh_rate(other.refresh_rate)
    , m_display_id(other.m_display_id)
    , m_impl(other.m_impl) {}

VX_API display_mode::display_mode(display_mode&& other) noexcept
    : resolution(other.resolution)
    , bpp(other.bpp)
    , pixel_format(other.pixel_format)
    , pixel_density(other.pixel_density)
    , refresh_rate(other.refresh_rate)
    , m_display_id(other.m_display_id)
    , m_impl(std::move(other.m_impl)) {}

VX_API display_mode::~display_mode() {}

VX_API display_mode& display_mode::operator=(const display_mode& other)
{
    resolution = other.resolution;
    bpp = other.bpp;
    pixel_format = other.pixel_format;
    pixel_density = other.pixel_density;
    refresh_rate = other.refresh_rate;
    m_display_id = other.m_display_id;
    m_impl = other.m_impl;

    return *this;
}

VX_API display_mode& display_mode::operator=(display_mode&& other) noexcept
{
    if (this != &other)
    {
        resolution = other.resolution;
        bpp = other.bpp;
        pixel_format = other.pixel_format;
        pixel_density = other.pixel_density;
        refresh_rate = other.refresh_rate;
        m_display_id = other.m_display_id;
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

VX_API bool display_mode::compare(const display_mode& mode1, const display_mode& mode2)
{
    return mode1.resolution == mode2.resolution
        && mode1.bpp == mode2.bpp
        && mode1.pixel_format == mode2.pixel_format
        && mode1.pixel_density == mode2.pixel_density
        && mode1.refresh_rate == mode2.refresh_rate;
}

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

VX_API void update_displays()
{
    VX_CHECK_VIDEO_INIT_VOID();
    update_displays_impl(s_video_data.displays);
}

VX_API display* get_display(device_id id)
{
    VX_CHECK_VIDEO_INIT(nullptr);

    for (auto& d : s_video_data.displays)
    {
        if (d->id() == id)
        {
            return d.get();
        }
    }

    return nullptr;
}

VX_API display* get_primary_display()
{
    VX_CHECK_VIDEO_INIT(nullptr);
    return !s_video_data.displays.empty() ? s_video_data.displays.front().get() : nullptr;
}

VX_API size_t display_count()
{
    return s_video_data.displays.size();
}

VX_API display* enum_displays(size_t i)
{
    return (i < display_count()) ? s_video_data.displays[i].get() : nullptr;
}

///////////////////////////////////////
// display members
///////////////////////////////////////

VX_API display::display()
    : m_id(INVALID_DEVICE_ID)
    , m_orientation(display_orientation::UNKNOWN)
    , m_impl(new _priv::display_impl) {}

VX_API display::~display() {}

VX_API display::display(display&& other) noexcept
    : m_id(other.m_id)
    , m_name(std::move(other.m_name))
    , m_desktop_mode(std::move(other.m_desktop_mode))
    , m_current_mode(std::move(other.m_current_mode))
    , m_modes(std::move(other.m_modes))
    , m_orientation(other.m_orientation)
    , m_content_scale(other.m_content_scale)
    , m_impl(std::move(other.m_impl)) {}

VX_API display& display::operator=(display&& other) noexcept
{
    if (this != &other)
    {
        m_id = other.m_id;
        m_name = std::move(other.m_name);
        m_desktop_mode = std::move(other.m_desktop_mode);
        m_current_mode = std::move(other.m_current_mode);
        m_modes = std::move(other.m_modes);
        m_orientation = other.m_orientation;
        m_content_scale = other.m_content_scale;
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

VX_API device_id display::id() const
{
    return m_id;
}

VX_API const std::string& display::name() const
{
    return m_name;
}

VX_API display_orientation display::get_orientation() const
{
    return m_orientation;
}

VX_API const math::vec2& display::get_content_scale() const
{
    return m_content_scale;
}

VX_API math::recti display::get_bounds() const
{
    math::recti bounds;
    VX_CHECK_VIDEO_INIT(bounds);
    // query the current bounds
    m_impl->get_bounds(bounds);
    return bounds;
}

VX_API math::recti display::get_work_area() const
{
    math::recti work_area;
    VX_CHECK_VIDEO_INIT(work_area);
    // query the current work area
    m_impl->get_work_area(work_area);
    return work_area;
}

void display::update_modes() const
{
    VX_CHECK_VIDEO_INIT_VOID();

    if (!m_modes.empty())
    {
        return;
    }

    m_impl->list_display_modes(m_modes);

    for (display_mode& m : m_modes)
    {
        m.m_display_id = m_id;
    }
}

VX_API const display_mode& display::get_desktop_mode() const
{
    VX_CHECK_VIDEO_INIT(m_desktop_mode);
    return m_desktop_mode;
}

VX_API const display_mode& display::get_current_mode() const
{
    VX_CHECK_VIDEO_INIT(m_current_mode);
    return m_current_mode;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1509

VX_API bool display::set_current_mode(const display_mode& mode)
{
    VX_CHECK_VIDEO_INIT(false);

    // check if the mode is already set
    if (display_mode::compare(m_current_mode, mode))
    {
        return true;
    }

    const bool is_desktop_mode = display_mode::compare(m_desktop_mode, mode);
    display_mode* native_mode = nullptr;

    if (is_desktop_mode)
    {
        native_mode = &m_desktop_mode;
    }
    else
    {
        // if we haven't listed the modes yet, list them now
        update_modes();

        // make sure the display supports the mode
        for (display_mode& m : m_modes)
        {
            if (display_mode::compare(m, mode))
            {
                native_mode = &m;
                break;
            }
        }

        if (!native_mode)
        {
            return false;
        }
    }

    // set the mode
    const bool set = m_impl->set_display_mode(*native_mode, is_desktop_mode);
    s_video_data.setting_display_mode = false;

    if (set)
    {
        _priv::video_internal::post_display_current_mode_changed(this, mode);
    }

    return set;
}

VX_API void display::clear_mode()
{
    set_current_mode(m_desktop_mode);
}

VX_API const std::vector<display_mode>& display::list_modes() const
{
    // if we haven't listed the modes yet, list them now
    update_modes();
    return m_modes;
}

VX_API const display_mode* display::find_mode(const display_mode& mode) const
{
    // if we haven't listed the modes yet, list them now
    update_modes();

    for (const display_mode& m : m_modes)
    {
        if (display_mode::compare(m, mode))
        {
            return &m;
        }
    }

    return nullptr;
}

VX_API const display_mode* display::find_closest_mode(int width, int height, float refresh_rate) const
{
    // compute target aspect ratio from width/height (fallback to 1.0f if height is 0 to avoid division by zero)
    const float aspect_ratio = (height > 0) ? (static_cast<float>(width) / height) : 1.0f;

    // if no refresh rate was specified, use the desktop's current refresh rate
    if (refresh_rate <= 0.0f)
    {
        refresh_rate = m_desktop_mode.refresh_rate;
    }

    // if we haven't listed the modes yet, list them now
    update_modes();

    const display_mode* closest_mode = nullptr;

    for (const display_mode& mode : m_modes)
    {
        // Skip modes with a smaller width than requested
        if (width > mode.resolution.x)
        {
            break;
        }
        // Skip modes with smaller height than requested
        if (height > mode.resolution.y)
        {
            continue;
        }
        // Skip modes with non-default pixel density (e.g., HiDPI scaling)
        if (mode.pixel_density != 1.0f)
        {
            continue;
        }

        // If we already have a candidate mode, check if this one is a worse match
        if (closest_mode)
        {
            const float current_mode_aspect_ratio = static_cast<float>(mode.resolution.x) / mode.resolution.y;
            const float closest_mode_aspect_ratio = static_cast<float>(closest_mode->resolution.x) / closest_mode->resolution.y;

            // Prefer mode with aspect ratio closer to target
            if (math::abs(aspect_ratio - closest_mode_aspect_ratio) < math::abs(aspect_ratio - current_mode_aspect_ratio))
            {
                continue;
            }

            // If resolutions are the same, prefer refresh rate closer to target
            if ((mode.resolution == closest_mode->resolution) &&
                (math::abs(refresh_rate - closest_mode->refresh_rate) < math::abs(refresh_rate - mode.refresh_rate)))
            {
                continue;
            }
        }

        closest_mode = &mode;
    }

    return closest_mode;
}

///////////////////////////////////////////////////////////////////////////////
// display events
///////////////////////////////////////////////////////////////////////////////

bool _priv::video_internal::post_display_added(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_ADDED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    on_display_added();

    return posted;
}

void _priv::video_internal::on_display_added()
{
    //for (const std::unique_ptr<window>& w : s_video_data.windows)
    //{
    //    check_window_display_changed(*w);
    //}
}

bool _priv::video_internal::post_display_removed(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_REMOVED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool _priv::video_internal::post_display_moved(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_MOVED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool _priv::video_internal::post_display_orientation_changed(display* d, display_orientation orientation)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    if (orientation == display_orientation::UNKNOWN || d->get_orientation() == orientation)
    {
        return false;
    }

    d->m_orientation = orientation;

    event::event e{};
    e.type = event::DISPLAY_ORIENTATION_CHANGED;
    e.display_id = d->id();
    e.display_orientation_changed.orientation = orientation;
    bool posted = event::post_event(e);

    return posted;
}

bool _priv::video_internal::post_display_desktop_mode_changed(display* d, const display_mode& mode)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    if (display_mode::compare(mode, d->get_desktop_mode()))
    {
        return false;
    }

    d->m_desktop_mode = mode;

    event::event e{};
    e.type = event::DISPLAY_DESKTOP_MODE_CHANGED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool _priv::video_internal::post_display_current_mode_changed(display* d, const display_mode& mode)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    if (display_mode::compare(mode, d->get_current_mode()))
    {
        return false;
    }

    d->m_current_mode = mode;

    event::event e{};
    e.type = event::DISPLAY_CURRENT_MODE_CHANGED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool _priv::video_internal::post_display_content_scale_changed(display* d, const math::vec2& content_scale)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    if (d->get_content_scale() == content_scale)
    {
        return false;
    }

    d->m_content_scale = content_scale;

    event::event e{};
    e.type = event::DISPLAY_CONTENT_SCALE_CHANGED;
    e.display_id = d->id();
    e.display_content_scale_changed.x = content_scale.x;
    e.display_content_scale_changed.y = content_scale.y;
    bool posted = event::post_event(e);

    return posted;
}


} // namespace video
} // namespace app
} // namespace vx