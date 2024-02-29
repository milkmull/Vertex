#include "sandbox/sandbox.h"

#include "vertex/image/img/image.h"
#include "vertex/image/img/image_fn_transform.h"

int main()
{
    using namespace vx;

    img::error_code err{};
    img::image src("../../assets/treasurechest.png", err);

    if (err == img::error_code::NONE)
    {
        src = rotate_180(src);
        err = img::write_png("../../assets/transform_test.png", src.get_info(), src.raw_data());
        std::cout << "image saved with error: " << (int)err << std::endl;
    }
    else
    {
        std::cout << (int)err << std::endl;
    }

    return 0;
}