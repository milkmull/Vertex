#include "vertex/app/message_box.hpp"
#include "vertex_impl/app/main/main_internal.hpp"
#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

namespace vx {
namespace app {

VX_API int VX_MINGW32_FORCE_ALIGN _priv::run(int argc, char* argv[], main_t main_fn)
{
    int result = -1;
    void* heap_allocated = NULL;
    const char* args_error = os::windows::check_default_args(&argc, &argv, &heap_allocated);

    if (args_error)
    {
        show_simple_message_box(message_box::type::error, "Fatal Error", args_error, invalid_id);
    }
    else
    {
        result = call_main_function(argc, argv, main_fn);

        if (heap_allocated)
        {
            ::HeapFree(::GetProcessHeap(), 0, heap_allocated);
        }
    }

    return result;
}

} // namespace app
} // namespace vx