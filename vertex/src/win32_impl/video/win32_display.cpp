#include "win32_display.h"

#include "vertex/system/log.h"
#include "vertex/system/string/string_fn.h"

namespace vx {
namespace app {

//std::vector<display_data> display_data::s_displays = std::vector<display_data>();
//size_t display_data::s_primary_display_index = -1;
//
//struct poll_display_data
//{
//    size_t index = 0;
//    std::vector<display_data> displays;
//    size_t primary_monitor_index = 0;
//};
//
//static bool create_display(size_t index, display_data& data, HMONITOR hMonitor, const MONITORINFOEX* info)
//{
//    data.id = index;
//    data.monitor_handle = hMonitor;
//
//    {
//        data.bounds = math::recti(
//            info->rcMonitor.left,
//            info->rcMonitor.top,
//            info->rcMonitor.right - info->rcMonitor.left,
//            info->rcMonitor.bottom - info->rcMonitor.top
//        );
//
//        data.work_area = math::recti(
//            info->rcWork.left,
//            info->rcWork.top,
//            info->rcWork.right - info->rcWork.left,
//            info->rcWork.bottom - info->rcWork.top
//        );
//    }
//
//    {
//        DISPLAY_DEVICE dev{};
//        dev.cb = sizeof(dev);
//
//        DWORD physical_monitor_index = 0;
//        while (EnumDisplayDevicesW(info->szDevice, physical_monitor_index++, &dev, NULL))
//        {
//            data.physical_monitor_names.push_back(dev.DeviceName);
//        }
//    }
//}
//
//static BOOL CALLBACK enum_display_monitors_callback(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam)
//{
//    poll_display_data data = *(poll_display_data*)(lParam);
//
//    MONITORINFOEX info{};
//    info.cbSize = sizeof(info);
//
//    if (GetMonitorInfoW(hMonitor, (MONITORINFO*)(&info)))
//    {
//        if (info.dwFlags & MONITORINFOF_PRIMARY)
//        {
//            data.primary_monitor_index = data.index;
//        }
//
//        VX_LOG_INFO << "Display: " << str::wstring_to_string(info.szDevice);
//    }
//
//    return TRUE;
//}
////
////static void set_video_modes(DISPLAY_DEVICEW* adapter, display_data& data)
////{
////    DEVMODEW dm{};
////    dm.dmSize = sizeof(dm);
////
////    DWORD mode_index = 0;
////    while (EnumDisplaySettingsW(adapter->DeviceName, mode_index, &dm))
////    {
////        video_mode mode;
////        mode.width = dm.dmPelsWidth;
////        mode.height = dm.dmPelsHeight;
////        mode.refresh_rate = dm.dmDisplayFrequency;
////        mode.red_bits = dm.dmBitsPerPel;
////
////        data.video_modes.push_back(mode);
////
////        //DeleteDC(dc);
////        ++mode_index;
////    }
////
////    HDC dc = CreateDCW(adapter->DeviceName, NULL, NULL, NULL);
////    if (!dc)
////    {
////        return;
////    }
////
////    PIXELFORMATDESCRIPTOR pfd{};
////    pfd.nSize = sizeof(pfd);
////
////    DWORD pixel_format_index = 1;
////    while (DescribePixelFormat(dc, pixel_format_index, pfd.nSize, &pfd))
////    {
////        ++pixel_format_index;
////    }
////
////    DeleteDC(dc);
////}
////
//////static bool add_display()
////
////static bool create_display(DISPLAY_DEVICEW* adapter_display, DISPLAY_DEVICEW* display, display_data& data)
////{
////    assert(adapter_display);
////    assert(display);
////
////    data.adapter_display_name = adapter_display->DeviceName;
////    data.name = display->DeviceName;
////
////    DEVMODEW dm{};
////    dm.dmSize = sizeof(dm);
////
////    RECT rect{};
////
////
////
////    //EnumDisplayMonitors(NULL, NULL, enum_display_monitors_callback, (LPARAM)&data);
////
////    // List and set the video modes
////    //set_video_modes(adapter, data);
////
////    return true;
////}
//
//void display_data::poll()
//{
//    // NOTE: An adapter is the hardware component responsible for for rendering
//    // and outputting graphics to a display. Each adapter can support a varying
//    // number of displays. Each display can be rendered to multpile displays.
//    // 
//    // When calling EnumDisplayDevices with lpDevice set to NULL, each display
//    // will be listed for the adaper that is currently in use.
//    // DISPLAY_DEVICE.DeviceString will contain the name of the adapter, and
//    // DISPLAY_DEVICE.DeviceName will contain the name of the specific display
//    // being used by the adapter.
//    // 
//    // Calling EnumDisplayDevices width lpDevice set to
//    // DISPLAY_DEVICE.DeviceName will list each display being rendered to by
//    // the given display. DISPLAY_DEVICE.DeviceName will be the name of the
//    // display.
//
//    // EnumDisplayMonitors will enumerate only the unique displays. If a
//    // display is being extended accross multiple monitors, there will be n
//    // display monitors listed. If the display is, instead, being duplicated
//    // accross multiple monitors, only the primary display monitor will be
//    // listed.
//
//    poll_display_data data;
//
//    EnumDisplayMonitors(NULL, NULL, enum_display_monitors_callback, (LPARAM)&data);
//}

}
}