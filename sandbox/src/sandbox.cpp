#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/iostream.hpp"
#include "vertex/stdlib/system/time.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    auto dt = time::time_to_datetime(time::get_current_time(), false);
    io::print(dt.str());

    return 0;
}
