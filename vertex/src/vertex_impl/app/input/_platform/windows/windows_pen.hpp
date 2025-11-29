#pragma once

#include "vertex_impl/app/_platform/windows/windows_header.hpp"
#include "vertex_impl/app/input/pen_internal.hpp"

namespace vx {
namespace app {
namespace pen {

//=============================================================================
// pen impl
//=============================================================================

struct pen_impl_data
{
    UINT32 handle;
};

class pen_instance_impl
{
public:

    pen_impl_data data;
};

} // namespace pen
} // namespace app
} // namespace vx