#pragma once

#include <string>
#include <vector>

#include "utf.h"

namespace vx {
namespace str {

inline std::wstring string_to_wstring(const std::string& str)
{
    const size_t count = utf8::count(str.begin(), str.end());
    std::wstring wstr(count, 0);
    utf8::to_wide(str.begin(), str.end(), wstr.begin());
    return wstr;
}

inline std::string wstring_to_string(const std::wstring& wstr)
{
    std::vector<typename std::string::value_type> str_data;
    utf8::from_wide(wstr.begin(), wstr.end(), std::back_inserter(str_data));
    return std::string(str_data.begin(), str_data.end());
}

}
}