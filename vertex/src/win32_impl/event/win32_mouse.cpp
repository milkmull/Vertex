#include "win32_mouse.h"
#include "vertex/image/fn_mask.h"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// cursor
///////////////////////////////////////////////////////////////////////////////

mouse::cursor::cursor_impl::~cursor_impl()
{
    destroy();
}

bool mouse::cursor::cursor_impl::validate() const
{
    return m_handle;
}

void mouse::cursor::cursor_impl::destroy()
{
    if (!m_is_system && m_handle)
    {
        DestroyCursor(m_handle);
        m_handle = NULL;
    }
}

static bool get_system_cursor_hotspot(HCURSOR handle, math::vec2i& hotspot)
{
    if (!handle)
    {
        return false;
    }

    ICONINFO ii;
    if (!GetIconInfo(handle, &ii))
    {
        return false;
    }

    hotspot.x = ii.xHotspot;
    hotspot.y = ii.yHotspot;

    // Clean up the bitmap handles if they are not null
    if (ii.hbmMask)
    {
        DeleteObject(ii.hbmMask);
    }
    if (ii.hbmColor)
    {
        DeleteObject(ii.hbmColor);
    }

    return true;
}

bool mouse::mouse_impl::create_system_cursor(cursor& c, cursor_shape shape)
{
    LPCTSTR name;

    switch (shape)
    {
        case cursor_shape::DEFAULT:
        {
            name = IDC_ARROW;
            break;
        }
        case cursor_shape::TEXT:
        {
            name = IDC_IBEAM;
            break;
        }
        case cursor_shape::WAIT:
        {
            name = IDC_WAIT;
            break;
        }
        case cursor_shape::CROSSHAIR:
        {
            name = IDC_CROSS;
            break;
        }
        case cursor_shape::PROGRESS:
        {
            name = IDC_WAIT;
            break;
        }
        case cursor_shape::NWSE_RESIZE:
        {
            name = IDC_SIZENWSE;
            break;
        }
        case cursor_shape::NESW_RESIZE:
        {
            name = IDC_SIZENESW;
            break;
        }
        case cursor_shape::EW_RESIZE:
        {
            name = IDC_SIZEWE;
            break;
        }
        case cursor_shape::NS_RESIZE:
        {
            name = IDC_SIZENS;
            break;
        }
        case cursor_shape::MOVE:
        {
            name = IDC_SIZEALL;
            break;
        }
        case cursor_shape::NOT_ALLOWED:
        {
            name = IDC_NO;
            break;
        }
        case cursor_shape::POINTER:
        {
            name = IDC_HAND;
            break;
        }
        case cursor_shape::NW_RESIZE:
        {
            name = IDC_SIZENWSE;
            break;
        }
        case cursor_shape::N_RESIZE:
        {
            name = IDC_SIZENS;
            break;
        }
        case cursor_shape::NE_RESIZE:
        {
            name = IDC_SIZENESW;
            break;
        }
        case cursor_shape::E_RESIZE:
        {
            name = IDC_SIZEWE;
            break;
        }
        case cursor_shape::SE_RESIZE:
        {
            name = IDC_SIZENWSE;
            break;
        }
        case cursor_shape::S_RESIZE:
        {
            name = IDC_SIZENS;
            break;
        }
        case cursor_shape::SW_RESIZE:
        {
            name = IDC_SIZENESW;
            break;
        }
        case cursor_shape::W_RESIZE:
        {
            name = IDC_SIZEWE;
            break;
        }
        default:
        {
            return nullptr;
        }
    }

    c.m_impl = std::make_unique<cursor::cursor_impl>();
    if (!c.m_impl)
    {
        return false;
    }

    HCURSOR handle = LoadCursor(NULL, name);
    if (!handle)
    {
        return false;
    }

    c.m_impl->m_handle = handle;
    c.m_impl->m_is_system = true;
    get_system_cursor_hotspot(handle, c.m_hotspot);

    return true;
}

static HCURSOR create_cursor(const img::image& image, const math::vec2i& hotspot)
{
    const int image_width = static_cast<int>(image.width());
    const int image_height = static_cast<int>(image.height());

    const size_t image_size = image.data_size();
    const uint8_t* image_pixels = image.data();

    std::vector<uint8_t> image_mask;
    img::create_xor_mask(image, image_mask);

    HBITMAP mask = CreateBitmap(image_width, image_height, 1, 1, image_mask.data());
    if (!mask)
    {
        return nullptr;
    }

    // Convert the image to BGRA
    std::vector<uint8_t> formatted_pixels(image_size);
    for (size_t pixel = 0; pixel < image_size; pixel += 4)
    {
        formatted_pixels[pixel + 0] = image_pixels[pixel + 2];
        formatted_pixels[pixel + 1] = image_pixels[pixel + 1];
        formatted_pixels[pixel + 2] = image_pixels[pixel + 0];
        formatted_pixels[pixel + 3] = image_pixels[pixel + 3];
    }

    // Create color bitmap
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = image_width;
    bi.bmiHeader.biHeight = -image_height; // Negative for top-down bitmap
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* pixels;
    HBITMAP color = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pixels, NULL, 0);
    if (!color || !pixels)
    {
        DeleteObject(mask);

        if (color)
        {
            DeleteObject(color);
        }

        return nullptr;
    }

    std::memcpy(pixels, formatted_pixels.data(), formatted_pixels.size());

    // Create icon info
    ICONINFO ii{};
    ii.fIcon = FALSE;
    ii.xHotspot = static_cast<DWORD>(hotspot.x);
    ii.yHotspot = static_cast<DWORD>(hotspot.y);
    ii.hbmMask = mask;
    ii.hbmColor = color;

    HCURSOR handle = CreateIconIndirect(&ii);

    DeleteObject(mask);
    DeleteObject(color);

    return handle;
}

bool mouse::mouse_impl::create_custom_cursor(cursor& c, const img::image& image, const math::vec2i& hotspot)
{
    c.m_impl = std::make_unique<cursor::cursor_impl>();
    if (!c.m_impl)
    {
        return false;
    }

    HCURSOR handle = create_cursor(image, hotspot);
    if (!handle)
    {
        return false;
    }

    c.m_impl->m_handle = handle;
    c.m_impl->m_is_system = false;
    c.m_hotspot = hotspot;

    return true;
}

HCURSOR mouse::mouse_impl::get_current_cursor_handle()
{
    if (s_mouse_data.current_cursor && s_mouse_data.current_cursor->validate())
    {
        return s_mouse_data.current_cursor->m_impl->m_handle;
    }
    if (s_mouse_data.default_cursor && s_mouse_data.default_cursor->validate())
    {
        return s_mouse_data.default_cursor->m_impl->m_handle;
    }

    return NULL;
}

bool mouse::mouse_impl::show_cursor(const cursor* c)
{
    HCURSOR handle = NULL;

    if (!c)
    {
        c = s_mouse_data.blank_cursor;
    }
    if (c)
    {
        handle = c->m_impl->m_handle;
    }

    //if (SDL_GetMouseFocus() != NULL)
    {
        SetCursor(handle);
    }

    return true;
}

}
}