#pragma once

#include "vertex/app/input/pen.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/app/event/event.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace pen {

//=============================================================================
// forward declares
//=============================================================================

class pen_instance;

//=============================================================================
// helpers
//=============================================================================

class pen_capability 
{
    using type = uint32_t;

    enum : type
    {
        NONE                = 0,
        PRESSURE            = VX_BIT(0),
        XTILT               = VX_BIT(1),
        YTILT               = VX_BIT(2),
        DISTANCE            = VX_BIT(3),
        ROTATION            = VX_BIT(4),
        SLIDER              = VX_BIT(5),
        TANGENTAIL_PRESSURE = VX_BIT(6),
        ERASER              = VX_BIT(7),
    };
}

enum class pen_type
{

};

//=============================================================================
// pen device
//=============================================================================

struct pen_device_data
{
    pen_id id;
    std::string name;
    pen_device_type type;
    std::vector<finger> fingers;
};

class pen_device_instance
{
public:

    bool add_finger(finger_id id, float x, float y, float pressure);
    void remove_finger(finger_id id);
    finger* get_finger(finger_id id);

public:

    pen_device_data data;
};

//=============================================================================
// pen data
//=============================================================================

struct pen_data
{
    bool finger_pening = false;
    finger_id track_finger = INVALID_ID;
    pen_id track_pen = INVALID_ID;
    std::vector<pen_device_instance> pen_devices;

    id_generator pen_id_generator;
    id_generator finger_id_generator;
};

//=============================================================================
// pen instance
//=============================================================================

class pen_instance
{
public:

    //=============================================================================
    // lifecycle
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    ~pen_instance() { quit(); }

    //=============================================================================
    // devices
    //=============================================================================

    bool add_pen(pen_id id, pen_device_type type, const char* name);
    void remove_pen(pen_id id);
    void clear_pen_devices();

    std::vector<pen_id> list_pen_devices();
    bool any_device_available() const noexcept { return !data.pen_devices.empty(); }
    bool is_device_available(pen_id id) const;

    pen_device_instance* get_pen_device_instance(pen_id id);
    const pen_device_instance* get_pen_device_instance(pen_id id) const;

    std::string get_device_name(pen_id id) const;
    pen_device_type get_device_type(pen_id id) const;
    std::vector<finger> get_fingers(pen_id id) const;

    //=============================================================================
    // events
    //=============================================================================

    void post_pen_event(time::time_point t, pen_id id, finger_id fid, video::window_id wid, event::event_type type, float x, float y, float pressure);
    void post_pen_motion(time::time_point t, pen_id id, finger_id fid, video::window_id wid, float x, float y, float pressure);
    bool post_pen_pinch(time::time_point t, event::event_type type, video::window_id wid, float scale);

    //=============================================================================
    // data
    //=============================================================================

    video::video_instance* video = nullptr;
    pen_data data;
};

} // namespace pen
} // namespace app
} // namespace vx