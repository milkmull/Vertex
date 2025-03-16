#include <cstring>

#include "vertex_test/test.hpp"
#include "vertex/util/string/string.hpp"
#include "vertex/util/string/string_compare.hpp"

using namespace vx;

VX_TEST_CASE(string_cast)
{
    const char* utf8_input = "Hello, \xE4\xB8\x96\xE7\x95\x8C!";
    const wchar_t* wide_input = L"Hello, \x4E16\x754C!";
    const std::u16string utf16_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0x4E16, 0x754C, '!' };
    const std::u32string utf32_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0x4E16, 0x754C, '!' };

    const std::string utf8_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', static_cast<char>(0xFF), static_cast<char>(0xFF), '!' };
    const std::wstring wide_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', static_cast<wchar_t>(0xDFFF), static_cast<wchar_t>(0xD800), '!' };
    const std::u16string utf16_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0xDFFF, 0xD800, '!' };
    const std::u32string utf32_invalid_input = { 'H', 'e', 'l', 'l', 'o', ',', ' ', 0xDFFF, 0xD800, '!' };

    const std::string utf8_invalid_corrected = "Hello, ??!";
    const std::wstring wide_invalid_corrected = L"Hello, ??!";
    const std::u16string utf16_invalid_corrected = u"Hello, ??!";
    const std::u32string utf32_invalid_corrected = U"Hello, ??!";

    ///////////////////////////////////////////////////////////////////////////////

    VX_SECTION("UTF-8 to UTF-8 Conversion")
    {
        VX_CHECK(str::string_cast<char>(utf8_input) == utf8_input);
        VX_CHECK(str::string_cast<char>(utf8_invalid_input, '?') == utf8_invalid_corrected);
    }

    VX_SECTION("UTF-8 to Wide Conversion")
    {
        VX_CHECK(str::string_cast<wchar_t>(utf8_input) == wide_input);
        VX_CHECK(str::string_cast<wchar_t>(utf8_invalid_input, '?') == wide_invalid_corrected);
    }

    VX_SECTION("UTF-8 to UTF-16 Conversion")
    {
        VX_CHECK(str::string_cast<char16_t>(utf8_input) == utf16_input);
        VX_CHECK(str::string_cast<char16_t>(utf8_invalid_input, '?') == utf16_invalid_corrected);
    }
    
    VX_SECTION("UTF-8 to UTF-32 Conversion")
    {
        VX_CHECK(str::string_cast<char32_t>(utf8_input) == utf32_input);
        VX_CHECK(str::string_cast<char32_t>(utf8_invalid_input, '?') == utf32_invalid_corrected);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VX_SECTION("Wide to UTF-8 Conversion")
    {
        VX_CHECK(str::string_cast<char>(wide_input) == utf8_input);
        VX_CHECK(str::string_cast<char>(wide_invalid_input, '?') == utf8_invalid_corrected);
    }

    VX_SECTION("Wide to Wide Conversion")
    {
        VX_CHECK(str::string_cast<wchar_t>(wide_input) == wide_input);
        VX_CHECK(str::string_cast<wchar_t>(wide_invalid_input, '?') == wide_invalid_corrected);
    }

    VX_SECTION("Wide to UTF-16 Conversion")
    {
        VX_CHECK(str::string_cast<char16_t>(wide_input) == utf16_input);
        VX_CHECK(str::string_cast<char16_t>(wide_invalid_input, '?') == utf16_invalid_corrected);
    }

    VX_SECTION("Wide to UTF-32 Conversion")
    {
        VX_CHECK(str::string_cast<char32_t>(wide_input) == utf32_input);
        VX_CHECK(str::string_cast<char32_t>(wide_invalid_input, '?') == utf32_invalid_corrected);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VX_SECTION("UTF-16 to UTF-8 Conversion")
    {
        VX_CHECK(str::string_cast<char>(utf16_input) == utf8_input);
        VX_CHECK(str::string_cast<char>(utf16_invalid_input, '?') == utf8_invalid_corrected);
    }

    VX_SECTION("UTF-16 to Wide Conversion")
    {
        VX_CHECK(str::string_cast<wchar_t>(utf16_input) == wide_input);
        VX_CHECK(str::string_cast<wchar_t>(utf16_invalid_input, '?') == wide_invalid_corrected);
    }

    VX_SECTION("UTF-16 to UTF-16 Conversion")
    {
        VX_CHECK(str::string_cast<char16_t>(utf16_input) == utf16_input);
        VX_CHECK(str::string_cast<char16_t>(utf16_invalid_input, '?') == utf16_invalid_corrected);
    }

    VX_SECTION("UTF-16 to UTF-32 Conversion")
    {
        VX_CHECK(str::string_cast<char32_t>(utf16_input) == utf32_input);
        VX_CHECK(str::string_cast<char32_t>(utf16_invalid_input, '?') == utf32_invalid_corrected);
    }

    ///////////////////////////////////////////////////////////////////////////////

    VX_SECTION("UTF-32 to UTF-8 Conversion")
    {
        VX_CHECK(str::string_cast<char>(utf32_input) == utf8_input);
        VX_CHECK(str::string_cast<char>(utf32_invalid_input, '?') == utf8_invalid_corrected);
    }

    VX_SECTION("UTF-32 to Wide Conversion")
    {
        VX_CHECK(str::string_cast<wchar_t>(utf32_input) == wide_input);
        VX_CHECK(str::string_cast<wchar_t>(utf32_invalid_input, '?') == wide_invalid_corrected);
    }

    VX_SECTION("UTF-32 to UTF-16 Conversion")
    {
        VX_CHECK(str::string_cast<char16_t>(utf32_input) == utf16_input);
        VX_CHECK(str::string_cast<char16_t>(utf32_invalid_input, '?') == utf16_invalid_corrected);
    }

    VX_SECTION("UTF-32 to UTF-32 Conversion")
    {
        VX_CHECK(str::string_cast<char32_t>(utf32_input) == utf32_input);
        VX_CHECK(str::string_cast<char32_t>(utf32_invalid_input, '?') == utf32_invalid_corrected);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(contains)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::contains("Hello, World!", '!'));
        VX_CHECK(!str::contains("Hello, World!", '.'));
        VX_CHECK(!str::contains("", 'a'));
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::contains("Hello, World!", "Hello"));
        VX_CHECK(str::contains("Hello, World!", "Hello"));
        VX_CHECK(!str::contains("", "Hello"));
        VX_CHECK(str::contains("Hello", ""));
    }
}

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
        VX_CHECK(str::is_alpha('a'));
        VX_CHECK(str::is_alpha('Z'));
        VX_CHECK(!str::is_alpha('1'));
        VX_CHECK(!str::is_alpha('@'));
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
    VX_CHECK(str::is_digit('0'));
    VX_CHECK(str::is_digit('9'));
    VX_CHECK(!str::is_digit('a'));
    VX_CHECK(!str::is_digit('!'));
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
        VX_CHECK(str::is_alnum('a'));
        VX_CHECK(str::is_alnum('Z'));
        VX_CHECK(str::is_alnum('1'));
        VX_CHECK(!str::is_alnum('@'));
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
        VX_CHECK(str::is_ascii('a'));
        VX_CHECK(str::is_ascii('Z'));
        VX_CHECK(!str::is_ascii('\xF1'));
        VX_CHECK(!str::is_ascii('\xA9'));
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
        VX_CHECK(str::is_space(' '));
        VX_CHECK(str::is_space('\t'));
        VX_CHECK(str::is_space('\n'));
        VX_CHECK(!str::is_space('a'));
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

VX_TEST_CASE(is_hex_digit)
{
    VX_CHECK(str::is_hex_digit('0'));
    VX_CHECK(str::is_hex_digit('1'));
    VX_CHECK(str::is_hex_digit('2'));
    VX_CHECK(str::is_hex_digit('3'));
    VX_CHECK(str::is_hex_digit('4'));
    VX_CHECK(str::is_hex_digit('5'));
    VX_CHECK(str::is_hex_digit('6'));
    VX_CHECK(str::is_hex_digit('7'));
    VX_CHECK(str::is_hex_digit('8'));
    VX_CHECK(str::is_hex_digit('9'));
    VX_CHECK(str::is_hex_digit('a'));
    VX_CHECK(str::is_hex_digit('b'));
    VX_CHECK(str::is_hex_digit('c'));
    VX_CHECK(str::is_hex_digit('d'));
    VX_CHECK(str::is_hex_digit('e'));
    VX_CHECK(str::is_hex_digit('f'));
    VX_CHECK(str::is_hex_digit('A'));
    VX_CHECK(str::is_hex_digit('B'));
    VX_CHECK(str::is_hex_digit('C'));
    VX_CHECK(str::is_hex_digit('D'));
    VX_CHECK(str::is_hex_digit('E'));
    VX_CHECK(str::is_hex_digit('F'));
    VX_CHECK(str::is_hex_digit('F'));

    VX_CHECK(!str::is_hex_digit('G'));
    VX_CHECK(!str::is_hex_digit('g'));
    VX_CHECK(!str::is_hex_digit('!'));
    VX_CHECK(!str::is_hex_digit('z'));
    VX_CHECK(!str::is_hex_digit('&'));
    VX_CHECK(!str::is_hex_digit('-'));
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
        VX_CHECK(str::to_lower('A') == 'a');
        VX_CHECK(str::to_lower('Z') == 'z');
        VX_CHECK(str::to_lower('1') == '1');
        VX_CHECK(str::to_lower('!') == '!');
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::to_lower("Hello World") == "hello world");
        VX_CHECK(str::to_lower("A1B2C3") == "a1b2c3");
        VX_CHECK(str::to_lower("12345") == "12345");

        VX_CHECK(str::to_lower("").empty());
    }
}

VX_TEST_CASE(to_upper)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::to_upper('a') == 'A');
        VX_CHECK(str::to_upper('z') == 'Z');
        VX_CHECK(str::to_upper('1') == '1');
        VX_CHECK(str::to_upper('!') == '!');
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::to_upper("hello world") == "HELLO WORLD");
        VX_CHECK(str::to_upper("a1b2c3") == "A1B2C3");
        VX_CHECK(str::to_upper("12345") == "12345");

        VX_CHECK(str::to_upper("").empty());
    }
}

VX_TEST_CASE(title)
{
    VX_CHECK(str::title("hello world") == "Hello World");
    VX_CHECK(str::title("a simple test") == "A Simple Test");
    VX_CHECK(str::title("HELLO") == "Hello");
    VX_CHECK(str::title("tItLe") == "Title");
    VX_CHECK(str::title("!!!!!hello&& &there!z") == "!!!!!Hello&& &There!Z");

    VX_CHECK(str::title("").empty());
}

VX_TEST_CASE(case_insensitive_compare)
{
    VX_CHECK(str::case_insensitive_compare("aBcDeFgHiJkLmNoPqRsTuVwXyZ", "AbCdEfGhIjKlMnOpQrStUvWxYz"));
    VX_CHECK(str::case_insensitive_compare("123", "123"));
    VX_CHECK(!str::case_insensitive_compare("abc", "abcd"));
    VX_CHECK(!str::case_insensitive_compare("abc", "ABCD123"));

    VX_CHECK(str::case_insensitive_compare("", ""));
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
        VX_CHECK(str::remove("hello world", 'o') == "hell wrld");
        VX_CHECK(str::remove("abc123abc", 'a') == "bc123bc");
        VX_CHECK(str::remove("12345", '5') == "1234");

        VX_CHECK(str::remove("", 'a').empty());
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::remove("hello world", "world") == "hello ");
        VX_CHECK(str::remove("abc123abc", "abc") == "123");
        VX_CHECK(str::remove("test", "hello") == "test");

        VX_CHECK(str::remove("", "hello").empty());
        VX_CHECK(str::remove("hello", "") == "hello");
        VX_CHECK(str::remove("", "").empty());
    }
}

VX_TEST_CASE(replace)
{
    VX_SECTION("char")
    {
        VX_CHECK(str::replace("hello world", 'o', 'O') == "hellO wOrld");
        VX_CHECK(str::replace("abc123abc", 'a', 'z') == "zbc123zbc");

        VX_CHECK(str::replace("", 'a', 'z').empty());
    }

    VX_SECTION("string")
    {
        VX_CHECK(str::replace("hello world", "world", "everyone") == "hello everyone");
        VX_CHECK(str::replace("abc123abc", "abc", "xyz") == "xyz123xyz");

        VX_CHECK(str::replace("", "abc", "xyz").empty());
        VX_CHECK(str::replace("abc123abc", "", "xyz") == "xyzaxyzbxyzcxyz1xyz2xyz3xyzaxyzbxyzcxyz");
    }
}

VX_TEST_CASE(concat)
{
    const std::vector<str::str_arg_t> v1 = { "hello", " ", "world" };
    VX_CHECK(str::concat(v1.begin(), v1.end()) == "hello world");

    const std::vector<str::str_arg_t> v2 = { "", "", "" };
    VX_CHECK(str::concat(v2.begin(), v2.end()).empty());
}

VX_TEST_CASE(join)
{
    VX_SECTION("string")
    {
        const std::vector<str::str_arg_t> v1 = { "hello", "world" };
        VX_CHECK(str::join(v1.begin(), v1.end(), ", ") == "hello, world");
    }

    VX_SECTION("custom delimiter")
    {
        const std::vector<str::str_arg_t> v1 = { "1", "2", "3" };
        VX_CHECK(str::join(v1.begin(), v1.end(), "-") == "1-2-3");
    }

    VX_SECTION("non-string")
    {
        const std::vector<int> v1 = { 1, 2, 3 };
        VX_CHECK(str::join(v1.begin(), v1.end(), false) == "10203");
    }

    VX_SECTION("empty vector")
    {
        const std::vector<str::str_arg_t> v1;
        VX_CHECK(str::join(v1.begin(), v1.end(), '-').empty());
    }
}

VX_TEST_CASE(split)
{
    VX_SECTION("char")
    {
        VX_CHECK((str::split("hello world", ' ') == std::vector<str::str_arg_t>{ "hello", "world" }));
        VX_CHECK((str::split("", ' ').empty()));
    }

    VX_SECTION("string")
    {
        VX_CHECK((str::split("hello,world,again", ",") == std::vector<str::str_arg_t>{ "hello", "world", "again" }));
        VX_CHECK((str::split("hello,world,again", "world") == std::vector<str::str_arg_t>{ "hello,", ",again" }));

        VX_CHECK((str::split("hello", "") == std::vector<str::str_arg_t>{ "h", "e", "l", "l", "o" }));
        VX_CHECK((str::split("", ",").empty()));
    }
}

VX_TEST_CASE(split_words)
{
    VX_CHECK((str::split_words("\t\nhello wor\vld, how ar\fe you?\r\r   ") == std::vector<str::str_arg_t>{ "hello", "wor", "ld,", "how", "ar", "e", "you?" }));
    VX_CHECK((str::split_words("\t\n\v  \f \r\r   ").empty()));
    VX_CHECK((str::split_words("").empty()));
}

VX_TEST_CASE(split_lines)
{
    VX_CHECK((str::split_lines("first line\nsecond line\nthird line\n") == std::vector<str::str_arg_t>{ "first line", "second line", "third line", "" }));
    VX_CHECK((str::split_lines("first line") == std::vector<str::str_arg_t>{ "first line" }));
    VX_CHECK((str::split_lines("").empty()));
    VX_CHECK((str::split_lines("\n") == std::vector<str::str_arg_t>{ "", "" }));
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
    VX_CHECK(str::reverse("hello") == "olleh");
    VX_CHECK(str::reverse("abc123") == "321cba");
    VX_CHECK(str::reverse("a") == "a");
    VX_CHECK(str::reverse("").empty());
}

VX_TEST_CASE(parse_digits)
{
    VX_SECTION("regular")
    {
        const std::string s = "123abc";
        int value = 0;
        size_t count = 0;
        const auto it = str::parse_digits(s.begin(), s.end(), value, &count);
        VX_CHECK(value == 123);
        VX_CHECK(count == 3);
        using diff_type = typename std::string::iterator::difference_type;
        VX_CHECK(it == s.begin() + static_cast<diff_type>(count));
    }

    VX_SECTION("overflow")
    {
        const std::string s = "1234";
        uint8_t value = 0;
        size_t count = 0;
        const auto it = str::parse_digits(s.begin(), s.end(), value, &count);
        VX_CHECK(value == 210);
        VX_CHECK(count == 4);
        using diff_type = typename std::string::iterator::difference_type;
        VX_CHECK(it == s.begin() + static_cast<diff_type>(count));
    }

    VX_SECTION("empty")
    {
        const std::string s = "";
        uint8_t value = 0;
        size_t count = 0;
        const auto it = str::parse_digits(s.begin(), s.end(), value, &count);
        VX_CHECK(value == 0);
        VX_CHECK(count == 0);
        using diff_type = typename std::string::iterator::difference_type;
        VX_CHECK(it == s.begin() + static_cast<diff_type>(count));
    }
}

VX_TEST_CASE(to_hex_string)
{
    constexpr uint8_t data[] = {
        0x01, 0x23, 0x45, 0x67,
        0x89, 0xAB, 0xCD, 0xEF
    };

    VX_CHECK(str::to_hex_string(data, sizeof(data)) == "0123456789ABCDEF");

    const uint8_t* empty_data = nullptr;
    VX_CHECK(str::to_hex_string(empty_data, 0).empty());
}

VX_TEST_CASE(to_int)
{
    VX_SECTION("int32")
    {
        VX_CHECK(str::to_int32("123") == 123);
        VX_CHECK(str::to_int32("+123") == 123);
        VX_CHECK(str::to_int32("-123") == -123);
        VX_CHECK(str::to_int32("  123   ") == 123);
        VX_CHECK(str::to_int32("00123") == 123);

        VX_CHECK(str::to_int32("7B", nullptr, 16) == 123); // Hexadecimal
        VX_CHECK(str::to_int32("1010", nullptr, 2) == 10); // Binary
        VX_CHECK(str::to_int32("0123", nullptr, 8) == 83); // Octal

        VX_EXPECT_ERROR_CODE(str::to_int32("abc"), err::INVALID_ARGUMENT);
        VX_CHECK_AND_EXPECT_NO_ERROR(str::to_int32("+2147483648") == INT32_MIN); // INT32_MAX + 1
        VX_CHECK_AND_EXPECT_NO_ERROR(str::to_int32("-2147483649") == INT32_MAX); // INT32_MIN - 1

        size_t count = 0;
        str::to_int32("  123", &count);
        VX_CHECK(count == 5);
    }

    VX_SECTION("int64")
    {
        VX_CHECK(str::to_int64("123") == 123);
        VX_CHECK(str::to_int64("+123") == 123);
        VX_CHECK(str::to_int64("-123") == -123);
        VX_CHECK(str::to_int64("  123   ") == 123);
        VX_CHECK(str::to_int64("00123") == 123);

        VX_CHECK(str::to_int64("7B", nullptr, 16) == 123); // Hexadecimal
        VX_CHECK(str::to_int64("1010", nullptr, 2) == 10); // Binary
        VX_CHECK(str::to_int64("0123", nullptr, 8) == 83); // Octal

        VX_EXPECT_ERROR_CODE(str::to_int64("abc"), err::INVALID_ARGUMENT);
        VX_EXPECT_ERROR_CODE(str::to_int64("+9223372036854775808"), err::OUT_OF_RANGE); // INT64_MAX + 1
        VX_EXPECT_ERROR_CODE(str::to_int64("-9223372036854775809"), err::OUT_OF_RANGE); // INT64_MIN - 1

        size_t count = 0;
        str::to_int64("  123", &count);
        VX_CHECK(count == 5);
    }
}

VX_TEST_CASE(to_uint)
{
    VX_SECTION("uint32")
    {
        VX_CHECK(str::to_uint32("123") == static_cast<uint32_t>(123));
        VX_CHECK(str::to_uint32("+123") == static_cast<uint32_t>(123));
        VX_CHECK(str::to_uint32("-123") == static_cast<uint32_t>(-123)); // Should wrap around
        VX_CHECK(str::to_uint32("  123   ") == static_cast<uint32_t>(123));
        VX_CHECK(str::to_uint32("00123") == static_cast<uint32_t>(123));

        VX_CHECK(str::to_uint32("7B", nullptr, 16) == static_cast<uint32_t>(123)); // Hexadecimal
        VX_CHECK(str::to_uint32("FFFFFFFF", nullptr, 16) == UINT32_MAX);
        VX_CHECK(str::to_uint32("1010", nullptr, 2) == static_cast<uint32_t>(10)); // Binary
        VX_CHECK(str::to_uint32("0123", nullptr, 8) == static_cast<uint32_t>(83)); // Octal

        VX_EXPECT_ERROR_CODE(str::to_uint32("abc"), err::INVALID_ARGUMENT);
        VX_CHECK_AND_EXPECT_NO_ERROR(str::to_uint32("+4294967296") == 0); // UINT32_MAX + 1

        size_t count = 0;
        str::to_uint32("  123", &count);
        VX_CHECK(count == 5);
    }

    VX_SECTION("uint64")
    {
        VX_CHECK(str::to_uint64("123") == static_cast<uint64_t>(123));
        VX_CHECK(str::to_uint64("+123") == static_cast<uint64_t>(123));
        VX_CHECK(str::to_uint64("-123") == static_cast<uint64_t>(-123)); // Should wrap around
        VX_CHECK(str::to_uint64("  123   ") == static_cast<uint64_t>(123));
        VX_CHECK(str::to_uint64("00123") == static_cast<uint64_t>(123));

        VX_CHECK(str::to_uint64("7B", nullptr, 16) == static_cast<uint64_t>(123)); // Hexadecimal
        VX_CHECK(str::to_uint64("FFFFFFFFFFFFFFFF", nullptr, 16) == UINT64_MAX);
        VX_CHECK(str::to_uint64("1010", nullptr, 2) == static_cast<uint64_t>(10)); // Binary
        VX_CHECK(str::to_uint64("0123", nullptr, 8) == static_cast<uint64_t>(83)); // Octal

        VX_EXPECT_ERROR_CODE(str::to_uint64("abc"), err::INVALID_ARGUMENT);
        VX_EXPECT_ERROR_CODE(str::to_uint64("+18446744073709551616"), err::OUT_OF_RANGE); // UINT64_MAX + 1

        size_t count = 0;
        str::to_uint64("  123", &count);
        VX_CHECK(count == 5);
    }
}

static bool is_equal_approx(const float a, const float b)
{
    return std::abs(a - b) <= 1e-6f;
}

static bool is_equal_approx(const double a, const double b)
{
    return std::abs(a - b) <= 1e-6;
}

VX_TEST_CASE(to_float)
{
    VX_SECTION("float")
    {
        VX_CHECK(str::to_float("123.45") == 123.45f);
        VX_CHECK(str::to_float("-123.45") == -123.45f);

        VX_CHECK(str::to_float("1.23e2") == 123.0f);
        VX_CHECK(str::to_float("-1.23E2") == -123.0f);

        VX_CHECK(str::to_float("-1.23E+2") == -123.0f);
        VX_CHECK(str::to_float("-1.23E-2") == -0.0123f);

        VX_CHECK(str::to_float("0") == 0.0f);
        VX_CHECK(str::to_float("0.0") == 0.0f);
        VX_CHECK(str::to_float("-0") == -0.0f);
        VX_CHECK(str::to_float("-0.0") == -0.0f);

        VX_CHECK(str::to_float("  123.45  ") == 123.45f);

        VX_EXPECT_ERROR_CODE(str::to_float("abc"), err::INVALID_ARGUMENT);
        VX_CHECK(is_equal_approx(str::to_float("1..23"), 1.0f));
        VX_CHECK(is_equal_approx(str::to_float("1.23e"), 1.23f));
        VX_CHECK(is_equal_approx(str::to_float("1.23e-"), 1.23f));

        VX_CHECK_AND_EXPECT_NO_ERROR(str::to_float("1e40") == std::numeric_limits<float>::infinity());
        VX_CHECK_AND_EXPECT_NO_ERROR(str::to_float("-1e40") == -std::numeric_limits<float>::infinity());

        size_t count = 0;
        str::to_float("  -123.45e-6", &count);
        VX_CHECK(count == 12);
    }

    VX_SECTION("double")
    {
        VX_CHECK(str::to_double("123.45") == 123.45);
        VX_CHECK(str::to_double("-123.45") == -123.45);

        VX_CHECK(str::to_double("1.23e2") == 123.0);
        VX_CHECK(str::to_double("-1.23E2") == -123.0);

        VX_CHECK(str::to_double("-1.23E+2") == -123.0);
        VX_CHECK(str::to_double("-1.23E-2") == -0.0123);

        VX_CHECK(str::to_double("0") == 0.0);
        VX_CHECK(str::to_double("0.0") == 0.0);
        VX_CHECK(str::to_double("-0") == -0.0);
        VX_CHECK(str::to_double("-0.0") == -0.0);

        VX_CHECK(str::to_double("  123.45  ") == 123.45);

        VX_EXPECT_ERROR_CODE(str::to_double("abc"), err::INVALID_ARGUMENT);
        VX_CHECK(is_equal_approx(str::to_double("1..23"), 1.0));
        VX_CHECK(is_equal_approx(str::to_double("1.23e"), 1.23));
        VX_CHECK(is_equal_approx(str::to_double("1.23e-"), 1.23));

        VX_EXPECT_ERROR_CODE(str::to_double("1e500"), err::OUT_OF_RANGE);
        VX_EXPECT_ERROR_CODE(str::to_double("-1e500"), err::OUT_OF_RANGE);

        size_t count = 0;
        str::to_double("  -123.45e-6", &count);
        VX_CHECK(count == 12);
    }
}

///////////////////////////////////////////////////////////////////////////////

VX_TEST_CASE(levenshtein_distance)
{
    VX_SECTION("Empty Strings")
    {
        const char* str1 = "";
        const char* str2 = "";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 0);
    }

    VX_SECTION("One Empty String")
    {
        const char* str1 = "abc";
        const char* str2 = "";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 3);
        VX_CHECK(str::levenshtein_distance(str2, str2 + std::strlen(str2), str1, str1 + std::strlen(str1)) == 3);
    }

    VX_SECTION("Identical Strings")
    {
        const char* str1 = "kitten";
        const char* str2 = "kitten";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 0);
    }

    VX_SECTION("Simple Replacements")
    {
        const char* str1 = "kitten";
        const char* str2 = "sitten";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);

        str1 = "flaw";
        str2 = "lawn";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 2);
    }

    VX_SECTION("Insertions and Deletions")
    {
        const char* str1 = "cat";
        const char* str2 = "cats";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);

        str1 = "cat";
        str2 = "at";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);
    }

    VX_SECTION("Mixed Operations")
    {
        const char* str1 = "kitten";
        const char* str2 = "sitting";
        VX_CHECK(str::levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 3);
    }
}

VX_TEST_CASE(damerau_levenshtein_distance)
{
    VX_SECTION("Empty Strings")
    {
        const char* str1 = "";
        const char* str2 = "";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 0);
    }

    VX_SECTION("One Empty String")
    {
        const char* str1 = "abc";
        const char* str2 = "";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 3);
        VX_CHECK(str::damerau_levenshtein_distance(str2, str2 + std::strlen(str2), str1, str1 + std::strlen(str1)) == 3);
    }

    VX_SECTION("Identical Strings")
    {
        const char* str1 = "kitten";
        const char* str2 = "kitten";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 0);
    }

    VX_SECTION("Simple Replacements")
    {
        const char* str1 = "kitten";
        const char* str2 = "sitten";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);

        str1 = "flaw";
        str2 = "lawn";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 2);
    }

    VX_SECTION("Transpositions")
    {
        const char* str1 = "abcd";
        const char* str2 = "abdc";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);

        str1 = "ca";
        str2 = "ac";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 1);
    }

    VX_SECTION("Mixed Operations")
    {
        const char* str1 = "kitten";
        const char* str2 = "sitting";
        VX_CHECK(str::damerau_levenshtein_distance(str1, str1 + std::strlen(str1), str2, str2 + std::strlen(str2)) == 3);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}