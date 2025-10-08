#include <vector>

#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/video/_platform/platform_video.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// hints
///////////////////////////////////////////////////////////////////////////////

static void sync_window_operations_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    video_instance* this_ = static_cast<video_instance*>(user_data);
    this_->data.hints.sync_window_operations = hint::parse_boolean(new_value, VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_SYNC_WINDOW_OPERATIONS));
}

static bool parse_display_usable_bounds_hint(const char* hint, math::recti& rect)
{
    return hint && (std::sscanf(
        hint, "%d,%d,%d,%d", 
        &rect.position.x, &rect.position.y,
        &rect.size.x, &rect.size.y
    ) == 4);
}

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L611

bool video_instance::init(app_instance* owner)
{
    VX_ASSERT(!app);
    VX_ASSERT(owner);
    app = owner;

    data.keyboard_ptr.reset(new keyboard::keyboard_instance);
    if (!data.keyboard_ptr || !data.keyboard_ptr->init(this))
    {
        quit();
        return false;
    }

    data.mouse_ptr.reset(new mouse::mouse_instance);
    if (!data.mouse_ptr || !data.mouse_ptr->init(this))
    {
        quit();
        return false;
    }

    // duplicate for touch and pen
    //data.mouse_ptr.reset(new mouse::mouse_instance);
    //if (!data.mouse_ptr || !data.mouse_ptr->init(this))
    //{
    //    quit();
    //    return false;
    //}

    impl_ptr.reset(new video_instance_impl);
    if (!impl_ptr || !impl_ptr->init(this))
    {
        quit();
        return false;
    }

    update_displays();
    if (data.displays.empty())
    {
        quit();
        return false;
    }

    VX_ASSERT(app->data.hints_ptr);
    {
        app->data.hints_ptr->add_hint_callback_and_default_value(
            VX_HINT_GET_NAME(hint::HINT_VIDEO_SYNC_WINDOW_OPERATIONS),
            sync_window_operations_hint_watcher,
            this,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_SYNC_WINDOW_OPERATIONS),
            true
        );

        // disable the screen saver by default
        if (!app->data.hints_ptr->get_hint_boolean(
            hint::HINT_VIDEO_ALLOW_SCREEN_SAVER,
            VX_HINT_GET_DEFAULT_VALUE(hint::HINT_VIDEO_ALLOW_SCREEN_SAVER)))
        {
            disable_screen_saver();
        }
    }

    return true;
}

////////////////////////////////////////

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L726

void video_instance::quit()
{
    // destroy pen, touch

    if (data.mouse_ptr)
    {
        data.mouse_ptr->quit();
        data.mouse_ptr.reset();
    }

    if (data.keyboard_ptr)
    {
        data.keyboard_ptr->quit();
        data.keyboard_ptr.reset();
    }

    enable_screen_saver();

    // Destroy any existing windows
    destroy_windows();

    if (impl_ptr)
    {
        impl_ptr->quit();
        impl_ptr.reset();
    }

    // remove hint callbacks
    {
        app->data.hints_ptr->remove_hint_callback(
            VX_HINT_GET_NAME(hint::HINT_VIDEO_SYNC_WINDOW_OPERATIONS),
            sync_window_operations_hint_watcher,
            this
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

VX_API process_dpi_awareness get_dpi_awareness()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(process_dpi_awareness::UNAWARE);
    return s_video_ptr->get_dpi_awareness();
}

process_dpi_awareness video_instance::get_dpi_awareness() const
{
    return impl_ptr->get_dpi_awareness();
}

///////////////////////////////////////////////////////////////////////////////
// system theme
///////////////////////////////////////////////////////////////////////////////

VX_API system_theme get_system_theme()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(system_theme::UNKNOWN);
    return s_video_ptr->get_system_theme();
}

system_theme video_instance::get_system_theme() const
{
    return impl_ptr->data.system_theme_cache;
}

void video_instance::set_system_theme(system_theme theme)
{
    impl_ptr->data.system_theme_cache = theme;
}

///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

VX_API bool compare_display_modes(const display_mode& mode1, const display_mode& mode2)
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

void video_instance::update_displays()
{
    impl_ptr->update_displays();
}

////////////////////////////////////////

std::vector<display_id> video_instance::list_displays() const
{
    std::vector<display_id> displays(data.displays.size());

    for (size_t i = 0; i < data.displays.size(); ++i)
    {
        displays[i] = data.displays[i].data.id;
    }

    return displays;
}

////////////////////////////////////////

size_t video_instance::get_display_index(display_id id) const
{
    for (size_t i = 0; i < data.displays.size(); ++i)
    {
        if (data.displays[i].data.id == id)
        {
            return i;
        }
    }

    return SIZE_MAX;
}

////////////////////////////////////////

bool video_instance::is_display_connected(display_id id) const
{
    for (const display_instance& d : data.displays)
    {
        if (d.data.id == id)
        {
            return true;
        }
    }

    return false;
}

////////////////////////////////////////

display_instance* video_instance::get_display_instance(display_id id)
{
    if (id == INVALID_ID)
    {
        return nullptr;
    }

    for (display_instance& d : data.displays)
    {
        if (d.data.id == id)
        {
            return &d;
        }
    }

    return nullptr;
}

const display_instance* video_instance::get_display_instance(display_id id) const
{
    if (id == INVALID_ID)
    {
        return nullptr;
    }

    for (const display_instance& d : data.displays)
    {
        if (d.data.id == id)
        {
            return &d;
        }
    }

    return nullptr;
}

////////////////////////////////////////

VX_API display get_primary_display()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(invalid_display);
    return s_video_ptr->get_primary_display();
}

display_id video_instance::get_primary_display() const
{
    return !data.displays.empty() ? INVALID_ID : data.displays.front().data.id;
}

////////////////////////////////////////

VX_API display get_display_for_point(const math::vec2i& p)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(invalid_display);
    return s_video_ptr->get_display_for_point(p);
}

display_id video_instance::get_display_for_point(const math::vec2i& p) const
{
    display_id closest = INVALID_ID;
    int32_t closest_dist = std::numeric_limits<int32_t>::max();

    for (const display_instance& di : data.displays)
    {
        const math::recti bounds = di.get_bounds();
        if (bounds.empty())
        {
            continue;
        }

        if (math::g2::contains(bounds, p))
        {
            return di.as_display();
        }

        const math::vec2i c = math::g2::closest(bounds, p);
        const int32_t dist = math::distance_squared(c, p);
        if (dist < closest_dist)
        {
            closest = di.as_display();
            closest_dist = dist;
        }
    }

    return closest;
}

////////////////////////////////////////

VX_API display get_display_for_rect(const math::recti& rect)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(invalid_display);
    return s_video_ptr->get_display_for_rect(rect);
}

display_id video_instance::get_display_for_rect(const math::recti& rect) const
{
    return get_display_for_point(rect.center());
}

////////////////////////////////////////

display_id video_instance::get_display_at_origin(const math::vec2i& o) const
{
    for (const display_instance& di : data.displays)
    {
        if (di.get_work_area().position == o)
        {
            return di.as_display();
        }
    }

    return invalid_display;
}

////////////////////////////////////////

VX_API display get_display_for_window(const window& w)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(invalid_display);
    return s_video_ptr->get_display_for_window(w);
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1769

display_id video_instance::get_display_for_window(const window& w) const
{
    display_id d = INVALID_ID;

    //if (!w.validate())
    //{
    //    return d;
    //}
    //
    //if (w.is_fullscreen())
    //{
    //    d = get_display(w.m_current_fullscreen_mode.m_display_id);
    //
    //    if (!d)
    //    {
    //        d = get_display(w.m_pending_display_id);
    //    }
    //}
    //
    //if (!d)
    //{
    //    // first try any built in method that the os may have
    //    d = get_display_for_window_impl(w);
    //}
    //
    //if (!d)
    //{
    //    if (w.is_fullscreen())
    //    {
    //        // When fullscreen windows are moved between displays of different sizes,
    //        // the window size and position updates may arrive out of order. This can
    //        // temporarily make the window larger than the display. In such cases,
    //        // using the center of the window rectangle might incorrectly identify the
    //        // display, so we use the origin instead.
    //        d = get_display_for_point(w.m_position);
    //    }
    //    else
    //    {
    //        d = get_display_for_rect(math::recti(w.m_position, w.m_size));
    //    }
    //}
    //
    //// The primary display is a good default
    //if (!d)
    //{
    //    d = get_primary_display();
    //}

    return d;
}

////////////////////////////////////////

VX_API math::recti get_desktop_area()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(math::recti{});
    return s_video_ptr->get_desktop_area();
}

math::recti video_instance::get_desktop_area() const
{
    math::recti area;

    for (const display_instance& di : data.displays)
    {
        area = math::g2::bounding_box(area, di.get_bounds());
    }

    return area;
}

///////////////////////////////////////
// display members
///////////////////////////////////////

VX_API std::string display::get_name() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
    return s_video_ptr->get_display_name(m_id);
}

std::string video_instance::get_display_name(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_name() : std::string{};
}

const std::string& display_instance::get_name() const
{
    return data.name;
}

///////////////////////////////////////

VX_API display_orientation display::get_orientation() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(display_orientation::UNKNOWN);
    return s_video_ptr->get_display_orientation(m_id);
}

display_orientation video_instance::get_display_orientation(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_orientation() : display_orientation::UNKNOWN;
}

display_orientation display_instance::get_orientation() const
{
    return data.orientation;
}

///////////////////////////////////////

VX_API math::vec2 display::get_content_scale() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(math::vec2{});
    return s_video_ptr->get_display_content_scale(m_id);
}

math::vec2 video_instance::get_display_content_scale(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_content_scale() : math::vec2{};
}

math::vec2 display_instance::get_content_scale() const
{
    return data.content_scale;
}

///////////////////////////////////////

VX_API math::recti display::get_bounds() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(math::recti{});
    return s_video_ptr->get_display_bounds(m_id);
}

math::recti video_instance::get_display_bounds(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_bounds() : math::recti{};
}

math::recti display_instance::get_bounds() const
{
    math::recti bounds;

#if VX_VIDEO_HAVE_DISPLAY_GET_BOUNDS

    if (impl_ptr->get_bounds(bounds))
    {
        return bounds;
    }

#endif // VX_VIDEO_HAVE_DISPLAY_GET_BOUNDS

    // assume displays are left to right
    if (data.id != video->get_primary_display())
    {
        const size_t i = video->get_display_index(data.id);
        VX_ASSERT(i != SIZE_MAX);
        bounds = video->data.displays[i].get_bounds();
        bounds.position.x += bounds.size.x;
    }

    bounds.size = data.current_mode.resolution;
    return bounds;
}

///////////////////////////////////////

VX_API math::recti display::get_work_area() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(math::recti{});
    return s_video_ptr->get_display_work_area(m_id);
}

math::recti video_instance::get_display_work_area(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_work_area() : math::recti{};
}

math::recti display_instance::get_work_area() const
{
    math::recti rect;

    if (data.id == video->get_primary_display())
    {
        const char* hint = video->app->data.hints_ptr->get_hint(hint::HINT_VIDEO_DISPLAY_USABLE_BOUNDS);
        if (parse_display_usable_bounds_hint(hint, rect))
        {
            return rect;
        }
    }

#if VX_VIDEO_HAVE_DISPLAY_GET_WORK_AREA

    if (impl_ptr->get_work_area(rect))
    {
        return rect;
    }

#endif // VX_VIDEO_HAVE_DISPLAY_GET_WORK_AREA

    // just give the entire display_bounds
    return get_bounds();
}

///////////////////////////////////////

void display_instance::init_modes() const
{
    if (!data.modes.empty())
    {
        return;
    }

#if VX_VIDEO_HAVE_DISPLAY_LIST_MODES

    impl_ptr->list_display_modes(data.modes);

    for (display_mode_instance& m : data.modes)
    {
        m.data.display_id = data.id;
    }

#endif // VX_VIDEO_HAVE_DISPLAY_LIST_MODES
}

///////////////////////////////////////

VX_API bool display::get_desktop_mode(display_mode& mode) const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->get_display_desktop_mode(m_id, mode);
}

bool video_instance::get_display_desktop_mode(display_id id, display_mode& mode) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_desktop_mode(mode) : false;
}

bool display_instance::get_desktop_mode(display_mode& mode) const
{
    mode = data.desktop_mode.data.mode;
    return true;
}

///////////////////////////////////////

VX_API bool display::get_current_mode(display_mode& mode) const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->get_display_current_mode(m_id, mode);
}

bool video_instance::get_display_current_mode(display_id id, display_mode& mode) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->get_current_mode(mode) : false;
}

bool display_instance::get_current_mode(display_mode& mode) const
{
    init_modes();
    mode = data.current_mode;
    return true;
}

///////////////////////////////////////

VX_API bool display::set_current_mode(const display_mode& mode)
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->set_display_current_mode(m_id, mode);
}

bool video_instance::set_display_current_mode(display_id id, const display_mode& mode)
{
    display_instance* d = get_display_instance(id);
    return d ? d->set_current_mode(mode) : false;
}

// https://github.com/libsdl-org/SDL/blob/main/src/video/SDL_video.c#L1509

VX_API bool display_instance::set_current_mode(const display_mode& mode)
{
#if VX_VIDEO_MODE_SWITCHING_EMULATED && defined(VX_VIDEO_X11)

    return true;

#endif // VX_VIDEO_MODE_SWITCHING_EMULATED

    display_mode_instance* native_mode = nullptr;

    // we treat the desktop mode like the default, we consider setting the
    // display mode to the desktop mode to be resetting the display mode
    const bool is_desktop_mode = compare_display_modes(data.desktop_mode.data.mode, mode);

    if (is_desktop_mode)
    {
        native_mode = &data.desktop_mode;
    }
    else
    {
        // make sure the display mode is supported
        native_mode = const_cast<display_mode_instance*>(find_mode(mode));

        // unsupported mode
        if (!native_mode)
        {
            return false;
        }
    }

    // check if the mode is already set
    if (compare_display_modes(native_mode->data.mode, mode))
    {
        return true;
    }

#if VX_VIDEO_HAVE_DISPLAY_SET_MODE

    // set the mode
    data.setting_display_mode = true;
    const bool set = impl_ptr->set_display_mode(*native_mode, is_desktop_mode);
    data.setting_display_mode = false;

    if (!set)
    {
        return false;
    }

#endif // VX_VIDEO_HAVE_DISPLAY_SET_MODE

    video->post_display_current_mode_changed(*this, mode);
    return true;
}

///////////////////////////////////////

VX_API void display::reset_mode()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID();
    s_video_ptr->reset_display_mode(m_id);
}

void video_instance::reset_display_mode(display_id id)
{
    display_instance* d = get_display_instance(id);
    if (d) d->reset_mode();
}

VX_API void display_instance::reset_mode()
{
    set_current_mode(data.desktop_mode.data.mode);
}

///////////////////////////////////////

VX_API std::vector<display_mode> display::list_modes() const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(std::vector<display_mode>{});
    return s_video_ptr->list_display_modes_for_display(m_id);
}

std::vector<display_mode> video_instance::list_display_modes_for_display(display_id id) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->list_modes() : std::vector<display_mode>{};
}

std::vector<display_mode> display_instance::list_modes() const
{
    init_modes();

    std::vector<display_mode> modes(data.modes.size());

    for (size_t i = 0; i < modes.size(); ++i)
    {
        modes[i] = data.modes[i].data.mode;
    }

    return modes;
}

///////////////////////////////////////

const display_mode_instance* display_instance::find_mode(const display_mode& mode) const
{
    // if we haven't listed the modes yet, list them now
    init_modes();

    for (const display_mode_instance& m : data.modes)
    {
        if (compare_display_modes(m.data.mode, mode))
        {
            return &m;
        }
    }

    return nullptr;
}

///////////////////////////////////////

VX_API bool display::has_mode(const display_mode& mode) const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->display_has_mode(m_id);
}

bool video_instance::display_has_mode(display_id id, const display_mode& mode) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->has_mode(mode) : false;
}

bool display_instance::has_mode(const display_mode& mode) const
{
    init_modes();

    for (const display_mode_instance& m : data.modes)
    {
        if (compare_display_modes(m.data.mode, mode))
        {
            return true;
        }
    }

    return false;
}

///////////////////////////////////////

VX_API bool display::find_closest_mode(const display_mode& mode, display_mode& closest) const
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->find_closest_display_mode_for_display(m_id, mode, closest);
}

bool video_instance::find_closest_display_mode_for_display(display_id id, const display_mode& mode, display_mode& closest) const
{
    const display_instance* d = get_display_instance(id);
    return d ? d->find_closest_mode(mode, closest) : false;
}

bool display_instance::find_closest_mode(const display_mode& mode, display_mode& closest) const
{
    const int width = mode.resolution.x;
    const int height = mode.resolution.y;
    float refresh_rate = mode.refresh_rate;

    // compute target aspect ratio from width/height (fallback to 1.0f if height is 0 to avoid division by zero)
    const float aspect_ratio = (height > 0) ? (static_cast<float>(width) / height) : 1.0f;

    // if no refresh rate was specified, use the desktop's current refresh rate
    if (refresh_rate <= 0.0f)
    {
        refresh_rate = data.desktop_mode.data.mode.refresh_rate;
    }

    // if we haven't listed the modes yet, list them now
    init_modes();

    bool closest_set = false;

    for (const display_mode_instance& mi : data.modes)
    {
        const display_mode& mode = mi.data.mode;

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
        if (closest_set)
        {
            const float current_mode_aspect_ratio = static_cast<float>(mode.resolution.x) / mode.resolution.y;
            const float closest_mode_aspect_ratio = static_cast<float>(closest.resolution.x) / closest.resolution.y;

            // Prefer mode with aspect ratio closer to target
            if (math::abs(aspect_ratio - closest_mode_aspect_ratio) < math::abs(aspect_ratio - current_mode_aspect_ratio))
            {
                continue;
            }

            // If resolutions are the same, prefer refresh rate closer to target
            if ((mode.resolution == closest.resolution) &&
                (math::abs(refresh_rate - closest.refresh_rate) < math::abs(refresh_rate - mode.refresh_rate)))
            {
                continue;
            }
        }

        closest = mode;
        closest_set = true;
    }

    return closest_set;
}

///////////////////////////////////////////////////////////////////////////////
// screen saver
///////////////////////////////////////////////////////////////////////////////

VX_API bool enable_screen_saver()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->enable_screen_saver();
}

bool video_instance::enable_screen_saver()
{
    if (!data.suspend_screen_saver)
    {
        return true;
    }

    data.suspend_screen_saver = false;

#if VX_VIDEO_HAVE_SUSPEND_SCREENSAVER

    return impl_ptr->suspend_screen_saver();

#endif // VX_VIDEO_HAVE_SUSPEND_SCREEN_SAVER

    VX_UNSUPPORTED("enable_screen_saver()");
    return false;
}

////////////////////////////////////////

VX_API bool screen_saver_enabled()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->screen_saver_enabled();
}

bool video_instance::screen_saver_enabled() const
{
    return !data.suspend_screen_saver;
}

////////////////////////////////////////

VX_API bool disable_screen_saver()
{
    VX_CHECK_VIDEO_SUBSYSTEM_INIT(false);
    return s_video_ptr->disable_screen_saver();
}

bool video_instance::disable_screen_saver()
{
    if (data.suspend_screen_saver)
    {
        return true;
    }

    data.suspend_screen_saver = true;

#if VX_VIDEO_HAVE_SUSPEND_SCREEN_SAVER

    return impl_ptr->suspend_screen_saver();

#endif // VX_VIDEO_HAVE_SUSPEND_SCREEN_SAVER

    VX_UNSUPPORTED("disable_screen_saver()");
    return false;
}

/////////////////////////////////////////////////////////////////////////////////
//// window
/////////////////////////////////////////////////////////////////////////////////
//
//window* video_instance::create_window(const window_config& config)
//{
//    owner_ptr<window> w(new window);
//    if (w && w->create(config) && w->validate())
//    {
//        data.windows.push_back(std::move(w));
//        return data.windows.back().get();
//    }
//
//    return nullptr;
//}
//
//VX_API window* const create_window(const window_config& config)
//{
//    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
//    return video_instance::create_window(config);
//}
//
//////////////////////////////////////////
//
//void video_instance::destroy_window(window& w)
//{
//    VX_ASSERT(w.validate());
//
//    const window_id id = w.m_id;
//    for (auto it = data.windows.begin(); it != data.windows.end(); ++it)
//    {
//        if ((*it)->m_id == w.m_id)
//        {
//            data.windows.erase(it);
//            break;
//        }
//    }
//
//    // Make sure no displays reference the window
//    for (auto& d : data.displays)
//    {
//        if (d->m_fullscreen_window_id == id)
//        {
//            d->m_fullscreen_window_id = INVALID_ID;
//        }
//    }
//}
//
//VX_API void destroy_window(window& w)
//{
//    VX_CHECK_VIDEO_SUBSYSTEM_INIT_VOID();
//    video_instance::destroy_window(w);
//}
//
//////////////////////////////////////////
//
//void video_instance::destroy_windows()
//{
//    data.windows.clear();
//}
//
//////////////////////////////////////////
//
//window* video_instance::get_window(window_id id)
//{
//    if (id == INVALID_ID)
//    {
//        return nullptr;
//    }
//
//    for (const auto& w : data.windows)
//    {
//        if (w->m_id == id)
//        {
//            return w.get();
//        }
//    }
//
//    return nullptr;
//}
//
//VX_API window* const get_window(window_id id)
//{
//    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
//    return video_instance::get_window(id);
//}
//
//////////////////////////////////////////
//
//window_id video_instance::get_window_id(const window& w)
//{
//    return w.m_id;
//}
//
//////////////////////////////////////////
//
//window_id video_instance::window_count()
//{
//    return data.windows.size();
//}
//
//VX_API size_t window_count()
//{
//    VX_CHECK_VIDEO_SUBSYSTEM_INIT(0);
//    return video_instance::window_count();
//}
//
//////////////////////////////////////////
//
//window* video_instance::enum_windows(size_t i)
//{
//    if (data.windows.size() < i)
//    {
//        return data.windows[i].get();
//    }
//
//    return nullptr;
//}
//
//VX_API window* const enum_windows(size_t i)
//{
//    VX_CHECK_VIDEO_SUBSYSTEM_INIT(nullptr);
//    return video_instance::enum_windows(i);
//}
//
//////////////////////////////////////////
//
//void video_instance::check_window_display_changed(window& w)
//{
//    VX_ASSERT(w.validate());
//
//    if (!has_capabilities(capabilities::SENDS_DISPLAY_CHANGES))
//    {
//        return;
//    }
//
//    display* new_display = get_display_for_window(w);
//    if (!new_display)
//    {
//        return;
//    }
//
//    window_id w_id = w.m_id;
//
//    display_id new_window_display_id = new_display->m_id;
//    display_id old_window_display_id = w.m_last_display_id;
//
//    window_id new_display_fullscreen_window_id = new_display->m_fullscreen_window_id;
//
//    if (new_window_display_id != old_window_display_id)
//    {
//        for (const auto& d : data.displays)
//        {
//            display_id display_id = d->m_id;
//            window_id display_fullscreen_window_id = d->m_fullscreen_window_id;
//
//            if (display_fullscreen_window_id == w_id)
//            {
//                if (display_id != new_window_display_id)
//                {
//                    // We found an old display that still has the window set as
//                    // its fullscreen window
//
//                    if (new_display_fullscreen_window_id && new_display_fullscreen_window_id != w_id)
//                    {
//                        // If the new display already has a fullscreen window, minimize it
//
//                        window* ow = get_window(new_display_fullscreen_window_id);
//                        if (ow)
//                        {
//                            ow->minimize();
//                        }
//                    }
//
//                    new_display->m_fullscreen_window_id = w_id;
//                    d->m_fullscreen_window_id = 0;
//                }
//
//                break;
//            }
//        }
//
//        w.post_window_display_changed(*new_display);
//    }
//}
//
//////////////////////////////////////////
//
//window* video_instance::get_active_window()
//{
//    for (auto& w : data.windows)
//    {
//        if (!w->m_destroying)
//        {
//            return w.get();
//        }
//    }
//
//    return nullptr;
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// fullscreen helpers
/////////////////////////////////////////////////////////////////////////////////
//
//window_id video_instance::get_display_fullscreen_window_id(const display& d)
//{
//    return d.m_fullscreen_window_id;
//}
//
//void video_instance::set_display_fullscreen_window_id(display& d, window_id id)
//{
//    d.m_fullscreen_window_id = id;
//}
//
//void video_instance::clear_display_fullscreen_window_id(display& d)
//{
//    d.m_fullscreen_window_id = INVALID_ID;
//}
//
//display* video_instance::find_display_with_fullscreen_window(const window& w)
//{
//    for (const auto& d : data.displays)
//    {
//        if (d->m_fullscreen_window_id == w.m_id)
//        {
//            return d.get();
//        }
//    }
//
//    return nullptr;
//}
//
/////////////////////////////////////////////////////////////////////////////////
//// event handlers
/////////////////////////////////////////////////////////////////////////////////
//
//#if VX_VIDEO_HAVE_WAIT_EVENT_TIMEOUT
//
//// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_events.c#L1572
//
//bool video_instance::wait_event_timeout(const window* w, time::time_point t)
//{
//    bool status = false;
//
//    // Obtain the pointer to the wakeup window so other threads
//    // know to wake this window up if an event is added to the queue.
//    data.wakeup_window = w;
//    //status = _priv::window_impl::wait_event_timeout();
//    data.wakeup_window = nullptr;
//
//    return status;
//}
//
//#endif // VX_VIDEO_HAVE_WAIT_EVENT_TIMEOUT
//
//#if VX_VIDEO_HAVE_SEND_WAKEUP_EVENT
//
//void video_instance::send_wakeup_event()
//{
//    // This function is called when a new event is added to the queue
//    // while another thread may be blocked waiting for events.
//    //
//    // The waiting thread sets data.wakeup_window before calling into the OS
//    // to wait for events. If we get here, it means new activity happened
//    // and we must wake that thread immediately.
//    //
//    // We clear data.wakeup_window atomically right away to ensure:
//    //   1. Only one wakeup happens per wait cycle (multiple threads adding
//    //      events won’t flood the OS queue with wakeup messages).
//    //   2. Other threads are free to reuse or take ownership of the window
//    //      handle without being blocked by us holding it longer than needed.
//    //
//    // If there was a valid wakeup window, we send the wakeup event through
//    // the platform-specific mechanism (e.g. PostMessage on Windows).
//
//    const window* w = data.wakeup_window.exchange(nullptr);
//    if (w)
//    {
//        //w->send_wakeup_event();
//    }
//}
//
//#endif // VX_VIDEO_HAVE_SEND_WAKEUP_EVENT

bool video_instance::post_system_theme_changed(system_theme theme)
{
    if (theme == impl_ptr->data.system_theme_cache)
    {
        return false;
    }

    event::event e{};
    e.type = event::APP_SYSTEM_THEME_CHANGED;
    e.system_theme_changed.system_theme = theme;

    const bool posted = app->data.events_ptr->push_event(e);
    return posted;
}

bool video_instance::post_display_added(const display_instance& d)
{
    if (d.data.id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_ADDED;
    e.display_added.display_id = d.data.id;
    const bool posted = app->data.events_ptr->push_event(e);

    on_display_added();
    return posted;
}

void video_instance::on_display_added()
{
    for (const auto& w : data.windows)
    {
        video_instance::check_window_display_changed(*w);
    }
}

bool video_instance::post_display_removed(const display_instance& d)
{
    if (d.data.id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_REMOVED;
    e.display_removed.display_id = d.data.id;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

bool video_instance::post_display_moved(const display_instance& d)
{
    if (d.data.id == INVALID_ID)
    {
        return false;
    }

    event::event e{};
    e.type = event::DISPLAY_MOVED;
    e.display_moved.display_id = d.data.id;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

bool video_instance::post_display_orientation_changed(display_instance& d, display_orientation orientation)
{
    if (d.data.id == INVALID_ID)
    {
        return false;
    }

    if (orientation == display_orientation::UNKNOWN || d.get_orientation() == orientation)
    {
        return false;
    }

    d.data.orientation = orientation;

    event::event e{};
    e.type = event::DISPLAY_ORIENTATION_CHANGED;
    e.display_orientation_changed.display_id = d.data.id;
    e.display_orientation_changed.orientation = orientation;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

bool video_instance::post_display_desktop_mode_changed(display_instance& d, const display_mode& mode)
{
    if (d.data.id == INVALID_ID)
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
    e.display_desktop_mode_changed.display_id = d.data.id;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

bool video_instance::post_display_current_mode_changed(display_instance& d, const display_mode& mode)
{
    if (d.data.id == INVALID_ID)
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
    e.display_current_mode_changed.display_id = d.data.id;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

bool video_instance::post_display_content_scale_changed(display_instance& d, const math::vec2& content_scale)
{
    if (d.data.id == INVALID_ID)
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
    e.display_content_scale_changed.display_id = d.data.id;
    e.display_content_scale_changed.x = content_scale.x;
    e.display_content_scale_changed.y = content_scale.y;
    const bool posted = app->data.events_ptr->push_event(e);

    return posted;
}

} // namespace video
} // namespace app
} // namespace vx