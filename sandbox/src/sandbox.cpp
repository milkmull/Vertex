#include "vertex/system/log.hpp"
#include "vertex/os/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace vx;

int main()
{
    const char* path = "../../assets/michael_shortcut";

    os::filesystem::directory_iterator it("../../assets");
    for (const auto& dir : it)
    {
        VX_LOG_INFO << dir.path;
    }

    return 0;
}
