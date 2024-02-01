#include "sandbox/sandbox.h"

#include "vertex/image/img/image_fn_transform.h"

int main()
{
    using namespace vx;

    std::vector<img::byte_type> data;
    img::image_info info;

    img::error_code err = img::load_image("../../tools/cloud1.png", info, data);
    std::cout << static_cast<int>(err) << std::endl;
    
    info = img::transform::transpose(info, data.data());

    img::write_png("../../tools/cloud2.png", info, data.data());

    return 0;
}