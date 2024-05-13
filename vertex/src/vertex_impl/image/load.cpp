#include "vertex/image/raw/load.h"

#include "vertex/image/util/image_format_helpers.h"
#include "vertex/image/util/image_info_helpers.h"
#include "vertex/system/error.h"

#ifdef VX_IMAGE_LOAD_IMPLIMENTATION

///////////////////////////////////////////////////////////////////////////////
// stb_image
///////////////////////////////////////////////////////////////////////////////

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA

//#define STBI_MAX_DIMENSIONS VX_MAX_IMAGE_DIMENSIONS
//#define STBI_NO_FAILURE_STRINGS

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
VX_DISABLE_WARNING("-Wconversion", 4244)
#include "stb_image/stb_image.h"
VX_DISABLE_WARNING_POP()

///////////////////////////////////////////////////////////////////////////////

namespace vx {
namespace img {
namespace raw {

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

static void image_load_error(const char* filename)
{
    std::ostringstream oss;
    oss << "failed to load image file \"" << filename << '"';

    if (stbi_failure_reason())
    {
        oss << ": " << stbi_failure_reason();
    }

    VX_ERROR(error::error_code::FILE_ERROR) << oss.str();
}

static void image_process_error(const char* filename, util::image_error_code code)
{
    VX_ERROR(error::error_code::UNSUPPORTED_FORMAT)
        << "failed to load image file \"" << filename << "\": " << util::image_error_code_to_string(code);
}

///////////////////////////////////////////////////////////////////////////////
// load
///////////////////////////////////////////////////////////////////////////////

bool get_file_info(const char* filename, image_info& info)
{
    info.width = info.height = 0;
    info.format = image_pixel_format::PIXEL_FORMAT_UNKNOWN;

    int width, height, channels;
    const bool success = stbi_info(filename, &width, &height, &channels);
    if (!success)
    {
        image_load_error(filename);
        return false;
    }

    info = image_info{
        static_cast<size_t>(width),
        static_cast<size_t>(height),
        util::pixel_format_from_channels(static_cast<size_t>(channels))
    };

    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Internal function to load an image from a file.
///
/// @param filename The path to the image file.
/// @param info Reference to an image_info object to store information about
/// the loaded image.
/// @param target_format The target image format to reinterpret the loaded image
/// to (use image_pixel_format::PIXEL_FORMAT_UNKNOWN for no reinterpretation).
/// @param data Reference to a vector to store the pixel data of the loaded image.
/// @param flip_vertically_on_load Whether to flip the image vertically during loading.

/// @return True if the image was loaded successfully, otherwise false.
///////////////////////////////////////////////////////////////////////////////
static bool load_image_internal(
    const char* filename,
    image_info& info,
    image_pixel_format target_format,
    std::vector<byte_type>& data,
    bool flip_vertically_on_load
)
{
    data.clear();

    if (!get_file_info(filename, info))
    {
        return false;
    }

    util::image_error_code err;

    // reinterpret info as target format
    if (util::is_valid_pixel_format(target_format))
    {
        err = util::reinterpret_info(info, target_format);

        if (err != util::image_error_code::NONE)
        {
            image_process_error(filename, err);
            return false; 
        }
    }

    // check for any errors in final image info
    err = util::get_image_info_error(info);
    if (err != util::image_error_code::NONE)
    {
        image_process_error(filename, err);
        return false;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(flip_vertically_on_load);
    stbi_uc* raw = stbi_load(filename, &width, &height, &channels, 0);

    if (raw == nullptr)
    {
        image_load_error(filename);
        return false;
    }

    data.assign(raw, raw + info.size());
    stbi_image_free(raw);

    return true;
}

bool load_image(const char* filename, image_info& info, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    return load_image_internal(filename, info, image_pixel_format::PIXEL_FORMAT_UNKNOWN, data, flip_vertically_on_load);
}

bool load_image(const char* filename, image_info& info, image_pixel_format target_format, std::vector<byte_type>& data, bool flip_vertically_on_load)
{
    return load_image_internal(filename, info, target_format, data, flip_vertically_on_load);
}

}
}
}

#endif