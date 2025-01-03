#include "vertex/system/log.hpp"
#include "vertex/os/filesystem.hpp"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace vx;

int main()
{
    const char* path = "../../assets/michael_shortcut";

    const auto result1 = os::filesystem::get_file_info(path);

    VX_LOG_INFO << result1.create_time.to_datetime().to_string();


    std::filesystem::directory_iterator

    return 0;
}
