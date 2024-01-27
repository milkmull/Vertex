#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    img::image_info info;
    std::vector<img::byte_type> data;

    int code = img::load_image("", info, data);

    std::cout << code;

    return 0;
}