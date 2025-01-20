#include "vertex/system/log.hpp"

#include "vertex/os/filesystem.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    os::filesystem::space_info info;
    const auto p = os::filesystem::get_current_path();

    if (os::filesystem::space(p, info))
    {
        VX_LOG_INFO << p;
        VX_LOG_INFO << "capacity: " << info.capacity;
        VX_LOG_INFO << "free: " << info.free;
        VX_LOG_INFO << "available: " << info.available;
    }

    return 0;
}
