#pragma once

#if !defined(NOMINMAX)
#   define NOMINMAX
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#   define WIN32_LEAN_AND_MEAN 1
#endif

#if !defined(STRICT)
#   define STRICT 1
#endif

#if !defined(UNICODE)
#   define UNICODE 1
#endif

#if !defined(_UNICODE)
#   define _UNICODE 1
#endif

#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
// status
///////////////////////////////////////////////////////////////////////////////

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

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