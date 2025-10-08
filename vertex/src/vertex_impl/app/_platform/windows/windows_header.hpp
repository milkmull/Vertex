#pragma once

#include "vertex_impl/os/_platform/windows/windows_tools.hpp"

///////////////////////////////////////////////////////////////////////////////

// macros for extracting signed values from WM_* lparam stolen from windowsx.h

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

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
#if !defined(DWMWA_USE_IMMERSIVE_DARK_MODE)
#   define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// Corner rounding (Win 11+)
#if !defined(DWMWA_WINDOW_CORNER_PREFERENCE) 
#   define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

enum DWM_WINDOW_CORNER_PREFERENCE
{
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
};

// Border Color support (Win 11+)
#if !defined(DWMWA_BORDER_COLOR)
#   define DWMWA_BORDER_COLOR 34
#endif

#if !defined(DWMWA_COLOR_DEFAULT)
#   define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
#endif

#if !defined(DWMWA_COLOR_NONE)
#   define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);