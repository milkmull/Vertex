#pragma once

#include <memory>
#include <string>
#include <vector>

namespace vx {
namespace app {

class display;

// represents each video device (gpu) attached to the system
class video_device
{
public:

    static const std::vector<video_device>& list_video_devices();

public:

    using video_device_id = size_t;
    video_device_id id() const;
    std::string name() const;
    std::vector<display> displays() const;
    bool is_primary() const;

private:

    class video_device_impl;
    std::unique_ptr<video_device_impl> m_video_device = nullptr;
};

}
}