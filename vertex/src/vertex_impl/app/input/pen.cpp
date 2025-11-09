#include "vertex/math/color/functions/common.hpp"
#include "vertex/util/memory.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/pen_internal.hpp"
#include "vertex_impl/app/input/_platform/platform_pen.hpp"
#include "vertex_impl/app/input/touch_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace pen {

//=============================================================================
// lifecycle
//=============================================================================

bool pen_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    return true;
}

//=============================================================================

void pen_instance::quit()
{
    clear_pens(false);
}

//=============================================================================
// pen device
//=============================================================================

pen_device_instance::pen_device_instance() = default;
pen_device_instance::~pen_device_instance() = default;

pen_device_instance::pen_device_instance(pen_device_instance&&) noexcept = default;
pen_device_instance& pen_device_instance::operator=(pen_device_instance&&) noexcept = default;

//=============================================================================

void pen_device_instance::finalize()
{
    if (data.name.empty())
    {
        data.name = std::to_string(data.id);
    }
}

//=============================================================================
// devices
//=============================================================================

bool pen_instance::add_pen(time::time_point t, pen_device_instance& pen)
{
    const pen_id id = data.pen_id_generator.next();
    pen.data.id = id;
    pen.finalize();

    data.mutex.lock();
    data.pens.push_back(std::move(pen));
    data.mutex.unlock();

    post_pen_proximity_in(t, id);
    return true;
}

//=============================================================================

void pen_instance::remove_pen(time::time_point t, pen_id id)
{
    if (!is_valid_id(id))
    {
        return;
    }

    data.mutex.lock();
    remove_pen_internal(t, id, true);
    data.mutex.unlock();
}

void pen_instance::remove_pen_internal(time::time_point t, pen_id id, bool post_event)
{
    for (auto it = data.pens.begin(); it != data.pens.end(); ++it)
    {
        if (it->data.id == id)
        {
            if (post_event)
            {
                post_pen_proximity_out(t, id);
            }

            data.pens.erase(it);
            break;
        }
    }
}

//=============================================================================

void pen_instance::clear_pens(bool post_events)
{
    data.mutex.lock();

    while (!data.pens.empty())
    {
        remove_pen_internal(time::zero(), data.pens[0].data.id, post_events);
    }

    data.mutex.unlock();
}

//=============================================================================

std::vector<pen_id> pen_instance::list_pens() const
{
    data.mutex.lock();

    std::vector<pen_id> devices(data.pens.size());

    for (size_t i = 0; i < data.pens.size(); ++i)
    {
        devices[i] = data.pens[i].data.id;
    }

    data.mutex.unlock();

    return devices;
}

//=============================================================================

bool pen_instance::pen_connected(pen_id id) const
{
    return get_pen_instance(id) != nullptr;
}

//=============================================================================

pen_device_instance* pen_instance::get_pen_instance(pen_id id)
{
    data.mutex.lock();
    pen_device_instance* pen = get_pen_instance_internal(id);
    data.mutex.unlock();

    return pen;
}

const pen_device_instance* pen_instance::get_pen_instance(pen_id id) const
{
    return const_cast<pen_instance*>(this)->get_pen_instance(id);
}

pen_device_instance* pen_instance::get_pen_instance_internal(pen_id id)
{
    for (pen_device_instance& device : data.pens)
    {
        if (device.data.id == id)
        {
            return &device;
        }
    }

    return nullptr;
}

//=============================================================================
// events
//=============================================================================

#define events_ptr video->app->data.events_ptr

void pen_instance::post_pen_proximity_in(time::time_point t, pen_id id)
{
    event::event e{};
    e.type = event::pen_proximity_in;
    e.time = t;
    e.pen_event.common.pen_id = id;

    events_ptr->push_event(e);
}

void pen_instance::post_pen_proximity_out(time::time_point t, pen_id id)
{
    event::event e{};
    e.type = event::pen_proximity_out;
    e.time = t;
    e.pen_event.common.pen_id = id;

    events_ptr->push_event(e);
}

void pen_instance::post_pen_touch(time::time_point t, pen_id id, video::window_id wid, bool eraser, bool down)
{
    bool post_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_device_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            state = pen->data.state.state;
            x = pen->data.state.x;
            y = pen->data.state.y;
            pressure = pen->data.state.axes[axis_type::pressure];

            if (down && !(state & input_state::down))
            {
                state |= input_state::down;
                post_event = true;
            }
            else if (!down && (state & input_state::down))
            {
                state &= ~input_state::down;
                post_event = true;
            }

            if (eraser && !(state & input_state::eraser_tip))
            {
                state |= input_state::eraser_tip;
                post_event = true;
            }
            else if (!eraser && (state & input_state::eraser_tip))
            {
                state &= ~input_state::eraser_tip;
                post_event = true;
            }

            pen->data.state.state = state;
        }
    }

    if (post_event)
    {
        event::event e{};
        e.type = down ? event::pen_down : event::pen_up;
        e.time = t;
        e.pen_event.common.pen_id = id;
        e.pen_event.common.x = x;
        e.pen_event.common.y = y;
        e.pen_event.common.state = state;
        e.pen_event.common.window_id = wid;
        e.pen_event.pen_touch.eraser = eraser;
        events_ptr->push_event(e);

        video::window_instance* w = video->get_window_instance(wid);
        if (w)
        {
            mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

            if (mouse->data.pen_mouse_events)
            {
                if (down)
                {
                    if (!data.pen_touching)
                    {
                        mouse->send_mouse_motion(t, wid, mouse::pen_mouse_id, false, x, y);
                        mouse->send_mouse_button(t, wid, mouse::pen_mouse_id, mouse::button::left, true);
                    }
                }
                else
                {
                    if (data.pen_touching == id)
                    {
                        mouse->send_mouse_button(t, wid, mouse::pen_mouse_id, mouse::button::left, false);
                    }
                }
            }

            if (mouse->data.pen_touch_events)
            {
                if (!data.pen_touching || (data.pen_touching == id))
                {
                    const event::event_type touch_event_type = down ? event::finger_down : event::finger_up;
                    const float nx = x / static_cast<float>(w->data.size.x);
                    const float ny = y / static_cast<float>(w->data.size.y);

                    touch::touch_instance* touch = video->data.touch_ptr.get();
                    touch->post_touch_event(t, touch::pen_touch_id, mouse::button::left, wid, touch_event_type, x, y, pressure);
                }
            }
        }

        if (down)
        {
            if (!is_valid_id(data.pen_touching))
            {
                data.pen_touching = id;
            }
        }
        else
        {
            if (data.pen_touching == id)
            {
                data.pen_touching = invalid_id;
            }
        }
    }
}

//=============================================================================

void pen_instance::post_pen_motion(time::time_point t, pen_id id, video::window_id wid, float x, float y)
{
    bool post_event = false;
    input_state state = input_state::none;
    float pressure = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_device_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            if (pen->data.state.x != x || pen->data.state.y != y)
            {
                pen->data.state.x = x;
                pen->data.state.y = y;
                state = pen->data.state.state;
                pressure = pen->data.state.axes[axis_type::pressure];
                post_event = true;
            }
        }
    }

    if (post_event)
    {
        event::event e{};
        e.type = event::pen_moved;
        e.time = t;
        e.pen_event.common.pen_id = id;
        e.pen_event.common.x = x;
        e.pen_event.common.y = y;
        e.pen_event.common.state = state;
        e.pen_event.common.window_id = wid;
        events_ptr->push_event(e);

        video::window_instance* w = video->get_window_instance(wid);
        if (w)
        {
            mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

            if (data.pen_touching == id)
            {
                if (mouse->data.pen_mouse_events)
                {
                    mouse->send_mouse_motion(t, wid, mouse::pen_mouse_id, false, x, y);
                }
                if (mouse->data.pen_touch_events)
                {
                    const float nx = x / static_cast<float>(w->data.size.x);
                    const float ny = y / static_cast<float>(w->data.size.y);

                    touch::touch_instance* touch = video->data.touch_ptr.get();
                    touch->post_touch_motion(t, touch::pen_touch_id, mouse::button::left, wid, x, y, pressure);
                }
            }
            else if (!is_valid_id(data.pen_touching))
            {
                // send mouse motion for pens that aren't touching
                mouse->send_mouse_motion(t, wid, mouse::pen_mouse_id, false, x, y);
            }
        }
    }
}

//=============================================================================

void pen_instance::post_pen_axis(time::time_point t, pen_id id, video::window_id wid, axis_type axis, float value)
{
    VX_ASSERT(axis >= 0 && axis < axis_count);

    bool post_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_device_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            if (pen->data.state.axes[axis] != value)
            {
                pen->data.state.axes[axis] = value;
                state = pen->data.state.state;
                x = pen->data.state.x;
                y = pen->data.state.y;
                post_event = true;
            }
        }
    }

    if (post_event)
    {
        event::event e{};
        e.type = event::pen_axis_changed;
        e.time = t;
        e.pen_event.common.pen_id = id;
        e.pen_event.common.x = x;
        e.pen_event.common.y = y;
        e.pen_event.common.state = state;
        e.pen_event.common.window_id = wid;
        e.pen_event.pen_axis_changed.axis = axis;
        e.pen_event.pen_axis_changed.value = value;
        events_ptr->push_event(e);

        if ((axis == axis_type::pressure) && (data.pen_touching == id))
        {
            video::window_instance* w = video->get_window_instance(wid);
            if (w)
            {
                mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

                if (mouse->data.pen_touch_events)
                {
                    const float nx = x / static_cast<float>(w->data.size.x);
                    const float ny = y / static_cast<float>(w->data.size.y);

                    touch::touch_instance* touch = video->data.touch_ptr.get();
                    touch->post_touch_motion(t, touch::pen_touch_id, mouse::button::left, wid, x, y, value);
                }
            }
        }
    }
}

//=============================================================================

void pen_instance::post_pen_button(time::time_point t, pen_id id, video::window_id wid, uint8_t button, bool down)
{
    if (button < 1 || button > 5)
    { 
        // clamp for now
        return;
    }

    bool post_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_device_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            state = pen->data.state.state;
            x = pen->data.state.x;
            y = pen->data.state.y;

            const input_state flag = static_cast<input_state>(1u << button);
            const bool current = static_cast<bool>(state & flag);

            if (down && !current)
            {
                state |= flag;
                post_event = true;
            }
            else if (!down && current)
            {
                state &= ~flag;
                post_event = true;
            }

            pen->data.state.state = state;
        }
    }

    if (post_event)
    {
        event::event e{};
        e.type = down ? event::pen_button_down : event::pen_button_up;
        e.time = t;
        e.pen_event.common.pen_id = id;
        e.pen_event.common.x = x;
        e.pen_event.common.y = y;
        e.pen_event.common.state = state;
        e.pen_event.common.window_id = wid;
        e.pen_event.pen_button.button = button;
        events_ptr->push_event(e);

        if (!is_valid_id(data.pen_touching) || (data.pen_touching == id))
        {
            video::window_instance* w = video->get_window_instance(wid);
            if (w)
            {
                mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

                if (mouse->data.pen_mouse_events)
                {
                    static const mouse::button mouse_buttons[] = {
                        mouse::button::left,
                        mouse::button::right,
                        mouse::button::middle,
                        mouse::button::extra_1,
                        mouse::button::extra_2
                    };

                    if (button < mem::array_size(mouse_buttons))
                    {
                        mouse->send_mouse_button(t, wid, mouse::pen_mouse_id, mouse_buttons[button], down);
                    }
                }
            }
        }
    }
}

} // namespace pen
} // namespace app
} // namespace vx