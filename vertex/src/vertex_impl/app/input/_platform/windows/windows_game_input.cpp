#include "vertex_impl/os/_platform/windows/windows_tools.hpp"
#include "vertex_impl/app/input/_platform/windows/windows_game_input.hpp"
#include "vertex/config/assert.hpp"

#if HAVE_GAMEINPUT_H

namespace vx {
namespace app {
namespace input {

///////////////////////////////////////////////////////////////////////////////
// iitialization
///////////////////////////////////////////////////////////////////////////////

bool game_input_manager::init(video::video_instance* owner)
{
    if (video)
    {
        quit();
    }
    video = owner;

    if (!data.game_input_dll.load("gameinput.dll"))
    {
        quit();
        return false;
    }

    using GameInputCreate_t = HRESULT(WINAPI*)(IGameInput** gameInput);

    GameInputCreate_t GameInputCreate = data.game_input_dll.get<GameInputCreate_t>("GameInputCreate");
    if (!GameInputCreate)
    {
        quit();
        return false;
    }

    data.ctx = NULL;

    HRESULT hr = GameInputCreate(&data.ctx);
    if (FAILED(hr))
    {
        quit();
        return false;
    }

#if GAMEINPUT_API_VERSION >= 1
    hr = data.ctx->QueryInterface(IID_IGameInput, (void**)&data.ctx);
#else
    // We require GameInput v1.1 or newer
    hr = E_NOINTERFACE;
#endif

    data.ctx->Release();

    if (FAILED(hr))
    {
        quit();
        os::windows::error_message("GameInput QueryInterface failed");
        return false;
    }

    return true;
}

void game_input_manager::quit()
{
    data.ctx = nullptr;
    data.game_input_dll.free();
    video = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// configure
///////////////////////////////////////////////////////////////////////////////

bool set_raw_mouse_enabled(bool enabled);
bool set_raw_keyboard_enabled(bool enabled);
bool refresh_raw_input_enabled();

} // namespace input
} // namespace app
} // namespace vx

#endif // HAVE_GAMEINPUT_H