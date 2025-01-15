#include "vertex/system/log.hpp"

#include "vertex/util/bit.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    VX_LOG_INFO << bit::byteswap(5u);

    return 0;
}
