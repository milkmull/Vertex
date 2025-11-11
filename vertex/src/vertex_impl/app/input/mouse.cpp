#include "vertex/app/input/touch.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/input/_platform/platform_mouse.hpp"
#include "vertex_impl/app/video/_platform/platform_features.hpp"
#include "vertex_impl/app/video/video_internal.hpp"
#include "vertex/math/geometry/2d/functions/collision.hpp"

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
        mouse->update_capture(false);
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
    data.quitting = true;

    if (data.added_mouse_touch_device)
    {
        video->data.touch_ptr->remove_touch(touch::mouse_touch_id);
        data.added_mouse_touch_device = false;
    }

    if (data.added_pen_touch_device)
    {
        video->data.touch_ptr->remove_touch(touch::pen_touch_id);
    }

    //if (data.cap)

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
// input source
//=============================================================================

click_state* input_source::get_click_state(uint8_t button)
{
    if (button >= data.click_states.size())
    {
        data.click_states.resize(button);
    }

    return &data.click_states.at(button);
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
        if (it->data.id == id)
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

input_source* mouse_instance::get_input_source(mouse_id id, bool down, uint8_t button)
{
    input_source* source = nullptr;

    for (input_source& is : data.sources)
    {
        if (is.data.id == id)
        {
            source = &is;
            break;
        }
    }

    if (!down && (!source || !(source->data.button_state & button_mask(button))))
    {
        // Might be a button release from a transition between mouse messages and raw input.
        // See if there is a source that already has the button down and use that instead.
        for (input_source& is : data.sources)
        {
            if (is.data.button_state & button_mask(button))
            {
                source = &is;
                break;
            }
        }
    }

    if (source)
    {
        return source;
    }

    // Add a new source if not found
    input_source new_source;
    new_source.data.id = id;
    data.sources.push_back(std::move(new_source));

    return &data.sources.back();
}

//=============================================================================
// Focus (window association)
//=============================================================================

video::window_id mouse_instance::get_focus() const
{
    return data.focus;
}

//=============================================================================

video::window_instance* mouse_instance::get_focus_instance()
{
    return video->get_window_instance(data.focus);
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
            w->send_mouse_leave();
        }
    }

    data.focus = w;
    data.has_position = false;

    if (is_valid_id(data.focus))
    {
        video::window_instance* w = get_focus_instance();
        if (w)
        {
            w->send_mouse_enter();
        }
    }

    // update cursor visibility
    redraw_cursor();
}

//=============================================================================

bool mouse_instance::update_focus(video::window_instance* w, float x, float y, button button_state, bool send_motion)
{
    const bool in_window = is_position_in_window(w, x, y);
    const video::window_id wid = w ? w->data.id : invalid_id;

    if (!in_window)
    {
        if (wid == data.focus)
        {
            if (send_motion)
            {
                send_motion_internal(time::zero(), w, global_mouse_id, false, x, y);
            }

            set_focus(invalid_id);
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
                send_motion_internal(time::zero(), w, global_mouse_id, false, x, y);
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
            if (include_touch || source.data.id == touch_mouse_id)
            {
                state |= source.data.button_state;
            }
        }
        else
        {
            if (id == source.data.id)
            {
                state |= source.data.button_state;
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

    // make sure these are never NULL for the backend implementations
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

bool mouse_instance::is_position_in_window(const video::window_instance* w, float x, float y)
{
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

//=============================================================================

void mouse_instance::warp_in_window(video::window_instance* w, float x, float y)
{
    maybe_enable_warp_emulation(w, x, y);
    warp_in_window_internal(w, x, y);
}

void mouse_instance::warp_in_window_internal(video::window_instance* w, float x, float y)
{
    if (!w)
    {
        w = get_focus_instance();

        if (!w)
        {
            return;
        }
    }

    if (w->data.flags & video::window_flags::minimized)
    {
        return;
    }

    // ignore previous position when warping
    data.last_x = x;
    data.last_y = y;
    data.has_position = false;

    if (data.relative_mode_enabled)
    {
        if (!data.relative_warp_motion)
        {
            data.x = x;
            data.y = y;
            data.has_position = true;
            return;
        }
    }

#if VX_VIDEO_BACKEND_HAVE_MOUSE_WARP

    if (!data.relative_mode_enabled)
    {
        impl_ptr->warp(w, x, y);
    }
    else

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_WARP

    {
        send_motion_internal(time::zero(), w, global_mouse_id, false, x, y);
    }
}

//=============================================================================

bool mouse_instance::warp_global(float x, float y)
{
#if VX_VIDEO_BACKEND_HAVE_MOUSE_WARP_GLOBAL

    return impl_ptr->warp_global(x, y);

#else

    VX_UNSUPPORTED("set_position_global()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_WARP_GLOBAL
}

//=============================================================================
// Relative Mode
//=============================================================================

void mouse_instance::maybe_enable_warp_emulation(const video::window_instance* w, float x, float y)
{
    if (!data.warp_emulation_prohibited && data.warp_emulation_hint && !data.cursor_visible && !data.warp_emulation_active)
    {
        if (!w)
        {
            w = get_focus_instance();
        }

        if (w)
        {
            const float cx = w->data.size.x * 0.5f;
            const float cy = w->data.size.y * 0.5f;

            if (x >= math::floor(cx) && x <= math::ceil(cx) &&
                y >= math::floor(cy) && y <= math::ceil(cy))
            {
                // Detect two rapid center-warps (within ~30ms):
                // this indicates the system tried to recenter the cursor
                // but relative mode is unavailable, so emulate it.

                const time::time_point now = os::get_ticks();
                if (now - data.last_center_warp_time < time::milliseconds(30))
                {
                    // Try to enable relative mode. If it fails, cancel emulation.
                    data.warp_emulation_active = set_relative_mode(true);
                }

                // Record last warp time to measure subsequent deltas.
                data.last_center_warp_time = now;
                return;
            }
        }

        // Reset timing if warp center condition not met.
        data.last_center_warp_time.zero();
    }
}

//=============================================================================

void mouse_instance::disable_warp_emulation()
{
    if (data.warp_emulation_active)
    {
        set_relative_mode(false);
    }

    data.warp_emulation_prohibited = true;
}

//=============================================================================

bool mouse_instance::set_relative_mode(bool enabled)
{
    if (!enabled)
    {
        data.warp_emulation_active = false;
    }

    if (enabled == data.relative_mode_enabled)
    {
        return true;
    }

#if VX_VIDEO_BACKEND_HAVE_MOUSE_SET_RELATIVE_MOUSE_MODE

    if (!impl_ptr->set_relative_mode(enabled))
    {
        return false;
    }

    data.relative_mode_enabled = enabled;

    if (enabled)
    {
        // update cursor visibility before we potantially warp the mouse
        redraw_cursor();
    }

    video::window_instance* focus_window = video->data.keyboard_ptr->get_focus_instance();

    if (enabled && focus_window)
    {
        set_focus(focus_window->data.id);
    }

    if (focus_window)
    {
        focus_window->update_grab();

        // move the cursor back to where the app expects it
        if (!enabled)
        {
            warp_in_window_internal(focus_window, data.x, data.y);
        }

        update_capture(false);
    }

    if (!enabled)
    {
        // update cursor visibility after restoring mouse position
        redraw_cursor();
    }

    // flush any pending mouse motion
    video->app->data.events_ptr->flush_events(event::mouse_moved);
    return true;

#else

    VX_UNSUPPORTED("set_relative_mode()");
    return false;

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_SET_RELATIVE_MOUSE_MODE
}

//=============================================================================

bool mouse_instance::get_relative_mode() const
{
    return data.relative_mode_enabled;
}

//=============================================================================

bool mouse_instance::update_relative_mode()
{
    video::window_instance* focus_window = video->data.keyboard_ptr->get_focus_instance();
    const bool enabled = (focus_window && (focus_window->data.flags & video::window_flags::mouse_relative_mode));

    if (enabled == data.relative_mode_enabled)
    {
        return true;
    }

    return set_relative_mode(enabled);
}

//=============================================================================
// Capture
//=============================================================================

void mouse_instance::constrain_position(const video::window_instance* w, float* x, float* y) const
{
    if (w && !(w->data.flags & video::window_flags::mouse_capture))
    {
        const math::recti rect = w->get_mouse_rect();
        const math::recti window_rect{ 0, 0, w->data.size.x, w->data.size.y };

        const math::recti mouse_rect = rect.empty()
            ? window_rect
            : math::g2::crop(window_rect, rect);

        if (mouse_rect.empty())
        {
            return;
        }

        const float x_min = static_cast<float>(mouse_rect.left());
        const float x_max = static_cast<float>(mouse_rect.right());
        const float y_min = static_cast<float>(mouse_rect.top());
        const float y_max = static_cast<float>(mouse_rect.bottom());

        if (*x >= static_cast<float>(x_max))
        {
            *x = math::max(static_cast<float>(x_max - 1), data.last_x);
        }
        if (*x < static_cast<float>(x_min))
        {
            *x = static_cast<float>(x_min);
        }

        if (*y >= static_cast<float>(y_max))
        {
            *y = math::max(static_cast<float>(y_max - 1), data.last_y);
        }
        if (*y < static_cast<float>(y_min))
        {
            *y = static_cast<float>(y_min);
        }
    }
}

//=============================================================================
// Event dispatch (internal)
//=============================================================================

#define events_ptr video->app->data.events_ptr

void mouse_instance::send_mouse_added(mouse_id id)
{
    VX_ASSERT(is_valid_id(id));

    event::event e{};
    e.type = event::mouse_added;
    e.mouse_event.common.mouse_id = id;
    events_ptr->push_event(e);
}

//=============================================================================

void mouse_instance::send_mouse_removed(mouse_id id)
{
    VX_ASSERT(is_valid_id(id));

    event::event e{};
    e.type = event::mouse_removed;
    e.mouse_event.common.mouse_id = id;
    events_ptr->push_event(e);
}

//=============================================================================

// https://github.com/libsdl-org/SDL/blob/main/src/events/SDL_mouse.c#L703

void mouse_instance::send_motion(time::time_point t, video::window_instance* w, mouse_id id, bool relative, float x, float y)
{
    if (w && !relative)
    {
        const button button_state = get_button_state(id, true);
        const bool send_motion = (id != touch_mouse_id && id != pen_mouse_id);

        if (!update_focus(w, x, y, button_state, send_motion))
        {
            return;
        }
    }

    send_motion_internal(t, w, id, relative, x, y);
}

//=============================================================================

void mouse_instance::send_motion_internal(time::time_point t, video::window_instance* w, mouse_id id, bool relative, float x, float y)
{
    const video::window_id wid = w ? w->data.id : invalid_id;

    // Convert mouse motion to touch motion if emulation is active.
    // Triggered only when not already from a synthetic touch/pen source,
    // and only when absolute (non-relative) motion occurs.
    if (data.mouse_touch_events)
    {
        if ((id != touch_mouse_id) && (id != pen_mouse_id) && !relative && data.track_mouse_down)
        {
            if (w)
            {
                const float nx = x / static_cast<float>(w->data.size.x);
                const float ny = y / static_cast<float>(w->data.size.y);
                video->data.touch_ptr->send_motion(t, touch::mouse_touch_id, static_cast<touch::finger_id>(button::left), w, nx, ny, 1.0f);
            }
        }
    }

    // Ignore mouse events that are actually coming from touch when that mapping is disabled.
    if (!data.touch_mouse_events && id == touch_mouse_id)
    {
        return;
    }

    float dx = 0.0f;
    float dy = 0.0f;

    if (relative)
    {
        // In relative mode, movement is reported as deltas rather than absolute coordinates.
        if (data.relative_mode_enabled)
        {
            if (data.relative_system_scale_enabled)
            {
#if VX_VIDEO_BACKEND_HAVE_MOUSE_APPLY_SYSTEM_SCALE

                // Optionally scale deltas using system DPI scale (if backend supports it).
                impl_ptr->apply_system_scale(t, w, id, &x, &y);

#endif // VX_VIDEO_BACKEND_HAVE_MOUSE_APPLY_SYSTEM_SCALE
            }

            // Apply user-defined speed scaling for relative mode.
            if (data.relative_speed_scale_enabled)
            {
                x *= data.relative_speed_scale;
                y *= data.relative_speed_scale;
            }
        }
        else
        {
            // Fallback: apply normal movement scaling outside relative mode.
            if (data.normal_speed_scale_enabled)
            {
                x *= data.normal_speed_scale;
                y *= data.normal_speed_scale;
            }
        }

        if (data.integer_mode & integer_mode::motion)
        {
            // Accumulate the fractional relative motion, only process integer portion
            data.integer_mode_residual_motion_x = math::modf(data.integer_mode_residual_motion_x + x, x);
            data.integer_mode_residual_motion_y = math::modf(data.integer_mode_residual_motion_y + y, y);
        }

        dx = x;
        dy = y;

        // Convert from delta to updated absolute coordinates.
        x = data.last_x + dx;
        y = data.last_y + dy;

        constrain_position(w, &x, &y);
    }
    else
    {
        if (data.integer_mode & integer_mode::motion)
        {
            // discard fractional component
            x = math::trunc(x);
            y = math::trunc(y);
        }

        constrain_position(w, &x, &y);

        // Compute deltas only if a previous position exists.
        if (data.has_position)
        {
            dx = x - data.last_x;
            dy = y - data.last_y;
        }
    }

    // Skip redundant events (no movement since last frame).
    if (data.has_position && dx == 0.0f && dy == 0.0f)
    {
        return;
    }

    // Ignore phantom relative movement generated on the first touch.
    if (id == touch_mouse_id && !get_button_state(id, true))
    {
        dx = 0.0f;
        dy = 0.0f;
    }

    // update internal state
    {
        data.x_accu += dx;
        data.y_accu += dy;

        // Update coordinates with constrained deltas in relative mode.
        if (relative && data.has_position)
        {
            data.x = dx;
            data.y = dy;
            constrain_position(w, &data.x, &data.y);
        }
        else
        {
            data.x = x;
            data.y = y;
        }

        data.has_position = true;

        // Record unclamped last known position (for next delta computation).
        data.last_x = relative ? data.x : x;
        data.last_y = relative ? data.y : y;

        // Accumulate click motion used for drag/double-click detection.
        data.click_motion_x += dx;
        data.click_motion_y += dy;
    }

    // post the event
    {
        // Convert event to global mouse if not in relative mode, or if relative motion is being emulated (warp mode).
        if ((!data.relative_mode_enabled || data.warp_emulation_active) && (id != touch_mouse_id) && (id != pen_mouse_id))
        {
            id = global_mouse_id;
        }

        const video::window_instance* focus = get_focus_instance();
        const bool window_is_relative = focus && (focus->data.flags & video::window_flags::mouse_relative_mode);

        // When in relative mode but receiving absolute coordinates suppress warp motion events.
        if (!relative && window_is_relative)
        {
            if (!data.relative_warp_motion)
            {
                return;
            }

            dx = 0.0f;
            dy = 0.0f;
        }

        event::event e{};
        e.type = event::mouse_moved;
        e.time = t;
        e.mouse_event.common.mouse_id = id;
        e.mouse_event.common.window_id = data.focus;
        e.mouse_event.common.x = data.x;
        e.mouse_event.common.y = data.y;
        e.mouse_event.mouse_moved.dx = dx;
        e.mouse_event.mouse_moved.dy = dy;
        events_ptr->push_event(e);
    }
}

//=============================================================================

void mouse_instance::send_button(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down)
{
    send_button_internal(t, w, id, b, down, -1);
}

//=============================================================================

void mouse_instance::send_button_internal(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down, int clicks)
{
    input_source* source = get_input_source(id, down, b);
    if (!source)
    {
        return;
    }

    button button_state = source->data.button_state;
    event::event_type type;

    // Convert mouse motion to touch motion if emulation is active.
    // Triggered only when not already from a synthetic touch/pen source,
    // and only when absolute (non-relative) motion occurs.
    if (data.mouse_touch_events)
    {
        if ((id != touch_mouse_id) && (id != pen_mouse_id) && (static_cast<button>(b) == button::left))
        {
            data.track_mouse_down = down;

            if (w)
            {
                const float nx = data.x / static_cast<float>(w->data.size.x);
                const float ny = data.y / static_cast<float>(w->data.size.y);

                type = data.track_mouse_down ? event::finger_down : event::finger_up;
                video->data.touch_ptr->send_event(t, touch::mouse_touch_id, static_cast<touch::finger_id>(button::left), w, type, nx, ny, 1.0f);
            }
        }
    }

    // Ignore mouse events that are actually coming from touch when that mapping is disabled.
    if (!data.touch_mouse_events && id == touch_mouse_id)
    {
        return;
    }

    // Determine event type
    if (down)
    {
        type = event::mouse_button_down;
        button_state |= button_mask(b);
    }
    else
    {
        type = event::mouse_button_up;
        button_state &= ~button_mask(b);
    }

    // Do this after calculating button state so button presses gain focus
    if (w && down)
    {
        update_focus(w, data.x, data.y, button_state, true);
    }

    // No update
    if (button_state == source->data.button_state)
    {
        return;
    }
    source->data.button_state = button_state;

    if (clicks < 0)
    {
        click_state* click_state = source->get_click_state(b);
        if (click_state)
        {
            if (down)
            {
                const time::time_point now = os::get_ticks();

                if ((now >= (click_state->last_click_time + data.double_click_time)) ||
                    (math::abs(data.click_motion_x - click_state->motion_x) > data.double_click_radius) ||
                    (math::abs(data.click_motion_y - click_state->motion_y) > data.double_click_radius))
                {
                    click_state->click_count = 0;
                }

                click_state->last_click_time = now;
                click_state->motion_x = data.click_motion_x;
                click_state->motion_y = data.click_motion_y;

                if (click_state->click_count < 255)
                {
                    ++click_state->click_count;
                }
            }

            clicks = click_state->click_count;
        }
        else
        {
            clicks = 1;
        }
    }

    // post the event
    {
        // Convert event to global mouse if not in relative mode, or if relative motion is being emulated (warp mode).
        if ((!data.relative_mode_enabled || data.warp_emulation_active) && (id != touch_mouse_id) && (id != pen_mouse_id))
        {
            id = global_mouse_id;
        }
        else
        {
            id = source->data.id;
        }

        const video::window_id wid = w ? w->data.id : invalid_id;

        event::event e{};
        e.type = type;
        e.mouse_event.common.mouse_id = id;
        e.mouse_event.common.window_id = wid;
        e.mouse_event.common.x = data.x;
        e.mouse_event.common.y = data.y;
        e.mouse_event.mouse_button_down.button = b;
        e.mouse_event.mouse_button_down.clicks = clicks;
        events_ptr->push_event(e);
    }

    // Do this after pushing event so bhtton release can lose focus
    if (w && !down)
    {
        update_focus(w, data.x, data.y, button_state, true);
    }

    // Auto capture the mouse when buttons are pressed
    if (data.auto_capture)
    {
        update_capture(false);
    }
}

//=============================================================================

void mouse_instance::send_button_clicks(time::time_point t, video::window_instance* w, mouse_id id, uint8_t b, bool down, int clicks)
{
    clicks = math::max(clicks, 0);
    send_button_internal(t, w, id, b, down, clicks);
}

//=============================================================================

void mouse_instance::send_wheel(time::time_point t, const video::window_instance* w, mouse_id id, float x, float y, wheel_direction direction)
{
    const video::window_id wid = w ? w->data.id : invalid_id;

    if (w)
    {
        set_focus(wid);
    }

    if (x == 0.0f && y == 0.0f)
    {
        return;
    }

    // push event
    {
        // Convert event to global mouse if not in relative mode, or if relative motion is being emulated (warp mode).
        if (!data.relative_mode_enabled || data.warp_emulation_active)
        {
            id = global_mouse_id;
        }

        event::event e{};
        e.type = event::mouse_wheel;
        e.time = t;
        e.mouse_event.common.mouse_id = id;
        e.mouse_event.common.window_id = wid;
        e.mouse_event.common.x = data.x;
        e.mouse_event.common.y = data.y;
        e.mouse_event.mouse_wheel.direction = direction;

        float ix, iy;
       
        data.integer_mode_residual_scroll_x = math::modf(data.integer_mode_residual_scroll_x + x, ix);
        data.integer_mode_residual_scroll_y = math::modf(data.integer_mode_residual_scroll_y + y, iy);

        e.mouse_event.mouse_wheel.ix = static_cast<int>(ix);
        e.mouse_event.mouse_wheel.iy = static_cast<int>(iy);

        if (data.integer_mode & integer_mode::scroll)
        {
            e.mouse_event.mouse_wheel.x = ix;
            e.mouse_event.mouse_wheel.y = iy;
        }
        else
        {
            e.mouse_event.mouse_wheel.x = x;
            e.mouse_event.mouse_wheel.y = y;
        }

        events_ptr->push_event(e);
    }
}

} // namespace mouse
} // namespace app
} // namespace vx