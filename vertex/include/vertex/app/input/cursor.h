#pragma once

#include "vertex/math/math/type/vec2i.h"

namespace vx {
namespace app {

struct cursor
{
public:
    
    enum shape : int
    {
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

    cursor(shape shape);
    ~cursor() = default;

    static cursor make_custom(const uint8_t* pixels, const math::vec2i& size, const math::vec2i& hotspot = math::vec2i(0));

    bool operator==(const cursor& other) const;
    bool operator!=(const cursor& other) const;

    shape get_shape() const;
    int get_id() const;

private:

    shape m_shape;
    int m_id;

};

}
}