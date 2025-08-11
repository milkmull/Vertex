#pragma once

#include <atomic>

namespace vx {
namespace app {

using device_id = uint64_t;
enum : device_id { INVALID_DEVICE_ID = 0 };

inline device_id generate_device_id()
{
    static std::atomic<device_id> s_id{ INVALID_DEVICE_ID };
    return ++s_id;
}

inline bool is_valid_device_id(device_id id) noexcept
{
    return id != INVALID_DEVICE_ID;
}

} // namespace app
} // namespace vx