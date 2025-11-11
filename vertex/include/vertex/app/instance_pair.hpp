#pragma once

#include "vertex/app/id.hpp"

namespace vx {
namespace app {

template <typename T, typename ID>
struct instance_pair
{
    ID id = invalid_id;
    T* ptr = nullptr;
};

} // namespace app
} // namespace vx