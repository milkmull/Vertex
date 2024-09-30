#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/format.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    auto x = "{0} {0}!";

    VX_LOG_INFO << str::format(x, "hello", "world");

    return 0;
}
