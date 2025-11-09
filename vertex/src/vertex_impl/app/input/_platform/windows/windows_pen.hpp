#pragma once

#include "vertex_impl/app/_platform/windows/windows_header.hpp"
#include "vertex_impl/app/input/pen_internal.hpp"

namespace vx {
namespace app {
namespace pen {

//=============================================================================
// pen_device_instance_impl
//=============================================================================

struct pen_device_impl_data
{
    UINT32 handle;
};

class pen_device_instance_impl
{
public:

    pen_device_impl_data data;
};

} // namespace pen
} // namespace app
} // namespace vx