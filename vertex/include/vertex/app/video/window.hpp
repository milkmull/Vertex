#pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace app {
namespace video {

class window;

//=============================================================================
// helper defs
//=============================================================================

enum class window_flash_op
{
    CANCEL,
    BRIEF,
    UNTIL_FOCUSED
};

//=============================================================================
// window instance
//=============================================================================

class window
{
public:

    window(window_id id) noexcept : m_id(id) {}

public:

    operator window_id() const noexcept { return m_id; }
    window_id id() const noexcept { return m_id; }

    bool is_valid() const noexcept { return is_valid_id(m_id); }
    bool exists() const { return window_exists(m_id); }

    //=============================================================================
    // sync
    //=============================================================================

    VX_API void sync();

    //=============================================================================
    // title
    //=============================================================================

    VX_API std::string get_title() const;
    VX_API bool set_title(const std::string& title);

    //=============================================================================
    // position and size
    //=============================================================================

    VX_API bool set_resizable(bool resizable);
    VX_API bool is_resizable() const;

    VX_API math::vec2i get_position() const;
    VX_API bool set_position(const math::vec2i& position);

    VX_API math::vec2i get_size() const;
    VX_API bool set_size(const math::vec2i& size);

    VX_API math::vec2i get_center() const;
    VX_API math::recti get_rect() const;

    VX_API math::recti get_border_size() const;
    VX_API math::vec2i get_size_in_pixels() const;
    VX_API float get_pixel_density() const;

    VX_API math::vec2i get_min_size() const;
    VX_API math::vec2i get_max_size() const;

    VX_API bool set_min_size(const math::vec2i& size);
    VX_API bool set_max_size(const math::vec2i& size);

    VX_API float get_aspect_ratio() const;
    VX_API float get_locked_aspect_ratio() const;
    VX_API bool lock_aspect_ratio(float aspect_ratio);

    //=============================================================================
    // safe area
    //=============================================================================

    VX_API bool set_safe_area(const math::recti& area);
    VX_API math::recti get_safe_area() const;

    //=============================================================================
    // bordered
    //=============================================================================

    VX_API bool set_bordered(bool bordered);
    VX_API bool is_bordered() const;

    //=============================================================================
    // always on top
    //=============================================================================

    VX_API bool set_always_on_top(bool always_on_top);
    VX_API bool is_always_on_top() const;

    //=============================================================================
    // visibility
    //=============================================================================

    VX_API bool show();
    VX_API bool hide();
    VX_API bool is_visible() const;
    VX_API bool is_hidden() const;

    VX_API bool is_occluded() const;
    VX_API bool is_exposed() const;

    //=============================================================================
    // minimize / maximize / restore
    //=============================================================================

    VX_API bool minimize();
    VX_API bool is_minimized() const;

    VX_API bool maximize();
    VX_API bool is_maximized() const;

    VX_API bool restore();
    VX_API bool raise();

    //=============================================================================
    // system
    //=============================================================================

    VX_API bool flash(window_flash_op operation);

    //=============================================================================
    // fullscreen
    //=============================================================================

    VX_API bool is_fullscreen() const;

    VX_API const display_mode& get_fullscreen_mode() const;
    VX_API bool set_fullscreen_mode(const display_mode* mode);

    VX_API bool set_fullscreen(bool fullscreen);

    //=============================================================================
    // appearence
    //=============================================================================

    VX_API bool set_icon(const pixel::surface_rgba8& surf);
    VX_API bool set_shape(const pixel::surface_rgba8& shape);

    VX_API float get_opacity() const;
    VX_API bool set_opacity(float opacity);

    VX_API std::vector<uint8_t> get_icc_profile() const;
    VX_API pixel::pixel_format get_pixel_format() const;

    //=============================================================================
    // grab
    //=============================================================================

    VX_API bool get_mouse_grab() const;
    VX_API bool get_keyboard_grab() const;

    VX_API bool set_mouse_grab(bool grabbed);
    VX_API bool set_keyboard_grab(bool grabbed);

    //=============================================================================
    // focus
    //=============================================================================

    VX_API bool set_focusable(bool focusable);
    VX_API bool has_mouse_focus() const;
    VX_API bool has_keyboard_focus() const;

    //=============================================================================
    // mouse
    //=============================================================================

    VX_API math::recti get_mouse_rect() const;
    VX_API bool set_mouse_rect(const math::recti& rect);

    //=============================================================================
    // drag and drop
    //=============================================================================

    VX_API bool toggle_drag_and_drop(bool enabled);

private:

    window_id m_id;
};

} // namespace video
} // namespace app
} // namespace vx