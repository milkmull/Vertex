#include <vertex/config/version.hpp>
#include <vertex/util/io/iostream.hpp>

int main()
{
    vx::io::println("hello world");
    vx::io::println("Vertex " VX_VERSION_STRING);
    return 0;
}