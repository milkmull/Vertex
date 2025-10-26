#include <iostream>

#include "vertex/system/error.hpp"
#include "vertex/system/log.hpp"
#include "vertex/pixel/palette_surface.hpp"
#include "vertex/image/load.hpp"
#include "vertex/image/write.hpp"
#include "vertex/pixel/palette_surface_iterator.hpp"

using namespace vx;
using namespace vx::pixel;

template <pixel::palette_format F>
void test_palette_format(const char* name)
{
    using psurf_t = palette_surface<F>;

    VX_LOG_INFO("Testing palette format: ", name);

    psurf_t psurf(
        128, 128,
        {
            math::color::black(),
            math::color::red(),
            math::color::green(),
            math::color::blue(),
            math::color::yellow(),
            math::color::magenta(),
            math::color::cyan(),
            math::color::white()
        }
    );

    // Fill with a sin/cos interference pattern
    for (auto it = psurf.begin(); it != psurf.end(); ++it)
    {
        const auto uv = it.uv();
        const float fx = math::sin(uv.x * 10.0f * math::constants<float>::pi);
        const float fy = math::cos(uv.y * 10.0f * math::constants<float>::pi);
        const float f = (fx * fy) * 0.5f + 0.5f;

        // Quantize f into an index based on the palette size
        const size_t idx = static_cast<size_t>(f * (psurf.get_palette().size() - 1));
        *it = static_cast<uint8_t>(idx);
    }

    // Convert to a regular RGBA surface for saving
    const surface_rgba8 final = psurf.to_surface<pixel_format::RGBA_8>();
    img::image img_out(final.data(), final.width(), final.height(), img::pixel_format::RGBA_8);

    std::string filename = std::string("../../assets/palette_test_") + name + ".png";
    if (!img::write_png(filename.c_str(), img_out))
    {
        VX_LOG_ERROR("Failed to write: ", filename);
    }
    else
    {
        VX_LOG_INFO("Wrote ", filename);
    }
}

int main()
{
    VX_LOG_INFO("=== Running Palette Format Tests ===");

    test_palette_format<palette_format::INDEX_1LSB>("INDEX_1LSB");
    test_palette_format<palette_format::INDEX_1MSB>("INDEX_1MSB");

    test_palette_format<palette_format::INDEX_2LSB>("INDEX_2LSB");
    test_palette_format<palette_format::INDEX_2MSB>("INDEX_2MSB");

    test_palette_format<palette_format::INDEX_4LSB>("INDEX_4LSB");
    test_palette_format<palette_format::INDEX_4MSB>("INDEX_4MSB");

    test_palette_format<palette_format::INDEX_8>("INDEX_8");

    VX_LOG_INFO("=== All Palette Format Tests Complete ===");
    return 0;
}