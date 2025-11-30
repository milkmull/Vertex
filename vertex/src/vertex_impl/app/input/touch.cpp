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
// helper macros
//=============================================================================

#define hints_ptr video->app->data.hints_ptr
#define events_ptr video->app->data.events_ptr

//=============================================================================
// initialization
//=============================================================================

touch_manager::touch_manager() = default;
touch_manager::~touch_manager() { quit(); }

//=============================================================================

bool touch_manager::init(video::video_instance* owner)
{
    if (video)
    {
        quit();
    }
    video = owner;

    return true;
}

//=============================================================================

void touch_manager::quit()
{
    data.touch_devices.clear();

    data.finger_touching = false;
    data.track_finger = invalid_id;
    data.track_touch = invalid_id;

    video = nullptr;
}

//=============================================================================
// touch device
//=============================================================================

bool touch_instance::add_finger(finger_id id, float x, float y, float pressure)
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

void touch_instance::remove_finger(finger_id id)
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

finger* touch_instance::get_finger(finger_id id)
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

bool touch_manager::add_touch(touch_id id, device_type type, const char* name)
{
    VX_ASSERT(is_valid_id(id));

    const touch_instance* tdi = get_touch_device_instance(id);
    if (tdi)
    {
        return true;
    }

    touch_instance touch;
    touch.data.id = id;
    touch.data.type = type;
    touch.data.name = name ? name : "";

    data.touch_devices.push_back(std::move(touch));
    return true;
}

//=============================================================================

void touch_manager::remove_touch(touch_id id)
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

std::vector<touch_id> touch_manager::list_touch_devices() const
{
    std::vector<touch_id> devices(data.touch_devices.size());

    for (size_t i = 0; i < data.touch_devices.size(); ++i)
    {
        devices[i] = data.touch_devices[i].data.id;
    }

    return devices;
}

//=============================================================================

touch_instance* touch_manager::get_touch_device_instance(touch_id id)
{
    for (touch_instance& device : data.touch_devices)
    {
        if (device.data.id == id)
        {
            return &device;
        }
    }

    return nullptr;
}

//=============================================================================

const touch_instance* touch_manager::get_touch_device_instance(touch_id id) const
{
    return const_cast<touch_manager*>(this)->get_touch_device_instance(id);
}

//=============================================================================

std::string touch_manager::get_device_name(touch_id id) const
{
    const touch_instance* device = get_touch_device_instance(id);
    return device ? device->data.name : std::string{};
}

//=============================================================================

device_type touch_manager::get_device_type(touch_id id) const
{
    const touch_instance* device = get_touch_device_instance(id);
    return device ? device->data.type : device_type::invalid;
}

//=============================================================================

std::vector<finger> touch_manager::get_fingers(touch_id id) const
{
    const touch_instance* device = get_touch_device_instance(id);
    return device ? device->data.fingers : std::vector<finger>{};
}

//=============================================================================
// events
//=============================================================================

#define events_ptr video->app->data.events_ptr

//=============================================================================

void touch_manager::send_event(
    time::time_point t, touch_id id, finger_id fid, video::window_instance* w,
    event::event_type type, float x, float y, float pressure
)
{
    VX_ASSERT(event::get_category(type) == event::category_touch);

    touch_instance* touch = get_touch_device_instance(id);
    if (!touch)
    {
        return;
    }

    const video::window_id wid = w ? w->data.id : invalid_id;
    mouse::mouse_instance* mouse = video->data.mouse_ptr.get();
    const bool down = (type == event::finger_down);

    // generating synthetic mouse events for touch input
    if (mouse->data.touch_mouse_events && (id != mouse_touch_id) && (id != pen_touch_id))
    {
        if (down)
        {
            if (!data.finger_touching)
            {
                if (w)
                {
                    const auto& size = w->data.size;

                    const float wx = math::clamp(x * static_cast<float>(size.x), 0.0f, static_cast<float>(size.x));
                    const float wy = math::clamp(y * static_cast<float>(size.y), 0.0f, static_cast<float>(size.y));

                    mouse->send_motion(t, w, mouse::touch_mouse_id, false, wx, wy);
                    mouse->send_button(t, w, mouse::touch_mouse_id, mouse::button::left, true);
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
                    mouse->send_button(t, w, mouse::touch_mouse_id, mouse::button::left, false);
                }

                data.finger_touching = true;
            }
        }
    }

    // discard synthetic touch or pen events coming from platform layer
    if (!mouse->data.touch_mouse_events && (id == mouse_touch_id))
    {
        return;
    }
    if (!mouse->data.pen_touch_events && (id == pen_touch_id))
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
            send_event(t, id, fid, w, event::finger_canceled, x, y, pressure);
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

void touch_manager::send_motion(
    time::time_point t, touch_id id, finger_id fid, video::window_instance* w,
    float x, float y, float pressure
)
{
    touch_instance* touch = get_touch_device_instance(id);
    if (!touch)
    {
        return;
    }

    const video::window_id wid = w ? w->data.id : invalid_id;
    mouse::mouse_instance* mouse = video->data.mouse_ptr.get();

    // generating synthetic mouse events for touch input
    if (w && mouse->data.touch_mouse_events && (id != mouse_touch_id) && (id != pen_touch_id))
    {
        const auto& size = w->data.size;
        const float wx = math::clamp(x * static_cast<float>(size.x), 0.0f, static_cast<float>(size.x));
        const float wy = math::clamp(y * static_cast<float>(size.y), 0.0f, static_cast<float>(size.y));

        mouse->send_motion(t, w, mouse::touch_mouse_id, false, wx, wy);
    }

    // discard synthetic touch or pen events coming from platform layer
    if (!mouse->data.touch_mouse_events && (id == mouse_touch_id))
    {
        return;
    }

    finger* finger = touch->get_finger(fid);
    if (!finger)
    {
        send_event(t, id, fid, w, event::finger_down, x, y, pressure);
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
    e.type = event::finger_moved;
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

void touch_manager::send_pinch(time::time_point t, event::event_type type, const video::window_instance* w, float scale)
{
    VX_ASSERT(event::get_category(type) == event::category_pinch);
    const video::window_id wid = w ? w->data.id : invalid_id;

    event::event e{};
    e.type = type;
    e.time = t;
    e.pinch_event.common.scale = scale;
    e.pinch_event.common.window_id = wid;
    events_ptr->push_event(e);
}

} // namespace touch
} // namespace app
} // namespace vx