#pragma once

#include "type/color8.h"

namespace vx {
namespace math {

////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a color from a 32-bit hexadecimal value.
///
/// @param hex The hexadecimal value representing the color.
/// 
/// @return The color created from the hexadecimal value.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr color8 color_from_hex(uint32_t hex)
{
    return color8(
        ((hex >>  0) & 0xFF),
        ((hex >>  8) & 0xFF),
        ((hex >> 16) & 0xFF),
        ((hex >> 24) & 0xFF)
    );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Converts a color to a 32-bit hexadecimal value.
///
/// @param c The color to be converted.
/// 
/// @return The 32-bit hexadecimal value representing the color.
////////////////////////////////////////////////////////////////////////////////
VX_FORCE_INLINE constexpr uint32_t color_to_hex(const color8& c)
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