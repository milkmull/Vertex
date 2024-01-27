#include "../image_load.h"

#include "vertex/detail/setup.h"
#include "../image_size_limit.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

// =============== stb image ===============

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_SIZE

#define STBI_NO_FAILURE_STRINGS

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
VX_DISABLE_WARNING("-Wconversion", 4244)
#include "stb_image/stb_image.h"
VX_DISABLE_WARNING_POP()

// =========================================

namespace vx {
namespace img {

// =============== load ===============

int load_image(const char* path, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    assert(path != nullptr);

    int width, height, channels;
    data.clear();

    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(path, &width, &height, &channels, 0);

    if (raw == nullptr)
    {
        int msg = stbi_failure_reason();
        //set_image_load_error_message((msg != nullptr) ? msg : "failed to load image");
        return msg;
    }

    image_size_limit err = check_image_size_limits(width, height, channels, 8);
    if (err != VX_IMAGE_SIZE_LIMIT_NONE)
    {
        const char* msg = get_image_size_limit_error_message(err);
        //set_image_load_error_message(msg);
        return 1;
    }

    info.width = width;
    info.height = height;
    info.format = choose_format(channels);
    data.assign(raw, raw + info.size());

    stbi_image_free(raw);

    return 0;
}

}
}

#endif