#pragma once

#include <memory>

namespace vx {
namespace app {

struct gamma_ramp
{
    std::unique_ptr<uint16_t[256]> red;
    std::unique_ptr<uint16_t[256]> green;
    std::unique_ptr<uint16_t[256]> blue;
};

}
}