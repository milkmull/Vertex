#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/crypto/SHA256.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    vx::crypto::SHA256 sha256;
    const uint8_t data[] = "";
    sha256.update(data, sizeof(data) - 1); // Exclude null terminator
    sha256.finalize();

    // e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    // e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    VX_LOG_INFO << sha256.to_string();

    return 0;
}
