#pragma once

#include "vertex/system/platform.hpp"

// https://github.com/libsdl-org/SDL/blob/4ba4f0a10639fc216287774228b655445dc6adc7/include/SDL3/SDL_main_impl.h

#if defined(VX_PLATFORM_WINDOWS) && 0

// Default main function, which can be overridden by the user
extern "C" int main(int argc, char* argv[]);

// These defines/typedefs are needed for the WinMain() definition

#if !defined(WINAPI)
#   define WINAPI __stdcall
#endif

typedef struct HINSTANCE__* HINSTANCE;
typedef char* LPSTR;
typedef wchar_t* PWSTR;

// The VC++ compiler needs main/wmain defined
#if defined(_MSC_VER)

// This is where execution begins [console apps]
//#   if defined(UNICODE) && UNICODE
//
//        int wmain(int argc, wchar_t* wargv[], wchar_t* wenvp)
//        {
//            (void)argc;
//            (void)wargv;
//            (void)wenvp;
//            return SDL_RunApp(0, NULL, SDL_main, NULL);
//        }
//
//#   else // ANSI
//
//        int main(int argc, char* argv[])
//        {
//            (void)argc;
//            (void)argv;
//            return SDL_RunApp(0, NULL, SDL_main, NULL);
//        }
//
//#   endif // UNICODE
//
//#endif // _MSC_VER

#include <cstdlib> // for '__argc' and '__argv'

extern "C" {

#if defined(UNICODE) && UNICODE
    int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR szCmdLine, int sw)
#else // ANSI
    int WINAPI WinMain(
        _In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR lpCmdLine,
        _In_ int nShowCmd
    )
#endif // UNICODE
    {
        (void)hInstance;
        (void)hPrevInstance;
        (void)lpCmdLine;
        (void)nShowCmd;
        return main(__argc, __argv);
    }

} // extern "C"

/* end of SDL_PLATFORM_WINDOWS impls */

#endif

#endif