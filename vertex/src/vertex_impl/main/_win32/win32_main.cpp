#include "vertex/build_config/build_config.h"

#if defined(VX_PLATFORM_WINDOWS)

#include "vertex_impl/_platform/_win32/win32_header.h"
#include <cstdlib> // for '__argc' and '__argv'

extern int main(int argc, char* argv[]);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    return main(__argc, __argv);
}

#endif