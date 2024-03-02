#include "mipmap.h"

#include "image.h"
#include "raw/filter/filter_box.h"

namespace vx {
namespace img {

std::vector<image> generate_mipmap(const image& img, size_type depth)
{
    if (img.empty())
    {
        return std::vector<image>();
    }

    size_type w = img.width();
    size_type h = img.height();

    size_type count = static_cast<size_type>(math::floor(math::log2(static_cast<float>(math::max(w, h)))));
    count = math::min(depth, count);

    std::vector<image> mipmap(count);

    for (size_type i = 0; i < count; ++i)
    {
        w /= 2;
        h /= 2;
    
        image map(w, h, img.format());

        filter_box(
            img.data(), img.width(), img.height(),
            map.data(), map.width(), map.height(),
            img.channels()
        );

        mipmap[i] = map;
    }

    return mipmap;
}

std::vector<image> generate_mipmap(const image& img)
{
    return generate_mipmap(img, -1);
}

}
}