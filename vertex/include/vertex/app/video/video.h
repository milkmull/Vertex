#pragma once

#include <memory>
#include <string>
#include <vector>

#include "vertex/pixel/pixel.h"
#include "vertex/math/math/type/vec2.h"
#include "vertex/math/geometry/type/recti.h"

namespace vx {
namespace app {

class window;

class video
{
private:

    class video_impl;

public:

    enum class process_dpi_awareness
    {
        UNAWARE,
        SYSTEM,
        PER_MONITOR
    };

    enum class display_orientation
    {
        UNKNOWN,

        PORTRAIT,
        LANDSCAPE,

        PORTRAIT_FLIPPED,
        LANDSCAPE_FLIPPED
    };

    struct display_mode
    {
        int width;
        int height;

        int bpp;
        pixel::pixel_format pixel_format;

        float refresh_rate;

        inline constexpr bool operator==(const display_mode& other) const
        {
            return width == other.width
                && height == other.height
                && bpp == other.bpp
                && pixel_format == other.pixel_format
                && refresh_rate == other.refresh_rate;
        }

        inline constexpr bool operator!=(const display_mode& other) const
        {
            return !(*this == other);
        }

        inline bool operator<(const display_mode& other) const
        {
            if (width != other.width)               return width < other.width;
            if (height != other.height)             return height < other.height;
            if (bpp != other.bpp)                   return bpp < other.bpp;
            if (pixel_format != other.pixel_format) return pixel_format < other.pixel_format;
            else                                    return refresh_rate < other.refresh_rate;
        }
    };

    class display
    {
    public:

        using display_id = size_t;
        display::display_id id;
        std::string name;

        bool get_mode(display_mode& mode) const;
        std::vector<display_mode> list_modes() const;

        display_orientation get_orientation() const;
        math::vec2 get_content_scale() const;

        bool get_bounds(math::recti& bounds) const;
        bool get_work_area(math::recti& work_area) const;

    private:

        friend video_impl;

        struct display_impl;
        std::shared_ptr<display_impl> m_impl;
    };

public:

    static bool init();
    static void quit();

    static process_dpi_awareness get_dpi_awareness();

    static void update_displays();
    static const display* get_primary_display();
    static const std::vector<const display*> list_displays();

    static const display* get_display_for_point(const math::vec2i& p);
    static const display* get_display_for_rect(const math::recti& rect);
    static const display* get_display_for_window_position(const window* window);
    static const display* get_display_for_window(const window* window);

};

}
}