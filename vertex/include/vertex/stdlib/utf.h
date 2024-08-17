#pragma once

#include <locale>

#include "vertex/system/compiler.h"

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// utf8
///////////////////////////////////////////////////////////////////////////////

namespace utf8 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint8_t replacement = 0)
{
    if (codepoint < 0x00 || codepoint > 0x10FFFF)
    {
        // Codepoint is outside valid range
        *output++ = replacement;
        return output;
    }

    size_t byte_count = 1;

    // 1-byte characters: 0xxxxxxx (ASCII)
    if      (codepoint <  0x80)       byte_count = 1;
    // 2-byte characters: 110xxxxx 10xxxxxx
    else if (codepoint <  0x800)      byte_count = 2;
    // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
    else if (codepoint <  0x10000)    byte_count = 3;
    // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (codepoint <= 0x0010FFFF) byte_count = 4;

    static uint8_t lead_bytes[5] = { 0b00000000, 0b00000000, 0b11000000, 0b11100000, 0b11110000 };

    uint8_t bytes[4]{};

    switch (byte_count)
    {
        case 4: bytes[3] = static_cast<uint8_t>((codepoint | 0b10000000) & 0b10111111); codepoint >>= 6; VX_FALLTHROUGH;
        case 3: bytes[2] = static_cast<uint8_t>((codepoint | 0b10000000) & 0b10111111); codepoint >>= 6; VX_FALLTHROUGH;
        case 2: bytes[1] = static_cast<uint8_t>((codepoint | 0b10000000) & 0b10111111); codepoint >>= 6; VX_FALLTHROUGH;
        case 1: bytes[0] = static_cast<uint8_t> (codepoint | lead_bytes[byte_count]);
    }

    for (size_t i = 0; i < byte_count; ++i)
    {
        *output++ = bytes[i];
    }

    return output;
}

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0)
{
    uint8_t lead = static_cast<uint8_t>(*begin);
    uint8_t trail_bytes = 0;

    if ((lead & 0b10000000) == 0b00000000)
    {
        // 1-byte characters: 0xxxxxxx (ASCII)
        trail_bytes = 0;
    }
    else if ((lead & 0b11100000) == 0b11000000)
    {
        // 2-byte characters: 110xxxxx 10xxxxxx
        trail_bytes = 1;
    }
    else if ((lead & 0b11110000) == 0b11100000)
    {
        // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
        trail_bytes = 2;
    }
    else if ((lead & 0b11111000) == 0b11110000)
    {
        // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        trail_bytes = 3;
    }
    else
    {
        // Invalid lead byte
        codepoint = replacement;
        return end;
    }

    if (trail_bytes >= std::distance(begin, end))
    {
        // Not enough space to contain expected trailing bytes
        codepoint = replacement;
        return end;
    }

    // Values to shave off leading bits after assembling final codepoint
    static uint32_t lead_masks[4] = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080 };

    codepoint = 0;

    switch (trail_bytes)
    {
        case 3: codepoint += static_cast<uint8_t>(*begin++); codepoint <<= 6; VX_FALLTHROUGH;
        case 2: codepoint += static_cast<uint8_t>(*begin++); codepoint <<= 6; VX_FALLTHROUGH;
        case 1: codepoint += static_cast<uint8_t>(*begin++); codepoint <<= 6; VX_FALLTHROUGH;
        case 0: codepoint += static_cast<uint8_t>(*begin++);
    }
    codepoint -= lead_masks[trail_bytes];

    return begin;
}

template <typename IT>
inline IT next(IT begin, IT end)
{
    uint32_t codepoint;
    return decode(begin, end, codepoint);
}

template <typename IT>
inline size_t count(IT begin, IT end)
{
    size_t count = 0;

    while (begin < end)
    {
        begin = next(begin, end);
        ++count;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        *output++ = *begin++;
    }

    return output;
}
template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf16::encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode(codepoint, output);
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_ansi(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint = utf32::decode_ansi(*begin++);
        output = encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_ansi(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode_ansi(codepoint, output, replacement, std::locale());
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32, we can directly encode
    while (begin < end)
    {
        output = encode(*begin++, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);

        // Make sure codepoint is in valid latin1 range
        *output++ = codepoint < 256 ? static_cast<char>(codepoint) : replacement;
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint = utf32::decode_wide(*begin++);
        output = encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode_wide(codepoint, output, replacement);
    }

    return output;
}

} // namespace utf8

///////////////////////////////////////////////////////////////////////////////
// utf16
///////////////////////////////////////////////////////////////////////////////

namespace utf16 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint16_t replacement = 0)
{
    if (codepoint < 0xFFFF)
    {
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
        {
            // Invalid codepoint in reserved range
            *output++ = replacement;
            return output;
        }

        // Codepoint is directly convertable
        *output++ = static_cast<uint16_t>(codepoint);
        return output;
    }

    if (codepoint > 0x10FFFF)
    {
        // Out of valid range
        *output++ = replacement;
        return output;
    }

    // Build surrogate pair
    codepoint -= 0x10000;
    *output++ = static_cast<uint16_t>((codepoint >> 10)   + 0xD800);
    *output++ = static_cast<uint16_t>((codepoint & 0x3FF) + 0xDC00);

    return output;
}

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0)
{
    const uint16_t first = *begin++;

    // Check for expected surrogate pair
    if (first >= 0xD800 && first <= 0xDBFF)
    {
        if (begin >= end)
        {
            // Not enough space to contain second surrogate
            codepoint = replacement;
            return begin;
        }

        const uint32_t second = *begin++;

        if (second >= 0xDC00 && second <= 0xDFFF)
        {
            // Valid second surrogate, combine surrogate pair
            codepoint = ((first - 0xD800) << 10) + (second - 0xDC00) + 0x10000;
            return begin;
        }
        else
        {
            // Invalid second surrogate
            codepoint = replacement;
            return begin;
        }
    }

    // Codepoint is directly convertable
    codepoint = first;

    return begin;
}

template <typename IT>
inline IT next(IT begin, IT end)
{
    uint32_t codepoint;
    return decode(begin, end, codepoint);
}

template <typename IT>
inline size_t count(IT begin, IT end)
{
    size_t count = 0;

    while (begin < end)
    {
        begin = next(begin, end);
        ++count;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf8::encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        *output++ = *begin++;
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode(codepoint, output);
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_ansi(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint = utf32::decode_ansi(*begin++, std::locale());
        output = encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_ansi(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode_ansi(codepoint, output, replacement, std::locale());
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32 and utf16, we can directly encode
    while (begin < end)
    {
        *output++ = *begin++;
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32 and utf16
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);

        // Make sure codepoint is in valid latin1 range
        *output++ = codepoint < 256 ? static_cast<char>(codepoint) : replacement;
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        uint32_t codepoint = utf32::decode_wide(*begin++);
        output = encode(codepoint, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0)
{
    while (begin < end)
    {
        uint32_t codepoint;
        begin = decode(begin, end, codepoint);
        output = utf32::encode_wide(codepoint, output, replacement);
    }

    return output;
}

} // namespace utf16

///////////////////////////////////////////////////////////////////////////////
// utf32
///////////////////////////////////////////////////////////////////////////////

namespace utf32 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint32_t replacement = 0)
{
    *output++ = codepoint;
    return output;
}

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0)
{
    codepoint = *begin++;
    return begin;
}

template <typename IT>
inline IT next(IT begin, IT end)
{
    return ++begin;
}

template <typename IT>
inline size_t count(IT begin, IT end)
{
    return std::distance(begin, end);
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        output = utf8::encode(*begin++, output);
    }

    return output;
}
template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        output = utf16::encode(*begin++, output);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        *output++ = *begin++;
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline IT encode_ansi(uint32_t codepoint, IT output, char replacement = 0)
{
    // We use the locale character conversion facet to translate each character to ansi
    const auto& facet = std::use_facet<std::ctype<wchar_t>>(std::locale());
    *output++ = facet.narrow(static_cast<wchar_t>(codepoint), replacement);
    return output;
}

template <typename IT>
inline uint32_t decode_ansi(IT input)
{
    // We use the locale character conversion facet to translate each character from ansi
    const auto& facet = std::use_facet<std::ctype<wchar_t>>(std::locale());
    return static_cast<uint32_t>(facet.widen(input));
}

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32, we can directly encode
    while (begin < end)
    {
        *output++ = *begin++;
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0)
{
    // Latin-1 uses 256 (1 byte) codepoints that are a subset of utf32
    while (begin < end)
    {
        // Make sure codepoint is in valid latin1 range
        *output++ = *begin < 256 ? static_cast<char>(*begin) : replacement;
        ++begin;
    }

    return output;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output)
{
    while (begin < end)
    {
        *output++ = decode_wide(*begin++);
    }

    return output;
}

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0)
{
    while (begin < end)
    {
        output = encode_wide(*begin++, output, replacement);
    }

    return output;
}

template <typename IT>
inline IT encode_wide(uint32_t codepoint, IT output, wchar_t replacement = 0)
{
    switch (sizeof(wchar_t))
    {
        case 4:
        {
            // UCS-4, is utf32, we can directly copy the codepoint
            *output++ = static_cast<wchar_t>(codepoint);
            break;
        }
        case 2:
        {
            // UCS-2, is utf16, and a subset of UCS-4 that uses 16 bit characters
            if (codepoint <= 0xFFFF && (codepoint < 0xD800 || codepoint > 0xDFFF))
            {
                // In the valid range for a single character utf16 codepoint
                *output++ = static_cast<wchar_t>(codepoint);
                break;
            }

            VX_FALLTHROUGH;
        }
        default:
        {
            *output++ = replacement;
            break;
        }
    }

    return output;
}

template <typename T>
inline uint32_t decode_wide(T input)
{
    // Assuming USC-2 or USC-4, both are directly convertable to utf32
    return static_cast<uint32_t>(input);
}

} // namespace utf32

}
}