#pragma once

#include "vertex/app/input/touch.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace touch {

//=============================================================================
// forward declares
//=============================================================================

class touch_instance;

//=============================================================================
// touch device
//=============================================================================

struct touch_device_data
{
    touch_id id;
    std::string name;
    touch_device_type type;
    std::vector<finger> fingers;
};

class touch_device_instance
{
public:

    bool add_finger(finger_id id, float x, float y, float pressure);
    void remove_finger(finger_id id);
    finger* get_finger(finger_id id);

public:

    touch_device_data data;
};

//=============================================================================
// touch data
//=============================================================================

struct touch_data
{
    bool finger_touching = false;
    finger_id track_finger = invalid_id;
    touch_id track_touch = invalid_id;
    std::vector<touch_device_instance> touch_devices;

    id_generator touch_id_generator;
    id_generator finger_id_generator;
};

//=============================================================================
// touch instance
//=============================================================================

class touch_instance
{
public:

    //=============================================================================
    // lifecycle
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    ~touch_instance() { quit(); }

    //=============================================================================
    // devices
    //=============================================================================

    bool add_touch(touch_id id, touch_device_type type, const char* name);
    void remove_touch(touch_id id);
    void clear_touch_devices();

    std::vector<touch_id> list_touch_devices();
    bool any_device_available() const noexcept { return !data.touch_devices.empty(); }
    bool is_device_available(touch_id id) const;

    touch_device_instance* get_touch_device_instance(touch_id id);
    const touch_device_instance* get_touch_device_instance(touch_id id) const;

    std::string get_device_name(touch_id id) const;
    touch_device_type get_device_type(touch_id id) const;
    std::vector<finger> get_fingers(touch_id id) const;

    //=============================================================================
    // events
    //=============================================================================

    void post_touch_event(time::time_point t, touch_id id, finger_id fid, video::window_id wid, event::event_type type, float x, float y, float pressure);
    void post_touch_motion(time::time_point t, touch_id id, finger_id fid, video::window_id wid, float x, float y, float pressure);
    bool post_touch_pinch(time::time_point t, event::event_type type, video::window_id wid, float scale);

    //=============================================================================
    // data
    //=============================================================================

    video::video_instance* video = nullptr;
    touch_data data;
};

} // namespace touch
} // namespace app
} // namespace vx