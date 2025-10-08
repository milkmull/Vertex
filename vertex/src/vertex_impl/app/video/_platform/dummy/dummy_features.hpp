#pragma once

#include "vertex/config/os.hpp"

// ------------------------------------------------------------
// Video backend feature flags
// ------------------------------------------------------------

// Backend lifecycle
#define VX_FEAT_VIDEO_INIT                      0
#define VX_FEAT_VIDEO_QUIT                      0

// Display management
#define VX_FEAT_DISPLAY_UPDATE                  0
#define VX_FEAT_DISPLAY_LIST_MODES              0
#define VX_FEAT_DISPLAY_SET_MODE                0
#define VX_FEAT_DISPLAY_GET_BOUNDS              0
#define VX_FEAT_DISPLAY_GET_WORK_AREA           0

// Event handling
#define VX_FEAT_EVENT_PUMP                      0
#define VX_FEAT_EVENT_WAIT_TIMEOUT              0
#define VX_FEAT_EVENT_SEND_WAKEUP               0
#define VX_FEAT_EVENT_SUSPEND_SCREEN_SAVER      0

// Window management
#define VX_FEAT_WINDOW_CREATE                   0
#define VX_FEAT_WINDOW_DESTROY                  0
#define VX_FEAT_WINDOW_SET_TITLE                0
#define VX_FEAT_WINDOW_SET_ICON                 0
#define VX_FEAT_WINDOW_SET_POSITION             0
#define VX_FEAT_WINDOW_SET_SIZE                 0
#define VX_FEAT_WINDOW_GET_BORDER_SIZE          0
#define VX_FEAT_WINDOW_GET_PIXEL_SIZE           0
#define VX_FEAT_WINDOW_SET_OPACITY              0
#define VX_FEAT_WINDOW_SHOW                     0
#define VX_FEAT_WINDOW_HIDE                     0
#define VX_FEAT_WINDOW_RAISE                    0
#define VX_FEAT_WINDOW_MAXIMIZE                 0
#define VX_FEAT_WINDOW_MINIMIZE                 0
#define VX_FEAT_WINDOW_RESTORE                  0
#define VX_FEAT_WINDOW_SET_BORDERED             0
#define VX_FEAT_WINDOW_SET_RESIZABLE            0
#define VX_FEAT_WINDOW_SET_ALWAYS_ON_TOP        0
#define VX_FEAT_WINDOW_SET_FULLSCREEN           0
#define VX_FEAT_WINDOW_GET_ICC_PROFILE          0
#define VX_FEAT_WINDOW_SET_MOUSE_RECT           0
#define VX_FEAT_WINDOW_SET_MOUSE_GRAB           0
#define VX_FEAT_WINDOW_SET_KEYBOARD_GRAB        0

// Text input
#define VX_FEAT_TEXT_INPUT_START                0
#define VX_FEAT_TEXT_INPUT_STOP                 0
#define VX_FEAT_TEXT_INPUT_UPDATE_AREA          0
#define VX_FEAT_TEXT_INPUT_CLEAR_COMPOSITION    0

// Clipboard
#define VX_FEAT_CLIPBOARD_SET_DATA              0
#define VX_FEAT_CLIPBOARD_GET_DATA              0
#define VX_FEAT_CLIPBOARD_HAS_DATA              0
#define VX_FEAT_CLIPBOARD_SCREEN_KEYBOARD       0
