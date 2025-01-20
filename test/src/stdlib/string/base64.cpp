#include "vertex/_test/test.hpp"

#include "vertex/util/encode/base64.hpp"

using namespace vx::test;

// Test for encoding empty data
VX_TEST(base64, encode_empty_data)
{
    std::string encoded;
    assert::that(vx::base64::encode(reinterpret_cast<const uint8_t*>(""), 0, encoded), "Encoding empty data should succeed");
    assert::that(encoded == "", "Encoded result should be empty for empty input");
}

// Test for encoding simple data
VX_TEST(base64, encode_simple_data)
{
    const uint8_t data[] = { 'a', 'b', 'c' };
    std::string encoded;
    assert::that(vx::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(encoded == "YWJj", "Encoded result should match Base64 for 'abc'");
}

// Test for encoding with padding
VX_TEST(base64, encode_with_padding)
{
    const uint8_t data[] = { 'a', 'b' }; // Two bytes, should pad
    std::string encoded;
    assert::that(vx::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(encoded == "YWI=", "Encoded result should match Base64 for 'ab' with padding");
}

// Test for decoding simple data
VX_TEST(base64, decode_simple_data)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::base64::decode("YWJj", decoded), "Decoding should succeed");
    assert::that(decoded.size() == 3, "Decoded size should be 3");
    assert::that(decoded[0] == 'a', "First byte should be 'a'");
    assert::that(decoded[1] == 'b', "Second byte should be 'b'");
    assert::that(decoded[2] == 'c', "Third byte should be 'c'");
}

// Test for decoding with padding
VX_TEST(base64, decode_with_padding)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::base64::decode("YWI=", decoded), "Decoding should succeed");
    assert::that(decoded.size() == 2, "Decoded size should be 2");
    assert::that(decoded[0] == 'a', "First byte should be 'a'");
    assert::that(decoded[1] == 'b', "Second byte should be 'b'");
}

// Test for invalid decoding
VX_TEST(base64, decode_invalid_input)
{
    std::vector<uint8_t> decoded;
    assert::that(!vx::base64::decode("InvalidBase64", decoded), "Decoding invalid input should fail");
    assert::that(decoded.size() == 0, "Decoded size should be 0 for invalid input");
}

// Test for decoding with validation
VX_TEST(base64, decode_with_validation)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::base64::decode("YWJj", decoded, true), "Decoding should succeed with validation");
    assert::that(decoded.size() == 3, "Decoded size should be 3");

    assert::that(!vx::base64::decode("InvalidBase64", decoded, true), "Decoding invalid input with validation should fail");
    assert::that(decoded.size() == 0, "Decoded size should be 0 for invalid input");
}

// Test for encode-decode round trip
VX_TEST(base64, encode_decode_round_trip)
{
    const uint8_t data[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    std::string encoded;
    std::vector<uint8_t> decoded;

    assert::that(vx::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(vx::base64::decode(encoded, decoded), "Decoding should succeed");

    assert::that(decoded.size() == sizeof(data), "Decoded size should match original data size");
    assert::that(std::memcmp(data, decoded.data(), sizeof(data)) == 0, "Decoded data should match original data");
}

// Test for encoding null pointer
VX_TEST(base64, encode_null_pointer)
{
    const uint8_t* data = nullptr;
    std::string encoded;
    assert::that(!vx::base64::encode(data, 5, encoded), "Encoding null pointer should fail");
}

// Test for decoding null pointer
VX_TEST(base64, decode_null_pointer)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::base64::decode("", decoded), "Decoding empty string should succeed");
    assert::that(decoded.size() == 0, "Decoded should have size of 0");
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    test_suite::instance().run_tests();

    return 0;
}