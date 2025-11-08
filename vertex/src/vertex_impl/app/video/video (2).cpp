#include "vertex/system/platform.h"
#include "vertex/system/error.h"

#include "vertex/app/video/window.h"
#include "vertex/app/event/event.h"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_video.h"
#   include "_win32/win32_window.h"
#endif

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// init stuff
///////////////////////////////////////////////////////////////////////////////

video::video_data video::s_video_data = video::video_data{};

bool video::init()
{
    if (s_video_data.is_init)
    {
        quit();
    }

    if (!mouse::init())
    {
        mouse::quit();
        return false;
    }

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

void video::quit()
{
    // Destroy any existing windows
    auto it = s_video_data.windows.begin();
    while (it != s_video_data.windows.end())
    {
        it = s_video_data.windows.erase(it);
    }

    mouse::quit();
    video_impl::quit();
}

static void uninitialized_video()
{
    VX_ERROR(error::error_code::unsupported_operation) << "Video subsystem has not been initialized";
}

bool video::is_init()
{
    VX_ERROR(error::error_code::unsupported_operation) << "Video subsystem has not been initialized";
    return s_video_data.is_init;
}

#define CHECK_VIDEO_INIT(return_value, ...) if (!is_init()) { uninitialized_video(); return return_value; }

video::process_dpi_awareness video::get_dpi_awareness()
{
    CHECK_VIDEO_INIT(process_dpi_awareness::UNAWARE);
    return video_impl::get_dpi_awareness();
}

video::system_theme video::get_system_theme()
{
    CHECK_VIDEO_INIT(system_theme::UNKNOWN);
    return video_impl::get_system_theme();
}

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

void video::update_displays()
{
    CHECK_VIDEO_INIT(,);
    video_impl::update_displays(s_video_data.displays);
}

video::display* video::get_display(device_id id)
{
    CHECK_VIDEO_INIT(nullptr);

    for (const std::unique_ptr<display>& d : s_video_data.displays)
    {
        if (d->id() == id)
        {
            return d.get();
        }
    }

    return nullptr;
}

video::display* video::get_primary_display()
{
    CHECK_VIDEO_INIT(nullptr);
    return !s_video_data.displays.empty() ? s_video_data.displays.front().get() : nullptr;
}

std::vector<video::display*> video::list_displays()
{
    CHECK_VIDEO_INIT(std::vector<video::display*>());

    std::vector<display*> displays(s_video_data.displays.size());
    for (size_t i = 0; i < s_video_data.displays.size(); ++i)
    {
        displays[i] = s_video_data.displays[i].get();
    }

    return displays;
}

video::display* video::get_display_for_point(const math::vec2i& p)
{
    CHECK_VIDEO_INIT(nullptr);

    display* closest = nullptr;
    int32_t closest_dist = std::numeric_limits<int32_t>::max();

    for (const std::unique_ptr<display>& d : s_video_data.displays)
    {
        math::recti bounds = d->get_bounds();
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

video::display* video::get_display_for_rect(const math::recti& rect)
{
    CHECK_VIDEO_INIT(nullptr);
    return get_display_for_point(rect.center());
}

video::display* video::get_display_for_window(const window* w)
{
    CHECK_VIDEO_INIT(nullptr);

    if (!w)
    {
        return nullptr;
    }

    display* d = nullptr;

    if (w->m_current_fullscreen_mode.m_display_id)
    {
        d = get_display(w->m_current_fullscreen_mode.m_display_id);
    }

    if (!d)
    {
        if (w->is_fullscreen())
        {
            if (!w->m_repositioning)
            {
                // When fullscreen windows are moved between displays of different sizes,
                // the window size and position updates may arrive out of order. This can
                // temporarily make the window larger than the display. In such cases,
                // using the center of the window rectangle might incorrectly identify the
                // display, so we use the origin instead.
                d = get_display_for_point(w->m_position);
            }
            else
            {
                // In some backends, the actual window position may not be updated at the
                // time of this call. If the window is being repositioned via a call to
                // set_position, the floating rect will have the most up-to-date area for
                // the window. 
                d = get_display_for_rect(w->m_floating_rect);
            }
        }
        else
        {
            d = get_display_for_rect(w->get_rect());
        }
    }

    // The primary display is a good default
    if (!d)
    {
        d = get_primary_display();
    }

    return d;
}

math::recti video::get_desktop_area()
{
    CHECK_VIDEO_INIT(math::recti());

    math::recti area;

    for (const std::unique_ptr<display>& d : s_video_data.displays)
    {
        area = math::recti::merge(area, d->get_bounds());
    }

    return area;
}

void video::check_window_display_changed(window& w)
{
    if (!(s_video_data.video_caps & caps::SEND_DISPLAY_CHANGES))
    {
        return;
    }

    display* new_display = get_display_for_window(&w);
    if (!new_display)
    {
        return;
    }

    device_id window_id = w.id();

    device_id new_window_display_id = new_display->id();
    device_id old_window_display_id = w.m_last_display_id;

    device_id new_display_fullscreen_window_id = new_display->m_fullscreen_window_id;

    if (new_window_display_id != old_window_display_id)
    {
        for (const std::unique_ptr<display>& d : s_video_data.displays)
        {
            device_id display_id = d->id();
            device_id display_fullscreen_window_id = d->m_fullscreen_window_id;

            if (display_fullscreen_window_id == window_id)
            {
                if (display_id != new_window_display_id)
                {
                    // We found an old display that still has the window set as
                    // its fullscreen window

                    if (new_display_fullscreen_window_id && new_display_fullscreen_window_id != window_id)
                    {
                        // If the new display already has a fullscreen window, minimize it

                        window* ow = get_window(new_display_fullscreen_window_id);
                        if (ow)
                        {
                            ow->minimize();
                        }
                    }

                    new_display->m_fullscreen_window_id = window_id;
                    d->m_fullscreen_window_id = 0;
                }

                break;
            }
        }

        w.post_window_display_changed(*new_display);
    }
}

///////////////////////////////////////////////////////////////////////////////

video::window* video::create_window(const window_config& config)
{
    CHECK_VIDEO_INIT(nullptr);

    s_video_data.windows.emplace_back(std::make_unique<window>());
    auto& w = s_video_data.windows.back();
    if (!w || !w->create(config) || !w->validate())
    {
        s_video_data.windows.pop_back();
        return nullptr;
    }

    return s_video_data.windows.back().get();
}

void video::destroy_window(window* w)
{
    CHECK_VIDEO_INIT(,);

    if (!w)
    {
        return;
    }

    for (auto it = s_video_data.windows.begin(); it != s_video_data.windows.end(); ++it)
    {
        if (it->get()->m_id == w->m_id)
        {
            s_video_data.windows.erase(it);
            break;
        }
    }
}

video::window* video::get_window(device_id id)
{
    CHECK_VIDEO_INIT(nullptr);

    for (const std::unique_ptr<window>& w : s_video_data.windows)
    {
        if (w->id() == id)
        {
            return w.get();
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// display events
///////////////////////////////////////////////////////////////////////////////

bool video::post_display_added(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event e {};
    e.type = event_type::DISPLAY_ADDED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    on_display_added();

    return posted;
}

void video::on_display_added()
{
    for (const std::unique_ptr<window>& w : s_video_data.windows)
    {
        check_window_display_changed(*w);
    }
}

bool video::post_display_removed(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event e {};
    e.type = event_type::DISPLAY_REMOVED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool video::post_display_moved(const display* d)
{
    if (!d || d->id() == 0)
    {
        return false;
    }

    event e {};
    e.type = event_type::DISPLAY_MOVED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool video::post_display_orientation_changed(display* d, display_orientation orientation)
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

    event e {};
    e.type = event_type::DISPLAY_ORIENTATION_CHANGED;
    e.display_id = d->id();
    e.display_orientation_changed.orientation = orientation;
    bool posted = event::post_event(e);

    return posted;
}

bool video::post_display_desktop_mode_changed(display* d, const display_mode& mode)
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

    event e {};
    e.type = event_type::DISPLAY_DESKTOP_MODE_CHANGED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool video::post_display_current_mode_changed(display* d, const display_mode& mode)
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

    event e {};
    e.type = event_type::DISPLAY_CURRENT_MODE_CHANGED;
    e.display_id = d->id();
    bool posted = event::post_event(e);

    return posted;
}

bool video::post_display_content_scale_changed(display* d, const math::vec2& content_scale)
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

    event e {};
    e.type = event_type::DISPLAY_CONTENT_SCALE_CHANGED;
    e.display_id = d->id();
    e.display_content_scale_changed.x = content_scale.x;
    e.display_content_scale_changed.y = content_scale.y;
    bool posted = event::post_event(e);

    return posted;
}

///////////////////////////////////////////////////////////////////////////////
// display_mode
///////////////////////////////////////////////////////////////////////////////

video::display_mode::display_mode()
    : bpp(0)
    , pixel_format(pixel::pixel_format::UNKNOWN)
    , pixel_density(0.0f)
    , refresh_rate(0.0f)
    , m_display_id(0) {}

video::display_mode::display_mode(const display_mode& dm)
    : resolution(dm.resolution)
    , bpp(dm.bpp)
    , pixel_format(dm.pixel_format)
    , pixel_density(dm.pixel_density)
    , refresh_rate(dm.refresh_rate)
    , m_display_id(dm.m_display_id)
    , m_impl(std::make_unique<display_mode_impl>(*dm.m_impl)) {}

video::display_mode::display_mode(display_mode&& dm) noexcept
    : resolution(dm.resolution)
    , bpp(dm.bpp)
    , pixel_format(dm.pixel_format)
    , pixel_density(dm.pixel_density)
    , refresh_rate(dm.refresh_rate)
    , m_display_id(dm.m_display_id)
    , m_impl(std::move(dm.m_impl)) {}

video::display_mode::~display_mode() {}

video::display_mode& video::display_mode::operator=(const display_mode& dm)
{
    resolution = dm.resolution;
    bpp = dm.bpp;
    pixel_format = dm.pixel_format;
    pixel_density = dm.pixel_density;
    refresh_rate = dm.refresh_rate;
    m_display_id = dm.m_display_id;

    if (dm.m_impl)
    {
        m_impl = std::make_unique<display_mode_impl>(*dm.m_impl);
    }

    return *this;
}

video::display_mode& video::display_mode::operator=(display_mode&& dm) noexcept
{
    resolution = dm.resolution;
    bpp = dm.bpp;
    pixel_format = dm.pixel_format;
    pixel_density = dm.pixel_density;
    refresh_rate = dm.refresh_rate;
    m_display_id = dm.m_display_id;
    m_impl = std::move(dm.m_impl);

    return *this;
}

bool video::display_mode::compare(const display_mode& mode1, const display_mode& mode2)
{
    return mode1.resolution == mode2.resolution
        && mode1.bpp == mode2.bpp
        && mode1.pixel_format == mode2.pixel_format
        && mode1.pixel_density == mode2.pixel_density
        && mode1.refresh_rate == mode2.refresh_rate;
}

///////////////////////////////////////////////////////////////////////////////
// display
///////////////////////////////////////////////////////////////////////////////

video::display::display()
    : m_id(0)
    , m_orientation(display_orientation::UNKNOWN)
    , m_fullscreen_window_id(0) {}

video::display::display(const display& d)
    : m_id(0)
    , m_orientation(display_orientation::UNKNOWN)
    , m_fullscreen_window_id(0)
    , m_impl(std::make_unique<display_impl>(*d.m_impl)) {}

video::display::display(display&& d) noexcept
    : m_id(d.m_id)
    , m_name(std::move(d.m_name))
    , m_desktop_mode(std::move(d.m_desktop_mode))
    , m_current_mode(std::move(d.m_current_mode))
    , m_modes(std::move(d.m_modes))
    , m_orientation(d.m_orientation)
    , m_content_scale(d.m_content_scale)
    , m_fullscreen_window_id(d.m_fullscreen_window_id)
    , m_impl(std::move(d.m_impl)) {}

video::display::~display() {}

video::display& video::display::operator=(const display& d)
{
    m_id = d.m_id;
    m_name = d.m_name;
    m_desktop_mode = d.m_desktop_mode;
    m_current_mode = d.m_current_mode;
    m_modes = d.m_modes;
    m_orientation = d.m_orientation;
    m_content_scale = d.m_content_scale;
    m_fullscreen_window_id = d.m_fullscreen_window_id;

    if (d.m_impl)
    {
        m_impl = std::make_unique<display_impl>(*d.m_impl);
    }

    return *this;
}

video::display& video::display::operator=(display&& d) noexcept
{
    m_id = d.m_id;
    m_name = std::move(d.m_name);
    m_desktop_mode = std::move(d.m_desktop_mode);
    m_current_mode = std::move(d.m_current_mode);
    m_modes = std::move(d.m_modes);
    m_orientation = d.m_orientation;
    m_content_scale = d.m_content_scale;
    m_fullscreen_window_id = d.m_fullscreen_window_id;
    m_impl = std::move(d.m_impl);

    return *this;
}

device_id video::display::id() const
{
    return m_id;
}

const std::string& video::display::name() const
{
    return m_name;
}

void video::display::update_modes() const
{
    CHECK_VIDEO_INIT(,);

    m_impl->list_display_modes(m_modes);

    for (display_mode& m : m_modes)
    {
        m.m_display_id = m_id;
    }
}

const video::display_mode& video::display::get_desktop_mode() const
{
    if (m_modes.empty())
    {
        m_impl->list_display_modes(m_modes);
    }

    return m_desktop_mode;
}

const video::display_mode& video::display::get_current_mode() const
{
    if (m_modes.empty())
    {
        update_modes();
    }

    return m_current_mode;
}

bool video::display::set_current_mode(display_mode& mode)
{
    CHECK_VIDEO_INIT(false);

    const display_mode& current_mode = get_current_mode();

    if (display_mode::compare(current_mode, mode))
    {
        // already set
        mode = current_mode;
        return true;
    }

    if (!find_mode(mode))
    {
        return false;
    }

    s_video_data.setting_display_mode = true;
    bool set = m_impl->set_display_mode(mode);
    s_video_data.setting_display_mode = false;

    if (set)
    {
        mode.m_display_id = id();
        post_display_current_mode_changed(this, mode);
    }

    return set;
}

void video::display::clear_mode()
{
    display_mode mode = get_desktop_mode();
    set_current_mode(mode);
}

const std::vector<video::display_mode>& video::display::list_modes() const
{
    if (m_modes.empty())
    {
        update_modes();
    }

    return m_modes;
}

const video::display_mode* video::display::find_mode(const display_mode& mode) const
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

const video::display_mode* video::display::find_closest_mode(int width, int height, float refresh_rate) const
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

video::display_orientation video::display::get_orientation() const
{
    return m_orientation;
}

const math::vec2& video::display::get_content_scale() const
{
    return m_content_scale;
}

math::recti video::display::get_bounds() const
{
    math::recti bounds;
    m_impl->get_bounds(bounds);
    return bounds;
}

math::recti video::display::get_work_area() const
{
    math::recti work_area;
    m_impl->get_work_area(work_area);
    return work_area;
}

}
}