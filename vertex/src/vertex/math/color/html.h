#pragma once

#include <sstream>
#include <iomanip>

#include "hex.h"

namespace vx {
namespace math {
namespace color_util {

static inline bool is_valid_html(const std::string& html)
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

static inline color8 color_from_html(const std::string& html)
{
    if (html.empty() || !is_valid_html(html))
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
        hex |= VX_COLOR_MASK_A;
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

static inline std::string color_to_html(const color8& c, bool hash = true)
{
    std::ostringstream oss;
    if (hash) oss << '#';
    oss << std::hex << std::uppercase << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << color_to_hex(c);
    return oss.str();
}

}
}
}