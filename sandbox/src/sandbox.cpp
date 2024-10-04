#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/format.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_LOG_INFO << str::format("{0} {{{}}}!", "hello", "{{{");

    VX_LOG_ERROR << error::get_error().message;

    return 0;
}
