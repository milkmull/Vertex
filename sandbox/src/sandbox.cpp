#include "sandbox/sandbox.hpp"
#include "vertex/util/system/iostream.hpp"
#include "vertex/util/string/format.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    std::string fmt = "Coordinates: ({1:}, {0})";

    io::print(str::format(fmt, 1.22222, 4.883638));

    return 0;
}