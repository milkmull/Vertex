#include "vertex/system/platform.h"

#include "vertex/app/event/mouse.h"

#if defined(VX_PLATFORM_WINDOWS)
#   include "_win32/win32_mouse.h"
#endif

namespace vx {
namespace app {

mouse::mouse_data mouse::s_mouse_data = mouse::mouse_data{};

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

mouse::cursor::cursor()
    : m_id(0)
{
}

mouse::cursor::~cursor()
{
}

mouse::cursor::cursor(const cursor& c)
    : m_id(c.m_id)
    , m_hotspot(c.m_hotspot)
    , m_impl(std::make_unique<cursor_impl>(*c.m_impl))
{
}

mouse::cursor::cursor(cursor&& c) noexcept
    : m_id(c.m_id)
    , m_hotspot(c.m_hotspot)
    , m_impl(std::move(c.m_impl))
{
}

mouse::cursor& mouse::cursor::operator=(const cursor& c)
{
    m_id = c.m_id;
    m_hotspot = c.m_hotspot;
    m_impl = c.m_impl ? std::make_unique<cursor_impl>(*c.m_impl) : nullptr;

    return *this;
}
mouse::cursor& mouse::cursor::operator=(cursor&& c) noexcept
{
    m_id = c.m_id;
    m_hotspot = c.m_hotspot;
    m_impl = std::move(c.m_impl);

    return *this;
}

bool mouse::cursor::validate() const
{
    return m_impl && m_impl->validate();
}

const mouse::cursor* mouse::create_system_cursor(cursor_shape shape)
{
    const cursor_id id = static_cast<cursor_id>(shape);

    auto it = s_mouse_data.cursors.find(id);
    if (it != s_mouse_data.cursors.end())
    {
        return it->second.get();
    }

    auto c = std::make_unique<cursor>();
    if (!mouse_impl::create_system_cursor(*c, shape))
    {
        return nullptr;
    }

    c->m_id = id;

    s_mouse_data.cursors[id] = std::move(c);
    return s_mouse_data.cursors.at(id).get();
}

const mouse::cursor* mouse::create_custom_cursor(const pixel::surface& surf, const math::vec2i& hotspot)
{
    if (surf.empty())
    {
        return nullptr;
    }

    pixel::surface cursor_surf = surf.convert(pixel::pixel_format::RGBA_8);
    math::vec2i cursor_hotspot = cursor_surf.get_rect().clip(hotspot);

    auto c = std::make_unique<cursor>();
    if (!mouse_impl::create_custom_cursor(*c, cursor_surf, cursor_hotspot))
    {
        return nullptr;
    }

    const cursor_id id = s_mouse_data.custom_cursor_id++;
    c->m_id = id;

    s_mouse_data.cursors[id] = std::move(c);
    return s_mouse_data.cursors.at(id).get();
}

void mouse::destroy_cursor(const cursor* c)
{
    if (!c)
    {
        return;
    }

    if (!c || c == s_mouse_data.default_cursor || c == s_mouse_data.blank_cursor)
    {
        return;
    }

    if (c == s_mouse_data.current_cursor)
    {
        set_cursor(s_mouse_data.default_cursor);
    }

    auto it = s_mouse_data.cursors.find(c->m_id);
    if (it != s_mouse_data.cursors.end())
    {
        s_mouse_data.cursors.erase(it);
    }
}

const mouse::cursor* mouse::get_cursor()
{
    return s_mouse_data.current_cursor;
}

bool mouse::set_cursor(const cursor* c)
{
    if (c == s_mouse_data.current_cursor)
    {
        return true;
    }

    if (c)
    {
        // Verify that the cursor is still available
        if (!s_mouse_data.cursors.count(c->m_id))
        {
            return false;
        }

        s_mouse_data.current_cursor = c;
    }
    else
    {
        if (s_mouse_data.has_focus)
        {
            s_mouse_data.current_cursor = s_mouse_data.current_cursor;
        }
        else
        {
            s_mouse_data.current_cursor = s_mouse_data.default_cursor;
        }
    }

    return mouse_impl::show_cursor(c);
}

const mouse::cursor* mouse::get_default_cursor()
{
    return s_mouse_data.default_cursor;
}

bool mouse::set_default_cursor(const cursor* c)
{
    if (c == s_mouse_data.default_cursor)
    {
        return true;
    }

    s_mouse_data.default_cursor = c;
    if (!s_mouse_data.current_cursor)
    {
        set_cursor(c);
    }

    return true;
}

bool mouse::init()
{
    s_mouse_data.default_cursor = create_system_cursor(cursor_shape::DEFAULT);

    pixel::surface surf(1, 1, pixel::pixel_format::RGBA_8);
    s_mouse_data.blank_cursor = create_custom_cursor(surf, math::vec2i(0));

    if (!s_mouse_data.default_cursor)
    {
        s_mouse_data.default_cursor = s_mouse_data.blank_cursor;
    }
    set_default_cursor(s_mouse_data.default_cursor);

    return true;
}

void mouse::quit()
{

}

}
}