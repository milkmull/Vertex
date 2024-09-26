#include "sandbox/sandbox.hpp"
#include "vertex/stdlib/string/string.hpp"
#include "vertex/stdlib/file/filesystem.hpp"

using namespace vx;

int main(int argc, char* argv[])
{
    const char* abcs = "a b c d e f g h i j k l m n o p q r s t u v w x y z ";
    
    for (const auto& w : str::split(abcs, " "))
    {
        VX_LOG_INFO << w;
    }

    VX_LOG_INFO << "hello";

    for (const auto& w : str::split_words(abcs))
    {
        VX_LOG_INFO << w;
    }

    return 0;
}
