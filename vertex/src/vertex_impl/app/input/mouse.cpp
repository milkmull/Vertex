#include "vertex/app/input/touch.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/input/_platform/platform_mouse.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace mouse {

//=============================================================================
// hints
//=============================================================================

static void double_click_time_hint_watcher(const hint::hint_t name, const char* old_value, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);

#   define default_value 500

#if VX_VIDEO_BACKEND_HAVE_MOUSE_GET_DOUBLE_CLICK_TIME

    int64_t t_ms = mouse->impl_ptr->get_double_click_time().as_milliseconds();

    if (t_ms <= 0)
    {
        t_ms = default_value;
    }

#else

    const int64_t t_ms = default_value;

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_GET_DOUBLE_CLICK_TIME

#   undef default_value

    mouse->data.double_click_time = time::milliseconds(hint::parse_integer(new_value, t_ms));
}

//=============================================================================

static void double_click_radius_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.double_click_radius = hint::parse_integer(new_value, 32);
}

//=============================================================================

static void normal_speed_scale_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);

    mouse->data.normal_speed_scale = hint::parse_float(new_value, 1.0f);

    if (mouse->data.normal_speed_scale != 1.0f)
    {
        mouse->data.normal_speed_scale_enabled = true;
    }
    else
    {
        mouse->data.normal_speed_scale_enabled = false;
    }
}

//=============================================================================

static void relative_speed_scale_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);

    mouse->data.relative_speed_scale = hint::parse_float(new_value, 1.0f);

    if (mouse->data.relative_speed_scale != 1.0f)
    {
        mouse->data.relative_speed_scale_enabled = true;
    }
    else
    {
        mouse->data.relative_speed_scale_enabled = false;
    }
}

//=============================================================================

static void relative_mode_center_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.relative_center = hint::parse_boolean(new_value, true);
}

//=============================================================================

static void relative_system_scale_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.relative_system_scale_enabled = hint::parse_boolean(new_value, false);
}

//=============================================================================

static void mouse_warp_emulation_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.warp_emulation_hint = hint::parse_boolean(new_value, true);

    if (!mouse->data.warp_emulation_hint && mouse->data.warp_emulation_active)
    {
        mouse->set_relative_mode(false);
        mouse->data.warp_emulation_active = false;
    }
}

//=============================================================================

static void touch_mouse_events_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.touch_mouse_events = hint::parse_boolean(new_value, true);
}

//=============================================================================

static void mouse_touch_events_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);

#if defined(VX_PLATFORM_MOBILE)
#   define default_value true
#else
#   define default_value false
#endif

    mouse->data.mouse_touch_events = hint::parse_boolean(new_value, default_value);

    if (mouse->data.mouse_touch_events)
    {
        if (!mouse->data.added_mouse_touch_device)
        {
            mouse->video->data.touch_ptr->add_touch(touch::mouse_touch_id, touch::device_type::direct, "mouse_input");
            mouse->data.added_mouse_touch_device = true;
        }
    }
    else
    {
        if (mouse->data.added_mouse_touch_device)
        {
            mouse->video->data.touch_ptr->remove_touch(touch::mouse_touch_id);
            mouse->data.added_mouse_touch_device = false;
        }
    }

#   undef default_value
}

//=============================================================================

static void pen_mouse_events_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.pen_mouse_events = hint::parse_boolean(new_value, true);
}

//=============================================================================

static void pen_touch_events_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.pen_touch_events = hint::parse_boolean(new_value, true);

    if (mouse->data.pen_touch_events)
    {
        if (!mouse->data.added_pen_touch_device)
        {
            mouse->video->data.touch_ptr->add_touch(touch::pen_touch_id, touch::device_type::direct, "pen_input");
            mouse->data.added_pen_touch_device = true;
        }
    }
    else
    {
        if (mouse->data.added_pen_touch_device)
        {
            mouse->video->data.touch_ptr->remove_touch(touch::pen_touch_id);
            mouse->data.added_pen_touch_device = false;
        }
    }
}

//=============================================================================

static void auto_capture_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    const bool auto_capture = hint::parse_boolean(new_value, true);

    if (auto_capture != mouse->data.auto_capture)
    {
        mouse->data.auto_capture = auto_capture;
        mouse->update_mouse_capture(false);
    }
}

//=============================================================================

static void relative_warp_motion_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.relative_warp_motion = hint::parse_boolean(new_value, false);
}

//=============================================================================

static void relative_cursor_visible_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.relative_hide_cursor = !hint::parse_boolean(new_value, false);
    mouse->redraw_cursor();
}

//=============================================================================

static void integer_mode_hint_watcher(const hint::hint_t name, const char*, const char* new_value, void* user_data)
{
    mouse_instance* mouse = static_cast<mouse_instance*>(user_data);
    mouse->data.integer_mode = static_cast<integer_mode>(hint::parse_integer(new_value, integer_mode::none));
}

//=============================================================================
// cursor
//=============================================================================

class cursor_instance_impl {};

cursor_instance::cursor_instance() = default;
cursor_instance::~cursor_instance() = default;

cursor_instance::cursor_instance(cursor_instance&&) noexcept = default;
cursor_instance& cursor_instance::operator=(cursor_instance&&) noexcept = default;

//=============================================================================
// mouse internal
//=============================================================================

mouse_instance::mouse_instance() = default;

mouse_instance::~mouse_instance()
{
    quit();
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_mouse.c#L249

bool mouse_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    data = mouse_data{};

    // hints
    {
        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_double_click_time,
            double_click_time_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_double_click_radius,
            double_click_radius_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_normal_speed_scale,
            normal_speed_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_relative_speed_scale,
            relative_speed_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_relative_system_scale,
            relative_system_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_relative_mode_center,
            relative_mode_center_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_emulate_warp_with_relative,
            mouse_warp_emulation_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::touch_mouse_events,
            touch_mouse_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_touch_events,
            mouse_touch_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::pen_mouse_events,
            pen_mouse_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::pen_touch_events,
            pen_touch_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_auto_capture,
            auto_capture_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_relative_warp_motion,
            relative_warp_motion_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_relative_cursor_visible,
            relative_cursor_visible_hint_watcher,
            this
        );

        video->app->data.hints_ptr->add_hint_callback(
            hint::mouse_integer_mode,
            integer_mode_hint_watcher,
            this
        );
    }

    return true;
}

//=============================================================================

void mouse_instance::quit()
{
    quit_impl();

    // hints
    {
        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_double_click_time,
            double_click_time_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_double_click_radius,
            double_click_radius_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_normal_speed_scale,
            normal_speed_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_relative_speed_scale,
            relative_speed_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_relative_system_scale,
            relative_system_scale_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_relative_mode_center,
            relative_mode_center_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_emulate_warp_with_relative,
            mouse_warp_emulation_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::touch_mouse_events,
            touch_mouse_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_touch_events,
            mouse_touch_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::pen_mouse_events,
            pen_mouse_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::pen_touch_events,
            pen_touch_events_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_auto_capture,
            auto_capture_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_relative_warp_motion,
            relative_warp_motion_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_relative_cursor_visible,
            relative_cursor_visible_hint_watcher,
            this
        );

        video->app->data.hints_ptr->remove_hint_callback(
            hint::mouse_integer_mode,
            integer_mode_hint_watcher,
            this
        );
    }
}

//=============================================================================

bool mouse_instance::init_impl()
{
    VX_ASSERT(video);

    impl_ptr.reset(new mouse_instance_impl);
    if (!impl_ptr)
    {
        return false;
    }

    if (!impl_ptr->init(this))
    {
        quit_impl();
        return false;
    }

    return true;
}

//=============================================================================

void mouse_instance::quit_impl()
{
    if (impl_ptr)
    {
        impl_ptr->quit();
        impl_ptr.reset();
    }
}

//=============================================================================
// Device Management
//=============================================================================

size_t mouse_instance::get_mouse_index(mouse_id id) const
{
    if (!is_valid_id(id))
    {
        return VX_INVALID_INDEX;
    }

    for (size_t i = 0; i < data.mice.size(); ++i)
    {
        if (data.mice[i].id == id)
        {
            return i;
        }
    }

    return VX_INVALID_INDEX;
}

//=============================================================================

const mouse_info* mouse_instance::get_mouse(mouse_id id) const
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (const mouse_info& m : data.mice)
    {
        if (m.id == id)
        {
            return &m;
        }
    }

    return nullptr;
}

mouse_info* mouse_instance::get_mouse(mouse_id id)
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (mouse_info& m : data.mice)
    {
        if (m.id == id)
        {
            return &m;
        }
    }

    return nullptr;
}

//=============================================================================

void mouse_instance::add_mouse(mouse_id id, const char* name)
{
    const size_t i = get_mouse_index(id);
    if (i == VX_INVALID_INDEX)
    {
        // we already know about this mouse
        return;
    }

    VX_ASSERT(is_valid_id(id));

    mouse_info info{};
    info.id = id;
    info.name = name ? name : "Mouse";

    data.mice.push_back(info);
    send_mouse_added(id);
}

//=============================================================================

void mouse_instance::remove_mouse(mouse_id id)
{
    for (auto it = data.mice.begin(); it != data.mice.end(); ++it)
    {
        if (it->id == id)
        {
            data.mice.erase(it);
            break;
        }
    }

    // Remove any mouse input sources for this id
    for (auto it = data.sources.begin(); it != data.sources.end(); ++it)
    {
        if (it->id == id)
        {
            it = data.sources.erase(it);
        }
    }

    if (!data.quitting)
    {
        send_mouse_removed(id);
    }
}

//=============================================================================

bool mouse_instance::any_connected() const
{
    return !data.mice.empty();
}

//=============================================================================

std::vector<mouse_id> mouse_instance::list_mice() const
{
    std::vector<mouse_id> mice(data.mice.size());

    for (size_t i = 0; i < data.mice.size(); ++i)
    {
        mice[i] = data.mice[i].id;
    }
    
    return mice;
}

//=============================================================================

const char* mouse_instance::get_name(mouse_id id) const
{
    const mouse_info* m = get_mouse(id);
    return m ? m->name.c_str() : nullptr;
}

//=============================================================================
// Focus (window association)
//=============================================================================

video::window_instance* mouse_instance::get_focus_instance()
{
    return video->get_window_instance(data.focus);
}

//=============================================================================

video::window_id mouse_instance::get_focus() const
{
    return data.focus;
}

//=============================================================================

void mouse_instance::set_focus(video::window_id w)
{
    if (data.focus == w)
    {
        return;
    }

    // See if the current window has lost focus
    if (is_valid_id(data.focus))
    {
        video::window_instance* w = get_focus_instance();
        if (w)
        {
            w->post_window_mouse_leave();
        }
    }

    data.focus = w;
    data.has_position = false;

    if (is_valid_id(data.focus))
    {
        video::window_instance* w = get_focus_instance();
        if (w)
        {
            w->post_window_mouse_enter();
        }
    }

    // update cursor visibility
    redraw_cursor();
}

//=============================================================================

bool mouse_instance::update_mouse_focus(video::window_id wid, float x, float y, button button_state, bool send_motion)
{
    const bool in_window = is_position_in_window(wid, x, y);

    if (!in_window)
    {
        if (wid == data.focus)
        {
            set_focus(invalid_id);

            if (send_motion)
            {
                //send_mouse_motion(wid, )
            }
        }

        return false;
    }
    else
    {
        if (wid != data.focus)
        {
            set_focus(wid);

            if (send_motion)
            {

            }
        }

        return true;
    }
}

//=============================================================================
// State (buttons + position)
//=============================================================================

button mouse_instance::get_button_state(mouse_id id, bool include_touch) const
{
    button state = button::none;

    for (const input_source& source : data.sources)
    {
        if (id == global_mouse_id || id == touch_mouse_id)
        {
            if (include_touch || source.id == touch_mouse_id)
            {
                state |= source.button_state;
            }
        }
        else
        {
            if (id == source.id)
            {
                state |= source.button_state;
                break;
            }
        }
    }

    return state;
}

//=============================================================================

button mouse_instance::get_state(float* x, float* y) const
{
    if (x)
    {
        *x = data.x;
    }
    if (y)
    {
        *y = data.y;
    }

    return get_button_state(global_mouse_id, true);
}

//=============================================================================

button mouse_instance::get_relative_state(float* x, float* y)
{
    if (x)
    {
        *x = data.x_accu;
    }
    if (y)
    {
        *y = data.y_accu;
    }

    data.x_accu = 0.0f;
    data.y_accu = 0.0f;

    return get_button_state(global_mouse_id, true);
}

//=============================================================================

button mouse_instance::get_global_state(float* x, float* y) const
{
#if VX_VIDEO_BACKEND_HAVE_MOUSE_GET_GLOBAL_MOUSE_STATE

    float tmpx, tmpy;

    // make sure these are never NULL for the backend implementations...
    if (!x)
    {
        x = &tmpx;
    }
    if (!y)
    {
        y = &tmpy;
    }

    *x = *y = 0.0f;
    return impl_ptr->get_global_state(x, y);

#else

    return get_state(x, y);

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_GET_GLOBAL_MOUSE_STATE
}

//=============================================================================
// Position control
//=============================================================================

bool mouse_instance::is_position_in_window(video::window_id wid, float x, float y)
{
    const video::window_instance* w = video->get_window_instance(wid);
    if (!w)
    {
        return false;
    }

    if (w->data.flags & video::window_flags::mouse_capture)
    {
        if (x < 0.0f ||
            y < 0.0f ||
            x >= static_cast<float>(w->data.position.x) ||
            y >= static_cast<float>(w->data.position.y))
        {
            return false;
        }
    }

    return true;
}

//void mouse_instance::set_position_in_window(video::window_id w, const math::vec2& position)
//{
//
//}
//
//void mouse_instance::set_position_global(const math::vec2& position)
//{
//
//}

//=============================================================================
// Event dispatch (internal)
//=============================================================================

bool mouse_instance::send_mouse_added(mouse_id id)
{
    VX_ASSERT(is_valid_id(id));

    event::event e{};
    e.type = event::mouse_added;
    e.mouse_event.common.mouse_id = id;
    const bool posted = video->app->data.events_ptr->push_event(e);

    return posted;
}

//=============================================================================

bool mouse_instance::send_mouse_removed(mouse_id id)
{
    VX_ASSERT(is_valid_id(id));

    event::event e{};
    e.type = event::mouse_removed;
    e.mouse_event.common.mouse_id = id;
    const bool posted = video->app->data.events_ptr->push_event(e);

    return posted;
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_mouse.c#L703

bool mouse_instance::send_mouse_motion(time::time_point t, video::window_id w, mouse_id id, bool relative, float x, float y)
{
    if (is_valid_id(w) && !relative)
    {
        const button button_state = get_button_state(id, true);
        const bool send_motion = (id != touch_mouse_id && id != pen_mouse_id);

        if (!update_mouse_focus(w, x, y, button_state, send_motion))
        {
            return false;
        }
    }

    return send_mouse_motion_internal(t, w, id, relative, x, y);
}

//=============================================================================

bool mouse_instance::send_mouse_motion_internal(time::time_point t, video::window_id w, mouse_id id, bool relative, float x, float y)
{
    return false;
}

//=============================================================================

bool mouse_instance::send_mouse_button(time::time_point t, video::window_id w, mouse_id id, button b, bool down) { return false; }
bool mouse_instance::send_mouse_button_clicks(video::window_id w, mouse_id id, button b, bool down, int clicks) { return false; }
bool mouse_instance::send_mouse_wheel(video::window_id w, mouse_id id, float x, float y, wheel_direction direction) { return false; }
bool mouse_instance::perform_warp_mouse_in_window(video::window_id w, float x, float y, bool ignore_relative_mode) { return false; }

} // namespace mouse
} // namespace app
} // namespace vx