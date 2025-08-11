#include <vector>

#include "vertex_impl/app/video/_platform/video.hpp"
#include "vertex/app/event/event.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

struct video_data
{
    bool is_init = false;

    std::vector<std::unique_ptr<display>> displays;
};

static video_data s_video_data;

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
// display mode
///////////////////////////////////////////////////////////////////////////////

VX_API display_mode::display_mode()
    : bpp(0)
    , pixel_format(pixel::pixel_format::UNKNOWN)
    , pixel_density(0.0f)
    , refresh_rate(0.0f)
    , m_display_id(0)
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

    if (is_init())
    {
        m_impl->get_bounds(bounds);
    }

    return bounds;
}

VX_API math::recti display::get_work_area() const
{
    math::recti work_area;

    if (is_init())
    {
        m_impl->get_work_area(work_area);
    }

    return work_area;
}

void display::update_modes() const
{
    VX_CHECK_VIDEO_INIT_VOID();

    m_modes.clear();
    m_impl->list_display_modes(m_modes);

    for (display_mode& m : m_modes)
    {
        m.m_display_id = m_id;
    }
}

VX_API const display_mode& display::get_desktop_mode() const
{
    if (is_init() && m_modes.empty())
    {
        m_impl->list_display_modes(m_modes);
    }

    return m_desktop_mode;
}

VX_API const display_mode& display::get_current_mode() const
{
    if (is_init() && m_modes.empty())
    {
        update_modes();
    }

    return m_current_mode;
}

VX_API bool display::set_current_mode(display_mode& mode)
{
    VX_CHECK_VIDEO_INIT(false);

    const display_mode& current_mode = get_current_mode();
    if (display_mode::compare(current_mode, mode))
    {
        // already set
        mode = current_mode;
        return true;
    }

    if (!find_mode(mode))
    {
        // mode is not applicable to this display
        return false;
    }

    //s_video_data.setting_display_mode = true;
    const bool set = m_impl->set_display_mode(mode);
    //s_video_data.setting_display_mode = false;

    if (set)
    {
        mode.m_display_id = m_id;
        //post_display_current_mode_changed(this, mode);
    }

    return set;
}

VX_API void display::clear_mode()
{
    display_mode mode = get_desktop_mode();
    set_current_mode(mode);
}

VX_API size_t display::mode_count() const
{
    return m_modes.size();
}

VX_API const display_mode* display::enum_modes(size_t i) const
{
    // if we haven't listed the modes yet, list them now
    if (m_modes.empty())
    {
        update_modes();
    }

    return (i < m_modes.size()) ? &m_modes[i] : nullptr;
}

VX_API const std::vector<display_mode>& display::list_modes() const
{
    // if we haven't listed the modes yet, list them now
    if (m_modes.empty())
    {
        update_modes();
    }

    return m_modes;
}

VX_API const display_mode* display::find_mode(const display_mode& mode) const
{
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
    float aspect_ratio = (height > 0) ? (static_cast<float>(width) / height) : 1.0f;
    if (refresh_rate <= 0.0f)
    {
        refresh_rate = get_desktop_mode().refresh_rate;
    }

    const display_mode* closest_mode = nullptr;

    for (const display_mode& mode : list_modes())
    {
        if (width > mode.resolution.x)
        {
            break;
        }
        if (height > mode.resolution.y)
        {
            continue;
        }
        if (mode.pixel_density != 1.0f)
        {
            continue;
        }
        if (closest_mode)
        {
            float current_mode_aspect_ratio = static_cast<float>(mode.resolution.x) / mode.resolution.y;
            float closest_mode_aspect_ratio = static_cast<float>(closest_mode->resolution.x) / closest_mode->resolution.y;
            if (math::abs(aspect_ratio - closest_mode_aspect_ratio) < math::abs(aspect_ratio - current_mode_aspect_ratio))
            {
                continue;
            }
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