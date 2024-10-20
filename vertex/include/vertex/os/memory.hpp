#pragma once

namespace vx {
namespace os {

struct memory_info
{
    size_t physical;
    size_t free;
    size_t available;
};

bool get_memory_info(memory_info& info);

} // namespace os
} // namespace vx