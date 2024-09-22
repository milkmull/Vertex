#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/base64.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    const char test[] = "hello";

    std::string encoded;
    std::vector<uint8_t> decoded;
    
    str::base64::encode(reinterpret_cast<const uint8_t*>(test), sizeof(test) - 1, encoded);
    VX_LOG_INFO << encoded;
    
    str::base64::decode(encoded, decoded, true);
    VX_LOG_INFO << std::string(decoded.begin(), decoded.end());

    return 0;
}
