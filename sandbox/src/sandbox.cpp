#include "vertex/system/log.hpp"
#include "vertex/os/filesystem.hpp"

#define VX_ENABLE_PROFILING

#include "vertex/system/profiler.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace vx;

static void vx_list()
{
    VX_PROFILE_FUNCTION();

    size_t count1 = 0;
    os::filesystem::recursive_directory_iterator it1("../..");
    for (const auto& dir : it1)
    {
        ++count1;
    }
}

static void std_list()
{
    VX_PROFILE_FUNCTION();

    size_t count2 = 0;
    std::filesystem::recursive_directory_iterator it2("../..");
    for (const auto& dir : it2)
    {
        ++count2;
    }
}

int main()
{
    //profile::start("../../assets/directory_iterator_timer.txt");

    const auto p1 = os::filesystem::get_current_path();
    VX_LOG_INFO << p1;

    os::filesystem::set_current_path("../../assets");

    const auto p2 = os::filesystem::get_current_path();
    VX_LOG_INFO << p2;

    const auto rel = os::filesystem::relative(p1, p2);
    VX_LOG_INFO << rel;

    VX_LOG_INFO << os::filesystem::equivalent(p1, rel);

    //profile::stop();

    return 0;
}
