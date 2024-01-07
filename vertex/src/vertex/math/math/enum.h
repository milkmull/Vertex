#pragma once

namespace vx {

enum class clock_direction
{
    clockwise,
    counter_clockwise
};

enum class handedness
{
    left,
    right
};

namespace math {

enum class anchor_point
{
    topleft,
    midtop,
    topright,
    midright,
    bottomright,
    midbottom,
    bottomleft,
    midleft,
    center
};

enum class side
{
    left,
    right,
    top,
    bottom,
    front,
    back
};

}
}