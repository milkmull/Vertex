#include "vertex_impl/app/app_internal.hpp"
#include "vertex_impl/app/hints/hints_internal.hpp"
#include "vertex_impl/app/hints/events_internal.hpp"

namespace vx {
namespace app {

///////////////////////////////////////////////////////////////////////////////
// initialization
///////////////////////////////////////////////////////////////////////////////

static owner_ptr<app_instance> s_app;

////////////////////////////////////////

VX_API bool init(init_flag flags)
{
    if (!s_app)
    {
        s_app.reset(new app_instance);

        if (!s_app)
        {
            return false;
        }
    }

    return s_app->init(flags);
}

bool app_instance::init(init_flag flags)
{
    if (!is_init_hints() && !init_hints())
    {
        data.hints.reset();
        return false;
    }

    return true;
}

////////////////////////////////////////

VX_API bool is_init(init_flag flags)
{
    VX_CHECK_APP_INIT(false);
    return s_app->is_init(flags);
}

bool app_instance::is_init(init_flag flags) const
{
    return false;
}

////////////////////////////////////////

VX_API void quit(init_flag flags)
{
    if (!s_app)
    {
        return;
    }

    s_app->quit(flags);

    if (s_app->can_shutdown())
    {
        s_app->shutdown();
        s_app.reset();
    }
}

void app_instance::quit(init_flag flags)
{
}

////////////////////////////////////////

bool app_instance::can_shutdown() const
{
    return !data.events;
}

////////////////////////////////////////

void app_instance::shutdown()
{
    data.hints->quit();
}

} // namespace app
} // namespace vx