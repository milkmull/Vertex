#include <vector>
#include <sstream>

#include "vertex/system/error.hpp"
#include "vertex/image/load.hpp"
#include "vertex_impl/image/util.hpp"

#define VX_IMG_LOAD_IMPLEMENTATION

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

#if defined(VX_IMG_LOAD_IMPLEMENTATION)

#   define STB_IMAGE_IMPLEMENTATION

#   if !defined(VX_IMG_NO_BMP)
#      define STBI_ONLY_BMP
#   endif

#   if !defined(VX_IMG_NO_JPG)
#      define STBI_ONLY_JPEG
#   endif

#   if !defined(VX_IMG_NO_PNG)
#      define STBI_ONLY_PNG
#   endif

#   define STBI_NO_HDR

#   define STB_IMAGE_STATIC
#   define STBI_NO_THREAD_LOCALS
#   define STBI_NO_GIF
#   define STBI_NO_HDR
#   define STBI_NO_LINEAR
#   define STBI_ASSERT assert

VX_DISABLE_WARNING_PUSH()
VX_DISABLE_WARNING("-Wconversion", 4244)
VX_DISABLE_WARNING("-Wsign-conversion", 0)
VX_DISABLE_WARNING("-Wdouble-promotion", 0)
VX_DISABLE_WARNING("-Wunused-function", 0)
#   include "stb_image/stb_image.h"
VX_DISABLE_WARNING_POP()

#endif // VX_IMG_LOAD_IMPLEMENTATION

namespace vx {
namespace img {

#if defined(VX_IMG_LOAD_IMPLEMENTATION)

///////////////////////////////////////////////////////////////////////////////
// error handling
///////////////////////////////////////////////////////////////////////////////

static void load_error(const char* filename)
{
    std::ostringstream oss;
    oss << "failed to load image file \"" << filename << '"';

    if (stbi_failure_reason())
    {
        oss << ": " << stbi_failure_reason();
    }

    err::set(err::FILE_OPEN_FAILED, oss.str().c_str());
}

static void load_process_error(const char* filename, error_code code)
{
    std::ostringstream oss;
    oss << "failed to load image file \"" << filename << "\": " << error_code_to_string(code);

    err::set(err::FAILED, oss.str().c_str());
}

///////////////////////////////////////////////////////////////////////////////
// load
///////////////////////////////////////////////////////////////////////////////

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
        pixel::channel_count_to_8_bit_format(static_cast<size_t>(channels))
    };

    return true;
}

static bool load_internal(const char* filename, image_info& info, std::vector<byte_type>& data)
{
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

    const size_t size = info.width * info.height * pixel::get_pixel_size(static_cast<pixel::pixel_format>(info.format));
    data.assign(raw, raw + size);
    stbi_image_free(raw);

    return true;
}

VX_API bool load(const std::string& filename, image& img)
{
    image_info info;
    std::vector<byte_type> data;

    const bool status = load_internal(filename.c_str(), info, data);
    img = image(data.data(), info.width, info.height, static_cast<pixel_format>(info.format));

    return status;
}

#else

VX_API bool load(const std::string&, image&)
{
    VX_UNSUPPORTED("img::load()");
    return false;
}

#endif // VX_IMG_LOAD_IMPLEMENTATION

} // namespace img
} // namespace vx