#pragma once

#include <vector>
#include <unordered_map>

#include "vertex/pixel/palette_surface.hpp"

namespace vx {
namespace pixel {
namespace quantize {

///////////////////////////////////////////////////////////////////////////////
// linear
///////////////////////////////////////////////////////////////////////////////

template <palette_format F2, pixel_format F1>
inline palette_surface<F2> linear(const surface<F1>& surf)
{
    using color_t = typename palette::color_type;
    using surf_t = surface<F1>;
    using palette_surf_t = palette_surface<F2>;

    if (surf.empty())
        return palette_surf_t(0, 0);

    constexpr size_t target_size = get_max_palette_size(F2);

    // Step 1. Gather all unique colors from surface
    const palette surf_palette = surf.generate_palette();
    std::vector<color_t> colors(surf_palette.begin(), surf_palette.end()); // Or surf.generate_palette().data()

    if (colors.empty())
        return palette_surf_t(surf.width(), surf.height());

    struct ColorBox {
        std::vector<color_t> colors;
        color_t min, max;

        void compute_bounds() noexcept {
            if (colors.empty()) return;
            min = max = colors[0];
            for (auto& c : colors) {
                min.r = math::min(min.r, c.r);  max.r = math::max(max.r, c.r);
                min.g = math::min(min.g, c.g);  max.g = math::max(max.g, c.g);
                min.b = math::min(min.b, c.b);  max.b = math::max(max.b, c.b);
            }
        }

        float range_r() const noexcept { return max.r - min.r; }
        float range_g() const noexcept { return max.g - min.g; }
        float range_b() const noexcept { return max.b - min.b; }

        float volume() const noexcept {
            return (max.r - min.r) + (max.g - min.g) + (max.b - min.b);
        }

        color_t average() const noexcept {
            color_t sum{};
            if (colors.empty()) return sum;
            for (auto& c : colors) sum += c;
            return sum / static_cast<float>(colors.size());
        }
    };

    std::vector<ColorBox> boxes;
    boxes.reserve(target_size);
    boxes.push_back({ colors });
    boxes.back().compute_bounds();

    // Step 2. Split boxes until we reach target_size
    while (boxes.size() < target_size)
    {
        // Find the box with largest color range
        auto it = std::max_element(boxes.begin(), boxes.end(),
            [](const ColorBox& a, const ColorBox& b) { return a.volume() < b.volume(); });

        if (it == boxes.end() || it->colors.size() <= 1)
            break;

        // Determine which axis to split along
        float r_range = it->range_r();
        float g_range = it->range_g();
        float b_range = it->range_b();

        enum Axis { R, G, B } axis;
        if (r_range >= g_range && r_range >= b_range) axis = R;
        else if (g_range >= b_range) axis = G;
        else axis = B;

        // Sort colors in this box along chosen axis
        switch (axis)
        {
            case R:
                std::sort(it->colors.begin(), it->colors.end(),
                    [](const color_t& a, const color_t& b) { return a.r < b.r; });
                break;
            case G:
                std::sort(it->colors.begin(), it->colors.end(),
                    [](const color_t& a, const color_t& b) { return a.g < b.g; });
                break;
            case B:
                std::sort(it->colors.begin(), it->colors.end(),
                    [](const color_t& a, const color_t& b) { return a.b < b.b; });
                break;
        }

        // Split at median
        size_t median_index = it->colors.size() / 2;
        std::vector<color_t> left(it->colors.begin(), it->colors.begin() + median_index);
        std::vector<color_t> right(it->colors.begin() + median_index, it->colors.end());

        // Replace old box with left, add right
        ColorBox left_box{ std::move(left) };
        ColorBox right_box{ std::move(right) };
        left_box.compute_bounds();
        right_box.compute_bounds();

        *it = std::move(left_box);
        boxes.push_back(std::move(right_box));
    }

    // Step 3. Compute final palette colors
    palette dst(std::min(boxes.size(), target_size));
    for (size_t i = 0; i < dst.size(); ++i)
        dst[i] = boxes[i].average();

    // Step 4. Build result surface
    palette_surf_t result(surf.width(), surf.height());
    result.set_palette_colors(dst.data(), dst.size());

    for (size_t y = 0; y < surf.height(); ++y)
    {
        for (size_t x = 0; x < surf.width(); ++x)
        {
            color_t px = surf.get_pixel(x, y);
            size_t index;
            dst.find_closest(px, &index);
            result.set_pixel_index(x, y, index);
        }
    }

    return result;
}

} // namespace quantize
} // namespace pixel
} // namespace vx