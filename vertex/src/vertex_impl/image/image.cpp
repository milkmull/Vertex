#include <sstream>

#include "vertex/image/image.h"
#include "vertex/system/error.h"

#define VX_IMG_LOAD 1
#define VX_IMG_SAVE 1

#define VX_IMG_BMP 1
#define VX_IMG_JPG 1
#define VX_IMG_PNG 1

///////////////////////////////////////////////////////////////////////////////
// stb_image
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_IMG_LOAD)

#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_BMP
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
VX_DISABLE_WARNING("-Wconversion", 4244)
#include "stb_image/stb_image.h"
VX_DISABLE_WARNING_POP()

#endif

#if defined(VX_IMG_SAVE)

#define STB_IMAGE_WRITE_IMPLEMENTATION

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
#include "stb_image/stb_image_write.h"
VX_DISABLE_WARNING_POP()

#endif

namespace vx {
namespace img {

struct image_info
{
    size_t width, height;
    pixel::pixel_format format;
};

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

enum : size_t
{
    IMAGE_SIZE_LIMIT_MAX_DIMENSIONS = 4096,

    IMAGE_SIZE_LIMIT_MIN_CHANNELS = 1,
    IMAGE_SIZE_LIMIT_MAX_CHANNELS = 4,

    IMAGE_SIZE_LIMIT_MAX_SIZE = (
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_DIMENSIONS *
        IMAGE_SIZE_LIMIT_MAX_CHANNELS
    )
};

enum class error_code : int
{
    NONE = 0,

    UNSUPPORTED_PIXEL_FORMAT,
    MAX_SIZE,
    UNSUPPORTED_CHANNEL_COUNT,
    UNSUPPORTED_BITDEPTH,
    REINTERPRETATION_ERROR
};

static const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::NONE:                          return "none";
        case error_code::UNSUPPORTED_PIXEL_FORMAT:      return "unknown pixel format";
        case error_code::MAX_SIZE:                      return "max image size";
        case error_code::UNSUPPORTED_CHANNEL_COUNT:     return "unsupported image channel count";
        case error_code::UNSUPPORTED_BITDEPTH:          return "unsupported image bit depth";
        case error_code::REINTERPRETATION_ERROR:        return "failed to interpret image as desired format";

        default:                                        break;
    }

    return nullptr;
}

static error_code get_image_info_error(const image_info& info)
{
    if (info.format == pixel::pixel_format::UNKNOWN)
    {
        return error_code::UNSUPPORTED_PIXEL_FORMAT;
    }
    if (info.width > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS || info.height > IMAGE_SIZE_LIMIT_MAX_DIMENSIONS)
    {
        return error_code::MAX_SIZE;
    }
    return error_code::NONE;
}

///////////////////////////////////////////////////////////////////////////////
// load
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_IMG_LOAD)

static void load_error(const char* filename)
{
    std::ostringstream oss;
    oss << "failed to load image file \"" << filename << '"';

    if (stbi_failure_reason())
    {
        oss << ": " << stbi_failure_reason();
    }

    VX_ERROR(error::error_code::FILE_ERROR) << oss.str();
}

static void load_process_error(const char* filename, error_code code)
{
    VX_ERROR(error::error_code::UNSUPPORTED_FORMAT)
        << "failed to load image file \"" << filename << "\": " << error_code_to_string(code);
}

static bool get_file_info(const char* filename, image_info& info)
{
    info.width = info.height = 0;
    info.format = pixel::pixel_format::UNKNOWN;

    int width, height, channels;
    const bool success = stbi_info(filename, &width, &height, &channels);
    if (!success)
    {
        load_error(filename);
        return false;
    }

    info = image_info
    {
        static_cast<size_t>(width),
        static_cast<size_t>(height),
        pixel::format_from_channels(static_cast<size_t>(channels))
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
static bool load_internal(const char* filename, image_info& info, std::vector<pixel::byte_type>& data)
{
    data.clear();

    if (!get_file_info(filename, info))
    {
        return false;
    }

    error_code err;

    // check for any errors in final image info
    err = get_image_info_error(info);
    if (err != error_code::NONE)
    {
        load_process_error(filename, err);
        return false;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    stbi_uc* raw = stbi_load(filename, &width, &height, &channels, 0);

    if (raw == nullptr)
    {
        load_error(filename);
        return false;
    }

    size_t size = info.width * info.height * pixel::get_pixel_size(info.format);
    data.assign(raw, raw + size);
    stbi_image_free(raw);

    return true;
}

bool load(const std::string& filename, pixel::surface& surf)
{
    image_info info;
    std::vector<pixel::byte_type> data;

    bool status = load_internal(filename.c_str(), info, data);
    surf = pixel::surface(data.data(), info.width, info.height, info.format);

    return status;
}

#else

bool load(const std::string& filename, pixel::surface& surf)
{
    return false;
}

#endif

///////////////////////////////////////////////////////////////////////////////
// save
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_IMG_SAVE)

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

static void save_error(const char* filename)
{
    VX_ERROR(error::error_code::FILE_ERROR) << "failed to write image file \"" << filename << '"';
}

static void save_process_error(const char* filename, error_code code)
{
    VX_ERROR(error::error_code::UNSUPPORTED_FORMAT)
        << "failed to save image file \"" << filename << "\": " << error_code_to_string(code);
}

struct image_save_data
{
    image_info info;
    const pixel::byte_type* data;
    pixel::surface converted_surface;
    error_code error;
};

static bool get_save_data(const char* filename, const pixel::surface& surf, image_save_data& data)
{
    data.info.width = surf.width();
    data.info.height = surf.height();
    data.info.format = pixel::format_from_channels(surf.channels());

    data.error = get_image_info_error(data.info);
    if (data.error != error_code::NONE)
    {
        save_process_error(filename, data.error);
        return false;
    }

    if (data.info.format == surf.format())
    {
        data.data = surf.data();
    }
    else
    {
        data.converted_surface = surf.convert(data.info.format);
        data.data = data.converted_surface.data();
    }

    assert(data.data);

    return true;
}

bool save_bmp(const std::string& filename, pixel::surface& surf)
{
    image_save_data data;
    if (!get_save_data(filename.c_str(), surf, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    bool success = stbi_write_bmp(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data
    );

    return success;
}

bool save_jpg(const std::string& filename, pixel::surface& surf, int quality)
{
    image_save_data data;
    if (!get_save_data(filename.c_str(), surf, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    bool success = stbi_write_jpg(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data,
        quality
    );

    return success;
}

bool save_png(const std::string& filename, pixel::surface& surf)
{
    image_save_data data;
    if (!get_save_data(filename.c_str(), surf, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    bool success = stbi_write_png(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data,
        static_cast<int>(data.info.width * pixel::get_pixel_size(data.info.format))
    );

    return success;
}

#else

bool save(file_format format, const std::string& filename, pixel::surface& surf)
{
    return false;
}

#endif

}
}