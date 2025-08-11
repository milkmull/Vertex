#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/app/device_id.hpp"
#include "vertex/math/rect.hpp"
#include "vertex/pixel/pixel_format.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// video
///////////////////////////////////////////////////////////////////////////////

VX_API bool init();
VX_API bool is_init();
VX_API void quit();

class display_mode;
class display;

namespace _priv { class video_internal; }
namespace _priv { class display_mode_impl; }
namespace _priv { class display_impl; }


///////////////////////////////////////////////////////////////////////////////
// display mode
///////////////////////////////////////////////////////////////////////////////

class display_mode
{
public:

    VX_API display_mode();
    VX_API display_mode(const display_mode&);
    VX_API display_mode(display_mode&&) noexcept;
    VX_API ~display_mode();

    VX_API display_mode& operator=(const display_mode&);
    VX_API display_mode& operator=(display_mode&&) noexcept;

    VX_API static bool compare(const display_mode& mode1, const display_mode& mode2);

public:

    math::vec2i resolution;
    int bpp;
    pixel::pixel_format pixel_format;
    float pixel_density;
    float refresh_rate;

private:

    friend _priv::video_internal;

    friend display;
    friend _priv::display_impl;
    device_id m_display_id;

    friend _priv::display_mode_impl;
    std::shared_ptr<_priv::display_mode_impl> m_impl;
};

///////////////////////////////////////////////////////////////////////////////
// displays
///////////////////////////////////////////////////////////////////////////////

VX_API void update_displays();

VX_API display* get_display(device_id id);
VX_API display* get_primary_display();

VX_API size_t display_count();
VX_API display* enum_displays(size_t i);

enum class display_orientation
{
    UNKNOWN,
    PORTRAIT,
    PORTRAIT_FLIPPED,
    LANDSCAPE,
    LANDSCAPE_FLIPPED
};

class display
{
public:

    VX_API display();
    VX_API ~display();

    VX_API display(display&&) noexcept;
    VX_API display& operator=(display&&) noexcept;

    display(const display&) = delete;
    display& operator=(const display&) = delete;

public:

    VX_API device_id id() const;
    VX_API const std::string& name() const;

    VX_API display_orientation get_orientation() const;
    VX_API const math::vec2& get_content_scale() const;

    VX_API math::recti get_bounds() const;
    VX_API math::recti get_work_area() const;

private:

    void update_modes() const;

public:

    VX_API const display_mode& get_desktop_mode() const;
    VX_API const display_mode& get_current_mode() const;

    VX_API bool set_current_mode(display_mode& mode);
    VX_API void clear_mode();

    VX_API size_t mode_count() const;
    VX_API const display_mode* enum_modes(size_t i) const;
    VX_API const std::vector<display_mode>& list_modes() const;

    VX_API const display_mode* find_mode(const display_mode& mode) const;
    VX_API const display_mode* find_closest_mode(int width, int height, float refresh_rate) const;

private:

    device_id m_id;
    std::string m_name;

    display_mode m_desktop_mode;
    display_mode m_current_mode;
    mutable std::vector<display_mode> m_modes;

    display_orientation m_orientation;
    math::vec2 m_content_scale;

private:

    friend _priv::video_internal;

    friend _priv::display_impl;
    std::unique_ptr<_priv::display_impl> m_impl;
};

} // namespace video
} // namespace app
} // namespace vx