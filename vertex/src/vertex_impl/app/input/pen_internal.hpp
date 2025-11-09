#pragma once

#include "vertex/app/event/event.hpp"
#include "vertex/app/input/pen.hpp"
#include "vertex/app/owner_ptr.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/os/mutex.hpp"
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
// pen device instance
//=============================================================================

class pen_device_instance_impl;
struct pen_device_instance_impl_deleter { void operator()(pen_device_instance_impl* ptr) const noexcept; };

//=============================================================================
// helpers
//=============================================================================

VX_FLAGS_UT_DECLARE_BEGIN(capability_flags, uint32_t)
{
    none                = 0,
    pressure            = VX_BIT(0),
    xtilt               = VX_BIT(1),
    ytilt               = VX_BIT(2),
    distance            = VX_BIT(3),
    rotation            = VX_BIT(4),
    slider              = VX_BIT(5),
    tangentail_pressure = VX_BIT(6),
    eraser              = VX_BIT(7),
}
VX_FLAGS_DECLARE_END(capability_flags)

//=============================================================================
// pen device
//=============================================================================

struct pen_info
{
    capability_flags capabilities = capability_flags::none;
    float max_tilt = 0.0f;
    uint32_t wacom_id = 0;
    size_t button_count = 0;
    device_type device_type = device_type::unknown;
    subtype subtype = subtype::unknown;
};

struct pen_device_data
{
    pen_id id = invalid_id;
    std::string name;
    pen_info info;
    pen_state state;
};

class pen_device_instance
{
public:

    void finalize();

public:

    pen_device_data data;
    owner_ptr<pen_device_instance_impl, pen_device_instance_impl_deleter> impl_ptr;
};

//=============================================================================
// pen data
//=============================================================================

struct pen_data
{
    pen_id pen_touching = invalid_id;
    std::vector<pen_device_instance> pens;
    id_generator pen_id_generator;
    mutable os::mutex mutex;
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

    bool add_pen(time::time_point t, pen_device_instance& pen);
    void remove_pen(time::time_point t, pen_id id);
    void remove_pen_internal(time::time_point t, pen_id id, bool post_event);
    void clear_pens(bool post_events);

    std::vector<pen_id> list_pens() const;
    bool pen_connected(pen_id id) const;

    pen_device_instance* get_pen_instance(pen_id id);
    const pen_device_instance* get_pen_instance(pen_id id) const;
    pen_device_instance* get_pen_instance_internal(pen_id id);

    pen_device_instance* get_pen_from_handle(void* handle);

    //=============================================================================
    // events
    //=============================================================================

    void post_pen_proximity_in(time::time_point t, pen_id id);
    void post_pen_proximity_out(time::time_point t, pen_id id);

    void post_pen_touch(time::time_point t, pen_id id, video::window_id wid, bool eraser, bool down);
    void post_pen_motion(time::time_point t, pen_id id, video::window_id wid, float x, float y);
    void post_pen_axis(time::time_point t, pen_id id, video::window_id wid, axis_type axis, float value);
    void post_pen_button(time::time_point t, pen_id id, video::window_id wid, uint8_t button, bool down);

    //=============================================================================
    // data
    //=============================================================================

    video::video_instance* video = nullptr;
    pen_data data;
};

} // namespace pen
} // namespace app
} // namespace vx