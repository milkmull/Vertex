#include "vertex_impl/app/main/main_internal.hpp"

namespace vx {
namespace app {

VX_API int _priv::run(int argc, char* argv[], main_t main_fn)
{
    return call_main_function(argc, argv, main_fn);
}

} // namespace app
} // namespace vx