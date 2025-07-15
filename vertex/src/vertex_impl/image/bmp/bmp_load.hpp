#pragma once

#include "../load_context.hpp"

namespace vx {
namespace img {
namespace bmp {

///////////////////////////////////////////////////////////////////////////////
// Microsoft/Windows BMP image
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// load BMP
///////////////////////////////////////////////////////////////////////////////

static bool test_raw(load_context& ctx)
{
    if (ctx.read_8() != 'B' || ctx.read_8() != 'M')
    {
        return false;
    }

    ctx.read_32_le(); // discard filesize
    ctx.read_16_le(); // discard reserved
    ctx.read_16_le(); // discard reserved
    ctx.read_32_le(); // discard data offset

    // check size of dib header
    const uint32_t hsz = ctx.read_32_le();
    return (hsz == 12 || hsz == 40 || hsz == 56 || hsz == 108 || hsz == 124);
}

static bool test(load_context& ctx)
{
    const bool r = test_raw(ctx);
    ctx.rewind_buffer();
    return r;
}

struct bmp_data
{
    uint32_t bpp, offset, hsz;
    uint32_t mr, mg, mb, ma;
    uint8_t all_a;
    size_t extra_read;
    bool flip_vertically;
};

static int high_bit(uint32_t z)
{
    int n = 0;
    if (z == 0) return -1;
    if (z >= 0x10000) { n += 16; z >>= 16; }
    if (z >= 0x00100) { n += 8; z >>= 8; }
    if (z >= 0x00010) { n += 4; z >>= 4; }
    if (z >= 0x00004) { n += 2; z >>= 2; }
    if (z >= 0x00002) { n += 1;/* >>=  1;*/ }
    return n;
}

static int bitcount(uint32_t a)
{
    a = (a & 0x55555555) + ((a >> 1) & 0x55555555); // max 2
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333); // max 4
    a = (a + (a >> 4)) & 0x0F0F0F0F; // max 8 per 4, now 8 bits
    a = (a + (a >> 8)); // max 16 per 8 bits
    a = (a + (a >> 16)); // max 32 per 8 bits
    return a & 0xFF;
}

// Extracts a value of N bits (where N = bits) from 'v' shifted by 'shift' bits,
// and scales it up to a full 8-bit value using bit-replication (approximate normalization).
static uint8_t shiftsigned(uint32_t v, uint32_t shift, uint32_t bits)
{
    // Multiplier lookup table for bit-replication to 8 bits
    // Indexed by number of bits (1..8). For example:
    //  - 5 bits: 00011111 -> 00011111 * 0000100001 (0x21) >> 2 ~= 255
    static constexpr uint32_t mul_table[9] = {
        0x00,       // bits = 0 (invalid, returns 0)
        0xFF,       // bits = 1 (1-bit replicated: 1 -> 255)
        0x55,       // bits = 2 (2-bit replicated: 3 -> 255)
        0x49,       // bits = 3
        0x11,       // bits = 4
        0x21,       // bits = 5
        0x41,       // bits = 6
        0x81,       // bits = 7
        0x01        // bits = 8 (no replication needed)
    };

    // Right-shift amounts for normalization after multiplying
    static constexpr uint32_t shift_table[9] = {
        0,  // bits = 0
        0,  // bits = 1
        0,  // bits = 2
        1,  // bits = 3
        0,  // bits = 4
        2,  // bits = 5
        4,  // bits = 6
        6,  // bits = 7
        0   // bits = 8
    };

    // Shift v into position to isolate the desired bits
    if (shift < 0)
    {
        v <<= -shift;
    }
    else
    {
        v >>= shift;
    }

    // Only keep the topmost 8 bits (the shifted value should fit here)
    v &= 0xFF;

    // Align to the lower bits by dropping any top padding
    v >>= (8 - bits);

    // Sanity check for valid range
    VX_ASSERT(bits >= 0 && bits <= 8);
    VX_ASSERT(v < 256); // after shifting and masking

    // Scale v from N-bit range to 0–255 using the bit-replication trick
    return static_cast<uint8_t>((v * mul_table[bits]) >> shift_table[bits]);
}


static bool set_mask_defaults(bmp_data& info, uint32_t compress)
{
    // BI_BITFIELDS specifies masks explicitly, don't override
    if (compress == 3)
    {
        return 1;
    }

    if (compress == 0)
    {
        if (info.bpp == 16)
        {
            info.mr = 31 << 10;
            info.mg = 31 << 5;
            info.mb = 31 << 0;
        }
        else if (info.bpp == 32)
        {
            info.mr = 0xff << 16;
            info.mg = 0xff << 8;
            info.mb = 0xff << 0;
            info.ma = 0xff << 24;
            // if all_a is 0 at end, then we loaded alpha channel but it was all 0
            info.all_a = 0;
        }
        else
        {
            // otherwise, use defaults, which is all-0
            info.mr = info.mg = info.mb = info.ma = 0;
        }

        return true;
    }

    // error
    return false;
}

static bool parse_header(load_context& ctx, bmp_data& info)
{
    // check for BM magic number
    if (ctx.read_8() != 'B' || ctx.read_8() != 'M')
    {
        ctx.err = load_error::BAD_BMP;
        return false;
    }

    ctx.read_32_le(); // discard filesize
    ctx.read_16_le(); // discard reserved
    ctx.read_16_le(); // discard reserved

    info.mr = info.mg = info.mb = info.ma = 0;
    info.extra_read = 14;

    // find where image data starts
    info.offset = ctx.read_32_le();
    if (info.offset < 0)
    {
        ctx.err = load_error::BAD_BMP;
        return false;
    }

    // size of dib header
    info.hsz = ctx.read_32_le();
    if (info.hsz != 12 &&
        info.hsz != 40 &&
        info.hsz != 56 &&
        info.hsz != 108 &&
        info.hsz != 124)
    {
        ctx.err = load_error::BAD_BMP;
        return false;
    }

    // extract the image dimensions
    if (info.hsz == 12)
    {
        // BITMAPCOREHEADER (older format) – width and height are 16-bit unsigned,
        // but height is still considered signed to determine orientation
        ctx.w = ctx.read_16_le();
        const int16_t height = static_cast<int16_t>(ctx.read_16_le());
        info.flip_vertically = (height < 0);
        ctx.h = static_cast<size_t>(info.flip_vertically ? -height : height);
    }
    else
    {
        // Modern DIB headers – width and height are 32-bit,
        // and height is signed to indicate orientation
        ctx.w = ctx.read_32_le();
        const int32_t height = static_cast<int32_t>(ctx.read_32_le());
        info.flip_vertically = (height < 0);
        ctx.h = static_cast<size_t>(info.flip_vertically ? -height : height);
    }

    // number of plains (always 1)
    if (ctx.read_16_le() != 1)
    {
        ctx.err = load_error::BAD_BMP;
        return false;
    }

    // extract bits per pixel
    info.bpp = ctx.read_16_le();

    if (info.hsz != 12)
    {
        // parse modern DIB header
        const uint32_t compress = ctx.read_32_le();

        if (compress == 1 || compress == 2)
        {
            // RLE compression
            ctx.err = load_error::BAD_BMP;
            ctx.msg = "BMP type not supported: RLE";
            return false;
        }
        if (compress >= 4)
        {
            // compressed with PNG/JPEG 
            ctx.err = load_error::BAD_BMP;
            ctx.msg = "BMP unsupported compression";
            return false;
        }
        if (compress == 3 && info.bpp != 16 && info.bpp != 32)
        {
            // BI_BITFIELDS (bitfields requires 16 or 32 bits/pixel)
            ctx.err = load_error::BAD_BMP;
            return false;
        }

        ctx.read_32_le(); // discard sizeof
        ctx.read_32_le(); // discard hres
        ctx.read_32_le(); // discard vres
        ctx.read_32_le(); // discard colorsused
        ctx.read_32_le(); // discard max important

        if (info.hsz == 40 || info.hsz == 56)
        {
            if (info.hsz == 56)
            {
                ctx.read_32_le();
                ctx.read_32_le();
                ctx.read_32_le();
                ctx.read_32_le();
            }

            if (info.bpp == 16 || info.bpp == 32)
            {
                // (BITMAPINFOHEADER, BITMAPV2HEADER)

                if (compress == 0)
                {
                    set_mask_defaults(info, compress);
                }
                else if (compress == 3)
                {
                    // BI_BITFIELDS

                    info.mr = ctx.read_32_le();
                    info.mg = ctx.read_32_le();
                    info.mb = ctx.read_32_le();
                    info.extra_read += 12;

                    if (info.mr == info.mg && info.mg == info.mb)
                    {
                        // not documented, but generated by photoshop and handled by mspaint
                        ctx.err = load_error::BAD_BMP;
                        return false;
                    }
                }
                else
                {
                    ctx.err = load_error::BAD_BMP;
                    return false;
                }
            }
        }
        else
        {
            // V4/V5 header (BITMAPCOREHEADER) 
            if (info.hsz != 108 && info.hsz != 124)
            {
                ctx.err = load_error::BAD_BMP;
                return false;
            }

            info.mr = ctx.read_32_le();
            info.mg = ctx.read_32_le();
            info.mb = ctx.read_32_le();
            info.ma = ctx.read_32_le();

            if (compress != 3)
            {
                // override mr/mg/mb unless in BI_BITFIELDS mode
                set_mask_defaults(info, compress);
            }

            ctx.read_32_le(); // discard color space

            for (int i = 0; i < 12; ++i)
            {
                ctx.read_32_le(); // discard color space parameters
            }

            if (info.hsz == 124)
            {
                // V5 header
                ctx.read_32_le(); // discard rendering intent
                ctx.read_32_le(); // discard offset of profile data
                ctx.read_32_le(); // discard size of profile data
                ctx.read_32_le(); // discard reserved
            }
        }
    }

    return true;
}

static bool load(load_context& ctx)
{
    bmp_data info{};

    info.all_a = 255;
    if (!parse_header(ctx, info))
    {
        // error code already set
        return false;
    }

    if (ctx.w > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || ctx.h > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        ctx.err = load_error::MAX_SIZE;
        return false;
    }

    size_t palette_size = 0;

    if (info.hsz == 12)
    {
        if (info.bpp < 24)
        {
            palette_size = (info.offset - info.extra_read - 24) / 3;
        }
    }
    else
    {
        if (info.bpp < 16)
        {
            palette_size = (info.offset - info.extra_read - info.hsz) >> 2;
        }
    }

    if (palette_size == 0)
    {
        // accept some number of extra bytes after the header, but if the offset points either to before
        // the header ends or implies a large amount of extra data, reject the file as malformed

        const size_t bytes_read_so_far = ctx.ptr - ctx.buffer;
        const size_t header_limit = 1024; // max we actually read is below 256 bytes currently.
        const size_t extra_data_limit = 256ull * 4; // what ordinarily goes here is a palette; 256 entries*4 bytes is its max size.

        if (bytes_read_so_far <= 0 || bytes_read_so_far > header_limit)
        {
            ctx.err = load_error::BAD_BMP;
            return false;
        }

        // we established that bytes_read_so_far is positive and sensible.
        // the first half of this test rejects offsets that are either too small positives, or
        // negative, and guarantees that info.offset >= bytes_read_so_far > 0. this in turn
        // ensures the number computed in the second half of the test can't overflow.

        if (info.offset < bytes_read_so_far || info.offset - bytes_read_so_far > extra_data_limit)
        {
            ctx.err = load_error::BAD_BMP;
            return false;
        }
        else
        {
            // Advance the pointer to the start of pixel data (skip extra palette or other data)
            ctx.skip(info.offset - bytes_read_so_far);
        }
    }

    if (info.bpp == 24 && info.ma == 0xff000000)
    {
        ctx.channels = 3;
    }
    else
    {
        ctx.channels = info.ma ? 4 : 3;
    }

    // sanity-check size
    if (will_multiply_overflow(ctx.w, ctx.h, ctx.channels))
    {
        ctx.err = load_error::MAX_SIZE;
        return false;
    }

    // create the output buffer
    ctx.out.reset(new byte_type[ctx.output_size()]);

    if (info.bpp < 16)
    {
        if (palette_size == 0 || palette_size > 256)
        {
            ctx.err = load_error::BAD_BMP;
            ctx.msg = "bad palette size";
            return false;
        }

        // build the palette

        byte_type palette[256][3];
        for (size_t i = 0; i < palette_size; ++i)
        {
            palette[i][2] = ctx.read_8();
            palette[i][1] = ctx.read_8();
            palette[i][0] = ctx.read_8();

            if (info.hsz != 12)
            {
                // not BITMAPCOREHEADER, skip padding or reserved alpha
                ctx.read_8();
            }
        }

        ctx.skip(info.offset - info.extra_read - info.hsz - palette_size * (info.hsz == 12 ? 3 : 4));

        // Number of bytes per row of pixel data before padding
        int32_t width = 0;

        // Compute the number of bytes needed for a single scanline based on bits per pixel.
        // BMP scanlines are packed, so pixel count must be converted into byte count.
        // These formats all use palette indices, so we only need to account for packed indices here.

        if (info.bpp == 1)
        {
            // 1 bit per pixel = 8 pixels per byte
            // (ctx.w + 7) ensures we round up (e.g. 9 pixels -> 2 bytes)
            width = (ctx.w + 7) >> 3;
        }
        else if (info.bpp == 4)
        {
            // 4 bits per pixel = 2 pixels per byte
            // (ctx.w + 1) ensures we round up for odd widths (e.g. 3 pixels -> 2 bytes)
            width = (ctx.w + 1) >> 1;
        }
        else if (info.bpp == 8)
        {
            // 8 bits per pixel = 1 pixel per byte
            // No rounding needed, just use the width as-is
            width = ctx.w;
        }
        else
        {
            ctx.err = load_error::BAD_BMP;
            ctx.msg = "bad bpp";
            return false;
        }

        // Compute the number of padding bytes at the end of each scanline.
        // BMP format requires each row to be aligned to a 4-byte boundary.
        // This bitwise trick gives the number of bytes to add to `width` to reach next multiple of 4.
        // Equivalent to: pad = (4 - (width % 4)) % 4;
        const int32_t pad = (-width) & 3;

        if (info.bpp == 1)
        {
            // Output pixel buffer index
            size_t z = 0;

            for (size_t y = 0; y < ctx.h; ++y)
            {
                uint8_t bit_offset = 7;
                uint8_t v = 0;

                for (size_t x = 0; x < ctx.w; ++x)
                {
                    // Load next byte if we’re at the start of a byte or just starting the row
                    if (bit_offset == 7)
                    {
                        v = ctx.read_8();
                    }

                    const size_t color = (v >> bit_offset) & 0x01;

                    ctx.out[z++] = palette[color][0]; // R
                    ctx.out[z++] = palette[color][1]; // G
                    ctx.out[z++] = palette[color][2]; // B
                    if (ctx.channels == 4)
                    {
                        ctx.out[z++] = 255; // A
                    }

                    // Shift to the next bit
                    if (bit_offset == 0)
                    {
                        bit_offset = 7;
                    }
                    else
                    {
                        --bit_offset;
                    }
                }

                ctx.skip(pad);
            }
        }
        else // (info.bpp == 4 || info.bpp == 8)
        {
            size_t z = 0; // Output buffer index

            for (size_t y = 0; y < ctx.h; ++y)
            {
                for (size_t x = 0; x < ctx.w;)
                {
                    const uint8_t v = ctx.read_8();

                    if (info.bpp == 8)
                    {
                        // 8bpp = one byte per pixel
                        const size_t color = v;

                        ctx.out[z++] = palette[color][0]; // R
                        ctx.out[z++] = palette[color][1]; // G
                        ctx.out[z++] = palette[color][2]; // B
                        if (ctx.channels == 4)
                        {
                            ctx.out[z++] = 255;
                        }

                        ++x;
                    }
                    else
                    {
                        // 4bpp = two pixels per byte (high nibble, then low nibble)

                        // First pixel (high nibble)
                        const size_t hi = (v >> 4) & 0x0F;
                        ctx.out[z++] = palette[hi][0];
                        ctx.out[z++] = palette[hi][1];
                        ctx.out[z++] = palette[hi][2];
                        if (ctx.channels == 4)
                        {
                            ctx.out[z++] = 255;
                        }

                        ++x;

                        // Second pixel (low nibble), only if we haven't reached width
                        if (x < ctx.w)
                        {
                            const size_t lo = v & 0x0F;
                            ctx.out[z++] = palette[lo][0];
                            ctx.out[z++] = palette[lo][1];
                            ctx.out[z++] = palette[lo][2];
                            if (ctx.channels == 4)
                            {
                                ctx.out[z++] = 255;
                            }

                            ++x;
                        }
                    }
                }

                ctx.skip(pad); // Skip padding to 4-byte row alignment
            }
        }
    }
    else // (info.bpp >= 16)
    {
        // index for pixel data being written
        size_t z = 0;

        ctx.skip(info.offset - info.extra_read - info.hsz);

        // Number of bytes per row of pixel data before padding
        int32_t width = 0;

        // Compute the number of bytes needed for a single scanline based on bits per pixel.
        // BMP scanlines are packed, so pixel count must be converted into byte count.
        // These formats all use palette indices, so we only need to account for packed indices here.

        if (info.bpp == 24)
        {
            // 24 bits per pixel = 3 bytes per pixel (RGB)
            // So total bytes per row = width in pixels * 3
            width = 3 * ctx.w;
        }
        else if (info.bpp == 16)
        {
            // 16 bits per pixel = 2 bytes per pixel (e.g., RGB555 or RGB565)
            width = 2 * ctx.w;
        }
        else
        {
            // bpp = 32 and pad = 0
        }

        const int32_t pad = (-width) & 3;

        enum packed_format { BGR8, BGRA8, UNKNOWN };
        packed_format format = UNKNOWN;

        if (info.bpp == 24)
        {
            // 24-bit BMPs store 3 bytes per pixel: Blue, Green, Red
            format = BGR8;
        }
        else if (info.bpp == 32)
        {
            // 32-bit BMPs may include alpha; check if bitmasks match BGRA layout
            if (info.mb == 0x000000ff &&
                info.mg == 0x0000ff00 &&
                info.mr == 0x00ff0000 &&
                info.ma == 0xff000000)
            {
                format = BGRA8;
            }
        }

        // If not a known format, fallback to custom bitmask decoding
        if (format == UNKNOWN)
        {
            if (!info.mr || !info.mg || !info.mb)
            {
                ctx.err = load_error::BAD_BMP;
                ctx.msg = "bad masks";
                return false;
            }

            // Calculate right-shift amounts to align each mask's highest bit to bit 7 (MSB of 8-bit)
            const uint32_t rshift = high_bit(info.mr) - 7;
            const uint32_t gshift = high_bit(info.mg) - 7;
            const uint32_t bshift = high_bit(info.mb) - 7;
            const uint32_t ashift = high_bit(info.ma) - 7;

            // Count the number of bits used in each color channel
            const uint32_t rcount = bitcount(info.mr);
            const uint32_t gcount = bitcount(info.mg);
            const uint32_t bcount = bitcount(info.mb);
            const uint32_t acount = bitcount(info.ma);

            // Ensure no channel uses more than 8 bits (we only output 8-bit channels)
            if (rcount > 8 || gcount > 8 || bcount > 8 || acount > 8)
            {
                ctx.err = load_error::BAD_BMP;
                ctx.msg = "bad masks";
                return false;
            }

            for (size_t y = 0; y < ctx.h; ++y)
            {
                for (size_t x = 0; x < ctx.w; ++x)
                {
                    // Read the raw packed pixel value (16 or 32 bits)
                    const uint32_t v = (info.bpp == 16 ? ctx.read_16_le() : ctx.read_32_le());

                    // Extract and normalize each color channel to 8 bits using shiftsigned()
                    ctx.out[z++] = shiftsigned(v & info.mr, rshift, rcount); // Red
                    ctx.out[z++] = shiftsigned(v & info.mg, gshift, gcount); // Green
                    ctx.out[z++] = shiftsigned(v & info.mb, bshift, bcount); // Blue

                    const uint8_t a = info.ma ? shiftsigned(v & info.ma, ashift, acount) : 0xFF;
                    if (ctx.channels == 4)
                    {
                        ctx.out[z++] = a;
                    }

                    info.all_a |= a;
                }

                ctx.skip(pad);
            }
        }
        else
        {
            for (size_t y = 0; y < ctx.h; ++y)
            {
                for (size_t x = 0; x < ctx.w; ++x)
                {
                    // Read B, G, R in that order
                    ctx.out[z + 2] = ctx.read_8(); // Red
                    ctx.out[z + 1] = ctx.read_8(); // Green
                    ctx.out[z + 0] = ctx.read_8(); // Blue
                    z += 3;

                    // Read alpha if it's a 32-bit BGRA format
                    const uint8_t a = (format == BGRA8) ? ctx.read_8() : 0xFF;
                    if (ctx.channels == 4)
                    {
                        ctx.out[z++] = a;
                    }

                    info.all_a |= a;
                }

                ctx.skip(pad);
            }
        }
    }

    // if alpha channel is all 0s, replace with all 255s
    if (ctx.channels == 4 && info.all_a == 0)
    {
        for (size_t i = ctx.channels; i < ctx.output_size(); i += ctx.channels)
        {
            ctx.out[i] = 0xFF;
        }
    }

    if (info.flip_vertically)
    {
        const size_t rowsz = ctx.w * ctx.channels;

        for (size_t y = 0; y < ctx.h / 2; ++y)
        {
            byte_type* row1 = &ctx.out[y * rowsz];
            byte_type* row2 = &ctx.out[(ctx.h - y - 1) * rowsz];

            for (size_t x = 0; x < ctx.w * ctx.channels; ++x)
            {
                const byte_type temp = row1[x];
                row1[x] = row2[x];
                row2[x] = temp;
            }
        }
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// write BMP
///////////////////////////////////////////////////////////////////////////////

} // namespace bmp
} // namespace img
} // namespace vx