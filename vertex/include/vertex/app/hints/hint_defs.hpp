#pragma once

#include <cstdint>

namespace vx {
namespace app {
namespace hint {

///////////////////////////////////////////////////////////////////////////////
// hint type
///////////////////////////////////////////////////////////////////////////////

using hint_t = uint64_t;

///////////////////////////////////////////////////////////////////////////////
// helpers
///////////////////////////////////////////////////////////////////////////////

template <hint_t hint>
struct hint_traits
{
    VX_STATIC_ASSERT(hint != hint, "invalid hint");
};

///////////////////////////////////////////////////////////////////////////////

#define CRETAE_HINT_DEF(name, id) enum : hint_t { name = id }

#define CREATE_HINT_TRAITS(name, dv) \
template <> \
struct hint_traits<name> \
{ \
    static constexpr hint_t hint = name; \
    static constexpr const char* default_value = dv; \
}

#define CRETAE_HINT(name, id, value) CRETAE_HINT_DEF(name, id); CREATE_HINT_TRAITS(name, value)

///////////////////////////////////////////////////////////////////////////////
// hints
///////////////////////////////////////////////////////////////////////////////

CRETAE_HINT(HINT_VIDEO_ALLOW_SCREEN_SAVER, 1, "0");
CRETAE_HINT(HINT_VIDEO_SYNC_WINDOW_OPERATIONS, 2, "0");
CRETAE_HINT(HINT_VIDEO_DISPLAY_USABLE_BOUNDS, 3, "0,0,0,0");

CRETAE_HINT(HINT_VIDEO_WINDOWS_USE_RAW_KEYBOARD, 4, "0");
CRETAE_HINT(HINT_VIDEO_WINDOWS_ENABLE_MESSAGE_LOOP, 5, "1");
CRETAE_HINT(HINT_VIDEO_WINDOWS_ENABLE_MENU_MNEMONICS, 6, "0");
CRETAE_HINT(HINT_VIDEO_WINDOW_FRAME_USABLE_WHILE_CURSOR_HIDDEN, 7, "1");

///////////////////////////////////////////////////////////////////////////////

#define VX_HINT_GET_NAME(hint_) ::vx::app::hint::hint_traits<(hint_)>::hint
#define VX_HINT_GET_DEFAULT_VALUE(hint_) ::vx::app::hint::hint_traits<(hint_)>::default_value

///////////////////////////////////////////////////////////////////////////////

#undef CRETAE_HINT_DEF
#undef CREATE_HINT_TRAITS
#undef CRETAE_HINT

} // namespace hint
} // namespace app
} // namespace vx