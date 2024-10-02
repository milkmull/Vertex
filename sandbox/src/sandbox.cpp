#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/string.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    std::string x1 = "hello my name is name is michael";
    std::string x2 = "HELLO MY NAME IS NAME IS MICHAEL";

    auto words = str::split_lines(str::replace(x1, ' ', '\n'));

    for (const auto& w : words)
    {
        VX_LOG_INFO << w;
    }

    return 0;
}
