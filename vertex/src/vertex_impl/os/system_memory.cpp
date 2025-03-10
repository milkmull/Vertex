#include "vertex_impl/os/__platform/system_memory.hpp"

namespace vx {
namespace os {

VX_API bool get_memory_info(memory_info& info)
{
    return get_memory_info_impl(info);
}

} // namespace os
} // namespace vx