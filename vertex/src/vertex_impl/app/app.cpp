#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

static owner_ptr<app_data> s_app_data_ptr;

#define data (*s_app_data_ptr)

////////////////////////////////////////

VX_API bool init(init_flag flags)
{
    if (!s_app_data_ptr)
    {
        s_app_data_ptr.reset(new app_data);

        if (!s_app_data_ptr)
        {
            return false;
        }
    }

    return app_internal::init(flags);
}

bool app_internal::init(init_flag flags)
{
    if (!is_init_hints() && !init_hints())
    {
        s_hint_data_ptr.reset();
        return false;
    }

    return true;
}

////////////////////////////////////////

VX_API bool is_init(init_flag flags)
{
    if (!s_app_data_ptr)
    {
        return false;
    }

    return app_internal::is_init(flags);
}

bool app_internal::is_init(init_flag flags)
{
    return false;
}

////////////////////////////////////////

VX_API void quit(init_flag flags)
{
    if (!s_app_data_ptr)
    {
        return;
    }

    app_internal::quit(flags);

    if (app_internal::can_shutdown())
    {
        app_internal::shutdown();
        s_app_data_ptr.reset();
    }
}

void app_internal::quit(init_flag flags)
{
}

////////////////////////////////////////

bool app_internal::can_shutdown()
{
    return !data.events;
}

////////////////////////////////////////

void app_internal::shutdown()
{
    data.hints->quit();
}

} // namespace app
} // namespace vx