#include "vertex_impl/os/_platform/platform_handle.hpp"

namespace vx {
namespace os {

void handle::close_impl() noexcept
{
    close_handle_impl(m_handle);
}

} // namespace os
} // namespace vx
