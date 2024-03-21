#pragma once

#include "vertex/math/math/type/vec2.h"

namespace vx {
namespace app {

// https://www.glfw.org/docs/3.3/window_guide.html

class window
{
public:

    enum window_mode
    {
        WINDOWED,
        MINIMIZED,
        MAXIMIZED,
        FULLSCREEN
    };

    enum window_flag
    {
        NONE        = 0,
        NO_RESIZE   = (1 << 1),
        NO_BORDER   = (1 << 2),
        TRANSPARENT = (1 << 3),
        NO_FOCUS    = (1 << 4)
    };

public:

    // =============== constructors ===============

    window();
    ~window();

    window(const window&) = delete;
    window(window&&) = delete;

    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;

    // =============== title ===============

    inline const char* title() const { return m_title; }
    void set_title(const char* title);

    // =============== position ===============

    inline math::vec2 position() const { return m_position; }
    void set_position(const math::vec2& position);

    // =============== size ===============

    inline math::vec2 size() const { return m_size; }
    void resize(const math::vec2& size);

    // =============== mode ===============

    inline window_mode mode() const { return m_mode; }

    // =============== flags ===============

    inline bool get_flag(window_flag flag) { return m_flags & flag; }
    void set_flag(window_flag flag);

    // =============== focus ===============

    bool has_focus() const;
    void focus();

private:

    void* m_window = nullptr;

    const char* m_title = nullptr;

    math::vec2 m_position;
    math::vec2 m_size;

    window_mode m_mode = window_mode::WINDOWED;
    window_flag m_flags = window_flag::NONE;

};

}
}