#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/string.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    constexpr auto x = str::contains("hello", "hell");

    return 0;
}
