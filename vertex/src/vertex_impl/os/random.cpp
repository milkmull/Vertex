#include "vertex/config/language_config.hpp"
#include "vertex_impl/os/__platform/random.hpp"

namespace vx {
namespace os {

VX_API bool get_entropy(uint8_t* bytes, size_t count)
{
    return get_entropy_impl(bytes, count);
}

} // namespace os
} // namespace vx