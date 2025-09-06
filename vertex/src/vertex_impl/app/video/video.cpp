#include <vector>

#include "vertex_impl/app/video/_platform/platform_video.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

// simulate data being a member of event_internal
#define data (*s_video_data_ptr)

bool video_internal::init()
{
    if (!init_impl())
    {
        return false;
    }

    video_internal::update_displays();
    if (data.displays.empty())
    {
        return false;
    }

    return true;
}

VX_API bool init()
{
    return video_internal::init();
}

////////////////////////////////////////

bool video_internal::is_init()
{
    return true;
}

VX_API bool is_init()
{
    return video_internal::is_init();
}

////////////////////////////////////////

void video_internal::quit()
{
    if (!is_init())

    // Destroy any existing windows
    video_internal::destroy_windows();

    quit_impl();
}

VX_API void quit()
{
    video_internal::quit();
}

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

process_dpi_awareness video_internal::get_dpi_awareness()
{
    return get_dpi_awareness_impl();
}

VX_API process_dpi_awareness get_dpi_awareness()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(process_dpi_awareness::UNAWARE);
    return video_internal::get_dpi_awareness();
}

///////////////////////////////////////////////////////////////////////////////
// system theme
///////////////////////////////////////////////////////////////////////////////

system_theme video_internal::get_system_theme()
{
    return get_system_theme_impl();
}

VX_API system_theme get_system_theme()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(system_theme::UNKNOWN);
    return video_internal::get_system_theme();
}

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

VX_API display_mode::display_mode()
    : bpp(0)
    , pixel_format(pixel::pixel_format::UNKNOWN)
    , pixel_density(0.0f)
    , refresh_rate(0.0f)
    , m_display_id(INVALID_ID)
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

////////////////////////////////////////

void video_internal::clear_display_mode_display_id(display_mode& dm)
{
    dm.m_display_id = INVALID_ID;
}

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

void video_internal::update_displays()
{
    update_displays_impl(data.displays);
}

VX_API void update_displays()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID();
    video_internal::update_displays();
}

////////////////////////////////////////

display* video_internal::get_display(display_id id)
{
    if (id == INVALID_ID)
    {
        return nullptr;
    }

    for (auto& d : data.displays)
    {
        if (d->m_id == id)
        {
            return d.get();
        }
    }

    return nullptr;
}

VX_API display* const get_display(display_id id)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_display(id);
}

////////////////////////////////////////

display* video_internal::get_primary_display()
{
    return !data.displays.empty() ? data.displays[0].get() : nullptr;
}

VX_API display* const get_primary_display()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_primary_display();
}

////////////////////////////////////////

size_t video_internal::display_count()
{
    return data.displays.size();
}

VX_API size_t display_count()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(0);
    return video_internal::display_count();
}

////////////////////////////////////////

display* video_internal::enum_displays(size_t i)
{
    return (i < display_count()) ? data.displays[i].get() : nullptr;
}

VX_API display* const enum_displays(size_t i)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::enum_displays(i);
}

////////////////////////////////////////

display* video_internal::get_display_for_point(const math::vec2i& p)
{
    display* closest = nullptr;
    int32_t closest_dist = std::numeric_limits<int32_t>::max();

    for (const auto& d : data.displays)
    {
        const math::recti bounds = d->get_bounds();
        if (bounds.empty())
        {
            continue;
        }

        if (math::g2::contains(bounds, p))
        {
            return d.get();
        }

        const math::vec2i c = math::g2::closest(bounds, p);
        const int32_t dist = math::distance_squared(c, p);
        if (dist < closest_dist)
        {
            closest = d.get();
            closest_dist = dist;
        }
    }

    return closest;
}

VX_API display* const get_display_for_point(const math::vec2i& p)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_display_for_point(p);
}

////////////////////////////////////////

display* video_internal::get_display_for_rect(const math::recti& rect)
{
    return get_display_for_point(rect.center());
}

VX_API display* const get_display_for_rect(const math::recti& rect)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_display_for_rect(rect);
}

////////////////////////////////////////

display* video_internal::get_display_at_origin(const math::vec2i& o)
{
    for (const auto& d : data.displays)
    {
        if (d->get_work_area().position == o)
        {
            return d.get();
        }
    }

    return nullptr;
}

////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1769

display* video_internal::get_display_for_window(const window& w)
{
    display* d = nullptr;

    if (!w.validate())
    {
        return d;
    }

    if (w.is_fullscreen())
    {
        d = get_display(w.m_current_fullscreen_mode.m_display_id);

        if (!d)
        {
            d = get_display(w.m_pending_display_id);
        }
    }

    if (!d)
    {
        // first try any built in method that the os may have
        d = get_display_for_window_impl(w);
    }

    if (!d)
    {
        if (w.is_fullscreen())
        {
            // When fullscreen windows are moved between displays of different sizes,
            // the window size and position updates may arrive out of order. This can
            // temporarily make the window larger than the display. In such cases,
            // using the center of the window rectangle might incorrectly identify the
            // display, so we use the origin instead.
            d = get_display_for_point(w.m_position);
        }
        else
        {
            d = get_display_for_rect(math::recti(w.m_position, w.m_size));
        }
    }

    // The primary display is a good default
    if (!d)
    {
        d = get_primary_display();
    }

    return d;
}

VX_API display* const get_display_for_window(const window& w)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_display_for_window(w);
}

////////////////////////////////////////

math::recti video_internal::get_desktop_area()
{
    math::recti area;
    for (const auto& d : data.displays)
    {
        area = math::g2::bounding_box(area, d->get_bounds());
    }

    return area;
}

VX_API math::recti get_desktop_area()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(math::recti{});
    return video_internal::get_desktop_area();
}

////////////////////////////////////////

display_id video_internal::get_display_id(const display& d)
{
    return d.m_id;
}

///////////////////////////////////////
// display members
///////////////////////////////////////

display::display()
    : m_id(INVALID_ID)
    , m_orientation(display_orientation::UNKNOWN)
    , m_fullscreen_window_id(INVALID_ID)
    , m_impl(new _priv::display_impl) {}

display::~display() {}

display::display(display&& other) noexcept
    : m_id(other.m_id)
    , m_name(std::move(other.m_name))
    , m_desktop_mode(std::move(other.m_desktop_mode))
    , m_current_mode(std::move(other.m_current_mode))
    , m_modes(std::move(other.m_modes))
    , m_orientation(other.m_orientation)
    , m_content_scale(other.m_content_scale)
    , m_fullscreen_window_id(other.m_fullscreen_window_id)
    , m_impl(std::move(other.m_impl)) {}

display& display::operator=(display&& other) noexcept
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
        m_fullscreen_window_id = other.m_fullscreen_window_id;
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

VX_API display_id display::id() const
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
        // query the current bounds
        m_impl->get_bounds(bounds);
    }

    return bounds;
}

VX_API math::recti display::get_work_area() const
{
    math::recti work_area;

    if (is_init())
    {
        // query the current work area
        m_impl->get_work_area(work_area);
    }

    return work_area;
}

void video_internal::init_display_modes(const display& d)
{
    if (!d.m_modes.empty())
    {
        return;
    }

    d.m_impl->list_display_modes(d.m_modes);

    for (display_mode& m : d.m_modes)
    {
        m.m_display_id = d.m_id;
    }
}

VX_API const display_mode& display::get_desktop_mode() const
{
    return m_desktop_mode;
}

VX_API const display_mode& display::get_current_mode() const
{
    return m_current_mode;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1509

VX_API bool display::set_current_mode(const display_mode& mode)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);

    // check if the mode is already set
    if (display_mode::compare(m_current_mode, mode))
    {
        return true;
    }

    display_mode* native_mode = nullptr;

    // we treat the desktop mode like the default, we consider setting the
    // display mode to the desktop mode to be resetting the display mode
    const bool is_desktop_mode = display_mode::compare(m_desktop_mode, mode);

    if (is_desktop_mode)
    {
        native_mode = &m_desktop_mode;
    }
    else
    {
        // make sure the display mode is supported
        native_mode = const_cast<display_mode*>(find_mode(mode));

        // unsupported mode
        if (!native_mode)
        {
            return false;
        }
    }

    // set the mode
    const bool set = m_impl->set_display_mode(*native_mode, is_desktop_mode);
    data.setting_display_mode = false;

    if (set)
    {
        video_internal::post_display_current_mode_changed(*this, mode);
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
    video_internal::init_display_modes(*this);
    return m_modes;
}

VX_API const display_mode* display::find_mode(const display_mode& mode) const
{
    // if we haven't listed the modes yet, list them now
    video_internal::init_display_modes(*this);

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
    video_internal::init_display_modes(*this);

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
// window
///////////////////////////////////////////////////////////////////////////////

window* video_internal::create_window(const window_config& config)
{
    owner_ptr<window> w(new window);
    if (w && w->create(config) && w->validate())
    {
        data.windows.push_back(std::move(w));
        return data.windows.back().get();
    }

    return nullptr;
}

VX_API window* const create_window(const window_config& config)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::create_window(config);
}

////////////////////////////////////////

void video_internal::destroy_window(window& w)
{
    VX_ASSERT(w.validate());

    const window_id id = w.m_id;
    for (auto it = data.windows.begin(); it != data.windows.end(); ++it)
    {
        if ((*it)->m_id == w.m_id)
        {
            data.windows.erase(it);
            break;
        }
    }

    // Make sure no displays reference the window
    for (auto& d : data.displays)
    {
        if (d->m_fullscreen_window_id == id)
        {
            d->m_fullscreen_window_id = INVALID_ID;
        }
    }
}

VX_API void destroy_window(window& w)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID();
    video_internal::destroy_window(w);
}

////////////////////////////////////////

void video_internal::destroy_windows()
{
    data.windows.clear();
}

////////////////////////////////////////

window* video_internal::get_window(window_id id)
{
    if (id == INVALID_ID)
    {
        return nullptr;
    }

    for (const auto& w : data.windows)
    {
        if (w->m_id == id)
        {
            return w.get();
        }
    }

    return nullptr;
}

VX_API window* const get_window(window_id id)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::get_window(id);
}

////////////////////////////////////////

window_id video_internal::get_window_id(const window& w)
{
    return w.m_id;
}

////////////////////////////////////////

window_id video_internal::window_count()
{
    return data.windows.size();
}

VX_API size_t window_count()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(0);
    return video_internal::window_count();
}

////////////////////////////////////////

window* video_internal::enum_windows(size_t i)
{
    if (data.windows.size() < i)
    {
        return data.windows[i].get();
    }

    return nullptr;
}

VX_API window* const enum_windows(size_t i)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return video_internal::enum_windows(i);
}

////////////////////////////////////////

void video_internal::check_window_display_changed(window& w)
{
    VX_ASSERT(w.validate());

    if (!has_capabilities(capabilities::SENDS_DISPLAY_CHANGES))
    {
        return;
    }

    display* new_display = get_display_for_window(w);
    if (!new_display)
    {
        return;
    }

    window_id w_id = w.m_id;

    display_id new_window_display_id = new_display->m_id;
    display_id old_window_display_id = w.m_last_display_id;

    window_id new_display_fullscreen_window_id = new_display->m_fullscreen_window_id;

    if (new_window_display_id != old_window_display_id)
    {
        for (const auto& d : data.displays)
        {
            display_id display_id = d->m_id;
            window_id display_fullscreen_window_id = d->m_fullscreen_window_id;

            if (display_fullscreen_window_id == w_id)
            {
                if (display_id != new_window_display_id)
                {
                    // We found an old display that still has the window set as
                    // its fullscreen window

                    if (new_display_fullscreen_window_id && new_display_fullscreen_window_id != w_id)
                    {
                        // If the new display already has a fullscreen window, minimize it

                        window* ow = get_window(new_display_fullscreen_window_id);
                        if (ow)
                        {
                            ow->minimize();
                        }
                    }

                    new_display->m_fullscreen_window_id = w_id;
                    d->m_fullscreen_window_id = 0;
                }

                break;
            }
        }

        w.post_window_display_changed(*new_display);
    }
}

////////////////////////////////////////

window* video_internal::get_active_window()
{
    for (auto& w : data.windows)
    {
        if (!w->m_destroying)
        {
            return w.get();
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// fullscreen helpers
///////////////////////////////////////////////////////////////////////////////

window_id video_internal::get_display_fullscreen_window_id(const display& d)
{
    return d.m_fullscreen_window_id;
}

void video_internal::set_display_fullscreen_window_id(display& d, window_id id)
{
    d.m_fullscreen_window_id = id;
}

void video_internal::clear_display_fullscreen_window_id(display& d)
{
    d.m_fullscreen_window_id = INVALID_ID;
}

display* video_internal::find_display_with_fullscreen_window(const window& w)
{
    for (const auto& d : data.displays)
    {
        if (d->m_fullscreen_window_id == w.m_id)
        {
            return d.get();
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// event handlers
///////////////////////////////////////////////////////////////////////////////

#if VX_VIDEO_HAVE_WAIT_EVENT_TIMEOUT

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1572

bool video_internal::wait_event_timeout(const window* w, time::time_point t)
{
    bool status = false;

    // Obtain the pointer to the wakeup window so other threads
    // know to wake this window up if an event is added to the queue.
    data.wakeup_window = w;
    //status = _priv::window_impl::wait_event_timeout();
    data.wakeup_window = nullptr;

    return status;
}

#endif // VX_VIDEO_HAVE_WAIT_EVENT_TIMEOUT

#if VX_VIDEO_HAVE_SEND_WAKEUP_EVENT

void video_internal::send_wakeup_event()
{
    // This function is called when a new event is added to the queue
    // while another thread may be blocked waiting for events.
    //
    // The waiting thread sets data.wakeup_window before calling into the OS
    // to wait for events. If we get here, it means new activity happened
    // and we must wake that thread immediately.
    //
    // We clear data.wakeup_window atomically right away to ensure:
    //   1. Only one wakeup happens per wait cycle (multiple threads adding
    //      events won’t flood the OS queue with wakeup messages).
    //   2. Other threads are free to reuse or take ownership of the window
    //      handle without being blocked by us holding it longer than needed.
    //
    // If there was a valid wakeup window, we send the wakeup event through
    // the platform-specific mechanism (e.g. PostMessage on Windows).

    const window* w = data.wakeup_window.exchange(nullptr);
    if (w)
    {
        //w->send_wakeup_event();
    }
}

#endif // VX_VIDEO_HAVE_SEND_WAKEUP_EVENT

bool video_internal::post_display_added(const display& d)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_ADDED;
    e.display_added.display_id = d.m_id;
    bool posted = event::post_event(e);

    on_display_added();

    return posted;
}

void video_internal::on_display_added()
{
    for (const auto& w : data.windows)
    {
        video_internal::check_window_display_changed(*w);
    }
}

bool video_internal::post_display_removed(const display& d)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_REMOVED;
    e.display_removed.display_id = d.m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video_internal::post_display_moved(const display& d)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_MOVED;
    e.display_moved.display_id = d.m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video_internal::post_display_orientation_changed(display& d, display_orientation orientation)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    if (orientation == display_orientation::UNKNOWN || d.get_orientation() == orientation)
    {
        return false;
    }

    d.m_orientation = orientation;

    event::event e{};
    e.type = event::DISPLAY_ORIENTATION_CHANGED;
    e.display_orientation_changed.display_id = d.m_id;
    e.display_orientation_changed.orientation = orientation;
    bool posted = event::post_event(e);

    return posted;
}

bool video_internal::post_display_desktop_mode_changed(display& d, const display_mode& mode)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    if (display_mode::compare(mode, d.get_desktop_mode()))
    {
        return false;
    }

    d.m_desktop_mode = mode;

    event::event e{};
    e.type = event::DISPLAY_DESKTOP_MODE_CHANGED;
    e.display_desktop_mode_changed.display_id = d.m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video_internal::post_display_current_mode_changed(display& d, const display_mode& mode)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    if (display_mode::compare(mode, d.get_current_mode()))
    {
        return false;
    }

    d.m_current_mode = mode;

    event::event e{};
    e.type = event::DISPLAY_CURRENT_MODE_CHANGED;
    e.display_current_mode_changed.display_id = d.m_id;
    bool posted = event::post_event(e);

    return posted;
}

bool video_internal::post_display_content_scale_changed(display& d, const math::vec2& content_scale)
{
    if (d.m_id == INVALID_ID)
    {
        return false;
    }

    if (d.get_content_scale() == content_scale)
    {
        return false;
    }

    d.m_content_scale = content_scale;

    event::event e{};
    e.type = event::DISPLAY_CONTENT_SCALE_CHANGED;
    e.display_content_scale_changed.display_id = d.m_id;
    e.display_content_scale_changed.x = content_scale.x;
    e.display_content_scale_changed.y = content_scale.y;
    bool posted = event::post_event(e);

    return posted;
}

} // namespace video
} // namespace app
} // namespace vx