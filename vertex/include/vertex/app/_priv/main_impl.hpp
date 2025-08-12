#pragma once

#include "vertex/config/os.hpp"

#if !defined(VX_MAIN_HANDLED)

///////////////////////////////////////////////////////////////////////////////
// windows entry
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_OS_WINDOWS)
#if defined(VX_GUI_APP)

extern int main(int argc, char* argv[]);

#include <stdlib.h> // for __argc, __argv

// these defines/typedefs are needed for the WinMain() definition
#if !defined(WINAPI)
#   define WINAPI __stdcall
#endif

typedef struct HINSTANCE__* HINSTANCE;
typedef char* LPSTR;
typedef wchar_t* PWSTR;

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(UNICODE) && UNICODE
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
#else /* ANSI */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    return main(__argc, __argv);
}

#if defined(__cplusplus)
} // extern C
#endif

#endif // VX_GUI_APP
#endif // VX_OS_WINDOWS

///////////////////////////////////////////////////////////////////////////////

#endif // VX_MAIN_HANDLED