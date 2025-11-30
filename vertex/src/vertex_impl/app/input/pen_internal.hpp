#pragma once

#include "vertex/app/event/event.hpp"
#include "vertex/app/input/pen.hpp"
#include "vertex/app/video/video.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/os/mutex.hpp"
#include "vertex/util/time.hpp"

namespace vx {
namespace app {

namespace video { class video_instance; }

namespace pen {

//=============================================================================
// pen data
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

struct pen_device_info
{
    capability_flags capabilities = capability_flags::none;
    float max_tilt = 0.0f;
    uint32_t wacom_id = 0;
    size_t button_count = 0;
    device_type type = device_type::unknown;
    device_subtype subtype = device_subtype::unknown;
};

struct pen_data
{
    pen_id id = invalid_id;
    std::string name;
    pen_device_info device_info;
    pen_state state;
};

//=============================================================================
// pen
//=============================================================================

class pen_instance_impl;

class pen_instance
{
public:

    pen_instance();
    ~pen_instance();

    pen_instance(pen_instance&&) noexcept;
    pen_instance& operator=(pen_instance&&) noexcept;

public:

    pen_data data;
    std::unique_ptr<pen_instance_impl> impl_ptr;
};

//=============================================================================
// pen manager data
//=============================================================================

struct pen_manager_data
{
    pen_id pen_touching = invalid_id;
    std::vector<pen_instance> pens;
    id_generator pen_id_generator;
    mutable os::mutex mutex;
};

//=============================================================================
// pen manager
//=============================================================================

class pen_manager
{
public:

    pen_manager();
    ~pen_manager();

    pen_manager(const pen_manager&) = delete;
    pen_manager& operator=(const pen_manager&) = delete;

    pen_manager(pen_manager&&) noexcept = delete;
    pen_manager& operator=(pen_manager&&) noexcept = delete;

public:

    //=============================================================================
    // initialization
    //=============================================================================

    bool init(video::video_instance* owner);
    void quit();

    //=============================================================================
    // devices
    //=============================================================================

    bool add_pen(time::time_point t, pen_instance& pen);
    void remove_pen(time::time_point t, pen_id id);

    std::vector<pen_id> list_pens() const;
    bool pen_connected(pen_id id) const;

    pen_instance* get_pen_instance(pen_id id);
    const pen_instance* get_pen_instance(pen_id id) const;
    pen_instance* get_pen_instance_internal(pen_id id);

    pen_instance* get_pen_from_handle(void*) { return nullptr; }

    //=============================================================================
    // events
    //=============================================================================

    void send_proximity_in(time::time_point t, pen_id id);
    void send_proximity_out(time::time_point t, pen_id id);
    void send_touch(time::time_point t, pen_id id, video::window_instance* w, bool eraser, bool down);
    void send_motion(time::time_point t, pen_id id, video::window_instance* w, float x, float y);
    void send_axis(time::time_point t, pen_id id, video::window_instance* w, axis_type axis, float value);
    void send_button(time::time_point t, pen_id id, video::window_instance* w, uint8_t button, bool down);

    //=============================================================================
    // data
    //=============================================================================

    video::video_instance* video = nullptr;
    pen_manager_data data;
};

} // namespace pen
} // namespace app
} // namespace vx