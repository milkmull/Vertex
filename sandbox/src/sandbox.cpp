#include "sandbox/sandbox.hpp"
#include "vertex/os/info.hpp"
#include "vertex/os/process.hpp"
#include "vertex/os/memory.hpp"
#include "vertex/os/power.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    os::power_info info;
    os::get_power_info(info);

    return 0;
}