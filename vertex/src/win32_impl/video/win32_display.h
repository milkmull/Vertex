//#pragma once
//
//#include <vector>
//#include <string>
//
//#include "../../win32_header.h"
//#include "vertex/app/video/display.h"
//#include "vertex/math/geometry/type/recti.h"
//
//namespace vx {
//namespace app {
//
//enum display_status
//{
//    NONE,
//    CHANGED,
//    MOVED,
//    CONNECTED,
//    DISCONNECTED
//};
//
////struct display_data
////{
////    display::id id = 0;
////    HMONITOR monitor_handle = NULL;
////
////    math::recti bounds;
////    math::recti work_area;
////
////    std::wstring display_adapter_name;
////    std::vector<std::wstring> physical_monitor_names;
////
////    std::vector<video_mode> modes;
////    size_t current_mode_index = 0;
////
////    display_status status = display_status::NONE;
////
////    static std::vector<display_data> s_displays;
////    static size_t s_primary_display_index;
////    static void poll();
////};
//
//}
//}