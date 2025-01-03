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
#include <versionhelpers.h>
#include <psapi.h>              // Performance Info
#include <shellapi.h>           // Dropfile support
#include <objbase.h>            // CoInitialize
#include <bcrypt.h>             // Random
#include <winioctl.h>

#include <string>               // Error handling

///////////////////////////////////////////////////////////////////////////////
// status
///////////////////////////////////////////////////////////////////////////////

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

enum DWM_WINDOW_CORNER_PREFERENCE
{
    DWMWCP_DEFAULT = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND = 2,
    DWMWCP_ROUNDSMALL = 3
};

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

using DwmSetWindowAttributePtr = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
using DwmGetWindowAttributePtr = HRESULT(WINAPI*)(HWND hwnd, DWORD dwAttribute, PVOID pvAttribute, DWORD cbAttribute);

///////////////////////////////////////////////////////////////////////////////
// Error Handling
///////////////////////////////////////////////////////////////////////////////

void windows_error_message(const std::string& msg);

///////////////////////////////////////////////////////////////////////////////
// COM
///////////////////////////////////////////////////////////////////////////////

class com_scoped_initializer
{
public:

    com_scoped_initializer();
    ~com_scoped_initializer();

    com_scoped_initializer(const com_scoped_initializer&) = delete;
    com_scoped_initializer& operator=(const com_scoped_initializer&) = delete;

    bool succeeded() const;

private:

    HRESULT m_hr;
};

typedef struct REPARSE_DATA_BUFFER {
    ULONG ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    };
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;