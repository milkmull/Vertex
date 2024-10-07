#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/iostream.hpp"
#include "vertex/stdlib/system/time.hpp"
#include "vertex/stdlib/thread/thread.hpp"

using namespace vx;

static void run(int id)
{
    time::sleep(5000);
    io::println(id);
}

int main(int argc, char* argv[])
{
    thread threads[10];

    for (int i = 0; i < 10; ++i)
    {
        threads[i] = thread(run, i);
    }

    for (int i = 0; i < 10; ++i)
    {
        io::print(threads[i].get_id());
        threads[i].start();
        io::println(threads[i].get_id());
    }

    for (int i = 0; i < 10; ++i)
    {
        threads[i].join();
    }

    return 0;
}