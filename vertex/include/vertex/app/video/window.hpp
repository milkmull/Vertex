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

enum class hit_test_result
{
    NORMAL,             /**< Region is normal. No special properties. */
    DRAGGABLE,          /**< Region can drag entire window. */
    RESIZE_TOPLEFT,     /**< Region is the resizable top-left corner border. */
    RESIZE_TOP,         /**< Region is the resizable top border. */
    RESIZE_TOPRIGHT,    /**< Region is the resizable top-right corner border. */
    RESIZE_RIGHT,       /**< Region is the resizable right border. */
    RESIZE_BOTTOMRIGHT, /**< Region is the resizable bottom-right corner border. */
    RESIZE_BOTTOM,      /**< Region is the resizable bottom border. */
    RESIZE_BOTTOMLEFT,  /**< Region is the resizable bottom-left corner border. */
    RESIZE_LEFT         /**< Region is the resizable left border. */
};

using window_hit_test = hit_test_result(*)(window* w, int x, int y, void* user_data);

//=============================================================================
// window instance
//=============================================================================

class window
{
public:

    window(window_id id) : m_id(id) {}
    ~window() {}

public:

    operator window_id() const noexcept { return m_id; }
    window_id id() const noexcept { return m_id; }

    bool is_valid() const noexcept { return is_valid_id(m_id); }
    VX_API bool exists() const;

    //=============================================================================
    // sync
    //=============================================================================

    VX_API void sync();

    //=============================================================================
    // title
    //=============================================================================

    VX_API const std::string& get_title() const;
    VX_API void set_title(const std::string& title);

    //=============================================================================
    // position and size
    //=============================================================================

    VX_API bool set_resizable(bool resizable);
    VX_API bool is_resizable() const;

    VX_API bool get_position(int32_t* x, int32_t* y) const;
    VX_API bool set_position(int32_t x, int32_t y);

    VX_API bool get_size(int32_t* w, int32_t* h) const;
    VX_API bool set_size(int32_t w, int32_t h);

    VX_API math::vec2i get_center() const;
    VX_API math::recti get_rect() const;

    VX_API bool get_border_size(int32_t* left, int32_t* right, int32_t* bottom, int32_t* top) const;
    VX_API bool get_size_in_pixels(int32_t* w, int32_t* h) const;
    VX_API float get_pixel_density() const;

    VX_API bool get_min_size(int32_t* w, int32_t* h) const;
    VX_API bool get_max_size(int32_t* w, int32_t* h) const;

    VX_API bool set_min_size(int32_t w, int32_t h);
    VX_API bool set_max_size(int32_t w, int32_t h);

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