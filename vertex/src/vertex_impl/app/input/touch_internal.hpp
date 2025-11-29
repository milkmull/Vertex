#pragma once

#include "vertex/app/input/touch.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace touch {

//=============================================================================
// touch data
//=============================================================================

struct touch_data
{
    touch_id id = invalid_id;
    std::string name;
    device_type type = device_type::invalid;
    std::vector<finger> fingers;
};

class touch_instance
{
public:

    bool add_finger(finger_id id, float x, float y, float pressure);
    void remove_finger(finger_id id);
    finger* get_finger(finger_id id);

public:

    touch_data data;
};

//=============================================================================
// touch manager data
//=============================================================================

struct touch_manager_data
{
    bool finger_touching = false;
    finger_id track_finger = invalid_id;
    touch_id track_touch = invalid_id;
    std::vector<touch_instance> touch_devices;
};

//=============================================================================
// touch instance
//=============================================================================

class touch_manager
{
public:

    touch_manager();
    ~touch_manager();

    touch_manager(const touch_manager&) = delete;
    touch_manager& operator=(const touch_manager&) = delete;

    touch_manager(touch_manager&&) noexcept = delete;
    touch_manager& operator=(touch_manager&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    //=============================================================================
    // devices
    //=============================================================================

    bool add_touch(touch_id id, device_type type, const char* name);
    void remove_touch(touch_id id);
    std::vector<touch_id> list_touch_devices() const;

    touch_instance* get_touch_device_instance(touch_id id);
    const touch_instance* get_touch_device_instance(touch_id id) const;

    std::string get_device_name(touch_id id) const;
    device_type get_device_type(touch_id id) const;
    std::vector<finger> get_fingers(touch_id id) const;

    //=============================================================================
    // events
    //=============================================================================

    void send_event(time::time_point t, touch_id id, finger_id fid, video::window_instance* w, event::event_type type, float x, float y, float pressure);
    void send_motion(time::time_point t, touch_id id, finger_id fid, video::window_instance* w, float x, float y, float pressure);
    void send_pinch(time::time_point t, event::event_type type, const video::window_instance* w, float scale);

    //=============================================================================
    // data
    //=============================================================================

    video::video_instance* video = nullptr;
    touch_manager_data data;
};

} // namespace touch
} // namespace app
} // namespace vx