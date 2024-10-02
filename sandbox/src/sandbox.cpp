#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/string.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    std::string x1 = "hello my name is name is michael";
    std::string x2 = "HELLO MY NAME IS NAME IS MICHAEL";

    VX_LOG_INFO << str::to_lower(str::to_hex_string(x1.c_str(), x1.size()));

    return 0;
}
