#pragma once

#include "raw_pixel.h"

namespace vx {
namespace pixel {

inline math::color pixel_to_color(const uint8_t* pixel, pixel_format format)
{
    switch (format)
    {
        case pixel_format::RGB_332:         return *(const raw_pixel<pixel_format::RGB_332>*)      (pixel);
        case pixel_format::RGBA_4444:       return *(const raw_pixel<pixel_format::RGBA_4444>*)    (pixel);
        case pixel_format::BGRA_4444:       return *(const raw_pixel<pixel_format::BGRA_4444>*)    (pixel);
        case pixel_format::RGB_565:         return *(const raw_pixel<pixel_format::RGB_565>*)      (pixel);
        case pixel_format::BGR_565:         return *(const raw_pixel<pixel_format::BGR_565>*)      (pixel);
        case pixel_format::RGBA_5551:       return *(const raw_pixel<pixel_format::RGBA_5551>*)    (pixel);
        case pixel_format::BGRA_5551:       return *(const raw_pixel<pixel_format::BGRA_5551>*)    (pixel);
        case pixel_format::ARGB_1555:       return *(const raw_pixel<pixel_format::ARGB_1555>*)    (pixel);
        case pixel_format::RGBA_8888:       return *(const raw_pixel<pixel_format::RGBA_8888>*)    (pixel);
        case pixel_format::BGRA_8888:       return *(const raw_pixel<pixel_format::BGRA_8888>*)    (pixel);
        case pixel_format::ABGR_8888:       return *(const raw_pixel<pixel_format::ABGR_8888>*)    (pixel);
        case pixel_format::ARGB_2101010:    return *(const raw_pixel<pixel_format::ARGB_2101010>*) (pixel);
        case pixel_format::ABGR_2101010:    return *(const raw_pixel<pixel_format::ABGR_2101010>*) (pixel);
        case pixel_format::R_8:             return *(const raw_pixel<pixel_format::R_8>*)          (pixel);
        case pixel_format::RG_8:            return *(const raw_pixel<pixel_format::RG_8>*)         (pixel);
        case pixel_format::RGB_8:           return *(const raw_pixel<pixel_format::RGB_8>*)        (pixel);
        case pixel_format::BGR_8:           return *(const raw_pixel<pixel_format::BGR_8>*)        (pixel);
        case pixel_format::RGBA_8:          return *(const raw_pixel<pixel_format::RGBA_8>*)       (pixel);
        case pixel_format::BGRA_8:          return *(const raw_pixel<pixel_format::BGRA_8>*)       (pixel);
        case pixel_format::ABGR_8:          return *(const raw_pixel<pixel_format::ABGR_8>*)       (pixel);
        case pixel_format::R_16:            return *(const raw_pixel<pixel_format::R_16>*)         (pixel);
        case pixel_format::RG_16:           return *(const raw_pixel<pixel_format::RG_16>*)        (pixel);
        case pixel_format::RGB_16:          return *(const raw_pixel<pixel_format::RGB_16>*)       (pixel);
        case pixel_format::RGBA_16:         return *(const raw_pixel<pixel_format::RGBA_16>*)      (pixel);
        //case pixel_format::R_32:            return *(const raw_pixel<pixel_format::R_32>*)         (pixel);
        //case pixel_format::RG_32:           return *(const raw_pixel<pixel_format::RG_32>*)        (pixel);
        //case pixel_format::RGB_32:          return *(const raw_pixel<pixel_format::RGB_32>*)       (pixel);
        //case pixel_format::RGBA_32:         return *(const raw_pixel<pixel_format::RGBA_32>*)      (pixel);
        case pixel_format::R_32F:           return *(const raw_pixel<pixel_format::R_32F>*)        (pixel);
        case pixel_format::RG_32F:          return *(const raw_pixel<pixel_format::RG_32F>*)       (pixel);
        case pixel_format::RGB_32F:         return *(const raw_pixel<pixel_format::RGB_32F>*)      (pixel);
        case pixel_format::RGBA_32F:        return *(const raw_pixel<pixel_format::RGBA_32F>*)     (pixel);
        default:                            return *(const raw_pixel<pixel_format::UNKNOWN>*)      (pixel);
    }
}

inline void color_to_pixel(const math::color& color, uint8_t* pixel, pixel_format format)
{
    if (!pixel)
    {
        return;
    }

    switch (format)
    {
        case pixel_format::RGB_332:         return (void)(*(raw_pixel<pixel_format::RGB_332>*)      (pixel) = color);
        case pixel_format::RGBA_4444:       return (void)(*(raw_pixel<pixel_format::RGBA_4444>*)    (pixel) = color);
        case pixel_format::BGRA_4444:       return (void)(*(raw_pixel<pixel_format::BGRA_4444>*)    (pixel) = color);
        case pixel_format::RGB_565:         return (void)(*(raw_pixel<pixel_format::RGB_565>*)      (pixel) = color);
        case pixel_format::BGR_565:         return (void)(*(raw_pixel<pixel_format::BGR_565>*)      (pixel) = color);
        case pixel_format::RGBA_5551:       return (void)(*(raw_pixel<pixel_format::RGBA_5551>*)    (pixel) = color);
        case pixel_format::BGRA_5551:       return (void)(*(raw_pixel<pixel_format::BGRA_5551>*)    (pixel) = color);
        case pixel_format::ARGB_1555:       return (void)(*(raw_pixel<pixel_format::ARGB_1555>*)    (pixel) = color);
        case pixel_format::RGBA_8888:       return (void)(*(raw_pixel<pixel_format::RGBA_8888>*)    (pixel) = color);
        case pixel_format::BGRA_8888:       return (void)(*(raw_pixel<pixel_format::BGRA_8888>*)    (pixel) = color);
        case pixel_format::ABGR_8888:       return (void)(*(raw_pixel<pixel_format::ABGR_8888>*)    (pixel) = color);
        case pixel_format::ARGB_2101010:    return (void)(*(raw_pixel<pixel_format::ARGB_2101010>*) (pixel) = color);
        case pixel_format::ABGR_2101010:    return (void)(*(raw_pixel<pixel_format::ABGR_2101010>*) (pixel) = color);
        case pixel_format::R_8:             return (void)(*(raw_pixel<pixel_format::R_8>*)          (pixel) = color);
        case pixel_format::RG_8:            return (void)(*(raw_pixel<pixel_format::RG_8>*)         (pixel) = color);
        case pixel_format::RGB_8:           return (void)(*(raw_pixel<pixel_format::RGB_8>*)        (pixel) = color);
        case pixel_format::BGR_8:           return (void)(*(raw_pixel<pixel_format::BGR_8>*)        (pixel) = color);
        case pixel_format::RGBA_8:          return (void)(*(raw_pixel<pixel_format::RGBA_8>*)       (pixel) = color);
        case pixel_format::BGRA_8:          return (void)(*(raw_pixel<pixel_format::BGRA_8>*)       (pixel) = color);
        case pixel_format::ABGR_8:          return (void)(*(raw_pixel<pixel_format::ABGR_8>*)       (pixel) = color);
        case pixel_format::R_16:            return (void)(*(raw_pixel<pixel_format::R_16>*)         (pixel) = color);
        case pixel_format::RG_16:           return (void)(*(raw_pixel<pixel_format::RG_16>*)        (pixel) = color);
        case pixel_format::RGB_16:          return (void)(*(raw_pixel<pixel_format::RGB_16>*)       (pixel) = color);
        case pixel_format::RGBA_16:         return (void)(*(raw_pixel<pixel_format::RGBA_16>*)      (pixel) = color);
        //case pixel_format::R_32:            return (void)(*(raw_pixel<pixel_format::R_32>*)         (pixel) = color);
        //case pixel_format::RG_32:           return (void)(*(raw_pixel<pixel_format::RG_32>*)        (pixel) = color);
        //case pixel_format::RGB_32:          return (void)(*(raw_pixel<pixel_format::RGB_32>*)       (pixel) = color);
        //case pixel_format::RGBA_32:         return (void)(*(raw_pixel<pixel_format::RGBA_32>*)      (pixel) = color);
        case pixel_format::R_32F:           return (void)(*(raw_pixel<pixel_format::R_32F>*)        (pixel) = color);
        case pixel_format::RG_32F:          return (void)(*(raw_pixel<pixel_format::RG_32F>*)       (pixel) = color);
        case pixel_format::RGB_32F:         return (void)(*(raw_pixel<pixel_format::RGB_32F>*)      (pixel) = color);
        case pixel_format::RGBA_32F:        return (void)(*(raw_pixel<pixel_format::RGBA_32F>*)     (pixel) = color);
        default:                            return;
    }
}

}
}