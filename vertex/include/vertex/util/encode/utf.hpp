#pragma once

#include "vertex/system/compiler.hpp"
#include "vertex/system/validate.hpp"

namespace vx {
namespace utf {

// https://github.com/boostorg/locale/blob/52cf8f8a5e23fc6ab5e3d5c8c5657f178ec35e90/include/boost/locale/utf.hpp

using code_point = uint32_t;

enum : code_point { INVALID_CODE_POINT = 0xFFFFFFFF };

enum : size_t { INVALID_TRAIL_COUNT = -1 };

constexpr bool is_valid_codepoint(code_point c)
{
    // Check if the codepoint is within the valid range
    if (c > 0x0010FFFF)
    {
        return false;
    }
    // Check if the codepoint falls within the surrogate range
    if (c >= 0x0000D800 && c <= 0x0000DFFF)
    {
        return false;
    }

    return true;
}

template <size_t size>
struct utf_base_traits;

template <typename char_t, size_t size = sizeof(char_t)>
struct utf_traits;

template <>
struct utf_base_traits<1> // utf8
{
    using utype = uint8_t;

    static constexpr size_t trail_count(utype lead)
    {
        if ((lead & 0b10000000) == 0b00000000)
        {
            // 1-byte characters: 0xxxxxxx (ASCII)
            return 0;
        }
        if ((lead & 0b11100000) == 0b11000000)
        {
            // 2-byte characters: 110xxxxx 10xxxxxx
            return 1;
        }
        if ((lead & 0b11110000) == 0b11100000)
        {
            // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
            return 2;
        }
        if ((lead & 0b11111000) == 0b11110000)
        {
            // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            return 3;
        }
        
        return INVALID_TRAIL_COUNT;
    }

    static constexpr size_t max_width() { return 4; }

    static constexpr size_t width(code_point c)
    {
        if (c <= 0x0000007F)
        {
            return 1;
        }
        if (c <= 0x000007FF)
        {
            return 2;
        }
        if (c <= 0x0000FFFF)
        {
            return 3;
        }

        return 4;
    }

    static constexpr bool is_trail(utype c)
    {
        return (c & 0b11000000) == 0b10000000;
    }

    static constexpr bool is_lead(utype c)
    {
        return !is_trail(c);
    }
};

template <typename char_t>
struct utf_traits<char_t, 1> : utf_base_traits<1>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<1>::utype;

    template <typename IT>
    static inline IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);
        const size_t size = static_cast<size_t>(std::distance(first, last));

        c = INVALID_CODE_POINT;

        // Empty range
        if (VX_UNLIKELY(size == 0))
        {
            return first;
        }

        const utype lead = static_cast<utype>(*first++);
        const size_t trail_bytes = trail_count(lead);

        // Indicates invalid lead byte
        if (trail_bytes == INVALID_TRAIL_COUNT)
        {
            return first;
        }

        c = lead;

        // ASCII is always single byte
        if (trail_bytes == 0)
        {
            return first;
        }

        // Mask out prefix bits in lead byte and
        // shift to make room for trail bytes
        c &= ((1 << (6 - trail_bytes)) - 1);

        switch (trail_bytes)
        {
            case 3:
            {
                if (first == last)
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
                VX_FALLTHROUGH;
            }
            case 2:
            {
                if (first == last)
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
                VX_FALLTHROUGH;
            }
            case 1:
            {
                if (first == last)
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = INVALID_CODE_POINT;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
            }
        }

        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            c = INVALID_CODE_POINT;
            return first;
        }

        // check for overlong encoding
        if (VX_UNLIKELY(width(c) != trail_bytes + 1))
        {
            c = INVALID_CODE_POINT;
            return first;
        }

        return first;
    }

    template <typename IT>
    static inline IT encode(code_point c, IT out, char_type replacement = 0)
    {
        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            // Codepoint is outside valid range
            *out++ = replacement;
            return out;
        }

        // 1-byte characters: 0xxxxxxx (ASCII)
        if (c < 0x00000080)
        {
            *out++ = static_cast<char_type>(c);
        }
        // 2-byte characters: 110xxxxx 10xxxxxx
        else if (c < 0x00000800)
        {
            *out++ = static_cast<char_type>((c >> 6) | 0b00111111);
            *out++ = static_cast<char_type>((c & 0b00111111) | 0b10000000);
        }
        // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
        else if (VX_LIKELY(c < 0x00010000))
        {
            *out++ = static_cast<char_type>((c >> 12) | 0b11100000);
            *out++ = static_cast<char_type>(((c >> 6) & 0b00111111) | 0b10000000);
            *out++ = static_cast<char_type>((c & 0b00111111) | 0b10000000);
        }
        // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else // (c <= 0x0010FFFF)
        {
            *out++ = static_cast<char_type>((c >> 18) | 0b11110000);
            *out++ = static_cast<char_type>(((c >> 12) & 0b00111111) | 0b10000000);
            *out++ = static_cast<char_type>(((c >>  6) & 0b00111111) | 0b10000000);
            *out++ = static_cast<char_type>((c & 0b00111111) | 0b10000000);
        }

        return out;
    }
};

template <>
struct utf_base_traits<2> // utf16
{
    using utype = uint16_t;

    // See RFC 2781
    static constexpr bool is_first_surrogate (utype w1) { return 0xD800 <= w1 && w1 <= 0xDBFF; }
    static constexpr bool is_second_surrogate(utype w2) { return 0xDC00 <= w2 && w2 <= 0xDFFF; }

    static constexpr code_point combine_surrogate(utype w1, utype w2)
    {
        return ((static_cast<code_point>(w1 & 0x03FF) << 10) | (w2 & 0x03FF)) + 0x10000;
    }

    static constexpr size_t trail_count(utype c)
    {
        if (is_first_surrogate(c))
        {
            return 1;
        }
        if (is_second_surrogate(c))
        {
            return INVALID_TRAIL_COUNT;
        }

        return 0;
    }

    static constexpr bool is_trail(utype c) { return is_second_surrogate(c); }
    static constexpr bool is_lead(utype c) { return !is_second_surrogate(c); }

    static constexpr size_t max_width() { return 2; }
    static constexpr size_t width(code_point c) { return c >= 0x10000 ? 2 : 1; }
};

template <typename char_t>
struct utf_traits<char_t, 2> : utf_base_traits<2>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<2>::utype;

    template <typename IT>
    static inline IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);

        c = INVALID_CODE_POINT;

        // Empty range
        if (VX_UNLIKELY(first == last))
        {
            return first;
        }

        const utype w1 = static_cast<utype>(*first++);
        
        if (VX_LIKELY(w1 < 0xD800 || 0xDFFF < w1))
        {
            // Single unicode character outside reserved surrogate range
            c = w1;
            return first;
        }

        if (w1 > 0xDBFF)
        {
            // Out of valid range for first surrogate
            return first;
        }

        if (first == last)
        {
            // Not enough space to contain second surrogate
            return first;
        }

        const utype w2 = static_cast<utype>(*first++);
        if (!is_second_surrogate(w2))
        {
            // Invalid second surrogate
            return first;
        }

        c = combine_surrogate(w1, w2);
        return first;
    }

    template <typename IT>
    static inline IT encode(code_point c, IT out, char_type replacement = 0)
    {
        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            // Codepoint is outside valid range
            *out++ = replacement;
            return out;
        }

        if (VX_LIKELY(c < 0x0010000))
        {
            // Codepoint is directly convertable
            *out++ = static_cast<char_type>(c);
        }
        else
        {
            // Build surrogate pair
            c -= 0x00010000;
            *out++ = static_cast<char_type>(0x0000D800 | (c >> 10));
            *out++ = static_cast<char_type>(0x0000DC00 | (c & 0x000003FF));
        }

        return out;
    }
};

template <>
struct utf_base_traits<4> // utf32
{
    using utype = uint32_t;

    static constexpr size_t trail_count(utype c)
    {
        if (is_valid_codepoint(c))
        {
            return 0;
        }

        return INVALID_TRAIL_COUNT;
    }

    static constexpr bool is_trail(utype c) { return false; }
    static constexpr bool is_lead(utype c) { return true; }

    static constexpr size_t max_width() { return 1; }
    static constexpr size_t width(code_point c) { return 1; }
};

template <typename char_t>
struct utf_traits<char_t, 4> : utf_base_traits<4>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<4>::utype;

    template <typename IT>
    static inline IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);

        c = INVALID_CODE_POINT;

        // Empty range
        if (VX_UNLIKELY(first == last))
        {
            return first;
        }

        c = static_cast<code_point>(*first++);

        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            c = INVALID_CODE_POINT;
            return first;
        }

        return first;
    }

    template <typename IT>
    static inline IT encode(code_point c, IT out, char_type replacement = 0)
    {
        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            // Codepoint is outside valid range
            *out++ = replacement;
            return out;
        }

        *out++ = static_cast<char_type>(c);

        return out;
    }
};

} // namespace utf
} // namespace vx