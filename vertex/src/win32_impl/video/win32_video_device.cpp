#include "win32_video_device.h"
#include "vertex/system/error.h"

//namespace vx {
//namespace app {

//static bool set_dpi_awareness()
//{
//    // https://learn.microsoft.com/en-gb/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows?redirectedfrom=MSDN
//
//    bool success = false;
//
//    // Try SetProcessDpiAwareness first
//    HINSTANCE sh_core_dll = LoadLibrary(L"Shcore.dll");
//
//    if (sh_core_dll)
//    {
//        // https://learn.microsoft.com/en-us/windows/win32/api/shellscalingapi/ne-shellscalingapi-process_dpi_awareness
//
//        enum PROCESS_DPI_AWARENESS
//        {
//            PROCESS_DPI_UNAWARE = 0,
//            PROCESS_SYSTEM_DPI_AWARE = 1,
//            PROCESS_PER_MONITOR_DPI_AWARE = 2
//        };
//
//        using SetProcessDpiAwarenessFuncType = HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS);
//        auto SetProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessFuncType>(
//            reinterpret_cast<void*>(GetProcAddress(sh_core_dll, "SetProcessDpiAwareness"))
//        );
//
//        // Check for E_INVALIDARG. A value of E_ACCESSDENIED incicates
//        // that the dpi has already been set, and a value of S_OK
//        // indicates that the process was successful.
//        // 
//        // We intentionally don't use Per Monitor V2 which can be
//        // enabled with SetProcessDpiAwarenessContext, because that
//        // would scale the title bar and thus change window size
//        // by default when moving the window between monitors.
//
//        if (SetProcessDpiAwareness && SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE) != E_INVALIDARG)
//        {
//            success = true;
//        }
//
//        FreeLibrary(sh_core_dll);
//    }
//
//    if (!success)
//    {
//        // Fall back to SetProcessDPIAware if SetProcessDpiAwareness
//        // is not available on this system
//        if (SetProcessDPIAware())
//        {
//            success = true;
//        }
//    }
//
//    if (!success)
//    {
//        VX_ERROR(error::error_code::PLATFORM_ERROR) << "Failed to set process DPI awareness";
//    }
//}
//
////void video_device::video_device_impl::init()
////{
////
////}
//
//}
//}