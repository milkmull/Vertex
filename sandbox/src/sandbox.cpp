#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/system/process.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    proc::process_config config;
    config.path = "";

    config.args = { "cmd", "/C ping localhost" };

    proc::process p;
    p.start(config);

    VX_LOG_ERROR << error::get_error().message;

    return 0;
}