#pragma once

#if defined(VX_APPLICATION)

namespace vx {
namespace app {
namespace cursor {

enum mode : int
{
    MODE_NORMAL,
    MODE_HIDDEN,
    MODE_DISABLED
};

enum shape : int
{
    SHAPE_ARROW,
    SHAPE_IBEAM,
    SHAPE_CROSSHAIR,
    SHAPE_HAND,
    SHAPE_HRESIZE,
    SHAPE_VRESIZE,
    SHAPE_ALL_RESIZE,
    SHAPE_NOT_ALLOWED
};

}
}
}

#endif