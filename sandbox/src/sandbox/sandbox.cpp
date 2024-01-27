#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    img::image_info info;
    std::vector<img::byte_type> data;

    img::error_code code = img::load_image("", info, data);

    std::cout << static_cast<int>(code);

    return 0;
}