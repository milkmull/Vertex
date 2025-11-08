#pragma once

#include <limits>

#include "vertex/system/validate.hpp"

#if defined(max)
#   undef max
#endif

namespace vx {
namespace utf {

// https://github.com/boostorg/locale/blob/52cf8f8a5e23fc6ab5e3d5c8c5657f178ec35e90/include/boost/locale/utf.hpp

using code_point = uint32_t;

enum : code_point { invalid_code_point = 0xFFFFFFFF };

enum : size_t { invalid_trail_count = std::numeric_limits<size_t>::max() };

/**
 * @brief Checks if a given Unicode codepoint is valid.
 *
 * This function checks if the provided codepoint is within the valid Unicode range,
 * and also ensures that the codepoint does not fall within the surrogate range,
 * which is reserved for UTF-16 encoding.
 *
 * @param c The Unicode codepoint to be checked.
 *
 * @return `true` if the codepoint is valid, otherwise `false`.
 *
 * @note The valid Unicode codepoint range is from 0 to 0x10FFFF, excluding
 * the surrogate range (0xD800 to 0xDFFF).
 */
constexpr bool is_valid_codepoint(code_point c) noexcept
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


    /**
     * @brief Determines the number of trailing bytes for a given lead byte.
     *
     * The number of trailing bytes is determined based on the first byte (the lead byte),
     * according to the UTF-8 encoding scheme.
     *
     * @param lead The lead byte to check.
     * @return The number of trailing bytes, or `invalid_trail_count` if the lead byte is invalid.
     */
    static constexpr size_t trail_count(utype lead) noexcept
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
        
        return invalid_trail_count;
    }

    /**
     * @brief Returns the maximum number of bytes used for encoding a character in UTF-8.
     *
     * @return The maximum width, which is 4 bytes for UTF-8 encoding.
     */
    static constexpr size_t max_width() noexcept { return 4; }

    /**
     * @brief Determines the number of bytes required to encode a given Unicode codepoint.
     *
     * This function calculates the number of bytes needed to encode a codepoint based on
     * the UTF-8 encoding rules.
     *
     * @param c The Unicode codepoint to check.
     * @return The number of bytes required to encode the codepoint.
     */
    static constexpr size_t width(code_point c) noexcept
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

    /**
     * @brief Checks if a byte is a valid trail byte in UTF-8 encoding.
     *
     * A valid trail byte in UTF-8 encoding starts with the prefix `10xxxxxx`.
     *
     * @param c The byte to check.
     * @return `true` if the byte is a valid trail byte, otherwise `false`.
     */
    static constexpr bool is_trail(utype c) noexcept
    {
        return (c & 0b11000000) == 0b10000000;
    }

    /**
     * @brief Checks if a byte is a valid lead byte in UTF-8 encoding.
     *
     * A valid lead byte in UTF-8 encoding is either an ASCII byte or a byte
     * that starts the multi-byte sequence.
     *
     * @param c The byte to check.
     * @return `true` if the byte is a valid lead byte, otherwise `false`.
     */
    static constexpr bool is_lead(utype c) noexcept
    {
        return (c <= 0b01111111) || (c >= 0b11000010 && c <= 0b11110100);
    }
};

template <typename char_t>
struct utf_traits<char_t, 1> : utf_base_traits<1>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<1>::utype;

    /**
     * @brief Decodes a UTF-8 sequence from a range of input iterators.
     *
     * This function decodes a Unicode codepoint from a sequence of UTF-8 encoded bytes.
     * It handles multi-byte UTF-8 sequences and ensures that the codepoint is valid.
     *
     * @param first The first iterator in the range.
     * @param last The last iterator in the range.
     * @param c The decoded Unicode codepoint (output).
     * @return The iterator past the last byte of the decoded sequence, or the input iterator
     * if the sequence is invalid.
     */
    template <typename IT>
    static IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);
        const size_t size = static_cast<size_t>(std::distance(first, last));

        c = invalid_code_point;

        // Empty range
        if (VX_UNLIKELY(size == 0))
        {
            return first;
        }

        const utype lead = static_cast<utype>(*first++);
        const size_t trail_bytes = trail_count(lead);

        // Indicates invalid lead byte
        if (trail_bytes == invalid_trail_count)
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
        c &= static_cast<code_point>((1 << (6 - trail_bytes)) - 1);

        switch (trail_bytes)
        {
            case 3:
            {
                if (first == last)
                {
                    c = invalid_code_point;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = invalid_code_point;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
                VX_FALLTHROUGH;
            }
            case 2:
            {
                if (first == last)
                {
                    c = invalid_code_point;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = invalid_code_point;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
                VX_FALLTHROUGH;
            }
            case 1:
            {
                if (first == last)
                {
                    c = invalid_code_point;
                    return first;
                }

                const utype byte = static_cast<utype>(*first++);

                if (!is_trail(byte))
                {
                    c = invalid_code_point;
                    return first;
                }

                c = (c << 6) | (byte & 0b00111111);
            }
        }

        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            c = invalid_code_point;
            return first;
        }

        // check for overlong encoding
        if (VX_UNLIKELY(width(c) != trail_bytes + 1))
        {
            c = invalid_code_point;
            return first;
        }

        return first;
    }

    /**
     * @brief Encodes a Unicode codepoint into a UTF-8 sequence.
     *
     * This function encodes a Unicode codepoint into one or more bytes in UTF-8 encoding.
     * It writes the resulting bytes into the output iterator and returns the iterator
     * pointing past the last byte written.
     *
     * @param c The Unicode codepoint to encode.
     * @param out The output iterator where the encoded bytes will be written.
     * @param replacement A byte to use as a replacement for invalid codepoints (default is `0`).
     * @return The output iterator past the last byte written.
     */
    template <typename IT>
    static IT encode(code_point c, IT out, char_type replacement = 0)
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

    /**
     * @brief Checks if the given 16-bit unit is a first surrogate in UTF-16.
     *
     * A first surrogate is in the range [0xD800, 0xDBFF].
     *
     * @param w1 The 16-bit value to check.
     * @return `true` if `w1` is a first surrogate, otherwise `false`.
     */
    static constexpr bool is_first_surrogate (utype w1) noexcept { return 0xD800 <= w1 && w1 <= 0xDBFF; }

    /**
     * @brief Checks if the given 16-bit unit is a second surrogate in UTF-16.
     *
     * A second surrogate is in the range [0xDC00, 0xDFFF].
     *
     * @param w2 The 16-bit value to check.
     * @return `true` if `w2` is a second surrogate, otherwise `false`.
     */
    static constexpr bool is_second_surrogate(utype w2) noexcept { return 0xDC00 <= w2 && w2 <= 0xDFFF; }

    /**
     * @brief Combines a first and second surrogate to form a Unicode codepoint.
     *
     * UTF-16 uses a pair of 16-bit values (surrogate pair) to represent codepoints
     * outside the Basic Multilingual Plane (BMP). This function combines the two
     * surrogate values into a single codepoint.
     *
     * @param w1 The first surrogate (lead surrogate).
     * @param w2 The second surrogate (trail surrogate).
     * @return The Unicode codepoint resulting from the combination of `w1` and `w2`.
     */
    static constexpr code_point combine_surrogate(utype w1, utype w2) noexcept
    {
        return ((static_cast<code_point>(w1 & 0x03FF) << 10) | (w2 & 0x03FF)) + 0x10000;
    }

    /**
     * @brief Determines the number of trailing bytes for a given UTF-16 codeunit.
     *
     * This function returns the number of trailing code units (surrogate pairs) for
     * the given code unit in UTF-16 encoding.
     *
     * @param c The UTF-16 code unit to check.
     * @return The number of trailing code units, or `invalid_trail_count` if not a valid surrogate.
     */
    static constexpr size_t trail_count(utype c) noexcept
    {
        if (is_first_surrogate(c))
        {
            return 1;
        }
        if (is_second_surrogate(c))
        {
            return invalid_trail_count;
        }

        return 0;
    }

    /**
     * @brief Checks if a 16-bit unit is a valid trailing surrogate in UTF-16.
     *
     * A trailing surrogate is a value in the range [0xDC00, 0xDFFF].
     *
     * @param c The 16-bit unit to check.
     * @return `true` if `c` is a valid trail surrogate, otherwise `false`.
     */
    static constexpr bool is_trail(utype c) noexcept { return is_second_surrogate(c); }

    /**
     * @brief Checks if a 16-bit unit is a valid lead surrogate in UTF-16.
     *
     * A lead surrogate is a value in the range [0xD800, 0xDBFF].
     *
     * @param c The 16-bit unit to check.
     * @return `true` if `c` is a valid lead surrogate, otherwise `false`.
     */
    static constexpr bool is_lead(utype c) noexcept { return !is_second_surrogate(c); }

    /**
     * @brief Returns the maximum number of code units used for encoding a character in UTF-16.
     *
     * UTF-16 uses either 1 or 2 code units to represent a character, depending on whether
     * the character is part of the Basic Multilingual Plane (BMP) or outside it.
     *
     * @return The maximum width, which is 2 for UTF-16 encoding.
     */
    static constexpr size_t max_width() noexcept { return 2; }

    /**
     * @brief Determines the number of code units required to encode a given Unicode codepoint in UTF-16.
     *
     * If the codepoint is within the BMP, only 1 code unit is needed. If the codepoint is outside the BMP,
     * a surrogate pair (2 code units) is required.
     *
     * @param c The Unicode codepoint to check.
     * @return The number of code units (1 or 2) required to encode the codepoint.
     */
    static constexpr size_t width(code_point c) noexcept { return c >= 0x10000 ? 2 : 1; }
};

template <typename char_t>
struct utf_traits<char_t, 2> : utf_base_traits<2>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<2>::utype;

    /**
     * @brief Decodes a UTF-16 sequence from a range of input iterators.
     *
     * This function decodes a Unicode codepoint from a sequence of UTF-16 encoded units,
     * handling both single 16-bit characters and surrogate pairs for codepoints outside the BMP.
     *
     * @param first The first iterator in the range.
     * @param last The last iterator in the range.
     * @param c The decoded Unicode codepoint (output).
     * @return The iterator past the last decoded unit, or the input iterator if the sequence is invalid.
     */
    template <typename IT>
    static IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);

        c = invalid_code_point;

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
            return --first;
        }

        c = combine_surrogate(w1, w2);
        return first;
    }

    /**
     * @brief Encodes a Unicode codepoint into a UTF-16 sequence.
     *
     * This function encodes a Unicode codepoint into one or two 16-bit units, depending
     * on whether the codepoint is within the Basic Multilingual Plane (BMP) or requires
     * a surrogate pair.
     *
     * @param c The Unicode codepoint to encode.
     * @param out The output iterator where the encoded units will be written.
     * @param replacement A 16-bit value to use as a replacement for invalid codepoints (default is `0`).
     * @return The output iterator past the last encoded unit.
     */
    template <typename IT>
    static IT encode(code_point c, IT out, char_type replacement = 0)
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

    /**
     * @brief Determines the number of trailing bytes for a given UTF-32 codepoint.
     *
     * UTF-32 is a fixed-length encoding, so it doesn't have trailing bytes for valid codepoints.
     *
     * @param c The UTF-32 codepoint to check.
     * @return Always 0 for valid codepoints, `invalid_trail_count` otherwise.
     */
    static constexpr size_t trail_count(utype c) noexcept
    {
        if (is_valid_codepoint(c))
        {
            return 0;
        }

        return invalid_trail_count;
    }

    /**
     * @brief Checks if a 32-bit unit is a valid trailing code unit in UTF-32.
     *
     * UTF-32 doesn't have trailing units; all codepoints are represented by a single 32-bit value.
     *
     * @param c The 32-bit unit to check.
     * @return Always `false`, as UTF-32 doesn't use trailing units.
     */
    static constexpr bool is_trail(utype) noexcept { return false; }

    /**
     * @brief Checks if a 32-bit unit is a valid leading code unit in UTF-32.
     *
     * UTF-32 uses one 32-bit unit for each codepoint, so this always returns `true` for valid codepoints.
     *
     * @param c The 32-bit unit to check.
     * @return Always `true` for valid codepoints.
     */
    static constexpr bool is_lead(utype) noexcept { return true; }

    /**
     * @brief Returns the maximum number of code units used for encoding a character in UTF-32.
     *
     * UTF-32 uses one 32-bit unit for each codepoint, so the maximum width is 1.
     *
     * @return Always 1, since UTF-32 encodes each codepoint in a single 32-bit unit.
     */
    static constexpr size_t max_width() noexcept { return 1; }

    /**
     * @brief Determines the number of code units required to encode a given Unicode codepoint in UTF-32.
     *
     * UTF-32 uses exactly one 32-bit unit for each codepoint, so the width is always 1.
     *
     * @param c The Unicode codepoint to check.
     * @return Always 1, since UTF-32 encodes each codepoint in a single 32-bit unit.
     */
    static constexpr size_t width(code_point) noexcept { return 1; }
};

template <typename char_t>
struct utf_traits<char_t, 4> : utf_base_traits<4>
{
    using char_type = char_t;
    using string_type = std::basic_string<char_type>;
    using utype = utf_base_traits<4>::utype;

    /**
     * @brief Decodes a UTF-32 sequence from a range of input iterators.
     *
     * This function decodes a single Unicode codepoint from a sequence of UTF-32 encoded units.
     *
     * @param first The first iterator in the range.
     * @param last The last iterator in the range.
     * @param c The decoded Unicode codepoint (output).
     * @return The iterator past the last decoded unit, or the input iterator if the sequence is invalid.
     */
    template <typename IT>
    static IT decode(IT first, IT last, code_point& c)
    {
        VX_ITERATOR_VALID_RANGE(first, last);

        c = invalid_code_point;

        // Empty range
        if (VX_UNLIKELY(first == last))
        {
            return first;
        }

        c = static_cast<code_point>(*first++);
        if (VX_UNLIKELY(!is_valid_codepoint(c)))
        {
            c = invalid_code_point;
        }

        return first;
    }

    /**
     * @brief Encodes a Unicode codepoint into a UTF-32 sequence.
     *
     * This function encodes a single Unicode codepoint into a 32-bit unit in UTF-32 encoding.
     *
     * @param c The Unicode codepoint to encode.
     * @param out The output iterator where the encoded units will be written.
     * @param replacement A 32-bit value to use as a replacement for invalid codepoints (default is `0`).
     * @return The output iterator past the last encoded unit.
     */
    template <typename IT>
    static IT encode(code_point c, IT out, char_type replacement = 0)
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

using utf8_traits = utf_base_traits<1>;
using utf16_traits = utf_base_traits<2>;
using utf32_traits = utf_base_traits<4>;

} // namespace utf
} // namespace vx