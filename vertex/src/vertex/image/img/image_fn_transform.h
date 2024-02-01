#pragma once

#include <vector>
#include <iostream>

#include "image_info.h"

namespace vx {
namespace img {
namespace transform {

inline image_info flip_x(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();

    for (size_type y = 0; y < info.height; ++y)
    {
        byte_type* row = &data[info.width * y * pxsz];

        for (size_type x = 0; x < info.width / 2; ++x)
        {
            byte_type* srcpx = &row[x * pxsz];
            byte_type* dstpx = &row[(info.width - x - 1) * pxsz];

            byte_type tmp;
            for (size_type i = 0; i < pxsz; ++i)
            {
                tmp = dstpx[i];
                dstpx[i] = srcpx[i];
                srcpx[i] = tmp;
            }
        }
    }

    return info;
}

inline image_info flip_y(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();
    const size_type rowsz = info.width * pxsz;
    std::vector<byte_type> tmp(rowsz);

    for (size_type y = 0; y < info.height / 2; ++y)
    {
        byte_type* row1 = &data[info.width * y * pxsz];
        byte_type* row2 = &data[info.width * (info.height - y - 1) * pxsz];

        std::memcpy(tmp.data(), row1, rowsz);
        std::memmove(row1, row2, rowsz);
        std::memcpy(row2, tmp.data(), rowsz);
    }

    return info;
}

inline image_info rotate_90_cw(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();
    const size_type rowsz = info.width * pxsz;
    std::vector<byte_type> tmp(info.size());

    for (size_type y = 0; y < info.height; ++y)
    {
        byte_type* srcrow = &data[info.width * y * pxsz];
        byte_type* dstcol = &tmp[static_cast<size_t>((info.height - y - 1) * pxsz)];

        for (size_type x = 0; x < info.width; ++x)
        {
            byte_type* srcpx = &srcrow[x * pxsz];
            byte_type* dstpx = &dstcol[x * info.height * pxsz];

            for (size_type i = 0; i < pxsz; ++i)
            {
                dstpx[i] = srcpx[i];
            }
        }
    }

    std::move(tmp.begin(), tmp.end(), data);
    return image_info{ info.height, info.width, info.format };
}

inline image_info rotate_90_ccw(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();
    const size_type rowsz = info.width * pxsz;
    std::vector<byte_type> tmp(info.size());

    for (size_type y = 0; y < info.height; ++y)
    {
        byte_type* srcrow = &data[info.width * y * pxsz];
        byte_type* dstcol = &tmp[static_cast<size_t>(y * pxsz)];

        for (size_type x = 0; x < info.width; ++x)
        {
            byte_type* srcpx = &srcrow[(info.width - x - 1) * pxsz];
            byte_type* dstpx = &dstcol[x * info.height * pxsz];

            for (size_type i = 0; i < pxsz; ++i)
            {
                dstpx[i] = srcpx[i];
            }
        }
    }

    std::move(tmp.begin(), tmp.end(), data);
    return image_info{ info.height, info.width, info.format };
}

inline image_info rotate_180(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();

    for (size_type y = 0; y < info.height / 2; ++y)
    {
        byte_type* srcrow = &data[info.width * y * pxsz];
        byte_type* dstrow = &data[info.width * (info.height - y - 1) * pxsz];

        for (size_type x = 0; x < info.width; ++x)
        {
            byte_type* srcpx = &srcrow[x * pxsz];
            byte_type* dstpx = &dstrow[(info.width - x - 1) * pxsz];

            byte_type tmp;
            for (size_type i = 0; i < pxsz; ++i)
            {
                tmp = dstpx[i];
                dstpx[i] = srcpx[i];
                srcpx[i] = tmp;
            }
        }
    }

    return info;
}

inline image_info transpose(const image_info& info, byte_type* data)
{
    const size_type pxsz = info.pixel_size();
    const size_type rowsz = info.width * pxsz;
    std::vector<byte_type> tmp(info.size());

    for (size_type y = 0; y < info.height; ++y)
    {
        byte_type* srcrow = &data[y * rowsz];
        byte_type* dstcol = &tmp[static_cast<size_t>(y * pxsz)];

        for (size_type x = 0; x < info.width; ++x)
        {
            byte_type* srcpx = &srcrow[x * pxsz];
            byte_type* dstpx = &dstcol[x * info.height * pxsz];

            for (size_type i = 0; i < pxsz; ++i)
            {
                dstpx[i] = srcpx[i];
            }
        }
    }

    std::move(tmp.begin(), tmp.end(), data);
    return image_info{ info.height, info.width, info.format };
}

}
}
}