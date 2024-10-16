#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/file/file.hpp"

using namespace vx;

#define WRITING 0

int main(int argc, char* argv[])
{
    const char* filename = "hello.world.txt";
    file f;

#if WRITING

    int nums[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    f.open(filename, file_mode::WRITE);

    for (int i = 0; i < 10; ++i)
    {
        f.write_from(nums[i]);
    }

#else

    int value = 0;
    f.open(filename, file_mode::READ);

    for (int i = 0; i < 10; ++i)
    {
        if (!f.read_to(value)) break;
        VX_LOG_INFO << value;
    }

#endif

    return 0;
}