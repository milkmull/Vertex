#pragma once

#include "vertex/app/video/video.hpp"
#include "vertex/pixel/surface.hpp"

namespace vx {
namespace app {
namespace video {

///////////////////////////////////////////////////////////////////////////////
// helper defs
///////////////////////////////////////////////////////////////////////////////

enum class window_flash_op
{
    CANCEL,
    BRIEF,
    UNTIL_FOCUSED
};

///////////////////////////////////////////////////////////////////////////////
// window instance
///////////////////////////////////////////////////////////////////////////////

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

    ///////////////////////////////////////////////////////////////////////////////
    // title
    ///////////////////////////////////////////////////////////////////////////////

    VX_API const std::string& get_title() const;
    VX_API void set_title(const std::string& title);

    ///////////////////////////////////////////////////////////////////////////////
    // position and size
    ///////////////////////////////////////////////////////////////////////////////

    VX_API void set_resizable(bool resizable);
    VX_API bool is_resizable() const;

    VX_API math::vec2i get_position() const;
    VX_API void set_position(const math::vec2i& position);

    VX_API const math::vec2i& get_size() const;
    VX_API void set_size(const math::vec2i& size);

    VX_API math::vec2i get_center() const;
    VX_API math::recti get_rect() const;
    VX_API void get_border_size(int32_t& left, int32_t& right, int32_t& bottom, int32_t& top) const;

    VX_API const math::vec2i& get_min_size() const;
    VX_API const math::vec2i& get_max_size() const;

    VX_API bool set_min_size(const math::vec2i& size);
    VX_API bool set_max_size(const math::vec2i& size);

    VX_API float get_aspect_ratio() const;
    VX_API void lock_aspect_ratio(float aspect_ratio);

    ///////////////////////////////////////////////////////////////////////////////
    // bordered
    ///////////////////////////////////////////////////////////////////////////////

    VX_API void set_bordered(bool bordered);
    VX_API bool is_bordered() const;

    ///////////////////////////////////////////////////////////////////////////////
    // always on top
    ///////////////////////////////////////////////////////////////////////////////

    VX_API void set_always_on_top(bool always_on_top);
    VX_API bool is_always_on_top() const;

    ///////////////////////////////////////////////////////////////////////////////
    // window_instance operators
    ///////////////////////////////////////////////////////////////////////////////

    VX_API void show();
    VX_API void hide();
    VX_API bool is_visible() const;
    VX_API bool is_hidden() const;

    VX_API void minimize();
    VX_API bool is_minimized() const;

    VX_API void maximize();
    VX_API bool is_maximized() const;

    VX_API void restore();

    VX_API void focus();
    VX_API bool is_focused() const;

    VX_API void flash(window_flash_op operation);

    ///////////////////////////////////////////////////////////////////////////////
    // fullscreen
    ///////////////////////////////////////////////////////////////////////////////

    VX_API bool is_fullscreen() const;
    VX_API bool set_fullscreen(bool fullscreen);

    VX_API const display_mode& get_fullscreen_mode() const;
    VX_API bool set_fullscreen_mode(const display_mode& mode);

    ///////////////////////////////////////////////////////////////////////////////
    // icon
    ///////////////////////////////////////////////////////////////////////////////

    VX_API bool set_icon(const pixel::surface_rgba8& surf);
    VX_API void clear_icon();

private:

    window_id m_id;
};

} // namespace video
} // namespace app
} // namespace vx