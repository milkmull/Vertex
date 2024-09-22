#pragma once

#include "vertex/system/error.hpp"
#include "vertex/stdlib/string/base64.hpp"

namespace vx {
namespace str {
namespace base64 {

// https://www.boost.org/doc/libs/1_68_0/boost/beast/core/detail/base64.hpp

/**
 * The process of converting 3 8-bit values (24 bits) into 4 6-bit Base64 values:
 *
 * +-----------------------------------------------------------+
 * |                  3 x 8-bit values (24 bits)               |
 * +-----------------------------------------------------------+
 * | 01101001 | 00011010 | 10111110 |                          |
 * +-----------------------------------------------------------+
 * |      1st |    2nd   |    3rd   |                          |
 * |    byte  |   byte   |   byte   |                          |
 * +-----------------------------------------------------------+
 *
 * The 24-bit block is split into four 6-bit segments:
 *
 * +-----------------------------------------------------------+
 * | 011010 | 010001 | 101010 | 111110 |                       |
 * +-----------------------------------------------------------+
 * |  1st   |  2nd   |  3rd   |  4th   |                       |
 * |  6-bit |  6-bit |  6-bit |  6-bit |                       |
 * +-----------------------------------------------------------+
 * |                  4 x 6-bit values (24 bits)               |
 * +-----------------------------------------------------------+
 *
 * Each 6-bit value is then mapped to a character in the Base64 alphabet.
 */

static constexpr char characters[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

#define PADDING '='

bool encode(const uint8_t* data, size_t size, std::string& encoded)
{
    encoded.clear();

    if (!data)
    {
        error::set_error(error::error_code::INVALID_ARGUMENT);
        return false;
    }
    if (size == 0)
    {
        return true;
    }

    // Calculate final size
    const size_t encoded_size = 4 * ((size + 2) / 3);
    encoded.reserve(encoded_size);

    for (size_t i = 0; i < size; i += 3)
    {
        const uint8_t byte1 = data[i];
        const uint8_t byte2 = (i + 1 < size) ? data[i + 1] : 0;
        const uint8_t byte3 = (i + 2 < size) ? data[i + 2] : 0;

        // Extract the 4 batches of 6 bits
        const uint8_t char1 = byte1 >> 2;
        const uint8_t char2 = ((byte1 & 0b00000011) << 4) | (byte2 >> 4);
        const uint8_t char3 = ((byte2 & 0b00001111) << 2) | (byte3 >> 6);
        const uint8_t char4 = byte3 & 0b00111111;

        // Add the encoded data, pad to complete the quartet
        encoded.push_back(characters[char1]);
        encoded.push_back(characters[char2]);
        encoded.push_back((i + 1 >= size) ? PADDING : characters[char3]);
        encoded.push_back((i + 2 >= size) ? PADDING : characters[char4]);
    }

    return true;
}

bool encode(const char* data, size_t size, std::string& encoded)
{
    return encode(reinterpret_cast<const uint8_t*>(data), size, encoded);
}

// Table to map ascii characters back to the base64 character set
static constexpr char decode_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 0 - 15
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, // 16 - 31
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, // 32 - 47
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64,  0, 64, 64, // 48 - 63
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, // 64 - 79
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, // 80 - 95
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 96 - 111
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, // 112 - 127
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

bool decode(const std::string& encoded, std::vector<uint8_t>& data, bool validate)
{
    data.clear();
    const size_t size = encoded.size();
    
    if (size == 0)
    {
        return true;
    }
    
    if (size % 4 != 0)
    {
        error::set_error(error::error_code::SIZE_ERROR);
        return false;
    }

    // Calculate final size
    const size_t padding_count = static_cast<size_t>(encoded[size - 1] == PADDING) + static_cast<size_t>(encoded[size - 2] == PADDING);
    const size_t decoded_size = (size / 4 * 3) - padding_count;
    data.reserve(decoded_size);

    for (size_t i = 0; i < size; i += 4)
    {
        // Get 4 ascii characters and map them to their base64 character ignore padding
        const uint32_t c1 = static_cast<uint32_t>(decode_table[static_cast<int>(encoded[i + 0])]);
        const uint32_t c2 = static_cast<uint32_t>(decode_table[static_cast<int>(encoded[i + 1])]);
        const uint32_t c3 = static_cast<uint32_t>(decode_table[static_cast<int>(encoded[i + 2])]);
        const uint32_t c4 = static_cast<uint32_t>(decode_table[static_cast<int>(encoded[i + 3])]);
        
        if (validate && (c1 == 64 || c2 == 64 || c3 == 64 || c4 == 64))
        {
            error::set_error(error::error_code::INVALID_ARGUMENT);
            return false;
        }
        
        // 24-bit value with the 3 final bytes
        const uint32_t combined = (c1 << 18) | (c2 << 12) | (c3 << 6) | (c4 << 0);

        // Extract the 3 8-bit values
        if (data.size() < decoded_size) data.push_back(static_cast<uint8_t>((combined >> 16) & 0xFF));
        if (data.size() < decoded_size) data.push_back(static_cast<uint8_t>((combined >>  8) & 0xFF));
        if (data.size() < decoded_size) data.push_back(static_cast<uint8_t>((combined >>  0) & 0xFF));
    }

    return true;
}

#undef PADDING

}
}
}