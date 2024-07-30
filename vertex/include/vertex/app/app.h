#pragma once

#include "vertex/config.h"

namespace vx {
namespace app {

struct subsystem
{
    using type = uint32_t;

    enum : type
    {
        NONE = 0,
        VIDEO = (1 << 0)
    };
};

bool init(subsystem::type systems);

}
}