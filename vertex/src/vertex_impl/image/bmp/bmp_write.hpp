#pragma once

#include "vertex_impl/image/write_context.hpp"
#include "vertex_impl/image/pixel_writer.hpp"

namespace vx {
namespace img {
namespace bmp {

///////////////////////////////////////////////////////////////////////////////
// Microsoft/Windows BMP image
///////////////////////////////////////////////////////////////////////////////

static bool write_bmp_core(write_context& ctx)
{
    pixel_writer pw{ ctx };

    const uint32_t w = static_cast<uint32_t>(ctx.w);
    const uint32_t h = static_cast<uint32_t>(ctx.h);

    if (ctx.channels != 4)
    {
        // write RGB bitmap

        constexpr uint32_t file_header_size = 14;
        constexpr uint32_t info_header_size = 40;
        constexpr uint16_t bpp = 24;

        // Each BMP scanline must be padded to a multiple of 4 bytes.
        // Each pixel is 3 bytes (RGB), so we compute the minimal padding.
        const uint32_t pad = (-static_cast<int32_t>(w) * 3) & 3;

        // file header
        {
            pw.write_field_1('B');
            pw.write_field_1('M');

            // File size in bytes
            const uint32_t pixel_data_size = (w * 3 + pad) * h;
            pw.write_field_4(file_header_size + info_header_size + pixel_data_size);

            // reserved
            pw.write_field_2(0);
            pw.write_field_2(0);

            // Offset to pixel data
            pw.write_field_4(file_header_size + info_header_size);
        }

        // bmp header
        {
            // header size
            pw.write_field_4(info_header_size);

            // image width and height
            pw.write_field_4(w);
            pw.write_field_4(h);

            pw.write_field_2(1); // planes (always 1)
            pw.write_field_2(bpp); // bpp

            pw.write_field_4(0); // compression

            pw.write_field_4(0); // Image size (can be 0 for BI_RGB)
            pw.write_field_4(0); // X pixels per meter (unspecified)
            pw.write_field_4(0); // Y pixels per meter (unspecified)
            pw.write_field_4(0); // Colors used (0 = all)
            pw.write_field_4(0); // Important colors (0 = all)
        }

        // pixels
        pw.write_pixels(rgb_order::BGR, alpha_order::NONE, true, virtical_direction::FLIPPED, pad);
    }
    else
    {
        // RGBA bitmaps need a v4 header
        // use BI_BITFIELDS mode with 32bpp and alpha mask
        // (straight BI_RGB with alpha mask doesn't work in most readers)

        constexpr uint32_t file_header_size = 14;
        constexpr uint32_t info_header_size = 108;
        constexpr uint16_t bpp = 32;

        // 4 channels means we are already aligned to 4 bytes
        constexpr uint32_t pad = 0;

        // file header
        {
            pw.write_field_1('B');
            pw.write_field_1('M');

            // File size in bytes
            const uint32_t pixel_data_size = (w * h * 4);
            pw.write_field_4(file_header_size + info_header_size + pixel_data_size);

            // reserved
            pw.write_field_2(0);
            pw.write_field_2(0);

            // Offset to pixel data
            pw.write_field_4(file_header_size + info_header_size);
        }

        // bitmap V4 header
        {
            // header size
            pw.write_field_4(info_header_size);

            // image width and height
            pw.write_field_4(w);
            pw.write_field_4(h);

            pw.write_field_2(1); // planes (always 1)
            pw.write_field_2(bpp); // bpp

            pw.write_field_4(3); // Compression: BI_BITFIELDS (allows color masks)

            pw.write_field_4(0); // Image size (can be 0 for BI_RGB)
            pw.write_field_4(0); // X pixels per meter (unspecified)
            pw.write_field_4(0); // Y pixels per meter (unspecified)
            pw.write_field_4(0); // Colors used (0 = all)
            pw.write_field_4(0); // Important colors (0 = all)

            // Color masks (BGR + alpha), required for BI_BITFIELDS
            pw.write_field_4(0x00FF0000); // Red mask
            pw.write_field_4(0x0000FF00); // Green mask
            pw.write_field_4(0x000000FF); // Blue mask
            pw.write_field_4(0xFF000000); // Alpha mask

            pw.write_field_4(0);

            pw.write_field_4(0);
            pw.write_field_4(0);
            pw.write_field_4(0);

            pw.write_field_4(0);
            pw.write_field_4(0);
            pw.write_field_4(0);

            pw.write_field_4(0);
            pw.write_field_4(0);
            pw.write_field_4(0);

            pw.write_field_4(0);
            pw.write_field_4(0);
            pw.write_field_4(0);
        }

        // pixels
        pw.write_pixels(rgb_order::BGR, alpha_order::LAST, true, virtical_direction::FLIPPED, pad);
    }

    return true;
}

static bool write_bmp(const char* filename, size_t width, size_t height, size_t channels, const uint8_t* data)
{
    write_context ctx{};
    ctx.w = width;
    ctx.h = height;
    ctx.channels = channels;
    ctx.data = data;

    if (!ctx.open_file(filename))
    {
        return false;
    }

    const bool ok = write_bmp_core(ctx);
    ctx.close_file();
    return ok;
}

} // namespace bmp
} // namespace img
} // namespace vx