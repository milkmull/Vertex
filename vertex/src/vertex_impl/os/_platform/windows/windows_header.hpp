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
// definitions
///////////////////////////////////////////////////////////////////////////////

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

typedef unsigned __int64 QWORD;
