#pragma once

#include "vertex/image/image.h"

namespace vx {
namespace app {

struct cursor
{
public:
    
    enum cursor_shape : int
    {
        SHAPE_INVALID_SHAPE = -1,

        SHAPE_ARROW,
        SHAPE_IBEAM,
        SHAPE_WAIT,
        SHAPE_CROSSHAIR,
        SHAPE_HAND,
        SHAPE_HRESIZE,
        SHAPE_VRESIZE,
        SHAPE_ALL_RESIZE,
        SHAPE_NOT_ALLOWED,

        SHAPE_CUSTOM
    };

public:

    cursor(cursor_shape shape);
    ~cursor() = default;

    static cursor make_custom(const uint8_t* pixels, const math::vec2i& size, const math::vec2i& hotspot = math::vec2i(0));
    bool is_valid() const;

    bool operator==(const cursor& other) const;
    bool operator!=(const cursor& other) const;

    cursor_shape shape() const;
    int id() const;

private:

    cursor_shape m_shape;
    int m_id;

};

}
}