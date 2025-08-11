#include <unistd.h>

#include "vertex_impl/os/_platform/unix/unix_tools.hpp"

namespace vx {
namespace os {

///////////////////////////////////////////////////////////////////////////////
// Handle
///////////////////////////////////////////////////////////////////////////////

void handle::close_impl() noexcept
{
    ::close(m_handle);
}

namespace unix_ {

} // namespace unix_
} // namespace os
} // namespace vx