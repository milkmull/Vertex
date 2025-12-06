#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/input/_platform/platform_pen.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/pen_internal.hpp"
#include "vertex_impl/app/input/touch_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace pen {

//=============================================================================
// helper macros
//=============================================================================

#define hints_ptr video->app->hints_ptr
#define events_ptr video->app->events_ptr

//=============================================================================
// initialization
//=============================================================================

pen_manager::pen_manager() = default;
pen_manager::~pen_manager() { quit(); }

//=============================================================================

bool pen_manager::init(video::video_instance* owner)
{
    if (video)
    {
        quit();
    }
    video = owner;

    return true;
}

//=============================================================================

void pen_manager::quit()
{
    data.pens.clear();
    data.pen_touching = invalid_id;
    video = nullptr;
}

//=============================================================================
// pen instance
//=============================================================================

pen_instance::pen_instance() = default;
pen_instance::~pen_instance() = default;

pen_instance::pen_instance(pen_instance&&) noexcept = default;
pen_instance& pen_instance::operator=(pen_instance&&) noexcept = default;

//=============================================================================
// devices
//=============================================================================

bool pen_manager::add_pen(time::time_point t, pen_instance& pen)
{
    const pen_id id = data.pen_id_generator.next();
    pen.data.id = id;
    if (pen.data.name.empty())
    {
        pen.data.name = "Pen";
    }

    data.mutex.lock();
    data.pens.push_back(std::move(pen));
    data.mutex.unlock();

    send_proximity_in(t, id);
    return true;
}

//=============================================================================

void pen_manager::remove_pen(time::time_point t, pen_id id)
{
    if (!is_valid_id(id))
    {
        return;
    }

    data.mutex.lock();

    for (auto it = data.pens.begin(); it != data.pens.end(); ++it)
    {
        if (it->data.id == id)
        {
            send_proximity_out(t, id);
            data.pens.erase(it);
            break;
        }
    }

    data.mutex.unlock();
}

//=============================================================================

std::vector<pen_id> pen_manager::list_pens() const
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

bool pen_manager::pen_connected(pen_id id) const
{
    return get_pen_instance(id) != nullptr;
}

//=============================================================================

pen_instance* pen_manager::get_pen_instance(pen_id id)
{
    data.mutex.lock();
    pen_instance* info = get_pen_instance_internal(id);
    data.mutex.unlock();

    return info;
}

const pen_instance* pen_manager::get_pen_instance(pen_id id) const
{
    return const_cast<pen_manager*>(this)->get_pen_instance(id);
}

pen_instance* pen_manager::get_pen_instance_internal(pen_id id)
{
    for (pen_instance& pen : data.pens)
    {
        if (pen.data.id == id)
        {
            return &pen;
        }
    }

    return nullptr;
}

//=============================================================================
// events
//=============================================================================

void pen_manager::send_proximity_in(time::time_point t, pen_id id)
{
    event::event e{};
    e.type = event::pen_proximity_in;
    e.time = t;
    e.pen_event.common.pen_id = id;

    events_ptr->push_event(e);
}

void pen_manager::send_proximity_out(time::time_point t, pen_id id)
{
    event::event e{};
    e.type = event::pen_proximity_out;
    e.time = t;
    e.pen_event.common.pen_id = id;

    events_ptr->push_event(e);
}

void pen_manager::send_touch(time::time_point t, pen_id id, video::window_instance* w, bool eraser, bool down)
{
    bool send_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            state = pen->data.state.state;
            x = pen->data.state.x;
            y = pen->data.state.y;
            pressure = pen->data.state.axes[axis_type::pressure];

            if (down && !(state & input_state::down))
            {
                state |= input_state::down;
                send_event = true;
            }
            else if (!down && (state & input_state::down))
            {
                state &= ~input_state::down;
                send_event = true;
            }

            if (eraser && !(state & input_state::eraser_tip))
            {
                state |= input_state::eraser_tip;
                send_event = true;
            }
            else if (!eraser && (state & input_state::eraser_tip))
            {
                state &= ~input_state::eraser_tip;
                send_event = true;
            }

            pen->data.state.state = state;
        }
    }

    if (send_event)
    {
        const video::window_id wid = w ? w->data.id : invalid_id;

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

        if (w)
        {
            mouse::mouse_instance* mouse = video->mouse_ptr;

            if (mouse->data.pen_mouse_events)
            {
                if (down)
                {
                    if (!data.pen_touching)
                    {
                        mouse->send_motion(t, w, mouse::pen_mouse_id, false, x, y);
                        mouse->send_button(t, w, mouse::pen_mouse_id, mouse::button::left, true);
                    }
                }
                else
                {
                    if (data.pen_touching == id)
                    {
                        mouse->send_button(t, w, mouse::pen_mouse_id, mouse::button::left, false);
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

                    touch::touch_manager* touch = video->touch_ptr;
                    touch->send_event(t, touch::pen_touch_id, mouse::button::left, w, touch_event_type, nx, ny, pressure);
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

void pen_manager::send_motion(time::time_point t, pen_id id, video::window_instance* w, float x, float y)
{
    bool send_event = false;
    input_state state = input_state::none;
    float pressure = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            if (pen->data.state.x != x || pen->data.state.y != y)
            {
                pen->data.state.x = x;
                pen->data.state.y = y;
                state = pen->data.state.state;
                pressure = pen->data.state.axes[axis_type::pressure];
                send_event = true;
            }
        }
    }

    if (send_event)
    {
        const video::window_id wid = w ? w->data.id : invalid_id;

        event::event e{};
        e.type = event::pen_moved;
        e.time = t;
        e.pen_event.common.pen_id = id;
        e.pen_event.common.x = x;
        e.pen_event.common.y = y;
        e.pen_event.common.state = state;
        e.pen_event.common.window_id = wid;
        events_ptr->push_event(e);

        if (w)
        {
            mouse::mouse_instance* mouse = video->mouse_ptr;

            if (data.pen_touching == id)
            {
                if (mouse->data.pen_mouse_events)
                {
                    mouse->send_motion(t, w, mouse::pen_mouse_id, false, x, y);
                }
                if (mouse->data.pen_touch_events)
                {
                    const float nx = x / static_cast<float>(w->data.size.x);
                    const float ny = y / static_cast<float>(w->data.size.y);

                    touch::touch_manager* touch = video->touch_ptr;
                    touch->send_motion(t, touch::pen_touch_id, mouse::button::left, w, nx, ny, pressure);
                }
            }
            else if (!is_valid_id(data.pen_touching))
            {
                // send mouse motion for pens that aren't touching
                mouse->send_motion(t, w, mouse::pen_mouse_id, false, x, y);
            }
        }
    }
}

//=============================================================================

void pen_manager::send_axis(time::time_point t, pen_id id, video::window_instance* w, axis_type axis, float value)
{
    VX_ASSERT(axis < axis_count);

    bool send_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_instance* pen = get_pen_instance_internal(id);
        if (pen)
        {
            if (pen->data.state.axes[axis] != value)
            {
                pen->data.state.axes[axis] = value;
                state = pen->data.state.state;
                x = pen->data.state.x;
                y = pen->data.state.y;
                send_event = true;
            }
        }
    }

    if (send_event)
    {
        const video::window_id wid = w ? w->data.id : invalid_id;

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
            if (w)
            {
                mouse::mouse_instance* mouse = video->mouse_ptr;

                if (mouse->data.pen_touch_events)
                {
                    const float nx = x / static_cast<float>(w->data.size.x);
                    const float ny = y / static_cast<float>(w->data.size.y);

                    touch::touch_manager* touch = video->touch_ptr;
                    touch->send_motion(t, touch::pen_touch_id, mouse::button::left, w, nx, ny, value);
                }
            }
        }
    }
}

//=============================================================================

void pen_manager::send_button(time::time_point t, pen_id id, video::window_instance* w, uint8_t button, bool down)
{
    if (button < 1 || button > 5)
    { 
        // clamp for now
        return;
    }

    bool send_event = false;
    input_state state = input_state::none;
    float x = 0.0f;
    float y = 0.0f;

    {
        os::lock_guard lock(data.mutex);

        pen_instance* pen = get_pen_instance_internal(id);
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
                send_event = true;
            }
            else if (!down && current)
            {
                state &= ~flag;
                send_event = true;
            }

            pen->data.state.state = state;
        }
    }

    if (send_event)
    {
        const video::window_id wid = w ? w->data.id : invalid_id;

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
            if (w)
            {
                mouse::mouse_instance* mouse = video->mouse_ptr;

                if (mouse->data.pen_mouse_events)
                {
                    if (button < mouse::button_count)
                    {
                        const mouse::button mouse_button = mouse::button_mask(button + 1);
                        mouse->send_button(t, w, mouse::pen_mouse_id, static_cast<uint8_t>(mouse_button), down);
                    }
                }
            }
        }
    }
}

} // namespace pen
} // namespace app
} // namespace vx
