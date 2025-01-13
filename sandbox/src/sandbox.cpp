#include "vertex/system/log.hpp"

#include "vertex/os/file.hpp"
#include "vertex/os/filesystem.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    os::filesystem::get_temp_path();

    return 0;
}
