// Tests for vx::utf (utf_traits / utf_base_traits / is_valid_codepoint)
//
// NOTE: adjust the include paths below to match your project layout.

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

#include "vertex/std/utf/utf.hpp"
#include "vertex_test/test.hpp"

namespace vx {
namespace test {

namespace {

using utf::code_point;
using utf::invalid_code_point;
using utf::invalid_trail_count;

using u8 = utf::utf_traits<char>;
using u16 = utf::utf_traits<char16_t>;
using u32 = utf::utf_traits<char32_t>;

struct decode_result
{
    code_point cp;
    size_t consumed;
};

template <typename traits, typename C>
decode_result decode_one(const C* s, size_t n)
{
    code_point cp = 0;
    const C* end = traits::decode(s, s + n, cp);
    return { cp, static_cast<size_t>(end - s) };
}

std::string bytes(std::initializer_list<int> list)
{
    std::string s;
    for (int b : list)
    {
        s.push_back(static_cast<char>(b));
    }
    return s;
}

std::u16string units16(std::initializer_list<int> list)
{
    std::u16string s;
    for (int u : list)
    {
        s.push_back(static_cast<char16_t>(u));
    }
    return s;
}

std::string decode8(const std::string& s)
{
    // helper: decode as one code point; returns "consumed" through the result
    return s;
}

} // namespace

//=============================================================================
// is_valid_codepoint
//=============================================================================

VX_TEST_CASE(utf_is_valid_codepoint)
{
    VX_SECTION("valid");
    VX_CHECK(utf::is_valid_codepoint(0));
    VX_CHECK(utf::is_valid_codepoint('A'));
    VX_CHECK(utf::is_valid_codepoint(0x7F));
    VX_CHECK(utf::is_valid_codepoint(0x80));
    VX_CHECK(utf::is_valid_codepoint(0xD7FF));
    VX_CHECK(utf::is_valid_codepoint(0xE000));
    VX_CHECK(utf::is_valid_codepoint(0xFFFF));
    VX_CHECK(utf::is_valid_codepoint(0x10000));
    VX_CHECK(utf::is_valid_codepoint(0x10FFFF));

    VX_SECTION("surrogates are invalid");
    VX_CHECK(!utf::is_valid_codepoint(0xD800));
    VX_CHECK(!utf::is_valid_codepoint(0xDBFF));
    VX_CHECK(!utf::is_valid_codepoint(0xDC00));
    VX_CHECK(!utf::is_valid_codepoint(0xDFFF));

    VX_SECTION("out of range");
    VX_CHECK(!utf::is_valid_codepoint(0x110000));
    VX_CHECK(!utf::is_valid_codepoint(0x7FFFFFFF));
    VX_CHECK(!utf::is_valid_codepoint(invalid_code_point));

    VX_SECTION("constexpr");
    VX_STATIC_CHECK(utf::is_valid_codepoint(0x41));
    VX_STATIC_CHECK(!utf::is_valid_codepoint(0xD800));
    VX_STATIC_CHECK(!utf::is_valid_codepoint(0x110000));
}

//=============================================================================
// UTF-8 base traits
//=============================================================================

VX_TEST_CASE(utf8_trail_count)
{
    using t = utf::utf8_traits;

    VX_CHECK(t::trail_count(0x00) == 0);
    VX_CHECK(t::trail_count('A') == 0);
    VX_CHECK(t::trail_count(0x7F) == 0);

    VX_CHECK(t::trail_count(0xC2) == 1);
    VX_CHECK(t::trail_count(0xDF) == 1);

    VX_CHECK(t::trail_count(0xE0) == 2);
    VX_CHECK(t::trail_count(0xEF) == 2);

    VX_CHECK(t::trail_count(0xF0) == 3);
    VX_CHECK(t::trail_count(0xF4) == 3);

    // continuation bytes and invalid leads
    VX_CHECK(t::trail_count(0x80) == invalid_trail_count);
    VX_CHECK(t::trail_count(0xBF) == invalid_trail_count);
    VX_CHECK(t::trail_count(0xF8) == invalid_trail_count);
    VX_CHECK(t::trail_count(0xFF) == invalid_trail_count);

    VX_STATIC_CHECK(t::trail_count(0xE2) == 2);
}

VX_TEST_CASE(utf8_width)
{
    using t = utf::utf8_traits;

    VX_CHECK(t::max_width() == 4);

    VX_CHECK(t::width(0x00) == 1);
    VX_CHECK(t::width(0x7F) == 1);
    VX_CHECK(t::width(0x80) == 2);
    VX_CHECK(t::width(0x7FF) == 2);
    VX_CHECK(t::width(0x800) == 3);
    VX_CHECK(t::width(0xFFFF) == 3);
    VX_CHECK(t::width(0x10000) == 4);
    VX_CHECK(t::width(0x10FFFF) == 4);
}

VX_TEST_CASE(utf8_is_trail_is_lead)
{
    using t = utf::utf8_traits;

    VX_SECTION("is_trail");
    VX_CHECK(!t::is_trail(0x00));
    VX_CHECK(!t::is_trail(0x7F));
    VX_CHECK(t::is_trail(0x80));
    VX_CHECK(t::is_trail(0xBF));
    VX_CHECK(!t::is_trail(0xC0));
    VX_CHECK(!t::is_trail(0xFF));

    VX_SECTION("is_lead");
    VX_CHECK(t::is_lead(0x00));
    VX_CHECK(t::is_lead(0x7F));
    VX_CHECK(!t::is_lead(0x80));
    VX_CHECK(!t::is_lead(0xBF));
    VX_CHECK(!t::is_lead(0xC0)); // overlong
    VX_CHECK(!t::is_lead(0xC1)); // overlong
    VX_CHECK(t::is_lead(0xC2));
    VX_CHECK(t::is_lead(0xF4));
    VX_CHECK(!t::is_lead(0xF5));
    VX_CHECK(!t::is_lead(0xFF));
}

//=============================================================================
// UTF-8 decode
//=============================================================================

VX_TEST_CASE(utf8_decode_valid)
{
    VX_SECTION("1 byte");
    {
        const std::string s = bytes({ 0x41 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0x41);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("null character");
    {
        const std::string s = bytes({ 0x00 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("2 bytes (U+00E9)");
    {
        const std::string s = bytes({ 0xC3, 0xA9 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0xE9);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("3 bytes (U+20AC)");
    {
        const std::string s = bytes({ 0xE2, 0x82, 0xAC });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0x20AC);
        VX_CHECK(r.consumed == 3);
    }

    VX_SECTION("4 bytes (U+1F600)");
    {
        const std::string s = bytes({ 0xF0, 0x9F, 0x98, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0x1F600);
        VX_CHECK(r.consumed == 4);
    }

    VX_SECTION("boundaries");
    {
        const std::string a = bytes({ 0xC2, 0x80 });             // U+0080
        const std::string b = bytes({ 0xDF, 0xBF });             // U+07FF
        const std::string c = bytes({ 0xE0, 0xA0, 0x80 });       // U+0800
        const std::string d = bytes({ 0xEF, 0xBF, 0xBF });       // U+FFFF
        const std::string e = bytes({ 0xF0, 0x90, 0x80, 0x80 }); // U+10000
        const std::string f = bytes({ 0xF4, 0x8F, 0xBF, 0xBF }); // U+10FFFF

        VX_CHECK(decode_one<u8>(a.data(), a.size()).cp == 0x80);
        VX_CHECK(decode_one<u8>(b.data(), b.size()).cp == 0x7FF);
        VX_CHECK(decode_one<u8>(c.data(), c.size()).cp == 0x800);
        VX_CHECK(decode_one<u8>(d.data(), d.size()).cp == 0xFFFF);
        VX_CHECK(decode_one<u8>(e.data(), e.size()).cp == 0x10000);
        VX_CHECK(decode_one<u8>(f.data(), f.size()).cp == 0x10FFFF);
    }

    VX_SECTION("stops after one code point");
    {
        const std::string s = bytes({ 0xC3, 0xA9, 0x41 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == 0xE9);
        VX_CHECK(r.consumed == 2);
    }
}

VX_TEST_CASE(utf8_decode_empty)
{
    const char dummy = 0;
    auto r = decode_one<u8>(&dummy, 0);
    VX_CHECK(r.cp == invalid_code_point);
    VX_CHECK(r.consumed == 0);
}

VX_TEST_CASE(utf8_decode_invalid_lead)
{
    // lone continuation bytes and never-valid leads are consumed (1 byte), result invalid
    for (int b : { 0x80, 0xBF, 0xF8, 0xFB, 0xFC, 0xFE, 0xFF })
    {
        const std::string s = bytes({ b, 0x80, 0x80, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);
    }
}

VX_TEST_CASE(utf8_decode_truncated)
{
    VX_SECTION("2-byte lead with no trail");
    {
        const std::string s = bytes({ 0xC3 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("3-byte missing last byte");
    {
        const std::string s = bytes({ 0xE2, 0x82 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("4-byte missing last byte");
    {
        const std::string s = bytes({ 0xF0, 0x9F, 0x98 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 3);
    }
}

VX_TEST_CASE(utf8_decode_bad_trail)
{
    // The non-trail byte must NOT be consumed so it can be decoded on its own.
    VX_SECTION("ASCII instead of trail");
    {
        const std::string s = bytes({ 0xE2, 0x41 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("bad second trail");
    {
        const std::string s = bytes({ 0xE2, 0x82, 0x41 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("lead followed by lead");
    {
        const std::string s = bytes({ 0xC3, 0xC3, 0xA9 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);

        // the next decode recovers the valid sequence
        auto r2 = decode_one<u8>(s.data() + 1, s.size() - 1);
        VX_CHECK(r2.cp == 0xE9);
        VX_CHECK(r2.consumed == 2);
    }
}

VX_TEST_CASE(utf8_decode_overlong)
{
    // overlong encodings must be rejected (but fully consumed)
    VX_SECTION("2-byte overlong NUL (C0 80)");
    {
        const std::string s = bytes({ 0xC0, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("2-byte overlong '/' (C1 AF)");
    {
        const std::string s = bytes({ 0xC1, 0xAF });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("3-byte overlong (E0 80 80)");
    {
        const std::string s = bytes({ 0xE0, 0x80, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 3);
    }

    VX_SECTION("3-byte overlong (E0 9F BF)");
    {
        const std::string s = bytes({ 0xE0, 0x9F, 0xBF });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 3);
    }

    VX_SECTION("4-byte overlong (F0 80 80 80)");
    {
        const std::string s = bytes({ 0xF0, 0x80, 0x80, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 4);
    }

    VX_SECTION("4-byte overlong (F0 8F BF BF)");
    {
        const std::string s = bytes({ 0xF0, 0x8F, 0xBF, 0xBF });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 4);
    }
}

VX_TEST_CASE(utf8_decode_invalid_codepoints)
{
    VX_SECTION("encoded surrogate U+D800");
    {
        const std::string s = bytes({ 0xED, 0xA0, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 3);
    }

    VX_SECTION("encoded surrogate U+DFFF");
    {
        const std::string s = bytes({ 0xED, 0xBF, 0xBF });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 3);
    }

    VX_SECTION("U+110000");
    {
        const std::string s = bytes({ 0xF4, 0x90, 0x80, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 4);
    }

    VX_SECTION("F5 lead (beyond Unicode range)");
    {
        const std::string s = bytes({ 0xF5, 0x80, 0x80, 0x80 });
        auto r = decode_one<u8>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 4);
    }
}

VX_TEST_CASE(utf8_decode_sequence)
{
    // "A" "é" "€" "😀"
    const std::string s = bytes({ 0x41, 0xC3, 0xA9, 0xE2, 0x82, 0xAC, 0xF0, 0x9F, 0x98, 0x80 });
    const code_point expected[] = { 0x41, 0xE9, 0x20AC, 0x1F600 };

    auto it = s.begin();
    for (code_point e : expected)
    {
        code_point cp = 0;
        it = u8::decode(it, s.end(), cp);
        VX_CHECK(cp == e);
    }
    VX_CHECK(it == s.end());
}

VX_TEST_CASE(utf8_decode_unsigned_char_input)
{
    const unsigned char s[] = { 0xE2, 0x82, 0xAC };
    code_point cp = 0;
    const unsigned char* end = u8::decode(s, s + 3, cp);
    VX_CHECK(cp == 0x20AC);
    VX_CHECK(end == s + 3);
}

//=============================================================================
// UTF-8 encode
//=============================================================================

VX_TEST_CASE(utf8_encode)
{
    auto enc = [](code_point c, char replacement = 0)
    {
        std::string out;
        u8::encode(c, std::back_inserter(out), replacement);
        return out;
    };

    VX_CHECK(enc(0x00) == bytes({ 0x00 }));
    VX_CHECK(enc(0x41) == bytes({ 0x41 }));
    VX_CHECK(enc(0x7F) == bytes({ 0x7F }));

    VX_CHECK(enc(0x80) == bytes({ 0xC2, 0x80 }));
    VX_CHECK(enc(0xE9) == bytes({ 0xC3, 0xA9 }));
    VX_CHECK(enc(0x7FF) == bytes({ 0xDF, 0xBF }));

    VX_CHECK(enc(0x800) == bytes({ 0xE0, 0xA0, 0x80 }));
    VX_CHECK(enc(0x20AC) == bytes({ 0xE2, 0x82, 0xAC }));
    VX_CHECK(enc(0xD7FF) == bytes({ 0xED, 0x9F, 0xBF }));
    VX_CHECK(enc(0xE000) == bytes({ 0xEE, 0x80, 0x80 }));
    VX_CHECK(enc(0xFFFF) == bytes({ 0xEF, 0xBF, 0xBF }));

    VX_CHECK(enc(0x10000) == bytes({ 0xF0, 0x90, 0x80, 0x80 }));
    VX_CHECK(enc(0x1F600) == bytes({ 0xF0, 0x9F, 0x98, 0x80 }));
    VX_CHECK(enc(0x10FFFF) == bytes({ 0xF4, 0x8F, 0xBF, 0xBF }));
}

VX_TEST_CASE(utf8_encode_invalid_uses_replacement)
{
    auto enc = [](code_point c, char replacement)
    {
        std::string out;
        u8::encode(c, std::back_inserter(out), replacement);
        return out;
    };

    VX_CHECK(enc(0xD800, '?') == "?");
    VX_CHECK(enc(0xDFFF, '?') == "?");
    VX_CHECK(enc(0x110000, '?') == "?");
    VX_CHECK(enc(invalid_code_point, '?') == "?");

    // default replacement is 0
    std::string out;
    u8::encode(0xD800, std::back_inserter(out));
    VX_CHECK(out.size() == 1);
    VX_CHECK(out[0] == '\0');
}

VX_TEST_CASE(utf8_encode_returns_end_iterator)
{
    char buf[8] = {};
    VX_CHECK(u8::encode(0x41, buf) == buf + 1);
    VX_CHECK(u8::encode(0xE9, buf) == buf + 2);
    VX_CHECK(u8::encode(0x20AC, buf) == buf + 3);
    VX_CHECK(u8::encode(0x1F600, buf) == buf + 4);
    VX_CHECK(u8::encode(0xD800, buf, '?') == buf + 1);
}

#if defined(__cpp_lib_char8_t)
VX_TEST_CASE(utf8_traits_char8_t)
{
    using t = utf::utf_traits<char8_t>;

    std::u8string out;
    t::encode(0x20AC, std::back_inserter(out));
    VX_CHECK(out.size() == 3);
    VX_CHECK(out[0] == 0xE2);
    VX_CHECK(out[1] == 0x82);
    VX_CHECK(out[2] == 0xAC);

    code_point cp = 0;
    auto it = t::decode(out.begin(), out.end(), cp);
    VX_CHECK(cp == 0x20AC);
    VX_CHECK(it == out.end());
}
#endif

//=============================================================================
// UTF-16 base traits
//=============================================================================

VX_TEST_CASE(utf16_surrogate_predicates)
{
    using t = utf::utf16_traits;

    VX_SECTION("is_first_surrogate");
    VX_CHECK(!t::is_first_surrogate(0xD7FF));
    VX_CHECK(t::is_first_surrogate(0xD800));
    VX_CHECK(t::is_first_surrogate(0xDBFF));
    VX_CHECK(!t::is_first_surrogate(0xDC00));
    VX_CHECK(!t::is_first_surrogate(0x0041));

    VX_SECTION("is_second_surrogate");
    VX_CHECK(!t::is_second_surrogate(0xDBFF));
    VX_CHECK(t::is_second_surrogate(0xDC00));
    VX_CHECK(t::is_second_surrogate(0xDFFF));
    VX_CHECK(!t::is_second_surrogate(0xE000));
    VX_CHECK(!t::is_second_surrogate(0x0041));

    VX_SECTION("is_trail / is_lead");
    VX_CHECK(!t::is_trail(0x0041));
    VX_CHECK(!t::is_trail(0xD800));
    VX_CHECK(t::is_trail(0xDC00));
    VX_CHECK(t::is_trail(0xDFFF));

    VX_CHECK(t::is_lead(0x0041));
    VX_CHECK(t::is_lead(0xD800));
    VX_CHECK(t::is_lead(0xDBFF));
    VX_CHECK(!t::is_lead(0xDC00));
    VX_CHECK(!t::is_lead(0xDFFF));
    VX_CHECK(t::is_lead(0xE000));
}

VX_TEST_CASE(utf16_combine_surrogate)
{
    using t = utf::utf16_traits;

    VX_CHECK(t::combine_surrogate(0xD800, 0xDC00) == 0x10000);
    VX_CHECK(t::combine_surrogate(0xD83D, 0xDE00) == 0x1F600);
    VX_CHECK(t::combine_surrogate(0xDBFF, 0xDFFF) == 0x10FFFF);

    VX_STATIC_CHECK(t::combine_surrogate(0xD83D, 0xDE00) == 0x1F600);
}

VX_TEST_CASE(utf16_trail_count_and_width)
{
    using t = utf::utf16_traits;

    VX_CHECK(t::trail_count(0x0041) == 0);
    VX_CHECK(t::trail_count(0xD7FF) == 0);
    VX_CHECK(t::trail_count(0xE000) == 0);
    VX_CHECK(t::trail_count(0xD800) == 1);
    VX_CHECK(t::trail_count(0xDBFF) == 1);
    VX_CHECK(t::trail_count(0xDC00) == invalid_trail_count);
    VX_CHECK(t::trail_count(0xDFFF) == invalid_trail_count);

    VX_CHECK(t::max_width() == 2);
    VX_CHECK(t::width(0x0000) == 1);
    VX_CHECK(t::width(0xFFFF) == 1);
    VX_CHECK(t::width(0x10000) == 2);
    VX_CHECK(t::width(0x10FFFF) == 2);
}

//=============================================================================
// UTF-16 decode
//=============================================================================

VX_TEST_CASE(utf16_decode_valid)
{
    VX_SECTION("BMP");
    {
        const std::u16string s = units16({ 0x0041 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == 0x41);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("BMP around surrogate range");
    {
        const std::u16string a = units16({ 0xD7FF });
        const std::u16string b = units16({ 0xE000 });
        const std::u16string c = units16({ 0xFFFF });
        VX_CHECK(decode_one<u16>(a.data(), 1).cp == 0xD7FF);
        VX_CHECK(decode_one<u16>(b.data(), 1).cp == 0xE000);
        VX_CHECK(decode_one<u16>(c.data(), 1).cp == 0xFFFF);
    }

    VX_SECTION("surrogate pair");
    {
        const std::u16string s = units16({ 0xD83D, 0xDE00 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == 0x1F600);
        VX_CHECK(r.consumed == 2);
    }

    VX_SECTION("surrogate pair boundaries");
    {
        const std::u16string lo = units16({ 0xD800, 0xDC00 });
        const std::u16string hi = units16({ 0xDBFF, 0xDFFF });
        VX_CHECK(decode_one<u16>(lo.data(), 2).cp == 0x10000);
        VX_CHECK(decode_one<u16>(hi.data(), 2).cp == 0x10FFFF);
    }

    VX_SECTION("stops after one code point");
    {
        const std::u16string s = units16({ 0xD83D, 0xDE00, 0x0041 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == 0x1F600);
        VX_CHECK(r.consumed == 2);
    }
}

VX_TEST_CASE(utf16_decode_empty)
{
    const char16_t dummy = 0;
    auto r = decode_one<u16>(&dummy, 0);
    VX_CHECK(r.cp == invalid_code_point);
    VX_CHECK(r.consumed == 0);
}

VX_TEST_CASE(utf16_decode_invalid)
{
    VX_SECTION("lone second surrogate");
    {
        const std::u16string s = units16({ 0xDC00, 0x0041 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("first surrogate at end of input");
    {
        const std::u16string s = units16({ 0xD83D });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("first surrogate followed by BMP char (not consumed)");
    {
        const std::u16string s = units16({ 0xD83D, 0x0041 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);

        auto r2 = decode_one<u16>(s.data() + 1, 1);
        VX_CHECK(r2.cp == 0x41);
    }

    VX_SECTION("first surrogate followed by first surrogate");
    {
        const std::u16string s = units16({ 0xD800, 0xD83D, 0xDE00 });
        auto r = decode_one<u16>(s.data(), s.size());
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 1);

        auto r2 = decode_one<u16>(s.data() + 1, 2);
        VX_CHECK(r2.cp == 0x1F600);
        VX_CHECK(r2.consumed == 2);
    }
}

VX_TEST_CASE(utf16_decode_sequence)
{
    // "A" "é" "€" "😀"
    const std::u16string s = units16({ 0x0041, 0x00E9, 0x20AC, 0xD83D, 0xDE00 });
    const code_point expected[] = { 0x41, 0xE9, 0x20AC, 0x1F600 };

    auto it = s.begin();
    for (code_point e : expected)
    {
        code_point cp = 0;
        it = u16::decode(it, s.end(), cp);
        VX_CHECK(cp == e);
    }
    VX_CHECK(it == s.end());
}

//=============================================================================
// UTF-16 encode
//=============================================================================

VX_TEST_CASE(utf16_encode)
{
    auto enc = [](code_point c, char16_t replacement = 0)
    {
        std::u16string out;
        u16::encode(c, std::back_inserter(out), replacement);
        return out;
    };

    VX_CHECK(enc(0x0000) == units16({ 0x0000 }));
    VX_CHECK(enc(0x0041) == units16({ 0x0041 }));
    VX_CHECK(enc(0x20AC) == units16({ 0x20AC }));
    VX_CHECK(enc(0xD7FF) == units16({ 0xD7FF }));
    VX_CHECK(enc(0xE000) == units16({ 0xE000 }));
    VX_CHECK(enc(0xFFFF) == units16({ 0xFFFF }));

    VX_CHECK(enc(0x10000) == units16({ 0xD800, 0xDC00 }));
    VX_CHECK(enc(0x1F600) == units16({ 0xD83D, 0xDE00 }));
    VX_CHECK(enc(0x10FFFF) == units16({ 0xDBFF, 0xDFFF }));
}

VX_TEST_CASE(utf16_encode_invalid_uses_replacement)
{
    auto enc = [](code_point c, char16_t replacement)
    {
        std::u16string out;
        u16::encode(c, std::back_inserter(out), replacement);
        return out;
    };

    VX_CHECK(enc(0xD800, 0xFFFD) == units16({ 0xFFFD }));
    VX_CHECK(enc(0xDFFF, 0xFFFD) == units16({ 0xFFFD }));
    VX_CHECK(enc(0x110000, 0xFFFD) == units16({ 0xFFFD }));
    VX_CHECK(enc(invalid_code_point, 0xFFFD) == units16({ 0xFFFD }));

    std::u16string out;
    u16::encode(0xD800, std::back_inserter(out));
    VX_CHECK(out.size() == 1);
    VX_CHECK(out[0] == 0);
}

VX_TEST_CASE(utf16_encode_returns_end_iterator)
{
    char16_t buf[4] = {};
    VX_CHECK(u16::encode(0x41, buf) == buf + 1);
    VX_CHECK(u16::encode(0x1F600, buf) == buf + 2);
    VX_CHECK(u16::encode(0xD800, buf, 0xFFFD) == buf + 1);
}

//=============================================================================
// UTF-32
//=============================================================================

VX_TEST_CASE(utf32_base_traits)
{
    using t = utf::utf32_traits;

    VX_CHECK(t::trail_count(0x41) == 0);
    VX_CHECK(t::trail_count(0x10FFFF) == 0);
    VX_CHECK(t::trail_count(0xD800) == invalid_trail_count);
    VX_CHECK(t::trail_count(0x110000) == invalid_trail_count);

    VX_CHECK(!t::is_trail(0x41));
    VX_CHECK(!t::is_trail(0xDC00));
    VX_CHECK(t::is_lead(0x41));
    VX_CHECK(t::is_lead(0xDC00));

    VX_CHECK(t::max_width() == 1);
    VX_CHECK(t::width(0x41) == 1);
    VX_CHECK(t::width(0x10FFFF) == 1);
}

VX_TEST_CASE(utf32_decode)
{
    VX_SECTION("valid");
    {
        const char32_t s[] = { 0x1F600, 0x41 };
        auto r = decode_one<u32>(s, 2);
        VX_CHECK(r.cp == 0x1F600);
        VX_CHECK(r.consumed == 1);
    }

    VX_SECTION("boundaries");
    {
        const char32_t a[] = { 0x0 };
        const char32_t b[] = { 0xD7FF };
        const char32_t c[] = { 0xE000 };
        const char32_t d[] = { 0x10FFFF };
        VX_CHECK(decode_one<u32>(a, 1).cp == 0x0);
        VX_CHECK(decode_one<u32>(b, 1).cp == 0xD7FF);
        VX_CHECK(decode_one<u32>(c, 1).cp == 0xE000);
        VX_CHECK(decode_one<u32>(d, 1).cp == 0x10FFFF);
    }

    VX_SECTION("empty");
    {
        const char32_t dummy = 0;
        auto r = decode_one<u32>(&dummy, 0);
        VX_CHECK(r.cp == invalid_code_point);
        VX_CHECK(r.consumed == 0);
    }

    VX_SECTION("invalid values are consumed but flagged invalid");
    {
        const char32_t a[] = { 0xD800 };
        const char32_t b[] = { 0xDFFF };
        const char32_t c[] = { 0x110000 };
        const char32_t d[] = { 0xFFFFFFFF };

        for (const char32_t* p : { a, b, c, d })
        {
            auto r = decode_one<u32>(p, 1);
            VX_CHECK(r.cp == invalid_code_point);
            VX_CHECK(r.consumed == 1);
        }
    }
}

VX_TEST_CASE(utf32_encode)
{
    auto enc = [](code_point c, char32_t replacement = 0)
    {
        std::u32string out;
        u32::encode(c, std::back_inserter(out), replacement);
        return out;
    };

    VX_CHECK(enc(0x41) == std::u32string(1, U'A'));
    VX_CHECK(enc(0x1F600) == std::u32string(1, static_cast<char32_t>(0x1F600)));
    VX_CHECK(enc(0x10FFFF) == std::u32string(1, static_cast<char32_t>(0x10FFFF)));

    VX_CHECK(enc(0xD800, 0xFFFD) == std::u32string(1, static_cast<char32_t>(0xFFFD)));
    VX_CHECK(enc(0x110000, 0xFFFD) == std::u32string(1, static_cast<char32_t>(0xFFFD)));
    VX_CHECK(enc(invalid_code_point, 0xFFFD) == std::u32string(1, static_cast<char32_t>(0xFFFD)));

    char32_t buf[2] = {};
    VX_CHECK(u32::encode(0x41, buf) == buf + 1);
}

//=============================================================================
// Round trips
//=============================================================================

VX_TEST_CASE(utf_roundtrip_all_valid_codepoints)
{
    for (code_point c = 0; c <= 0x10FFFF; ++c)
    {
        if (!utf::is_valid_codepoint(c))
        {
            continue;
        }

        // UTF-8
        {
            char buf[4] = {};
            char* end = u8::encode(c, buf);
            const size_t len = static_cast<size_t>(end - buf);
            VX_CHECK(len == u8::width(c));

            code_point out = 0;
            const char* dec_end = u8::decode(static_cast<const char*>(buf), static_cast<const char*>(end), out);
            VX_CHECK(out == c);
            VX_CHECK(dec_end == end);
        }

        // UTF-16
        {
            char16_t buf[2] = {};
            char16_t* end = u16::encode(c, buf);
            const size_t len = static_cast<size_t>(end - buf);
            VX_CHECK(len == u16::width(c));

            code_point out = 0;
            const char16_t* dec_end = u16::decode(static_cast<const char16_t*>(buf), static_cast<const char16_t*>(end), out);
            VX_CHECK(out == c);
            VX_CHECK(dec_end == end);
        }

        // UTF-32
        {
            char32_t buf[1] = {};
            char32_t* end = u32::encode(c, buf);
            VX_CHECK(end == buf + 1);

            code_point out = 0;
            u32::decode(static_cast<const char32_t*>(buf), static_cast<const char32_t*>(end), out);
            VX_CHECK(out == c);
        }
    }
}

VX_TEST_CASE(utf_transcode_utf8_to_utf16_to_utf32)
{
    // "Aé€😀" in all three encodings
    const std::string s8 = bytes({ 0x41, 0xC3, 0xA9, 0xE2, 0x82, 0xAC, 0xF0, 0x9F, 0x98, 0x80 });

    std::u16string s16;
    std::u32string s32;

    for (auto it = s8.begin(); it != s8.end();)
    {
        code_point cp = 0;
        it = u8::decode(it, s8.end(), cp);
        VX_CHECK(cp != invalid_code_point);
        u16::encode(cp, std::back_inserter(s16));
        u32::encode(cp, std::back_inserter(s32));
    }

    VX_CHECK(s16 == units16({ 0x0041, 0x00E9, 0x20AC, 0xD83D, 0xDE00 }));
    VX_CHECK(s32.size() == 4);
    VX_CHECK(s32[0] == 0x41);
    VX_CHECK(s32[1] == 0xE9);
    VX_CHECK(s32[2] == 0x20AC);
    VX_CHECK(s32[3] == 0x1F600);

    // and back to UTF-8 from UTF-16
    std::string back;
    for (auto it = s16.begin(); it != s16.end();)
    {
        code_point cp = 0;
        it = u16::decode(it, s16.end(), cp);
        u8::encode(cp, std::back_inserter(back));
    }
    VX_CHECK(back == s8);
}

} // namespace test
} // namespace vx

//==============================================================================

int main()
{
    VX_RUN_TESTS();
    return 0;
}
