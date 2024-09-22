#include "vertex/test/test.hpp"

#include "vertex/stdlib/string/base64.hpp"

#include "vertex/stdlib/crypto/MD5.hpp"

using namespace vx::test;

// Test for encoding empty data
VX_TEST(base64, encode_empty_data)
{
    std::string encoded;
    assert::that(vx::str::base64::encode("", 0, encoded), "Encoding empty data should succeed");
    assert::that(encoded == "", "Encoded result should be empty for empty input");
}

// Test for encoding simple data
VX_TEST(base64, encode_simple_data)
{
    const uint8_t data[] = { 'a', 'b', 'c' };
    std::string encoded;
    assert::that(vx::str::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(encoded == "YWJj", "Encoded result should match Base64 for 'abc'");
}

// Test for encoding with padding
VX_TEST(base64, encode_with_padding)
{
    const uint8_t data[] = { 'a', 'b' }; // Two bytes, should pad
    std::string encoded;
    assert::that(vx::str::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(encoded == "YWI=", "Encoded result should match Base64 for 'ab' with padding");
}

// Test for decoding simple data
VX_TEST(base64, decode_simple_data)
{
    std::vector<uint8_t> decoded;
    vx::str::base64::decode("YWJj", decoded);
    assert::that(vx::str::base64::decode("YWJj", decoded), "Decoding should succeed");
    assert::that(decoded.size() == 3, "Decoded size should be 3");
    assert::that(decoded[0] == 'a', "First byte should be 'a'");
    assert::that(decoded[1] == 'b', "Second byte should be 'b'");
    assert::that(decoded[2] == 'c', "Third byte should be 'c'");
}

// Test for decoding with padding
VX_TEST(base64, decode_with_padding)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::str::base64::decode("YWI=", decoded), "Decoding should succeed");
    assert::that(decoded.size() == 2, "Decoded size should be 2");
    assert::that(decoded[0] == 'a', "First byte should be 'a'");
    assert::that(decoded[1] == 'b', "Second byte should be 'b'");
}

// Test for invalid decoding
VX_TEST(base64, decode_invalid_input)
{
    std::vector<uint8_t> decoded;
    assert::that(!vx::str::base64::decode("InvalidBase64", decoded), "Decoding invalid input should fail");
    assert::that(decoded.size() == 0, "Decoded size should be 0 for invalid input");
}

// Test for decoding with validation
VX_TEST(base64, decode_with_validation)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::str::base64::decode("YWJj", decoded, true), "Decoding should succeed with validation");
    assert::that(decoded.size() == 3, "Decoded size should be 3");

    assert::that(!vx::str::base64::decode("InvalidBase64", decoded, true), "Decoding invalid input with validation should fail");
    assert::that(decoded.size() == 0, "Decoded size should be 0 for invalid input");
}

// Test for encode-decode round trip
VX_TEST(base64, encode_decode_round_trip)
{
    const uint8_t data[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
    std::string encoded;
    std::vector<uint8_t> decoded;

    assert::that(vx::str::base64::encode(data, sizeof(data), encoded), "Encoding should succeed");
    assert::that(vx::str::base64::decode(encoded, decoded), "Decoding should succeed");

    assert::that(decoded.size() == sizeof(data), "Decoded size should match original data size");
    assert::that(std::memcmp(data, decoded.data(), sizeof(data)) == 0, "Decoded data should match original data");
}

// Test for encoding null pointer
VX_TEST(base64, encode_null_pointer)
{
    const uint8_t* data = nullptr;
    std::string encoded;
    assert::that(!vx::str::base64::encode(data, 5, encoded), "Encoding null pointer should fail");
}

// Test for decoding null pointer
VX_TEST(base64, decode_null_pointer)
{
    std::vector<uint8_t> decoded;
    assert::that(vx::str::base64::decode("", decoded), "Decoding empty string should succeed");
    assert::that(decoded.size() == 0, "Decoded should have size of 0");
}

// Test for MD5 initialization and empty input
VX_TEST(MD5, InitialState)
{
    vx::crypto::MD5 md5;
    md5.finalize(); // Finalize without any input
    assert::that(md5.to_string() == "d41d8cd98f00b204e9800998ecf8427e", "MD5 of empty input should match expected hash");
}

// Test for MD5 hashing of simple input
VX_TEST(MD5, HashSimpleInput)
{
    vx::crypto::MD5 md5;
    const uint8_t data[] = "abc";
    md5.update(data, sizeof(data) - 1); // Exclude null terminator
    md5.finalize();
    assert::that(md5.to_string() == "900150983cd24fb0d6963f7d28e17f72", "MD5 of 'abc' should match expected hash");
}

// Test for MD5 hashing of longer input
VX_TEST(MD5, HashLongInput)
{
    vx::crypto::MD5 md5;
    const uint8_t data[] = "The quick brown fox jumps over the lazy dog";
    md5.update(data, sizeof(data) - 1);
    md5.finalize();
    assert::that(md5.to_string() == "9e107d9d372bb6826bd81d3542a419d6", "MD5 of 'The quick brown fox jumps over the lazy dog' should match expected hash");
}

// Test for MD5 hashing of large input data
VX_TEST(MD5, HashLargeInput)
{
    vx::crypto::MD5 md5;
    std::string large_input(1000, 'a'); // Create a string of 1000 'a' characters
    md5.update(reinterpret_cast<const uint8_t*>(large_input.data()), large_input.size());
    md5.finalize();
    assert::that(md5.to_string() == "cabe45dcc9ae5b66ba86600cca6b8ba8", "MD5 of 1000 'a' characters should match expected hash");
}

int main()
{
    test_suite::get_suite().run_tests();

    return 0;
}