#pragma once

#include <atomic>

namespace vx {
namespace app {

using device_id = uint32_t;

inline device_id get_next_device_id()
{
    static std::atomic<device_id> id{ 0 };
    return ++id;
}

}
}