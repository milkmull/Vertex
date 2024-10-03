#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/regex.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    str::regex re("[");

    VX_LOG_INFO << re.is_valid();

    str::regex_cmatch m;
    VX_LOG_INFO << re.match("[", m);



    return 0;
}
