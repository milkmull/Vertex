#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/file/filesystem.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_LOG_INFO << filesystem::copy(
        "C:/Users/Owner/Desktop/Pergatory/Sandbox.dir",
        "C:/Users/Owner/Desktop/Pergatory/Sandbox.dir2",
        filesystem::copy_options::RECURSIVE | filesystem::copy_options::DIRECTORY_ONLY
    );

    VX_LOG_INFO << error::get_error().message;

    return 0;
}
