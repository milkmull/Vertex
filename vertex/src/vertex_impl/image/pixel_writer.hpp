#pragma once

#include "vertex_impl/image/write_context.hpp"

namespace vx {
namespace img {

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

enum rgb_order : int
{
    rgb = 1,
    bgr = -1
};

enum alpha_order : int
{
    none = 0,
    first = 1,
    last = -1
};

enum virtical_direction : int
{
    normal = 1,
    flipped = -1
};

struct pixel_writer
{
    write_context& ctx;

    ///////////////////////////////////////////////////////////////////////////////
    // buffer
    ///////////////////////////////////////////////////////////////////////////////

    uint8_t buffer[64];
    size_t buffer_size = 0;

    VX_FORCE_INLINE void write(const uint8_t* d, size_t size)
    {
        ctx.file.write(d, size);
    }

    VX_FORCE_INLINE void flush()
    {
        if (buffer_size > 0)
        {
            write(buffer, buffer_size);
            buffer_size = 0;
        }
    }

    VX_FORCE_INLINE void write_1(uint8_t a)
    {
        buffer[buffer_size++] = a;

        if (buffer_size == sizeof(buffer))
        {
            flush();
        }
    }

    VX_FORCE_INLINE void write_3(uint8_t a, uint8_t b, uint8_t c)
    {
        if (buffer_size + 3 >= sizeof(buffer))
        {
            flush();
        }

        buffer[buffer_size++] = a;
        buffer[buffer_size++] = b;
        buffer[buffer_size++] = c;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // field writing
    ///////////////////////////////////////////////////////////////////////////////

    void write_field_1(uint8_t value)
    {
        write(&value, 1);
    }

    void write_field_2(uint16_t value)
    {
        const uint8_t d[2] = {
            static_cast<uint8_t>((value >> 0) & 0xFF),
            static_cast<uint8_t>((value >> 8) & 0xFF)
        };

        write(d, 2);
    }

    void write_field_4(uint32_t value)
    {
        const uint8_t d[4] = {
            static_cast<uint8_t>((value >>  0) & 0xFF),
            static_cast<uint8_t>((value >>  8) & 0xFF),
            static_cast<uint8_t>((value >> 16) & 0xFF),
            static_cast<uint8_t>((value >> 24) & 0xFF)
        };

        write(d, 4);
    }

    void write_pixel(
        rgb_order rgb_ord, alpha_order alpha_ord, bool expand_mono,
        const uint8_t* srcpx
    )
    {
        if (alpha_ord == alpha_order::FIRST)
        {
            // alpha should be written first
            write_1(srcpx[ctx.channels - 1]);
        }

        switch (ctx.channels)
        {
            case 2: // 2 pixels = mono + alpha, alpha is written separately, so same as 1-channel case
            case 1:
            {
                if (expand_mono)
                {
                    // monochrome bmp
                    write_3(srcpx[0], srcpx[0], srcpx[0]);
                }
                else
                {
                    // monochrome TGA
                    write_1(srcpx[0]);
                }
                break;
            }
            case 4:
            {
                if (alpha_ord == alpha_order::NONE)
                {
                    constexpr uint8_t bg[3] = { 255, 0, 255 };
                    uint8_t dstpx[3];

                    // composite against pink background
                    dstpx[0] = bg[0] + ((srcpx[0] - bg[0]) * srcpx[3]) / 255;
                    dstpx[1] = bg[1] + ((srcpx[1] - bg[1]) * srcpx[3]) / 255;
                    dstpx[2] = bg[2] + ((srcpx[2] - bg[2]) * srcpx[3]) / 255;

                    write_3(dstpx[1 - rgb_ord], dstpx[1], dstpx[1 + rgb_ord]);
                    break;
                }
                VX_FALLTHROUGH;
            }
            case 3:
            {
                write_3(srcpx[1 - rgb_ord], srcpx[1], srcpx[1 + rgb_ord]);
                break;
            }
        }

        if (alpha_ord == alpha_order::LAST)
        {
            write_1(srcpx[ctx.channels - 1]);
        }
    }

    void write_pixels(
        rgb_order rgb_ord, alpha_order alpha_ord, bool expand_mono, 
        virtical_direction vdir, size_t scan_line_padding
    )
    {
        constexpr uint64_t zero = 0;

        for (size_t i = 0; i < ctx.h; ++i)
        {
            const size_t y = (vdir == virtical_direction::FLIPPED) ? (ctx.h - 1 - i) : i;

            for (size_t x = 0; x < ctx.w; ++x)
            {
                const uint8_t* srcpx = &ctx.data[(y * ctx.w + x) * ctx.channels];
                write_pixel(rgb_ord, alpha_ord, expand_mono, srcpx);
            }

            write(reinterpret_cast<const uint8_t*>(&zero), scan_line_padding);
        }

        flush();
    }

};

} // namespace img
} // namespace vx