#include "sandbox/sandbox.h"

#include "vertex/image/img/image_fn_transform.h"

int main()
{
    using namespace vx;

    img::image_info info{};
    img::error_code err{};

    img::image image("../../tools/cloud1.png", err);
    std::cout << static_cast<int>(err) << std::endl;
    
    img::transform::flip_x(image);

    img::write_png("../../tools/cloud2.png", image.get_info(), image.raw_data());

    return 0;
}