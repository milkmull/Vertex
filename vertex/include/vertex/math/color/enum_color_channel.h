#pragma once

namespace vx {

enum class color_channel
{
    RED   = 0,
    GREEN = 1,
    BLUE  = 2,
    ALPHA = 3
};

enum class color_mask
{
    RED   = 0x000000FF,
    GREEN = 0x0000FF00,
    BLUE  = 0x00FF0000,
    ALPHA = 0xFF000000
};

}