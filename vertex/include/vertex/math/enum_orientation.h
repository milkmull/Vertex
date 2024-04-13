#pragma once

namespace vx {

enum class clock_direction
{
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

enum class handedness
{
    LEFT,
    RIGHT
};

enum class side
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    FRONT,
    BACK
};

enum class anchor_point
{
    TOPLEFT,
    MIDTOP,
    TOPRIGHT,
    MIDRIGHT,
    BOTTOMRIGHT,
    MIDBOTTOM,
    BOTTOMLEFT,
    MIDLEFT,
    CENTER
};

}