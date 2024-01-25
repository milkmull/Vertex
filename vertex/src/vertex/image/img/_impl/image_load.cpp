#include "../image_load.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

#include "stb_image/stb_image.h"

#include "../image_size_limit.h"

namespace vx {
namespace img {

static const char* error_message = nullptr;

static void set_image_load_error_message(const char* msg)
{
    error_message = msg;
}

const char* get_image_load_error_message()
{
    return error_message;
}

bool load_image(const char* path, image_info& specs, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    assert(path != nullptr);

    int width, height, channels;
    data.clear();

    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(path, &width, &height, &channels, 0);

    if (raw == nullptr)
    {
        const char* msg = stbi_failure_reason();
        set_image_load_error_message((msg != nullptr) ? msg : "Failed to load image");
        return false;
    }

    image_size_limit err = check_image_size_limits(width, height, channels, 8);
    if (err != image_size_limit::VX_IMAGE_SIZE_LIMIT_NONE)
    {
        const char* msg = get_image_size_limit_error_message(err);
        set_image_load_error_message(msg);
        return false;
    }

    specs.width = width;
    specs.height = height;
    specs.format = choose_format(channels);
    data.assign(raw, raw + specs.size());

    return true;
}

}
}

#endif