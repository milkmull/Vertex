#pragma once

#include "vertex/system/validate.hpp"

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// utf8
///////////////////////////////////////////////////////////////////////////////

namespace utf8 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint8_t replacement)
{
    if (codepoint < 0x00000000 || codepoint > 0x0010FFFF)
    {
        // Codepoint is outside valid range
        *out++ = replacement;
        return out;
    }

    size_t byte_count = 1;

    // 1-byte characters: 0xxxxxxx (ASCII)
    if      (codepoint <  0x00000080) byte_count = 1;
    // 2-byte characters: 110xxxxx 10xxxxxx
    else if (codepoint <  0x00000800) byte_count = 2;
    // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
    else if (codepoint <  0x00010000) byte_count = 3;
    // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (codepoint <= 0x0010FFFF) byte_count = 4;

    static constexpr uint8_t lead_bytes[5] = { 0b00000000, 0b00000000, 0b11000000, 0b11100000, 0b11110000 };

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
        *out++ = bytes[i];
    }

    return out;
}

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    uint8_t lead = static_cast<uint8_t>(*first);
    uint8_t trail_bytes = 0;

    if      ((lead & 0b10000000) == 0b00000000)
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
        return last;
    }

    if (trail_bytes >= std::distance(first, last))
    {
        // Not enough space to contain expected trailing bytes
        codepoint = replacement;
        return last;
    }

    // Values to shave off leading bits after assembling final codepoint
    static constexpr uint32_t lead_masks[4] = { 0x00000000, 0x00003080, 0x000E2080, 0x03C82080 };

    codepoint = 0;

    switch (trail_bytes)
    {
        case 3: codepoint += static_cast<uint8_t>(*first++); codepoint <<= 6; VX_FALLTHROUGH;
        case 2: codepoint += static_cast<uint8_t>(*first++); codepoint <<= 6; VX_FALLTHROUGH;
        case 1: codepoint += static_cast<uint8_t>(*first++); codepoint <<= 6; VX_FALLTHROUGH;
        case 0: codepoint += static_cast<uint8_t>(*first++);
    }
    codepoint -= lead_masks[trail_bytes];

    return first;
}

template <typename IT>
inline IT next(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    uint32_t codepoint{};
    return decode(first, last, codepoint);
}

template <typename IT>
inline size_t count(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    size_t count = 0;

    while (first < last)
    {
        first = next(first, last);
        ++count;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        *out++ = *first++;
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf16::encode(codepoint, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf32::encode(codepoint, out);
    }

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint = utf32::decode_wide(*first++);
        out = encode(codepoint, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf32::encode_wide(codepoint, out, replacement);
    }

    return out;
}

} // namespace utf8

///////////////////////////////////////////////////////////////////////////////
// utf16
///////////////////////////////////////////////////////////////////////////////

namespace utf16 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint16_t replacement)
{
    if (codepoint < 0x0000FFFF)
    {
        if (codepoint >= 0x0000D800 && codepoint <= 0x0000DFFF)
        {
            // Invalid codepoint in reserved range
            *out++ = replacement;
            return out;
        }

        // Codepoint is directly convertable
        *out++ = static_cast<uint16_t>(codepoint);
        return out;
    }

    if (codepoint > 0x0010FFFF)
    {
        // Out of valid range
        *out++ = replacement;
        return out;
    }

    // Build surrogate pair
    codepoint -= 0x00010000;
    *out++ = static_cast<uint16_t>((codepoint >> 10)        + 0x0000D800);
    *out++ = static_cast<uint16_t>((codepoint & 0x000003FF) + 0x0000DC00);

    return out;
}

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    const uint16_t first = *first++;

    // Check for expected surrogate pair
    if (first >= 0x0000D800 && first <= 0x0000DBFF)
    {
        if (first >= last)
        {
            // Not enough space to contain second surrogate
            codepoint = replacement;
            return first;
        }

        const uint32_t second = *first++;

        if (second >= 0x0000DC00 && second <= 0x0000DFFF)
        {
            // Valid second surrogate, combine surrogate pair
            codepoint = ((first - 0x0000D800) << 10) + (second - 0x0000DC00) + 0x00010000;
            return first;
        }
        else
        {
            // Invalid second surrogate
            codepoint = replacement;
            return first;
        }
    }

    // Codepoint is directly convertable
    codepoint = first;

    return first;
}

template <typename IT>
inline IT next(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    uint32_t codepoint{};
    return decode(first, last, codepoint);
}

template <typename IT>
inline size_t count(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    size_t count = 0;

    while (first < last)
    {
        first = next(first, last);
        ++count;
    }

    return count;
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf8::encode(codepoint, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        *out++ = *first++;
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf32::encode(codepoint, out);
    }

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint = utf32::decode_wide(*first++);
        out = encode(codepoint, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        uint32_t codepoint{};
        first = decode(first, last, codepoint);
        out = utf32::encode_wide(codepoint, out, replacement);
    }

    return out;
}

} // namespace utf16

///////////////////////////////////////////////////////////////////////////////
// utf32
///////////////////////////////////////////////////////////////////////////////

namespace utf32 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint32_t replacement)
{
    *out++ = codepoint;
    return out;
}

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    codepoint = *first++;
    return first;
}

template <typename IT>
inline IT next(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    return ++first;
}

template <typename IT>
inline size_t count(IT first, IT last)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    return static_cast<size_t>(std::distance(first, last));
}

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        out = utf8::encode(*first++, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        out = utf16::encode(*first++, out);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        *out++ = *first++;
    }

    return out;
}

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        *out++ = decode_wide(*first++);
    }

    return out;
}

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement)
{
    VX_ITERATOR_VALID_RANGE(first, last);

    while (first < last)
    {
        out = encode_wide(*first++, out, replacement);
    }

    return out;
}

template <typename IT>
inline IT encode_wide(uint32_t codepoint, IT out, wchar_t replacement)
{
    switch (sizeof(wchar_t))
    {
        case 4:
        {
            // UCS-4, is utf32, we can directly copy the codepoint
            *out++ = static_cast<wchar_t>(codepoint);
            break;
        }
        case 2:
        {
            // UCS-2, is utf16, and a subset of UCS-4 that uses 16 bit characters
            if (codepoint <= 0x0000FFFF && (codepoint < 0x0000D800 || codepoint > 0x0000DFFF))
            {
                // In the valid range for a single character utf16 codepoint
                *out++ = static_cast<wchar_t>(codepoint);
                break;
            }

            VX_FALLTHROUGH;
        }
        default:
        {
            *out++ = replacement;
            break;
        }
    }

    return out;
}

template <typename T>
inline uint32_t decode_wide(T input)
{
    // Assuming USC-2 or USC-4, both are directly convertable to utf32
    return static_cast<uint32_t>(input);
}

} // namespace utf32

} // namespace vx