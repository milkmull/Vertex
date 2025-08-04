#pragma once

#include <sstream>
#include <iomanip>

#include "hex.h"
#include "enum_color_channel.h"

namespace vx {
namespace math {

////////////////////////////////////////////////////////////////////////////////
/// @brief Checks whether a string represents a valid HTML color code.
///
/// Valid color codes can have formats like "#RGB", "#RGBA", "#RRGGBB", or "#RRGGBBAA".
///
/// @param html The string representing the HTML color code to validate.
/// 
/// @return True if the string is a valid HTML color code, false otherwise.
////////////////////////////////////////////////////////////////////////////////
inline bool is_valid_html_color(const std::string& html)
{
    const bool has_hash = (html[0] == '#');
    const size_t size = html.size();

    if ((has_hash && (size == 4 || size == 5 || size == 7 || size == 9)) ||
                     (size == 3 || size == 4 || size == 6 || size == 8))
    {
        try
        {
            (void)std::stoul(html.substr(has_hash ? 1 : 0), nullptr, 16);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Creates a color from a given HTML color code string.
/// 
/// The HTML color code can be in formats like "#RGB", "#RGBA", "#RRGGBB", or "#RRGGBBAA".
///
/// @param html The string representing the HTML color code.
/// @return The color represented by the HTML color code. If the HTML color code
/// is invalid or empty, a default color (black) is returned.
////////////////////////////////////////////////////////////////////////////////
inline color8 color_from_html(const std::string& html)
{
    if (html.empty() || !is_valid_html_color(html))
    {
        return color8(0);
    }

    std::string color = html.substr((html[0] == '#') ? 1 : 0);
    size_t size = color.size();

    // #RGB(A) format
    if (size == 3 || size == 4)
    {
        std::string tmp;
        for (const char c : color)
        {
            tmp += c;
            tmp += c;
        }
        color = tmp;
        size *= 2;
    }

    // #RRGGBB format
    if (size == 6)
    {
        uint32_t hex = static_cast<uint32_t>(std::stoul(color, nullptr, 16));
        hex |= color_mask::ALPHA;
        return color_from_hex(hex);
    }

    // #RRGGBBAA format
    if (size == 8)
    {
        const uint32_t hex = static_cast<uint32_t>(std::stoul(color, nullptr, 16));
        return color_from_hex(hex);
    }

    return color8(0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Converts a color to its corresponding HTML color code string.
/// 
/// Optionally, you can include a hash character (#) at the beginning of the
/// string.
///
/// @param c The color to convert.
/// @param hash Whether to include a hash character (#) at the beginning of the
/// HTML color code. Default is true.
/// 
/// @return The HTML color code string representing the color.
////////////////////////////////////////////////////////////////////////////////
inline std::string color_to_html(const color8& c, bool hash = true)
{
    std::ostringstream oss;
    if (hash) oss << '#';
    oss << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << color_to_hex(c);
    return oss.str();
}

}
}