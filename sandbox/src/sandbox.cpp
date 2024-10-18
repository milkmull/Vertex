#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/system/time.hpp"
#include "vertex/os/process.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    os::process::config config;
    config.background = false;
    config.args = { "cmd", "/C", "ping localhost" };
    config.stdout_option = os::process::io_option::CREATE;
    
    os::process p;
    p.start(config);

    p.wait(true);

    //std::string text;
    //p.get_stdout().read_text(text, 1000);
    //VX_LOG_INFO << text;

    return 0;
}