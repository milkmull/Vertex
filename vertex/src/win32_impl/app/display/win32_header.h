#pragma once

#include <windows.h>

// macros for extracting signed values from WM_* lparam

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))