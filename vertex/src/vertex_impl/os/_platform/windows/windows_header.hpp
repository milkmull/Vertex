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
#include <objbase.h>            // CoInitialize
#include <bcrypt.h>             // Random
#include <winioctl.h>
#include <shlobj.h>
#include <shellapi.h>

#include <string>               // Error handling

///////////////////////////////////////////////////////////////////////////////
// definitions
///////////////////////////////////////////////////////////////////////////////

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer

typedef struct REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;