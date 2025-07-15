#pragma once

#include "vertex_impl/image/write_context.hpp"

namespace vx {
namespace img {
namespace jpg {

///////////////////////////////////////////////////////////////////////////////
// JPEG/JIF
///////////////////////////////////////////////////////////////////////////////

static const uint8_t zigzag[] = {
    0, 1, 5, 6, 14, 15, 27, 28, 2, 4, 7, 13, 16, 26, 29, 42,
    3, 8, 12, 17, 25, 30, 41, 43, 9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54, 20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61, 35, 36, 48, 49, 57, 58, 62, 63
};

// Encodes a signed integer value into JPEG-compatible bit representation
// [0] contains the bit-encoded value (with sign handled per JPEG spec)
// [1] contains the number of bits required to encode the value
static void calc_bits(int val, uint16_t bits[2])
{
    // Compute the absolute value to determine the bit length (category)
    int abs_val = val < 0 ? -val : val;

    // find the number of bits set in the value
    // This is the JPEG "category" (bit length)
    bits[1] = 1;
    while (abs_val >>= 1)
    {
        ++bits[1];
    }

    // Mask to keep only the lowest n bits of the encoded value
    val = val < 0 ? val - 1 : val;
    bits[0] = val & ((1 << bits[1]) - 1);
}

static void write_bits(write_context& ctx, int* bit_buffer_ptr, int* bit_count_ptr, const uint16_t* bit_spec)
{
    int bit_buffer = *bit_buffer_ptr;
    int bit_count = *bit_count_ptr;

    // Add the new bits to the buffer.
    bit_count += bit_spec[1]; // Number of bits.
    bit_buffer |= static_cast<int>(bit_spec[0]) << (24 - bit_count); // Insert bits at the top.

    // Write out 8 bits at a time (from the top).
    while (bit_count >= 8)
    {
        const uint8_t byte_out = static_cast<uint8_t>((bit_buffer >> 16) & 0xFF);
        ctx.write_byte(byte_out);

        // Special JPEG case: byte stuffing after 0xFF
        if (byte_out == 0xFF)
        {
            ctx.write_byte(0x00);
        }

        bit_buffer <<= 8;
        bit_count -= 8;
    }

    *bit_buffer_ptr = bit_buffer;
    *bit_count_ptr = bit_count;
}

// Performs a 1D 8-point DCT on 8 float inputs, in-place.
// This is the core DCT kernel used in JPEG encoding.
static void descrete_cosine_transform(
    float& d0, float& d1, float& d2, float& d3,
    float& d4, float& d5, float& d6, float& d7
)
{
    constexpr float c4 = 0.707106781f;   // cos(pi/4)
    constexpr float c2 = 1.306562965f;   // cos(pi/8)*sqrt(2)
    constexpr float c6 = 0.382683433f;   // sin(pi/8)*sqrt(2)
    constexpr float c2_minus_c6 = 0.541196100f;
    constexpr float c2_plus_c6 = 1.306562965f;

    const float tmp0 = d0 + d7;
    const float tmp7 = d0 - d7;
    const float tmp1 = d1 + d6;
    const float tmp6 = d1 - d6;
    const float tmp2 = d2 + d5;
    const float tmp5 = d2 - d5;
    const float tmp3 = d3 + d4;
    const float tmp4 = d3 - d4;

    // Even part
    const float tmp10 = tmp0 + tmp3;
    const float tmp11 = tmp1 + tmp2;
    const float tmp12 = tmp1 - tmp2;
    const float tmp13 = tmp0 - tmp3;

    const float z1 = (tmp12 + tmp13) * c4;

    d0 = tmp10 + tmp11;
    d4 = tmp10 - tmp11;
    d2 = tmp13 + z1;
    d6 = tmp13 - z1;

    // Odd part
    const float tmp45 = tmp4 + tmp5;
    const float tmp56 = tmp5 + tmp6;
    const float tmp67 = tmp6 + tmp7;

    const float z5 = (tmp45 - tmp67) * c6;
    const float z2 = tmp45 * c2_minus_c6 + z5;
    const float z4 = tmp67 * c2_plus_c6 + z5;
    const float z3 = tmp56 * c4;

    const float z11 = tmp7 + z3;
    const float z13 = tmp7 - z3;

    d5 = z13 + z2;
    d3 = z13 - z2;
    d1 = z11 + z4;
    d7 = z11 - z4;
}

static bool encode_data_unit_block(
    write_context& ctx,
    int& bit_buffer, int& bit_count,
    float* cdu, int du_stride, float* fdtbl, int dc_prev,
    const uint16_t htdc[256][2], const uint16_t htac[256][2]
)
{
    // Predefined Huffman codes for special symbols:
    // EOB (End Of Block) indicates no more non-zero AC coefficients.
    const uint16_t eob[2] = { htac[0x00][0], htac[0x00][1] };
    // Special run-length symbol for 16 consecutive zero AC coefficients.
    const uint16_t m16zeroes[2] = { htac[0xF0][0], htac[0xF0][1] };

    int du[64]; // Holds quantized and zigzag reordered coefficients

    // Step 1: Perform 1D DCT on each row of the 8x8 block
    for (int offset = 0, stride_8 = du_stride * 8; offset < stride_8; offset += du_stride)
    {
        descrete_cosine_transform(
            cdu[offset + 0], cdu[offset + 1], cdu[offset + 2], cdu[offset + 3],
            cdu[offset + 4], cdu[offset + 5], cdu[offset + 6], cdu[offset + 7]
        );
    }

    // Step 2: Perform 1D DCT on each column of the 8x8 block (completes 2D DCT)
    for (int col = 0; col < 8; ++col)
    {
        descrete_cosine_transform(
            cdu[col + du_stride * 0], cdu[col + du_stride * 1],
            cdu[col + du_stride * 2], cdu[col + du_stride * 3],
            cdu[col + du_stride * 4], cdu[col + du_stride * 5],
            cdu[col + du_stride * 6], cdu[col + du_stride * 7]
        );
    }

    // Step 3: Quantize the DCT coefficients, round, and apply zigzag reorder
    // Quantization reduces precision of high-frequency coefficients.
    // Zigzag reordering arranges coefficients from low to high frequency,
    // grouping zeros for efficient run-length encoding.
    for (int y = 0, j = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x, ++j)
        {
            const int i = y * du_stride + x;
            const float scaled = cdu[i] * fdtbl[j];
            // Round with proper handling for negative values
            du[zigzag[j]] = static_cast<int>(scaled < 0 ? scaled - 0.5f : scaled + 0.5f);
        }
    }

    // Step 4: Encode DC coefficient difference from previous block's DC
    // DC coefficients typically change slowly between blocks,
    // so encoding the difference is more efficient.
    int diff = du[0] - dc_prev;
    if (diff == 0)
    {
        // If no difference, write zero category Huffman code
        write_bits(ctx, &bit_buffer, &bit_count, htdc[0]);
    }
    else
    {
        uint16_t bits[2];
        calc_bits(diff, bits); // Calculate bit representation of diff
        write_bits(ctx, &bit_buffer, &bit_count, htdc[bits[1]]); // Write Huffman code for category
        write_bits(ctx, &bit_buffer, &bit_count, bits);          // Write actual bits representing diff
    }

    // Step 5: Find position of last non-zero AC coefficient (end-of-block)
    int end_pos = 63;
    while (end_pos > 0 && du[end_pos] == 0)
    {
        --end_pos;
    }

    // If no AC coefficients are non-zero, write End-Of-Block symbol and return
    if (end_pos == 0)
    {
        write_bits(ctx, &bit_buffer, &bit_count, eob);
        return du[0];
    }

    // Step 6: Encode AC coefficients using run-length and Huffman coding
    for (int i = 1; i <= end_pos; ++i)
    {
        int start = i;

        // Count consecutive zero AC coefficients (run length)
        while (i <= end_pos && du[i] == 0)
        {
            ++i;
        }

        int zero_run = i - start;

        // For runs of 16 or more zeros, write special 16-zero run marker repeatedly
        if (zero_run >= 16)
        {
            const int full_zero_blocks = zero_run >> 4; // number of 16-zero blocks
            for (int z = 0; z < full_zero_blocks; ++z)
            {
                write_bits(ctx, &bit_buffer, &bit_count, m16zeroes);
            }
            zero_run &= 0x0F; // remainder zeros after full blocks
        }

        uint16_t bits[2];
        calc_bits(du[i], bits);  // Calculate bit representation of AC coefficient
        const int symbol = (zero_run << 4) | bits[1]; // Combine run length and bit length category

        write_bits(ctx, &bit_buffer, &bit_count, htac[symbol]); // Huffman encode run-length + category
        write_bits(ctx, &bit_buffer, &bit_count, bits); // Write bits for AC coefficient
    }

    // Step 7: If block not fully encoded, write End-Of-Block symbol to finish
    if (end_pos != 63)
    {
        write_bits(ctx, &bit_buffer, &bit_count, eob);
    }

    // Return current DC coefficient for next block's differential encoding
    return du[0];
}

static bool write_jpg_core(write_context& ctx, uint8_t quality)
{
    // Constants that don't pollute global namespace
    static constexpr uint8_t std_dc_luminance_nrcodes[] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
    static constexpr uint8_t std_dc_luminance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr uint8_t std_ac_luminance_nrcodes[] = { 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D };
    static constexpr uint8_t std_ac_luminance_values[] = {
       0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 
       0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28, 
       0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 
       0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 
       0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 
       0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 
       0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
    };

    static constexpr uint8_t std_dc_chrominance_nrcodes[] = { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
    static constexpr uint8_t std_dc_chrominance_values[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    static constexpr uint8_t std_ac_chrominance_nrcodes[] = { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
    static constexpr uint8_t std_ac_chrominance_values[] = {
       0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 
       0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 
       0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 
       0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
       0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 
       0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 
       0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
    };

    // Huffman tables
    static constexpr uint16_t ydc_ht[256][2] = { { 0, 2 }, { 2, 3 }, { 3, 3 }, { 4, 3 }, { 5, 3 }, { 6, 3 }, { 14, 4 }, { 30, 5 }, { 62, 6 }, { 126, 7 }, { 254, 8 }, { 510, 9 } };
    static constexpr uint16_t uvdc_ht[256][2] = { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 6, 3 }, { 14, 4 }, { 30, 5 }, { 62, 6 }, { 126, 7 }, { 254, 8 }, { 510, 9 }, { 1022, 10 }, { 2046, 11 } };
    static constexpr uint16_t yac_ht[256][2] = {
        { 10, 4 }, { 0, 2 }, { 1, 2 }, { 4, 3 }, { 11, 4 }, { 26, 5 }, { 120, 7 }, { 248, 8 }, { 1014, 10 }, { 65410, 16 }, { 65411, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, {0, 0}, 
        { 12, 4 }, { 27, 5 }, { 121, 7 }, { 502, 9 }, { 2038, 11 }, { 65412, 16 }, { 65413, 16 }, { 65414, 16 }, { 65415, 16 }, { 65416, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 28, 5 }, { 249, 8 }, { 1015, 10 }, { 4084, 12 }, { 65417, 16 }, { 65418, 16 }, { 65419, 16 }, { 65420, 16 }, { 65421, 16 }, { 65422, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 58, 6 }, { 503, 9 }, { 4085, 12 }, { 65423, 16 }, { 65424, 16 }, { 65425, 16 }, { 65426, 16 }, { 65427, 16 }, { 65428, 16 }, { 65429, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 59, 6 }, { 1016, 10 }, { 65430, 16 }, { 65431, 16 }, { 65432, 16 }, { 65433, 16 }, { 65434, 16 }, { 65435, 16 }, { 65436, 16 }, { 65437, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 122, 7 }, { 2039, 11 }, { 65438, 16 }, { 65439, 16 }, { 65440, 16 }, { 65441, 16 }, { 65442, 16 }, { 65443, 16 }, { 65444, 16 }, { 65445, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 123, 7 }, { 4086, 12 }, { 65446, 16 }, { 65447, 16 }, { 65448, 16 }, { 65449, 16 }, { 65450, 16 }, { 65451, 16 }, { 65452, 16 }, { 65453, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 250, 8 }, { 4087, 12 }, { 65454, 16 }, { 65455, 16 }, { 65456, 16 }, { 65457, 16 }, { 65458, 16 }, { 65459, 16 }, { 65460, 16 }, { 65461, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 504, 9 }, { 32704, 15 }, { 65462, 16 }, { 65463, 16 }, { 65464, 16 }, { 65465, 16 }, { 65466, 16 }, { 65467, 16 }, { 65468, 16 }, { 65469, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 505, 9 }, { 65470, 16 }, { 65471, 16 }, { 65472, 16 }, { 65473, 16 }, { 65474, 16 }, { 65475, 16 }, { 65476, 16 }, { 65477, 16 }, { 65478, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 506, 9 }, { 65479, 16 }, { 65480, 16 }, { 65481, 16 }, { 65482, 16 }, { 65483, 16 }, { 65484, 16 }, { 65485, 16 }, { 65486, 16 }, { 65487, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 1017, 10 }, { 65488, 16 }, { 65489, 16 }, { 65490, 16 }, { 65491, 16 }, { 65492, 16 }, { 65493, 16 }, { 65494, 16 }, { 65495, 16 }, { 65496, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 1018, 10 }, { 65497, 16 }, { 65498, 16 }, { 65499, 16 }, { 65500, 16 }, { 65501, 16 }, { 65502, 16 }, { 65503, 16 }, { 65504, 16 }, { 65505, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 2040, 11 }, { 65506, 16 }, { 65507, 16 }, { 65508, 16 }, { 65509, 16 }, { 65510, 16 }, { 65511, 16 }, { 65512, 16 }, { 65513, 16 }, { 65514, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 65515, 16 }, { 65516, 16 }, { 65517, 16 }, { 65518, 16 }, { 65519, 16 }, { 65520, 16 }, { 65521, 16 }, { 65522, 16 }, { 65523, 16 }, { 65524, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 2041, 11 }, { 65525, 16 }, { 65526, 16 }, { 65527, 16 }, { 65528, 16 }, { 65529, 16 }, { 65530, 16 }, { 65531, 16 }, { 65532, 16 }, { 65533, 16 }, { 65534, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    };
    static constexpr uint16_t uvac_ht[256][2] = {
        { 0, 2 }, { 1, 2 }, { 4, 3 }, { 10, 4 }, { 24, 5 }, { 25, 5 }, { 56, 6 }, { 120, 7 }, { 500, 9 }, { 1014, 10 }, { 4084, 12 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 11, 4 }, { 57, 6 }, { 246, 8 }, { 501, 9 }, { 2038, 11 }, { 4085, 12 }, { 65416, 16 }, { 65417, 16 }, { 65418, 16 }, { 65419, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 26, 5 }, { 247, 8 }, { 1015, 10 }, { 4086, 12 }, { 32706, 15 }, { 65420, 16 }, { 65421, 16 }, { 65422, 16 }, { 65423, 16 }, { 65424, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 27, 5 }, { 248, 8 }, { 1016, 10 }, { 4087, 12 }, { 65425, 16 }, { 65426, 16 }, { 65427, 16 }, { 65428, 16 }, { 65429, 16 }, { 65430, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 58, 6 }, { 502, 9 }, { 65431, 16 }, { 65432, 16 }, { 65433, 16 }, { 65434, 16 }, { 65435, 16 }, { 65436, 16 }, { 65437, 16 }, { 65438, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 59, 6 }, { 1017, 10 }, { 65439, 16 }, { 65440, 16 }, { 65441, 16 }, { 65442, 16 }, { 65443, 16 }, { 65444, 16 }, { 65445, 16 }, { 65446, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 121, 7 }, { 2039, 11 }, { 65447, 16 }, { 65448, 16 }, { 65449, 16 }, { 65450, 16 }, { 65451, 16 }, { 65452, 16 }, { 65453, 16 }, { 65454, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 122, 7 }, { 2040, 11 }, { 65455, 16 }, { 65456, 16 }, { 65457, 16 }, { 65458, 16 }, { 65459, 16 }, { 65460, 16 }, { 65461, 16 }, { 65462, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 249, 8 }, { 65463, 16 }, { 65464, 16 }, { 65465, 16 }, { 65466, 16 }, { 65467, 16 }, { 65468, 16 }, { 65469, 16 }, { 65470, 16 }, { 65471, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 503, 9 }, { 65472, 16 }, { 65473, 16 }, { 65474, 16 }, { 65475, 16 }, { 65476, 16 }, { 65477, 16 }, { 65478, 16 }, { 65479, 16 }, { 65480, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 504, 9 }, { 65481, 16 }, { 65482, 16 }, { 65483, 16 }, { 65484, 16 }, { 65485, 16 }, { 65486, 16 }, { 65487, 16 }, { 65488, 16 }, { 65489, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 505, 9 }, { 65490, 16 }, { 65491, 16 }, { 65492, 16 }, { 65493, 16 }, { 65494, 16 }, { 65495, 16 }, { 65496, 16 }, { 65497, 16 }, { 65498, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 506, 9 }, { 65499, 16 }, { 65500, 16 }, { 65501, 16 }, { 65502, 16 }, { 65503, 16 }, { 65504, 16 }, { 65505, 16 }, { 65506, 16 }, { 65507, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 2041, 11 }, { 65508, 16 }, { 65509, 16 }, { 65510, 16 }, { 65511, 16 }, { 65512, 16 }, { 65513, 16 }, { 65514, 16 }, { 65515, 16 }, { 65516, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 16352, 14 }, { 65517, 16 }, { 65518, 16 }, { 65519, 16 }, { 65520, 16 }, { 65521, 16 }, { 65522, 16 }, { 65523, 16 }, { 65524, 16 }, { 65525, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 
        { 1018, 10 }, { 32707, 15 }, { 65526, 16 }, { 65527, 16 }, { 65528, 16 }, { 65529, 16 }, { 65530, 16 }, { 65531, 16 }, { 65532, 16 }, { 65533, 16 }, { 65534, 16 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }
    };
    static constexpr int yqt[] = {
        16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 
        37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
    };
    static constexpr int uvqt[] = {
        17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 
        99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
    };
    static constexpr float aasf[] = {
        1.0f * 2.828427125f, 1.387039845f * 2.828427125f, 1.306562965f * 2.828427125f, 1.175875602f * 2.828427125f,
        1.0f * 2.828427125f, 0.785694958f * 2.828427125f, 0.541196100f * 2.828427125f, 0.275899379f * 2.828427125f
    };

    const uint16_t w = static_cast<uint16_t>(ctx.w);
    const uint16_t h = static_cast<uint16_t>(ctx.h);

    quality = quality ? quality : 90;
    const bool subsample = (quality <= 90);

    // clamp quality to range 1-100
    quality = (quality > 100) ? 100 : quality;
    quality = quality < 50 ? 5000 / quality : 200 - quality * 2;

    // Quantization tables
    uint8_t y_table[64], uv_table[64];
    for (size_t i = 0; i < 64; ++i)
    {
        const int yti = (yqt[i] * quality + 50) / 100;
        y_table[zigzag[i]] = static_cast<uint8_t>(yti < 1 ? 1 : yti > 255 ? 255 : yti);

        const int uvti = (uvqt[i] * quality + 50) / 100;
        uv_table[zigzag[i]] = static_cast<uint8_t>(uvti < 1 ? 1 : uvti > 255 ? 255 : uvti);
    }

    // Precompute scaled quantization tables for DCT
    float fdtbl_y[64], fdtbl_uv[64];
    for (size_t row = 0, k = 0; row < 8; ++row)
    {
        for (size_t col = 0; col < 8; ++col, ++k)
        {
            fdtbl_y [k] = 1.0f / (y_table[zigzag[k]] * aasf[row] * aasf[col]);
            fdtbl_uv[k] = 1.0f / (uv_table[zigzag[k]] * aasf[row] * aasf[col]);
        }
    }

    // Write Headers
    {
        static constexpr uint8_t head0[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0, 0x10, 'J', 'F', 'I', 'F', 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0xFF, 0xDB, 0, 0x84, 0 };

        const uint8_t head1[] = {
            0xFF, 0xC0, 0, 0x11, 8,
            static_cast<uint8_t>(h >> 8), static_cast<uint8_t>(h & 0xFF),
            static_cast<uint8_t>(w >> 8), static_cast<uint8_t>(w & 0xFF),
            3, 1,
            static_cast<uint8_t>(subsample ? 0x22 : 0x11),
            0, 2, 0x11, 1, 3, 0x11, 1, 0xFF, 0xC4, 0x01, 0xA2, 0
        };

        static constexpr uint8_t head2[] = { 0xFF, 0xDA, 0, 0xC, 3, 1, 0, 2, 0x11, 3, 0x11, 0, 0x3F, 0 };

        ctx.write(head0, sizeof(head0));
        ctx.write(y_table, sizeof(y_table));

        ctx.write_byte(1);

        ctx.write(uv_table, sizeof(uv_table));
        ctx.write(head1, sizeof(head1));

        ctx.write((std_dc_luminance_nrcodes + 1), sizeof(std_dc_luminance_nrcodes) - 1);
        ctx.write(std_dc_luminance_values, sizeof(std_dc_luminance_values));
        ctx.write_byte(0x10); // HTYACinfo

        ctx.write((std_ac_luminance_nrcodes + 1), sizeof(std_ac_luminance_nrcodes) - 1);
        ctx.write(std_ac_luminance_values, sizeof(std_ac_luminance_values));
        ctx.write_byte(1); // HTUDCinfo

        ctx.write((std_dc_chrominance_nrcodes + 1), sizeof(std_dc_chrominance_nrcodes) - 1);
        ctx.write(std_dc_chrominance_values, sizeof(std_dc_chrominance_values));
        ctx.write_byte(0x11); // HTUACinfo

        ctx.write((std_ac_chrominance_nrcodes + 1), sizeof(std_ac_chrominance_nrcodes) - 1);
        ctx.write(std_ac_chrominance_values, sizeof(std_ac_chrominance_values));

        ctx.write(head2, sizeof(head2));
    }

    // Encode 8x8 macroblocks
    {
        static constexpr uint16_t fill_bits[] = { 0x7F, 7 };

        int dcy = 0, dcu = 0, dcv = 0;
        int bit_buffer = 0, bit_count = 0;

        // ctx.channels == 2 is grey+alpha (alpha is ignored)
        const int ofs_g = ctx.channels > 2 ? 1 : 0;
        const int ofs_b = ctx.channels > 2 ? 2 : 0;

        const uint8_t* data_r = ctx.data;
        const uint8_t* data_g = data_r + ofs_g;
        const uint8_t* data_b = data_r + ofs_b;

        if (subsample)
        {
            for (size_t y = 0; y < h; y += 16)
            {
                for (size_t x = 0; x < w; x += 16)
                {
                    float Y[256], U[256], V[256];

                    for (size_t row = y, pos = 0; row < y + 16; ++row)
                    {
                        // row >= height => use last input row
                        const size_t clamped_row = (row < h) ? row : static_cast<size_t>(h) - 1;
                        const size_t base_px = (static_cast<size_t>(h) - 1 - clamped_row) * w * ctx.channels;

                        for (size_t col = x; col < x + 16; ++col, ++pos)
                        {
                            // if col >= width => use pixel from last input column
                            const size_t px = base_px + ((col < w) ? col : (static_cast<size_t>(w) - 1)) * ctx.channels;

                            const float r = data_r[px];
                            const float g = data_g[px];
                            const float b = data_b[px];

                            Y[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
                            U[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
                            V[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
                        }
                    }

                    dcy = encode_data_unit_block(ctx, bit_buffer, bit_count, Y +   0, 16, fdtbl_y, dcy, ydc_ht, yac_ht);
                    dcy = encode_data_unit_block(ctx, bit_buffer, bit_count, Y +   8, 16, fdtbl_y, dcy, ydc_ht, yac_ht);
                    dcy = encode_data_unit_block(ctx, bit_buffer, bit_count, Y + 128, 16, fdtbl_y, dcy, ydc_ht, yac_ht);
                    dcy = encode_data_unit_block(ctx, bit_buffer, bit_count, Y + 136, 16, fdtbl_y, dcy, ydc_ht, yac_ht);

                    // subsample U, V
                    {
                        float sub_u[64], sub_v[64];

                        for (size_t yy = 0, pos = 0; yy < 8; ++yy)
                        {
                            for (size_t xx = 0; xx < 8; ++xx, ++pos)
                            {
                                const size_t j = yy * 32 + xx * 2;
                                sub_u[pos] = (U[j + 0] + U[j + 1] + U[j + 16] + U[j + 17]) * 0.25f;
                                sub_v[pos] = (V[j + 0] + V[j + 1] + V[j + 16] + V[j + 17]) * 0.25f;
                            }
                        }

                        dcu = encode_data_unit_block(ctx, bit_buffer, bit_count, sub_u, 8, fdtbl_uv, dcu, uvdc_ht, uvac_ht);
                        dcv = encode_data_unit_block(ctx, bit_buffer, bit_count, sub_v, 8, fdtbl_uv, dcv, uvdc_ht, uvac_ht);
                    }
                }
            }
        }
        else
        {
            for (size_t y = 0; y < h; y += 8)
            {
                for (size_t x = 0; x < w; x += 8)
                {
                    float Y[64], U[64], V[64];

                    for (size_t row = y, pos = 0; row < y + 8; ++row)
                    {
                        // row >= h => use last input row
                        const size_t clamped_row = (row < h) ? row : static_cast<size_t>(h) - 1;
                        const size_t base_px = (static_cast<size_t>(h) - 1 - clamped_row) * w * ctx.channels;

                        for (size_t col = x; col < x + 8; ++col, ++pos)
                        {
                            // if col >= w => use pixel from last input column
                            const size_t px = base_px + ((col < w) ? col : (static_cast<size_t>(w) - 1)) * ctx.channels;

                            const float r = data_r[px];
                            const float g = data_g[px];
                            const float b = data_b[px];

                            Y[pos] = +0.29900f * r + 0.58700f * g + 0.11400f * b - 128;
                            U[pos] = -0.16874f * r - 0.33126f * g + 0.50000f * b;
                            V[pos] = +0.50000f * r - 0.41869f * g - 0.08131f * b;
                        }
                    }

                    dcy = encode_data_unit_block(ctx, bit_buffer, bit_count, Y, 8, fdtbl_y,  dcy, ydc_ht,   yac_ht);
                    dcu = encode_data_unit_block(ctx, bit_buffer, bit_count, U, 8, fdtbl_uv, dcu, uvdc_ht, uvac_ht);
                    dcv = encode_data_unit_block(ctx, bit_buffer, bit_count, V, 8, fdtbl_uv, dcv, uvdc_ht, uvac_ht);
                }
            }
        }

        // Do the bit alignment of the EOI marker
        write_bits(ctx, &bit_buffer, &bit_count, fill_bits);
    }

    // EOI
    ctx.write_byte(0xFF);
    ctx.write_byte(0xD9);

    return true;
}

static bool write_jpg(const char* filename, size_t width, size_t height, size_t channels, const uint8_t* data, uint8_t quality)
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

    const bool ok = write_jpg_core(ctx, quality);
    ctx.close_file();
    return ok;
}

} // namespace jpg
} // namespace img
} // namespace vx