#include <cstring>

#include "vertex_test/test.hpp"
//#include "vertex/util/string/string.hpp"
//#include "vertex/util/string/string_compare.hpp"

#include "vertex/std/string_utils.hpp"

using namespace vx;

//VX_TEST_CASE(string_cast)
//{
//    const char* utf8_input = "Hello, \xE4\xB8\x96\xE7\x95\x8C!";
//    const wchar_t* wide_input = L"Hello, \x4E16\x754C!";
//    const std::u16string utf16_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0x4E16, 0x754C, '!' };
//    const std::u32string utf32_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0x4E16, 0x754C, '!' };
//
//    const std::string utf8_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', static_cast<char>(0xFF), static_cast<char>(0xFF), '!' };
//    const std::wstring wide_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', static_cast<wchar_t>(0xDFFF), static_cast<wchar_t>(0xD800), '!' };
//    const std::u16string utf16_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0xDFFF, 0xD800, '!' };
//    const std::u32string utf32_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0xDFFF, 0xD800, '!' };
//
//    const std::string utf8_invalid_corrected = "Hello, ??!";
//    const std::wstring wide_invalid_corrected = L"Hello, ??!";
//    const std::u16string utf16_invalid_corrected = u"Hello, ??!";
//    const std::u32string utf32_invalid_corrected = U"Hello, ??!";
//
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_SECTION("UTF-8 to UTF-8 Conversion")
//    {
//        VX_CHECK(str::string_cast<char>(utf8_input) == utf8_input);
//        VX_CHECK(str::string_cast<char>(utf8_invalid_input, '?') == utf8_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-8 to Wide Conversion")
//    {
//        VX_CHECK(str::string_cast<wchar_t>(utf8_input) == wide_input);
//        VX_CHECK(str::string_cast<wchar_t>(utf8_invalid_input, '?') == wide_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-8 to UTF-16 Conversion")
//    {
//        VX_CHECK(str::string_cast<char16_t>(utf8_input) == utf16_input);
//        VX_CHECK(str::string_cast<char16_t>(utf8_invalid_input, '?') == utf16_invalid_corrected);
//    }
//    
//    VX_SECTION("UTF-8 to UTF-32 Conversion")
//    {
//        VX_CHECK(str::string_cast<char32_t>(utf8_input) == utf32_input);
//        VX_CHECK(str::string_cast<char32_t>(utf8_invalid_input, '?') == utf32_invalid_corrected);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_SECTION("Wide to UTF-8 Conversion")
//    {
//        VX_CHECK(str::string_cast<char>(wide_input) == utf8_input);
//        VX_CHECK(str::string_cast<char>(wide_invalid_input, '?') == utf8_invalid_corrected);
//    }
//
//    VX_SECTION("Wide to Wide Conversion")
//    {
//        VX_CHECK(str::string_cast<wchar_t>(wide_input) == wide_input);
//        VX_CHECK(str::string_cast<wchar_t>(wide_invalid_input, '?') == wide_invalid_corrected);
//    }
//
//    VX_SECTION("Wide to UTF-16 Conversion")
//    {
//        VX_CHECK(str::string_cast<char16_t>(wide_input) == utf16_input);
//        VX_CHECK(str::string_cast<char16_t>(wide_invalid_input, '?') == utf16_invalid_corrected);
//    }
//
//    VX_SECTION("Wide to UTF-32 Conversion")
//    {
//        VX_CHECK(str::string_cast<char32_t>(wide_input) == utf32_input);
//        VX_CHECK(str::string_cast<char32_t>(wide_invalid_input, '?') == utf32_invalid_corrected);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_SECTION("UTF-16 to UTF-8 Conversion")
//    {
//        VX_CHECK(str::string_cast<char>(utf16_input) == utf8_input);
//        VX_CHECK(str::string_cast<char>(utf16_invalid_input, '?') == utf8_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-16 to Wide Conversion")
//    {
//        VX_CHECK(str::string_cast<wchar_t>(utf16_input) == wide_input);
//        VX_CHECK(str::string_cast<wchar_t>(utf16_invalid_input, '?') == wide_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-16 to UTF-16 Conversion")
//    {
//        VX_CHECK(str::string_cast<char16_t>(utf16_input) == utf16_input);
//        VX_CHECK(str::string_cast<char16_t>(utf16_invalid_input, '?') == utf16_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-16 to UTF-32 Conversion")
//    {
//        VX_CHECK(str::string_cast<char32_t>(utf16_input) == utf32_input);
//        VX_CHECK(str::string_cast<char32_t>(utf16_invalid_input, '?') == utf32_invalid_corrected);
//    }
//
//    ///////////////////////////////////////////////////////////////////////////////
//
//    VX_SECTION("UTF-32 to UTF-8 Conversion")
//    {
//        VX_CHECK(str::string_cast<char>(utf32_input) == utf8_input);
//        VX_CHECK(str::string_cast<char>(utf32_invalid_input, '?') == utf8_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-32 to Wide Conversion")
//    {
//        VX_CHECK(str::string_cast<wchar_t>(utf32_input) == wide_input);
//        VX_CHECK(str::string_cast<wchar_t>(utf32_invalid_input, '?') == wide_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-32 to UTF-16 Conversion")
//    {
//        VX_CHECK(str::string_cast<char16_t>(utf32_input) == utf16_input);
//        VX_CHECK(str::string_cast<char16_t>(utf32_invalid_input, '?') == utf16_invalid_corrected);
//    }
//
//    VX_SECTION("UTF-32 to UTF-32 Conversion")
//    {
//        VX_CHECK(str::string_cast<char32_t>(utf32_input) == utf32_input);
//        VX_CHECK(str::string_cast<char32_t>(utf32_invalid_input, '?') == utf32_invalid_corrected);
//    }
//}

///////////////////////////////////////////////////////////////////////////////

//VX_TEST_CASE(contains)
//{
//    VX_SECTION("char")
//    {
//        VX_CHECK(str::contains("Hello, World!", '!'));
//        VX_CHECK(!str::contains("Hello, World!", '.'));
//        VX_CHECK(!str::contains("", 'a'));
//    }
//
//    VX_SECTION("string")
//    {
//        VX_CHECK(str::contains("Hello, World!", "Hello"));
//        VX_CHECK(str::contains("Hello, World!", "Hello"));
//        VX_CHECK(!str::contains("", "Hello"));
//        VX_CHECK(str::contains("Hello", ""));
//    }
//}

VX_TEST_CASE(count)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::count("hello world", 'l') == 3);
        VX_CHECK(str::count("hello world", 'z') == 0);
        VX_CHECK(str::count("aaaaaa", 'a') == 6);
        VX_CHECK(str::count("hello, world!", '!') == 1);

        VX_CHECK(str::count("", 'a') == 0);
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::count("banana", "ana") == 1);
        VX_CHECK(str::count("aaaaaa", "aaa") == 2);
        VX_CHECK(str::count("hello world", "xyz") == 0);
        VX_CHECK(str::count("test", "test") == 1);
        VX_CHECK(str::count("short", "longer") == 0);
        VX_CHECK(str::count("abc!@#abc!@#", "!@#") == 2);

        VX_CHECK(str::count("hello", "") == 6);
        VX_CHECK(str::count("", "hello") == 0);
    }
}

VX_TEST_CASE(starts_with)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::starts_with("hello world", 'h'));
        VX_CHECK(!str::starts_with("hello world", 'w'));
        VX_CHECK(str::starts_with("12345", '1'));

        VX_CHECK(!str::starts_with("", 'a'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::starts_with("hello world", "hello"));
        VX_CHECK(!str::starts_with("hello world", "world"));
        VX_CHECK(str::starts_with("banana", "ban"));
        VX_CHECK(str::starts_with("123abc", "123"));
    }
}

VX_TEST_CASE(ends_with)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::ends_with("hello world", 'd'));
        VX_CHECK(!str::ends_with("hello world", 'h'));
        VX_CHECK(str::ends_with("12345", '5'));

        VX_CHECK(!str::ends_with("", 'a'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::ends_with("hello world", "world"));
        VX_CHECK(!str::ends_with("hello world", "hello"));
        VX_CHECK(str::ends_with("banana", "na"));
        VX_CHECK(str::ends_with("123abc", "abc"));

        VX_CHECK(str::ends_with("test", ""));
        VX_CHECK(!str::ends_with("", "test"));
    }
}

VX_TEST_CASE(is_alpha)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::is_alpha('a'));
        VX_STATIC_CHECK(str::is_alpha('Z'));
        VX_STATIC_CHECK(!str::is_alpha('1'));
        VX_STATIC_CHECK(!str::is_alpha('@'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::is_alpha("abcDEF"));
        VX_CHECK(!str::is_alpha("abc123"));
        VX_CHECK(!str::is_alpha("!@#"));

        VX_CHECK(!str::is_alpha(""));
    }
}

VX_TEST_CASE(is_digit)
{
    VX_STATIC_CHECK(str::is_digit('0'));
    VX_STATIC_CHECK(str::is_digit('9'));
    VX_STATIC_CHECK(!str::is_digit('a'));
    VX_STATIC_CHECK(!str::is_digit('!'));
}

VX_TEST_CASE(is_numeric)
{
    VX_CHECK(str::is_numeric("12345"));
    VX_CHECK(!str::is_numeric("123a45"));
    VX_CHECK(!str::is_numeric("12.45"));
    VX_CHECK(!str::is_numeric("!@#"));

    VX_CHECK(!str::is_numeric(""));
}

VX_TEST_CASE(is_alnum)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::is_alnum('a'));
        VX_STATIC_CHECK(str::is_alnum('Z'));
        VX_STATIC_CHECK(str::is_alnum('1'));
        VX_STATIC_CHECK(!str::is_alnum('@'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::is_alnum("abc123"));
        VX_CHECK(!str::is_alnum("abc 123"));
        VX_CHECK(!str::is_alnum("abc@123"));
        VX_CHECK(str::is_alnum("abc"));
        VX_CHECK(str::is_alnum("123"));
        VX_CHECK(!str::is_alnum("@"));

        VX_CHECK(!str::is_alnum(""));
    }
}

VX_TEST_CASE(is_ascii)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::is_ascii('a'));
        VX_STATIC_CHECK(str::is_ascii('Z'));
        VX_STATIC_CHECK(!str::is_ascii('\xF1'));
        VX_STATIC_CHECK(!str::is_ascii('\xA9'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::is_ascii("abc123"));
        VX_CHECK(!str::is_ascii("abc123\xF1"));
        VX_CHECK(!str::is_ascii("abc123\xA9"));
        VX_CHECK(str::is_ascii("ASCII"));

        VX_CHECK(!str::is_ascii(""));
    }
}

VX_TEST_CASE(is_space)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::is_space(' '));
        VX_STATIC_CHECK(str::is_space('\t'));
        VX_STATIC_CHECK(str::is_space('\n'));
        VX_STATIC_CHECK(!str::is_space('a'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::is_space(" \t\n"));
        VX_CHECK(!str::is_space("abc"));
        VX_CHECK(str::is_space("   "));
        VX_CHECK(!str::is_space(" space"));

        VX_CHECK(!str::is_space(""));
    }
}

VX_TEST_CASE(is_ctrl)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::is_ctrl('\0'));    // Null character (control)
        VX_STATIC_CHECK(str::is_ctrl('\x1F'));  // Unit Separator (control)
        VX_STATIC_CHECK(str::is_ctrl('\x7F'));  // Delete (control)
        VX_STATIC_CHECK(!str::is_ctrl('a'));    // Regular character
        VX_STATIC_CHECK(!str::is_ctrl(' '));    // Space is not control
        VX_STATIC_CHECK(!str::is_ctrl('1'));    // Digit is not control
    }
}

VX_TEST_CASE(is_hex_digit)
{
    VX_STATIC_CHECK(str::is_hex_digit('0'));
    VX_STATIC_CHECK(str::is_hex_digit('1'));
    VX_STATIC_CHECK(str::is_hex_digit('2'));
    VX_STATIC_CHECK(str::is_hex_digit('3'));
    VX_STATIC_CHECK(str::is_hex_digit('4'));
    VX_STATIC_CHECK(str::is_hex_digit('5'));
    VX_STATIC_CHECK(str::is_hex_digit('6'));
    VX_STATIC_CHECK(str::is_hex_digit('7'));
    VX_STATIC_CHECK(str::is_hex_digit('8'));
    VX_STATIC_CHECK(str::is_hex_digit('9'));
    VX_STATIC_CHECK(str::is_hex_digit('a'));
    VX_STATIC_CHECK(str::is_hex_digit('b'));
    VX_STATIC_CHECK(str::is_hex_digit('c'));
    VX_STATIC_CHECK(str::is_hex_digit('d'));
    VX_STATIC_CHECK(str::is_hex_digit('e'));
    VX_STATIC_CHECK(str::is_hex_digit('f'));
    VX_STATIC_CHECK(str::is_hex_digit('A'));
    VX_STATIC_CHECK(str::is_hex_digit('B'));
    VX_STATIC_CHECK(str::is_hex_digit('C'));
    VX_STATIC_CHECK(str::is_hex_digit('D'));
    VX_STATIC_CHECK(str::is_hex_digit('E'));
    VX_STATIC_CHECK(str::is_hex_digit('F'));
    VX_STATIC_CHECK(str::is_hex_digit('F'));

    VX_STATIC_CHECK(!str::is_hex_digit('G'));
    VX_STATIC_CHECK(!str::is_hex_digit('g'));
    VX_STATIC_CHECK(!str::is_hex_digit('!'));
    VX_STATIC_CHECK(!str::is_hex_digit('z'));
    VX_STATIC_CHECK(!str::is_hex_digit('&'));
    VX_STATIC_CHECK(!str::is_hex_digit('-'));
}

VX_TEST_CASE(is_hex)
{
    VX_CHECK(str::is_hex("123abc", false));
    VX_CHECK(str::is_hex("0x123abc", true));
    VX_CHECK(!str::is_hex("0x123abc", false));
    VX_CHECK(!str::is_hex("123xyz", false));
    VX_CHECK(!str::is_hex("123abcXYZ", false));

    VX_CHECK(!str::is_hex("0x", true));
    VX_CHECK(str::is_hex("0", true));

    VX_CHECK(!str::is_hex("", true));
    VX_CHECK(!str::is_hex("", false));
}

VX_TEST_CASE(to_lower)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::to_lower('A') == 'a');
        VX_STATIC_CHECK(str::to_lower('Z') == 'z');
        VX_STATIC_CHECK(str::to_lower('1') == '1');
        VX_STATIC_CHECK(str::to_lower('!') == '!');
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::to_lower_copy("Hello World") == "hello world");
        VX_CHECK(str::to_lower_copy("A1B2C3") == "a1b2c3");
        VX_CHECK(str::to_lower_copy("12345") == "12345");

        VX_CHECK(str::to_lower_copy("").empty());
    }
}

VX_TEST_CASE(to_upper)
{
    VX_SECTION("char")
    {
        VX_STATIC_CHECK(str::to_upper('a') == 'A');
        VX_STATIC_CHECK(str::to_upper('z') == 'Z');
        VX_STATIC_CHECK(str::to_upper('1') == '1');
        VX_STATIC_CHECK(str::to_upper('!') == '!');
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::to_upper_copy("hello world") == "HELLO WORLD");
        VX_CHECK(str::to_upper_copy("a1b2c3") == "A1B2C3");
        VX_CHECK(str::to_upper_copy("12345") == "12345");

        VX_CHECK(str::to_upper_copy("").empty());
    }
}

VX_TEST_CASE(to_title)
{
    VX_CHECK(str::to_title_copy("hello world") == "Hello World");
    VX_CHECK(str::to_title_copy("a simple test") == "A Simple Test");
    VX_CHECK(str::to_title_copy("HELLO") == "Hello");
    VX_CHECK(str::to_title_copy("tItLe") == "Title");
    VX_CHECK(str::to_title_copy("!!!!!hello&& &there!z") == "!!!!!Hello&& &There!Z");

    VX_CHECK(str::to_title_copy("").empty());
}

VX_TEST_CASE(case_compare)
{
    VX_CHECK(str::case_compare("aBcDeFgHiJkLmNoPqRsTuVwXyZ", "AbCdEfGhIjKlMnOpQrStUvWxYz") == 0);
    VX_CHECK(str::case_compare("123", "123") == 0);
    VX_CHECK(str::case_compare("abc", "abcd") == -1);
    VX_CHECK(str::case_compare("ABCD123", "abc") == +1);

    VX_CHECK(str::case_compare("", "") == 0);
}

VX_TEST_CASE(strip)
{
    VX_SECTION("string")
    {
        VX_CHECK(str::strip("  hello  ") == "hello");
        VX_CHECK(str::strip("  \t \n hello \n ") == "hello");
        VX_CHECK(str::strip("no_strip") == "no_strip");
        VX_CHECK(str::strip("\t\n\v\f\r").empty());

        VX_CHECK(str::strip("").empty());
    }

    VX_SECTION("custom characters")
    {
        VX_CHECK(str::strip("xxhelloxx", "x") == "hello");
        VX_CHECK(str::strip("xx123xx", "x") == "123");
        VX_CHECK(str::strip("aabbbbaa", "ab").empty());

        VX_CHECK(str::strip("", "ab").empty());
        VX_CHECK(str::strip("ab", "") == "ab");
    }
}

VX_TEST_CASE(lstrip)
{
    VX_SECTION("string")
    {
        VX_CHECK(str::lstrip("  hello  ") == "hello  ");
        VX_CHECK(str::lstrip("  \t \n hello \n ") == "hello \n ");
        VX_CHECK(str::lstrip("no_strip") == "no_strip");
        VX_CHECK(str::lstrip("\t\n\v\f\r").empty());

        VX_CHECK(str::lstrip("").empty());
    }

    VX_SECTION("custom characters")
    {
        VX_CHECK(str::lstrip("xxhelloxx", "x") == "helloxx");
        VX_CHECK(str::lstrip("xx123xx", "x") == "123xx");
        VX_CHECK(str::lstrip("aabbbbaa", "ab").empty());

        VX_CHECK(str::lstrip("", "ab").empty());
        VX_CHECK(str::lstrip("ab", "") == "ab");
    }
}

VX_TEST_CASE(rstrip)
{
    VX_SECTION("string")
    {
        VX_CHECK(str::rstrip("  hello  ") == "  hello");
        VX_CHECK(str::rstrip("  \t \n hello \n ") == "  \t \n hello");
        VX_CHECK(str::rstrip("no_strip") == "no_strip");
        VX_CHECK(str::rstrip("\t\n\v\f\r").empty());

        VX_CHECK(str::rstrip("").empty());
    }

    VX_SECTION("custom characters")
    {
        VX_CHECK(str::rstrip("xxhelloxx", "x") == "xxhello");
        VX_CHECK(str::rstrip("xx123xx", "x") == "xx123");
        VX_CHECK(str::rstrip("aabbbbaa", "ab").empty());

        VX_CHECK(str::rstrip("", "ab").empty());
        VX_CHECK(str::rstrip("ab", "") == "ab");
    }
}

VX_TEST_CASE(trim_prefix)
{
    VX_CHECK(str::trim_prefix("hello world", "hello") == " world");
    VX_CHECK(str::trim_prefix("abc123abc", "abc") == "123abc");
    VX_CHECK(str::trim_prefix("12345", "123") == "45");
    VX_CHECK(str::trim_prefix("test", "hello") == "test");

    VX_CHECK(str::trim_prefix("", "hello").empty());
    VX_CHECK(str::trim_prefix("hello", "") == "hello");
    VX_CHECK(str::trim_prefix("", "").empty());
}

VX_TEST_CASE(trim_suffix)
{
    VX_CHECK(str::trim_suffix("hello world", "world") == "hello ");
    VX_CHECK(str::trim_suffix("abc123abc", "abc") == "abc123");
    VX_CHECK(str::trim_suffix("12345", "45") == "123");
    VX_CHECK(str::trim_suffix("test", "hello") == "test");

    VX_CHECK(str::trim_prefix("", "hello").empty());
    VX_CHECK(str::trim_prefix("hello", "") == "hello");
    VX_CHECK(str::trim_prefix("", "").empty());
}

VX_TEST_CASE(remove_)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::remove_copy("hello world", 'o') == "hell wrld");
        VX_CHECK(str::remove_copy("abc123abc", 'a') == "bc123bc");
        VX_CHECK(str::remove_copy("12345", '5') == "1234");

        VX_CHECK(str::remove_copy("", 'a').empty());
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::remove_copy("hello world", "world") == "hello ");
        VX_CHECK(str::remove_copy("abc123abc", "abc") == "123");
        VX_CHECK(str::remove_copy("test", "hello") == "test");

        VX_CHECK(str::remove_copy("", "hello").empty());
        VX_CHECK(str::remove_copy("hello", "") == "hello");
        VX_CHECK(str::remove_copy("", "").empty());
    }
}

VX_TEST_CASE(replace)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::replace_copy("hello world", 'o', 'O') == "hellO wOrld");
        VX_CHECK(str::replace_copy("abc123abc", 'a', 'z') == "zbc123zbc");

        VX_CHECK(str::replace_copy("", 'a', 'z').empty());
    }

    VX_SECTION("string")
    {
        str::replace_copy("hello world", string_view("world"), string_view("everyone"));
        str::replace_copy("hello world", "world", "everyone");
        VX_CHECK(str::replace_copy("hello world", "world", "everyone") == "hello everyone");
        VX_CHECK(str::replace_copy("abc123abc", "abc", "xyz") == "xyz123xyz");

        VX_CHECK(str::replace_copy("", "abc", "xyz").empty());
        VX_CHECK(str::replace_copy("abc123abc", "", "xyz") == "xyzaxyzbxyzcxyz1xyz2xyz3xyzaxyzbxyzcxyz");
    }
}

VX_TEST_CASE(repeat)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::repeat('a', 5) == "aaaaa");
        VX_CHECK(str::repeat('a', 0).empty());
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::repeat("abc", 0).empty());
        VX_CHECK(str::repeat("abc", 1) == "abc");
        VX_CHECK(str::repeat("abc", 3) == "abcabcabc");
        VX_CHECK(str::repeat("", 1000).empty());
    }
}

VX_TEST_CASE(reverse)
{
    VX_CHECK(str::reverse_copy("hello") == "olleh");
    VX_CHECK(str::reverse_copy("abc123") == "321cba");
    VX_CHECK(str::reverse_copy("a") == "a");
    VX_CHECK(str::reverse_copy("").empty());
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}
