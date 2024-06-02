#pragma once

#include <unordered_map>

#include "vertex/app/input/cursor.h"
#include "../../win32_header.h"

namespace vx {
namespace app {

struct cursor_data
{
    HCURSOR cursor;
    ~cursor_data() { DestroyCursor(cursor); }

    static int s_cursor_id;
    static std::unordered_map<int, cursor_data> s_cursor_cache;
};

}
}