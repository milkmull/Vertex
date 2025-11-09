#pragma once

#include <vector>

#include "vertex/config/language_config.hpp"
#include "vertex/config/flags.hpp"
#include "vertex/app/id.hpp"

namespace vx {
namespace app {
namespace pen {

//=============================================================================
// pen id
//=============================================================================

using pen_id = id_type;

enum : pen_id
{
    default_pen_id = default_id
};

//=============================================================================
// input
//=============================================================================

VX_FLAGS_DECLARE_BEGIN(input_state)
{
    none        = 0,
    down        = VX_BIT(0),
    button_1    = VX_BIT(1),
    button_2    = VX_BIT(2),
    button_3    = VX_BIT(3),
    button_4    = VX_BIT(4),
    button_5    = VX_BIT(5),
    eraser_tip  = VX_BIT(30)
}
VX_FLAGS_DECLARE_END(input_state)

struct axis_type
{
    enum : size_t
    {
        pressure,
        xtilt,
        ytilt,
        distance,
        rotation,
        slider,
        tangential_pressure,
        _count
    };

    axis_type() noexcept = default;
    constexpr explicit axis_type(size_t value) noexcept : m_value(value) {}
    constexpr operator size_t() const noexcept { return m_value; }

    friend constexpr bool operator==(axis_type lhs, axis_type rhs) noexcept { return lhs.m_value == rhs.m_value; }
    friend constexpr bool operator!=(axis_type lhs, axis_type rhs) noexcept { return lhs.m_value != rhs.m_value; }

private:

    size_t m_value;
};

enum : size_t { axis_count = axis_type::_count };

struct pen_state
{
    float x, y;
    input_state state;
    float axes[axis_count];
};

//=============================================================================
// type
//=============================================================================

enum class device_type
{
    invalid = -1,
    unknown,
    direct,
    indirect
};

enum class subtype
{
    unknown,
    eraser,
    pen,
    pencil,
    brush,
    airbrush
};

} // namespace pen
} // namespace app
} // namespace vx