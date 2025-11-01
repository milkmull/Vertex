#pragma once

#include <vector>
#include <string>

#include "vertex/config/language_config.hpp"
#include "vertex/app/id.hpp"

namespace vx {
namespace app {
namespace touch {

//=============================================================================
// touch id
//=============================================================================

using touch_id = id_type;
using finger_id = id_type;

enum : touch_id
{
    MOUSE_TOUCH_ID = MAX_ID,
    PEN_TOUCH_ID = MAX_ID - 1
};

//=============================================================================
// type
//=============================================================================

enum touch_device_type
{
    TOUCH_DEVICE_INVALID = -1,
    TOUCH_DEVICE_DIRECT,
    TOUCH_DEVICE_INDIRECT_ABSOLUTE,
    TOUCH_DEVICE_INDIRECT_RELATIVE
};

//=============================================================================
// finger
//=============================================================================

struct finger
{
    finger_id id;
    float x, y;
    float pressure;
};

//=============================================================================
// touch_device
//=============================================================================

class touch_device
{
public:

    // implicit on purpose: allows seamless conversion
    touch_device(touch_id id) noexcept : m_id(id) {}

public:

    // implicit conversion back to id
    operator touch_id() const noexcept { return m_id; }
    touch_id id() const noexcept { return m_id; }

    bool is_valid() const noexcept { return is_valid_id(m_id); }
    VX_API bool is_available() const;

    VX_API std::string get_name() const;
    VX_API touch_device_type get_type() const;
    VX_API std::vector<finger> get_fingers() const;

private:

    touch_id m_id;
};

//=============================================================================
// api
//=============================================================================

VX_API std::vector<touch_id> list_touch_devices();
inline bool is_device_available(touch_id id) { return touch_device(id).is_available(); }

} // namespace touch
} // namespace app
} // namespace vx