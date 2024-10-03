#pragma once

#include <locale>

namespace vx {
namespace str {

///////////////////////////////////////////////////////////////////////////////
// utf8
///////////////////////////////////////////////////////////////////////////////

namespace utf8 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint8_t replacement = 0);

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT begin, IT end);

template <typename IT>
inline size_t count(IT begin, IT end);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output);

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_ansi(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_ansi(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0, const std::locale& locale = std::locale());

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0);

} // namespace utf8

///////////////////////////////////////////////////////////////////////////////
// utf16
///////////////////////////////////////////////////////////////////////////////

namespace utf16 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint16_t replacement = 0);

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT begin, IT end);

template <typename IT>
inline size_t count(IT begin, IT end);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output);

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_ansi(IN_IT begin, IN_IT end, OUT_IT output, const std::locale& locale = std::locale());

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_ansi(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0, const std::locale& locale = std::locale());

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0);

} // namespace utf16

///////////////////////////////////////////////////////////////////////////////
// utf32
///////////////////////////////////////////////////////////////////////////////

namespace utf32 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT output, uint32_t replacement = 0);

template <typename IT>
inline IT decode(IT begin, IT end, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT begin, IT end);

template <typename IT>
inline size_t count(IT begin, IT end);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf8(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf16(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_utf32(IN_IT begin, IN_IT end, OUT_IT output);

///////////////////////////////////////////////////////////////////////////////
// ansi
///////////////////////////////////////////////////////////////////////////////

template <typename IT>
inline IT encode_ansi(uint32_t codepoint, IT output, char replacement = 0, const std::locale& locale = std::locale());

template <typename IT>
inline uint32_t decode_ansi(IT input, const std::locale& locale = std::locale());

///////////////////////////////////////////////////////////////////////////////
// latin-1
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_latin1(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_latin1(IN_IT begin, IN_IT end, OUT_IT output, char replacement = 0);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IN_IT, typename OUT_IT>
inline OUT_IT from_wide(IN_IT begin, IN_IT end, OUT_IT output);

template <typename IN_IT, typename OUT_IT>
inline OUT_IT to_wide(IN_IT begin, IN_IT end, OUT_IT output, wchar_t replacement = 0);

template <typename IT>
inline IT encode_wide(uint32_t codepoint, IT output, wchar_t replacement = 0);

template <typename T>
inline uint32_t decode_wide(T input);

} // namespace utf32

}
}

#include "utf.inl"