#include "vertex/_test/test.hpp"

#include "vertex/util/encode/utf.hpp"

using namespace vx::test;

///////////////////////////////////////////////////////////////////////////////
// valid codepoint
///////////////////////////////////////////////////////////////////////////////

// Test all valid codepoints
VX_TEST(utf, is_valid_codepoint)
{
    // Iterate over the valid Unicode codepoints from 0x0000 to 0x10FFFF
    for (vx::utf::code_point cp = 0x0000; cp <= 0x10FFFF; ++cp)
    {
        if (cp >= 0xD800 && cp <= 0xDFFF)
        {
            // Surrogates should be invalid
            assert::that(!vx::utf::is_valid_codepoint(cp), "Surrogate codepoint should be invalid");
        }
        else
        {
            // Check if the codepoint is valid
            assert::that(vx::utf::is_valid_codepoint(cp), "Codepoint should be valid");
        }
    }
}

// Test utf8 lead and trail
VX_TEST(utf, lead_and_trail)
{
    // Check all possible byte values (0x00 to 0xFF)
    for (uint16_t byte = 0x00; byte <= 0xFF; ++byte)
    {
        const uint8_t b = static_cast<uint8_t>(byte);

        // Check if the byte is a valid lead byte (ASCII (0x00-0x7F) or multibyte leads (0xC2-0xF4))
        const bool is_lead = vx::utf::utf8_traits::is_lead(b);
        if ((b <= 0x7F) || (b >= 0xC2 && b <= 0xF4))
        {
            assert::that(is_lead, "Byte should be a valid UTF-8 lead byte");
        }
        else
        {
            assert::that(!is_lead, "Byte should not be a valid UTF-8 lead byte");
        }

        // Check if the byte is a valid trail byte (0x80-0xBF)
        const bool is_trail = vx::utf::utf8_traits::is_trail(b);
        if (b >= 0x80 && b <= 0xBF)
        {
            assert::that(is_trail, "Byte should be a valid UTF-8 trail byte");
        }
        else
        {
            assert::that(!is_trail, "Byte should not be a valid UTF-8 trail byte");
        }
    }
}

// Test for utf8 decode
VX_TEST(utf, utf8_decode)
{
    using traits = vx::utf::utf_traits<char>;

    const char* valid_input[] = {
        "\x24",                 // '$' (U+0024)
        "\xC2\xA2",             // '¢' (U+00A2)
        "\xE2\x82\xAC",         // '€' (U+20AC)
        "\xF0\x9F\x92\xA9"      // 'poop emoji' (U+1F4A9)
    };

    vx::utf::code_point expected_output[] = {
        0x24,    // '$'
        0x00A2,  // '¢'
        0x20AC,  // '€'
        0x1F4A9  // 'poop emoji'
    };

    // Test valid inputs
    for (size_t i = 0; i < sizeof(valid_input) / sizeof(valid_input[0]); ++i)
    {
        const char* input = valid_input[i];
        size_t size = 0;
        vx::utf::code_point result = traits::decode(std::begin(input), std::end(input));

        assert::that(result == expected_output[i], "Decoded code point should match expected value");
        assert::that(size == strlen(valid_input[i]), "Decoded size should match the length of the input");
    }

    // Test invalid inputs
    const char* invalid_input[] = {
        "\x80",         // Lone continuation byte
        "\xC0\x80",     // Overlong encoding for U+0000
        "\xE0\x80\x80", // Overlong encoding for U+0000
        "\xF8\x88\x80\x80\x80" // Invalid UTF-8 (more than 4 bytes)
    };
    for (const char* input : invalid_input)
    {
        size_t size = 0;
        vx::utf::code_point result = traits::decode(input, size);

        assert::that(result == traits::invalid_code_point, "Invalid input should result in invalid code point");
        assert::that(size == 1, "Size should advance by 1 for invalid input");
    }

    // Test empty input
    const char* empty_input = "";
    size_t size = 0;
    vx::utf::code_point result = traits::decode(empty_input, size);

    assert::that(result == traits::invalid_code_point, "Empty input should result in invalid code point");
    assert::that(size == 0, "Size should not advance for empty input");
}

int main()
{
    test_suite::instance().run_tests();

    return 0;
}