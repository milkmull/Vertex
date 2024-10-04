#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/iostream.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    io::print_err("please enter a number: ");
    int x;
    io::read_input(x);

    return 0;
}
