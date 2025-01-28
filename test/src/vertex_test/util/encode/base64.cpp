#include "vertex_test/test.hpp"
#include "vertex/util/encode/base64.hpp"

using namespace vx::test;

VX_TEST_CASE(base64)
{
    VX_SECTION("encode empty data")
    {
        std::string encoded;
        VX_CHECK(vx::base64::encode(reinterpret_cast<const uint8_t*>(""), 0, encoded));
        VX_CHECK(encoded == "");
    }

    VX_SECTION("encode simple data")
    {
        const uint8_t data[] = { 'a', 'b', 'c' };
        std::string encoded;
        VX_CHECK(vx::base64::encode(data, sizeof(data), encoded));
        VX_CHECK(encoded == "YWJj");
    }

    VX_SECTION("encode with padding")
    {
        const uint8_t data[] = { 'a', 'b' }; // Two bytes, should pad
        std::string encoded;
        VX_CHECK(vx::base64::encode(data, sizeof(data), encoded));
        VX_CHECK(encoded == "YWI=");
    }

    VX_SECTION("decode simple data")
    {
        std::vector<uint8_t> decoded;
        VX_CHECK(vx::base64::decode("YWJj", decoded));
        VX_CHECK(decoded.size() == 3);
        VX_CHECK(decoded[0] == 'a');
        VX_CHECK(decoded[1] == 'b');
        VX_CHECK(decoded[2] == 'c');
    }

    VX_SECTION("decode with padding")
    {
        std::vector<uint8_t> decoded;
        VX_CHECK(vx::base64::decode("YWI=", decoded));
        VX_CHECK(decoded.size() == 2);
        VX_CHECK(decoded[0] == 'a');
        VX_CHECK(decoded[1] == 'b');
    }

    VX_SECTION("decode invalid input")
    {
        std::vector<uint8_t> decoded;
        VX_CHECK(!vx::base64::decode("InvalidBase64", decoded));
        VX_CHECK(decoded.size() == 0);
    }

    VX_SECTION("decode with validation")
    {
        std::vector<uint8_t> decoded;
        VX_CHECK(vx::base64::decode("YWJj", decoded, true));
        VX_CHECK(decoded.size() == 3);

        VX_CHECK(!vx::base64::decode("InvalidBase64", decoded, true));
        VX_CHECK(decoded.size() == 0);
    }

    VX_SECTION("encode decode round trip")
    {
        const uint8_t data[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        std::string encoded;
        std::vector<uint8_t> decoded;

        VX_CHECK(vx::base64::encode(data, sizeof(data), encoded));
        VX_CHECK(vx::base64::decode(encoded, decoded));

        VX_CHECK(decoded.size() == sizeof(data));
        VX_CHECK(std::memcmp(data, decoded.data(), sizeof(data)) == 0);
    }

    VX_SECTION("encode null pointer")
    {
        const uint8_t* data = nullptr;
        std::string encoded;
        VX_CHECK(!vx::base64::encode(data, 5, encoded));
    }

    VX_SECTION("decode null pointer")
    {
        std::vector<uint8_t> decoded;
        VX_CHECK(vx::base64::decode("", decoded));
        VX_CHECK(decoded.size() == 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
    VX_RUN_TESTS();
    return 0;
}