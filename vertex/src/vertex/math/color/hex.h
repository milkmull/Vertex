#pragma once

#include "type/color8.h"

namespace vx {
namespace math {

enum rgba_mask : uint32_t
{
    VX_COLOR_MASK_R = 0x000000FF,
    VX_COLOR_MASK_G = 0x0000FF00,
    VX_COLOR_MASK_B = 0x00FF0000,
    VX_COLOR_MASK_A = 0xFF000000
};

// =============== hex ===============

/**
 * @brief Creates a color from a hexadecimal value.
 *
 * This function creates a color from a 32-bit hexadecimal value.
 *
 * @param hex The hexadecimal value representing the color.
 * @return The color created from the hexadecimal value.
 */
inline constexpr color8 color_from_hex(uint32_t hex)
{
    return color8(
        ((hex >>  0) & 0xFF),
        ((hex >>  8) & 0xFF),
        ((hex >> 16) & 0xFF),
        ((hex >> 24) & 0xFF)
    );
}

/**
 * @brief Converts a color to a hexadecimal value.
 *
 * This function converts a color to a 32-bit hexadecimal value.
 *
 * @param c The color to be converted.
 * @return The 32-bit hexadecimal value representing the color.
 */
inline constexpr uint32_t color_to_hex(const color8& c)
{
    return (
        (static_cast<uint32_t>(c.r) <<  0) |
        (static_cast<uint32_t>(c.g) <<  8) |
        (static_cast<uint32_t>(c.b) << 16) |
        (static_cast<uint32_t>(c.a) << 24)
    );
}

}
}