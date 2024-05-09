#pragma once

namespace vx {
namespace app {

struct display_mode
{
    int width;
    int height;

    int red_bits;
    int green_bits;
    int blue_bits;

    float refresh_rate;

    // float pixel_density;
};

}
}