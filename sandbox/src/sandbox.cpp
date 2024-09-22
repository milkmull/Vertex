#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/crypto/MD5.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    vx::crypto::MD5 md5;
    const uint8_t data[] = "abc";
    md5.update(data, sizeof(data) - 1); // Exclude null terminator
    md5.finalize();

    // 900150983cd24fb0d6963f7d28e17f72
    VX_LOG_INFO << md5.to_string();

    return 0;
}
