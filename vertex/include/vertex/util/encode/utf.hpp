#pragma once

namespace vx {

///////////////////////////////////////////////////////////////////////////////
// utf8
///////////////////////////////////////////////////////////////////////////////

namespace utf8 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint8_t replacement = 0);

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT first, IT last);

template <typename IT>
inline size_t count(IT first, IT last);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement = 0);

} // namespace utf8

///////////////////////////////////////////////////////////////////////////////
// utf16
///////////////////////////////////////////////////////////////////////////////

namespace utf16 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint16_t replacement = 0);

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT first, IT last);

template <typename IT>
inline size_t count(IT first, IT last);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement = 0);

} // namespace utf16

///////////////////////////////////////////////////////////////////////////////
// utf32
///////////////////////////////////////////////////////////////////////////////

namespace utf32 {

template <typename IT>
inline IT encode(uint32_t codepoint, IT out, uint32_t replacement = 0);

template <typename IT>
inline IT decode(IT first, IT last, uint32_t& codepoint, uint32_t replacement = 0);

template <typename IT>
inline IT next(IT first, IT last);

template <typename IT>
inline size_t count(IT first, IT last);

///////////////////////////////////////////////////////////////////////////////
// utf conversions
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 to_utf8(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf16(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_utf32(IT1 first, IT1 last, IT2 out);

///////////////////////////////////////////////////////////////////////////////
// wide
///////////////////////////////////////////////////////////////////////////////

template <typename IT1, typename IT2>
inline IT2 from_wide(IT1 first, IT1 last, IT2 out);

template <typename IT1, typename IT2>
inline IT2 to_wide(IT1 first, IT1 last, IT2 out, wchar_t replacement = 0);

template <typename IT>
inline IT encode_wide(uint32_t codepoint, IT out, wchar_t replacement = 0);

template <typename T>
inline uint32_t decode_wide(T input);

} // namespace utf32

} // namespace vx

#include "utf.inl"