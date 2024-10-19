#include "sandbox/sandbox.hpp"
#include "vertex/os/info.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_LOG_INFO << "Name: " << os::get_name();
    VX_LOG_INFO << "Model Name: " << os::get_model_name();
    VX_LOG_INFO << "System Name: " << os::get_system_name();

    VX_LOG_INFO << "Processor Name: " << os::get_processor_name();
    VX_LOG_INFO << "Processor Count: " << os::get_processor_count();

    uint32_t major, minor, patch;
    os::get_version(&major, &minor, &patch);
    VX_LOG_INFO << "Version: " << major << '.' << minor << '.' << patch;

    return 0;
}