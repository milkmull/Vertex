#pragma once

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#   define UNICODE 1
#endif

#ifndef _UNICODE
#   define _UNICODE 1
#endif

#include <windows.h>
#include <VersionHelpers.h>

#define WIN32_LOAD_FUNCTION(module, name) module ? reinterpret_cast<void*>(GetProcAddress(module, name)) : NULL

// macros for extracting signed values from WM_* lparam stolen from windowsx.h

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))