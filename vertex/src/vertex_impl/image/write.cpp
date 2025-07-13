#include <sstream>

#include "vertex/system/error.hpp"
#include "vertex/image/write.hpp"
#include "vertex_impl/image/util.hpp"

#define VX_IMG_WRITE_IMPLEMENTATION

#if defined(VX_IMG_ONLY_BMP) || defined(VX_IMG_ONLY_JPG) || defined(VX_IMG_ONLY_PNG)

#   if defined(VX_IMG_ONLY_BMP)
#       define VX_IMG_NO_BMP
#   endif

#   if !defined(VX_IMG_ONLY_JPG)
#       define VX_IMG_NO_JPG
#   endif

#   if !defined(VX_IMG_ONLY_PNG)
#       define VX_IMG_NO_PNG
#   endif

#endif

///////////////////////////////////////////////////////////////////////////////
// stb_image
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_IMG_WRITE_IMPLEMENTATION)

#   define STB_IMAGE_WRITE_IMPLEMENTATION

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wimplicit-fallthrough", 26819)
#   include "stb_image/stb_image_write.h"
VX_DISABLE_WARNING_POP()

#endif // VX_IMG_WRITE_IMPLEMENTATION

namespace vx {
namespace img {

#if defined(VX_IMG_WRITE_IMPLEMENTATION)

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

static void write_process_error(const char* filename, error_code code)
{
    std::ostringstream oss;
    oss << "failed to save image file \"" << filename << "\": " << error_code_to_string(code);
    err::set(err::FILE_WRITE_FAILED, oss.str().c_str());
}

///////////////////////////////////////////////////////////////////////////////
// write
///////////////////////////////////////////////////////////////////////////////

struct image_write_data
{
    image_info info{};
    const byte_type* data = nullptr;
    error_code error = error_code::NONE;
};

static bool get_write_data(const char* filename, const image& img, image_write_data& data)
{
    data.info.width = img.width();
    data.info.height = img.height();
    data.info.format = pixel::format_from_channels(img.channels());

    data.error = get_image_info_error(data.info);
    if (data.error != error_code::NONE)
    {
        write_process_error(filename, data.error);
        return false;
    }

    data.data = img.data();
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// save bmp
///////////////////////////////////////////////////////////////////////////////

#if !defined(VX_IMG_NO_BMP)

bool write_bmp(const std::string& filename, const image& img)
{
    image_write_data data;
    if (!get_write_data(filename.c_str(), img, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    const bool success = stbi_write_bmp(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data
    );

    return success;
}

#else // VX_IMG_NO_BMP

bool write_bmp(const std::string&, const image&)
{
    VX_UNSUPPORTED("img::write_bmp()");
    return false;
}

#endif // VX_IMG_NO_BMP

///////////////////////////////////////////////////////////////////////////////
// save jpg
///////////////////////////////////////////////////////////////////////////////

#if !defined(VX_IMG_NO_JPG)

bool write_jpg(const std::string& filename, const image& img, int quality)
{
    image_write_data data;
    if (!get_write_data(filename.c_str(), img, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    const bool success = stbi_write_jpg(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data,
        quality
    );

    return success;
}

#else // VX_IMG_NO_JPG

bool write_jpg(const std::string&, const image&, int)
{
    VX_UNSUPPORTED("img::write_jpg()");
    return false;
}

#endif // VX_IMG_NO_JPG

///////////////////////////////////////////////////////////////////////////////
// save png
///////////////////////////////////////////////////////////////////////////////

#if !defined(VX_IMG_NO_PNG)

bool write_png(const std::string& filename, const image& img)
{
    image_write_data data;
    if (!get_write_data(filename.c_str(), img, data))
    {
        return false;
    }

    stbi_flip_vertically_on_write(false);
    const bool success = stbi_write_png(
        filename.c_str(),
        static_cast<int>(data.info.width),
        static_cast<int>(data.info.height),
        static_cast<int>(pixel::get_pixel_channel_count(data.info.format)),
        data.data,
        static_cast<int>(data.info.width * pixel::get_pixel_size(data.info.format))
    );

    return success;
}

#else // VX_IMG_NO_PNG

bool write_png(const std::string&, const image&)
{
    VX_UNSUPPORTED("img::write_png()");
    return false;
}

#endif // VX_IMG_NO_PNG

#else

bool write_bmp(const std::string&, const image&)
{
    VX_UNSUPPORTED("img::write_bmp()");
    return false;
}

bool write_jpg(const std::string&, const image&, int)
{
    VX_UNSUPPORTED("img::write_jpg()");
    return false;
}

bool write_png(const std::string&, const image&)
{
    VX_UNSUPPORTED("img::write_png()");
    return false;
}

#endif // VX_IMG_WRITE_IMPLEMENTATION

} // namespace img
} // namespace vx