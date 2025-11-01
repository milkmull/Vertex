#include "vertex/math/color/functions/common.hpp"
#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/event/event_internal.hpp"
#include "vertex_impl/app/input/mouse_internal.hpp"
#include "vertex_impl/app/input/touch_internal.hpp"
#include "vertex_impl/app/video/video_internal.hpp"

namespace vx {
namespace app {
namespace touch {

//=============================================================================
// lifecycle
//=============================================================================

bool touch_instance::init(video::video_instance* owner)
{
    VX_ASSERT(!video);
    VX_ASSERT(owner);
    video = owner;

    return true;
}

//=============================================================================

void touch_instance::quit()
{
    clear_touch_devices();
}

//=============================================================================
// touch device
//=============================================================================

bool touch_device_instance::add_finger(finger_id id, float x, float y, float pressure)
{
    finger f;
    f.id = id;
    f.x = x;
    f.y = y;
    f.pressure = pressure;

    data.fingers.push_back(std::move(f));
    return true;
}

//=============================================================================

void touch_device_instance::remove_finger(finger_id id)
{
    for (auto it = data.fingers.begin(); it != data.fingers.end(); ++it)
    {
        if (it->id == id)
        {
            data.fingers.erase(it);
            break;
        }
    }
}

//=============================================================================

finger* touch_device_instance::get_finger(finger_id id)
{
    if (!is_valid_id(id))
    {
        return nullptr;
    }

    for (finger& f : data.fingers)
    {
        if (f.id == id)
        {
            return &f;
        }
    }

    return nullptr;
}

//=============================================================================
// devices
//=============================================================================

bool touch_instance::add_touch(touch_id id, touch_device_type type, const char* name)
{
    VX_ASSERT(is_valid_id(id));

    const touch_device_instance* tdi = get_touch_device_instance(id);
    if (tdi)
    {
        return true;
    }

    touch_device_instance touch;
    touch.data.id = id;
    touch.data.type = type;
    touch.data.name = name ? name : "";

    data.touch_devices.push_back(std::move(touch));
    return true;
}

//=============================================================================

void touch_instance::remove_touch(touch_id id)
{
    for (auto it = data.touch_devices.begin(); it != data.touch_devices.end(); ++it)
    {
        if (it->data.id == id)
        {
            data.touch_devices.erase(it);
            break;
        }
    }
}

//=============================================================================

void touch_instance::clear_touch_devices()
{
    while (!data.touch_devices.empty())
    {
        remove_touch(data.touch_devices[0].data.id);
    }
}

//=============================================================================

std::vector<touch_id> touch_instance::list_touch_devices()
{
    std::vector<touch_id> devices(data.touch_devices.size());

    for (size_t i = 0; i < data.touch_devices.size(); ++i)
    {
        devices[i] = data.touch_devices[i].data.id;
    }

    return devices;
}

//=============================================================================

bool touch_instance::is_device_available(touch_id id) const
{
    return get_touch_device_instance(id) != nullptr;
}

//=============================================================================

touch_device_instance* touch_instance::get_touch_device_instance(touch_id id)
{
    for (touch_device_instance& device : data.touch_devices)
    {
        if (device.data.id == id)
        {
            return &device;
        }
    }

    return nullptr;
}

//=============================================================================

const touch_device_instance* touch_instance::get_touch_device_instance(touch_id id) const
{
    return const_cast<touch_instance*>(this)->get_touch_device_instance(id);
}

//=============================================================================

std::string touch_instance::get_device_name(touch_id id) const
{
    const touch_device_instance* device = get_touch_device_instance(id);
    return device ? device->data.name : std::string{};
}

//=============================================================================

touch_device_type touch_instance::get_device_type(touch_id id) const
{
    const touch_device_instance* device = get_touch_device_instance(id);
    return device ? device->data.type : touch_device_type::TOUCH_DEVICE_INVALID;
}

//=============================================================================

std::vector<finger> touch_instance::get_fingers(touch_id id) const
{
    const touch_device_instance* device = get_touch_device_instance(id);
    return device ? device->data.fingers : std::vector<finger>{};
}

//=============================================================================
// events
//=============================================================================

#define events_ptr video->app->data.events_ptr

void touch_instance::post_touch_event(
    time::time_point t, touch_id id, finger_id fid, video::window_id wid,
    event::event_type type, float x, float y, float pressure
)
{
    VX_ASSERT(event::get_category(type) == event::CATEGORY_TOUCH);

    touch_device_instance* touch = get_touch_device_instance(id);
    if (!touch)
    {
        return;
    }

    mouse::mouse_instance* mouse = video->data.mouse_ptr.get();
    const bool down = (type == event::FINGER_DOWN);

    // generating synthetic mouse events for touch input
    if (mouse->data.touch_mouse_events && (id != MOUSE_TOUCH_ID) && (id != PEN_TOUCH_ID))
    {
        video::window_instance* w = video->get_window_instance(wid);

        if (down)
        {
            if (!data.finger_touching)
            {
                if (w)
                {
                    const auto& size = w->data.size;

                    const float wx = math::clamp(x * size.x, 0.0f, static_cast<float>(size.x));
                    const float wy = math::clamp(y * size.y, 0.0f, static_cast<float>(size.y));

                    mouse->send_mouse_motion(t, wid, mouse::TOUCH_MOUSE_ID, false, wx, wy);
                    mouse->send_mouse_button(t, wid, mouse::TOUCH_MOUSE_ID, mouse::BUTTON_LEFT, true);
                }

                data.finger_touching = true;
                data.track_touch = id;
                data.track_finger = fid;
            }
        }
        else
        {
            if (data.finger_touching && data.track_touch == id && data.track_finger == fid)
            {
                if (w)
                {
                    mouse->send_mouse_button(t, wid, mouse::TOUCH_MOUSE_ID, mouse::BUTTON_LEFT, false);
                }

                data.finger_touching = true;
            }
        }
    }

    // discard synthetic touch or pen events coming from platform layer
    if (!mouse->data.touch_mouse_events && (id == MOUSE_TOUCH_ID))
    {
        return;
    }
    if (!mouse->data.pen_touch_events && (id == PEN_TOUCH_ID))
    {
        return;
    }

    finger* finger = touch->get_finger(fid);

    if (down)
    {
        if (finger)
        {
            // finger exists so it is already down, assume
            // the finger up event was lost and send a cancel
            post_touch_event(t, id, fid, wid, event::FINGER_CANCELED, x, y, pressure);
        }

        if (!touch->add_finger(fid, x, y, pressure))
        {
            return;
        }
    }
    else
    {
        if (!finger)
        {
            // finger already up
            return;
        }

        // override with the last known coordinates
        x = finger->x;
        y = finger->y;
    }

    // post the event
    event::event e{};
    e.type = type;
    e.time = t;
    e.touch_event.common.touch_id = id;
    e.touch_event.common.finger_id = fid;
    e.touch_event.common.x = x;
    e.touch_event.common.y = y;
    e.touch_event.common.pressure = pressure;
    e.touch_event.common.window_id = wid;
    events_ptr->push_event(e);

    if (!down)
    {
        touch->remove_finger(fid);
    }
}

//=============================================================================

void touch_instance::post_touch_motion(
    time::time_point t, touch_id id, finger_id fid, video::window_id wid,
    float x, float y, float pressure
)
{
    touch_device_instance* touch = get_touch_device_instance(id);
    if (!touch)
    {
        return;
    }

    mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

    // generating synthetic mouse events for touch input
    if (mouse->data.touch_mouse_events && (id != MOUSE_TOUCH_ID) && (id != PEN_TOUCH_ID))
    {
        video::window_instance* w = video->get_window_instance(wid);

        const auto& size = w->data.size;
        const float wx = math::clamp(x * size.x, 0.0f, static_cast<float>(size.x));
        const float wy = math::clamp(y * size.y, 0.0f, static_cast<float>(size.y));

        mouse->send_mouse_motion(t, wid, mouse::TOUCH_MOUSE_ID, false, wx, wy);
    }

    // discard synthetic touch or pen events coming from platform layer
    if (!mouse->data.touch_mouse_events && (id == MOUSE_TOUCH_ID))
    {
        return;
    }

    finger* finger = touch->get_finger(fid);
    if (!finger)
    {
        post_touch_event(t, id, fid, wid, event::FINGER_DOWN, x, y, pressure);
        return;
    }

    const float dx = x - finger->x;
    const float dy = y - finger->y;
    const float dp = pressure - finger->pressure;

    // drop events that don't change state
    if (dx == 0.0f && dy == 0.0f && dp == 0.0f)
    {
        return;
    }

    // update finger state
    finger->x = x;
    finger->y = y;
    finger->pressure = pressure;

    // post the event
    event::event e{};
    e.type = event::FINGER_MOVED;
    e.time = t;
    e.touch_event.common.touch_id = id;
    e.touch_event.common.finger_id = fid;
    e.touch_event.common.x = x;
    e.touch_event.common.y = y;
    e.touch_event.common.pressure = pressure;
    e.touch_event.common.window_id = wid;
    e.touch_event.finger_moved.dx = dx;
    e.touch_event.finger_moved.dy = dy;
    e.touch_event.finger_moved.pressure_difference = dp;
    events_ptr->push_event(e);
}

//=============================================================================

bool touch_instance::post_touch_pinch(time::time_point t, event::event_type type, video::window_id wid, float scale)
{
    VX_ASSERT(event::get_category(type) == event::CATEGORY_PINCH);

    event::event e{};
    e.type = type;
    e.time = t;
    e.pinch_event.common.scale = scale;
    e.pinch_event.common.window_id = wid;

    return events_ptr->push_event(e);
}

} // namespace touch
} // namespace app
} // namespace vx