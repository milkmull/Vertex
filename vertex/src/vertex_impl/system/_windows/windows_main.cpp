#include "vertex/system/platform_config.hpp"

#if defined(__VX_WINDOWS_MAIN)

#include "vertex_impl/_platform/_windows/windows_header.hpp"
#include <cstdlib> // for '__argc' and '__argv'

// Default main function, which can be overridden by the user
extern "C" int main(int argc, char* argv[]);

// Reminder: Ensure the project is configured as a Windows application
// (not Console) in the Linker settings to execute WinMain.

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    return main(__argc, __argv);
}

#endif // VX_WINDOWS_MAIN