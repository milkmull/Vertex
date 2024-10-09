#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/file/filesystem.hpp"
#include "vertex/stdlib/file/file.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_LOG_INFO << filesystem::get_current_directory();

    file f;
    f.open("hello.txt", file_mode::WRITE);
    int a = 12345;
    f.write_from(a);
    f.close();

    f.open("hello.txt", file_mode::READ);
    
    int b;
    VX_LOG_INFO << f.read_to(b);
    VX_LOG_INFO << b;
    //
    ////auto data = f.read();
    ////VX_LOG_INFO << std::string(data.begin(), data.end());
    //
    //f.close();

    return 0;
}