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
    profile::start("../../assets/directory_iterator_timer.txt");

    for (int i = 0; i < 5000; ++i)
    {
        vx_list();
        std_list();
    }

    profile::stop();

    return 0;
}
