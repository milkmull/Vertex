#pragma once

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef STRICT
#   define STRICT 1
#endif

#ifndef UNICODE
#   define UNICODE 1
#endif

#ifndef _UNICODE
#   define _UNICODE 1
#endif

#include <windows.h>

// Dropfile support
#include <shellapi.h>

// macros for extracting signed values from WM_* lparam stolen from windowsx.h

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

///////////////////////////////////////////////////////////////////////////////
// dpi
///////////////////////////////////////////////////////////////////////////////

enum PROCESS_DPI_AWARENESS
{
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
};

enum MONITOR_DPI_TYPE
{
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT
};

///////////////////////////////////////////////////////////////////////////////
// DWM support
///////////////////////////////////////////////////////////////////////////////

// https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/ne-dwmapi-dwmwindowattribute

// Dark mode
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#   define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// Corner rounding (Win 11+)
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE 
#   define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

typedef enum {
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
} DWM_WINDOW_CORNER_PREFERENCE;

// Border Color support (Win 11+)
#ifndef DWMWA_BORDER_COLOR
#   define DWMWA_BORDER_COLOR 34
#endif

#ifndef DWMWA_COLOR_DEFAULT
#   define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

#ifndef DWMWA_COLOR_NONE
#   define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);