#pragma once

#if defined(VX_APPLICATION)

namespace vx {
namespace app {
namespace cursor {

enum class mode : int
{
    NORMAL,
    HIDDEN,
    DISABLED
};

enum class shape : int
{
    ARROW,
    IBEAM,
    CROSSHAIR,
    HAND,
    HRESIZE,
    VRESIZE,
    ALL_RESIZE,
    NOT_ALLOWED
};

}
}
}

#endif