#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    img::image_info info;
    std::vector<uint8_t> data;
    bool success = img::load_image("../../tools/cloud1.png", info, data);
    
    if (!success)
    {
        std::cout << img::get_image_load_error_message() << std::endl;
    }
    else
    {
        std::cout << info.size() << ' ' << info.width << ' ' << info.height << std::endl;
        std::cout << data.size() << std::endl;

        success = img::write_png("../../tools/cloud2.png", info, data.data());

        std::cout << (success ? "success" : img::get_image_write_error_message()) << std::endl;
    }

    return 0;
}