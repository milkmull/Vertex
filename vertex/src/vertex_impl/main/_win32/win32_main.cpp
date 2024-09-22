#include "vertex/system/platform.hpp"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.hpp"
#include <cstdlib> // for '__argc' and '__argv'

// Default main function, which can be overridden by the user
extern "C" int main(int argc, char* argv[])
{
    return 0;
}

extern "C" int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    return main(__argc, __argv);
}

#endif