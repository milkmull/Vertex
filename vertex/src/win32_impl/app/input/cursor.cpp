#include "cursor_internal.h"
#include "vertex/system/error.h"

namespace vx {
namespace app {

int cursor_data::s_cursor_id = static_cast<int>(cursor::cursor_shape::SHAPE_CUSTOM) + 1;
std::unordered_map<int, cursor_data> cursor_data::s_cursor_cache = std::unordered_map<int, cursor_data>();

static void load_system_cursor(cursor::cursor_shape shape)
{
    LPCWSTR cursor_name = 0;

    switch (shape)
    {
        case cursor::cursor_shape::SHAPE_ARROW:       cursor_name = IDC_ARROW;   break;
        case cursor::cursor_shape::SHAPE_IBEAM:       cursor_name = IDC_IBEAM;   break;
        case cursor::cursor_shape::SHAPE_WAIT:        cursor_name = IDC_WAIT;    break;
        case cursor::cursor_shape::SHAPE_CROSSHAIR:   cursor_name = IDC_CROSS;   break;
        case cursor::cursor_shape::SHAPE_HAND:        cursor_name = IDC_HAND;    break;
        case cursor::cursor_shape::SHAPE_HRESIZE:     cursor_name = IDC_SIZEWE;  break;
        case cursor::cursor_shape::SHAPE_VRESIZE:     cursor_name = IDC_SIZENS;  break;
        case cursor::cursor_shape::SHAPE_ALL_RESIZE:  cursor_name = IDC_SIZEALL; break;
        case cursor::cursor_shape::SHAPE_NOT_ALLOWED: cursor_name = IDC_NO;      break;
        default:                                                                 return;
    }

    cursor_data::s_cursor_cache[static_cast<int>(shape)].cursor = LoadCursorW(nullptr, cursor_name);
}

cursor::cursor(cursor_shape shape)
    : m_shape(shape), m_id(shape)
{
    // Load the cursor if we need to
    if (shape != cursor_shape::SHAPE_CUSTOM && cursor_data::s_cursor_cache.find(static_cast<int>(shape)) == cursor_data::s_cursor_cache.end())
    {
        load_system_cursor(shape);
    }
}

cursor cursor::make_custom(const uint8_t* pixels, const math::vec2i& size, const math::vec2i& hotspot)
{
    const size_t image_size = static_cast<size_t>(size.x * size.y * 4);

    // Format is expected to have 4 8-bit channels in RGBA format
    if (image_size % 4)
    {
        VX_ERROR(error::error_code::INVALID_ARGUMENT) << "Cursor pixels must be RGBA format";
        return cursor(cursor_shape::INVALID_SHAPE);
    }

    // Convert the image to an BGRA
    std::vector<uint8_t> formatted_pixels(image_size);
    for (size_t pixel = 0; pixel < image_size; pixel += 4)
    {
        formatted_pixels[pixel + 0] = pixels[pixel + 2];
        formatted_pixels[pixel + 1] = pixels[pixel + 1];
        formatted_pixels[pixel + 2] = pixels[pixel + 0];
        formatted_pixels[pixel + 3] = pixels[pixel + 3];
    }

    // Create empty bitmap for monochrome mask
    HBITMAP mask = CreateBitmap(size.x, size.y, 1, 1, NULL);
    if (!mask)
    {
        return cursor(cursor_shape::INVALID_SHAPE);
    }

    // Create color bitmap
    HBITMAP color = CreateBitmap(size.x, size.y, 1, 32, formatted_pixels.data());
    if (!color)
    {
        DeleteObject(mask);
        return cursor(cursor_shape::INVALID_SHAPE);
    }

    // Create icon info
    ICONINFO info{};
    info.fIcon = FALSE;
    info.xHotspot = hotspot.x;
    info.yHotspot = hotspot.y;
    info.hbmMask = mask;
    info.hbmColor = color;

    HCURSOR hcursor = CreateIconIndirect(&info);

    DeleteObject(color);
    DeleteObject(mask);

    if (!hcursor)
    {
        return cursor(cursor_shape::INVALID_SHAPE);
    }

    const int id = cursor_data::s_cursor_id++;

    // Cache the new cursor
    cursor_data::s_cursor_cache[id].cursor = hcursor;

    // Create the cursor
    ::vx::app::cursor custom_cursor(cursor_shape::SHAPE_CUSTOM);
    custom_cursor.m_id = id;

    return custom_cursor;
}

bool cursor::is_valid() const
{
    return cursor_data::s_cursor_cache.find(m_id) != cursor_data::s_cursor_cache.end();
}

bool cursor::operator==(const cursor& other) const { return m_id == other.m_id; }
bool cursor::operator!=(const cursor& other) const { return m_id != other.m_id; }

cursor::cursor_shape cursor::shape() const { return m_shape; }
int cursor::id() const { return m_id; }

}
}