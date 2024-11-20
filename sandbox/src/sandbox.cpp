#include "sandbox/sandbox.hpp"

#include "vertex/util/string.hpp"
#include "vertex/util/string/string_compare.hpp"
#include "vertex/util/encode/utf.hpp"
#include "vertex/os/filesystem.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    using decoder = utf::utf_traits<char, 1>;
    using code_point = utf::code_point;

    // Invalid first byte
    {
        const std::vector<uint8_t> data = { 0xFF };
        auto it = data.begin();
        code_point cp;
        it = decoder::decode(it, data.end(), cp);

        assert(cp == utf::INVALID_CODE_POINT);
    }

    // Truncated two-byte sequence
    {
        const std::vector<uint8_t> data = { 0xC3 };
        auto it = data.begin();
        code_point cp;
        it = decoder::decode(it, data.end(), cp);

        assert(cp == utf::INVALID_CODE_POINT);
    }

    // Overlong encoding for ASCII 'A'
    {
        const std::vector<uint8_t> data = { 0xC1, 0x81 }; // Overlong encoding for 0x41
        auto it = data.begin();
        code_point cp;
        it = decoder::decode(it, data.end(), cp);

        assert(cp == utf::INVALID_CODE_POINT);
    }

    // Invalid surrogate pair
    {
        const std::vector<uint8_t> data = { 0xED, 0xA0, 0x80 }; // Surrogate code point
        auto it = data.begin();
        code_point cp;
        it = decoder::decode(it, data.end(), cp);

        assert(cp == utf::INVALID_CODE_POINT);
    }

    // Overlog encoding
    {
        const std::vector<uint8_t> data = { 0xC0, 0xAF }; // Overlong encoding of '/'
        auto it = data.begin();
        code_point cp;
        it = decoder::decode(it, data.end(), cp);

        // Expect the decoder to reject this as an invalid code point
        assert(cp == utf::INVALID_CODE_POINT);
        assert(it == data.end()); // The iterator should move past the invalid sequence
    }

    return 0;
}