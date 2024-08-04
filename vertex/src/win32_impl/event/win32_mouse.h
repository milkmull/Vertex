#pragma once

#include "../win32_header.h"
#include "vertex/app/event/mouse.h"

namespace vx {
namespace app {

class mouse::cursor::cursor_impl
{
private:

    friend mouse_impl;
    friend cursor;

public:

    ~cursor_impl();

private:

    bool validate() const;
    void destroy();

private:

    HCURSOR m_handle;
    bool m_is_system;

};

class mouse::mouse_impl
{
public:

    static bool create_system_cursor(cursor& c, cursor_shape shape);
    static bool create_custom_cursor(cursor& c, const img::image& image, const math::vec2i& hotspot);

    static HCURSOR get_current_cursor_handle();
    static bool show_cursor(const cursor* c);

};

}
}