#pragma once

#include "vertex/pixel/surface.h"

namespace vx {
namespace img {

enum class file_format
{
    UNKNOWN = 0,
    BMP,
    JPG,
    JPEG = JPG,
    PNG
    //PNM,
    //SVG
};

bool load(const std::string& filename, pixel::surface& surf);

bool save_bmp(const std::string& filename, pixel::surface& surf);
bool save_jpg(const std::string& filename, pixel::surface& surf, int quality = 75);
bool save_png(const std::string& filename, pixel::surface& surf);

}
}