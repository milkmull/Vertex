#include "sandbox/sandbox.h"

int main()
{
    using namespace vx;

    img::error_code err{};

    img::image img("../../tools/cloud1.png", err);

    std::cout << "error: "      << static_cast<int>(err) << std::endl;
    std::cout << "width: "      << img.width() << std::endl;
    std::cout << "height: "     << img.height() << std::endl;
    std::cout << "format: "     << static_cast<int>(img.format()) << std::endl;
    std::cout << "channels: "   << img.channels() << std::endl;
    std::cout << "bitdepth: "   << img.bitdepth() << std::endl;
    std::cout << "pixel_size: " << img.pixel_size() << std::endl;
    std::cout << "length: "     << img.data().size() << std::endl;

    img.fill(color::RED());

    err = img::write_png("../../tools/cloud2.png", img.get_info(), img.raw_data());
    std::cout << static_cast<int>(err);

    return 0;
}