#include "vertex/system/log.hpp"
#include "vertex/os/time.hpp"
#include "vertex/os/process.hpp"
#include "vertex/system/error.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_PRINT_ERRORS(true);
    
    os::process::config cfg;
    cfg.args = { "ping", "127.0.0.1", "-n", "5" };
    //cfg.background = true;
    cfg.stdout_option = os::process::io_option::INHERIT;

    os::process p;
    p.start(cfg);

    while (p.is_alive())
    {
        os::sleep(time::milliseconds(100));
        p.get_stdout().write("hello");
    }


    return 0;
}
