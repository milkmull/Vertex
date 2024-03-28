#include "sandbox/sandbox.h"

#include "vertex/tools/encoding/utf.h"

int main()
{
    using namespace vx;

    uint32_t x = 0xA1C2;
    VX_LOG_INFO << std::hex << x;
    
    uint32_t codepoint;
    tools::encode::utf8::decode((uint8_t*)(&x), (uint8_t*)(&x) + sizeof(x), codepoint);
    
    VX_LOG_INFO << codepoint;

    uint32_t y;
    tools::encode::utf8::encode(codepoint, (uint8_t*)(&y));

    VX_LOG_INFO << std::hex << y;

    return 0;
}
