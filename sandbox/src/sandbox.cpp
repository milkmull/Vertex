#include "vertex/util/io/iostream.hpp"
#include "vertex/os/thread.hpp"

#include <numeric>

using namespace vx;

static void hello(std::vector<int>) {}

int main(int argc, char* argv[])
{
    for (int i = 0; i < 1000000000000; ++i)
    {
        std::vector<int> data(1000);
        std::iota(data.begin(), data.end(), 0);

        os::thread t;
        t.start(hello, data);
        t.join();
    }
}
