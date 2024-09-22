#include "vertex/system/error.hpp"

namespace vx {
namespace error {

const char* error_code_to_string(error_code code)
{
    switch (code)
    {
        case error_code::NONE:                      return "none";
        case error_code::ERROR:                     return "error";

        case error_code::OUT_OF_RANGE:              return "out of range";
        case error_code::OUT_OF_MEMORY:             return "out of memory";
        case error_code::SIZE_ERROR:                return "size error";

        case error_code::INVALID_ARGUMENT:          return "invalid argument";
        case error_code::UNSUPPORTED_TYPE:          return "unsupported type";
        case error_code::UNSUPPORTED_FORMAT:        return "unsupported format";
        case error_code::UNSUPPORTED_CONVERSION:    return "unsupported conversion";
        case error_code::UNSUPPORTED_OPERATION:     return "unsupported operation";

        case error_code::FILE_NOT_FOUND:            return "file not found";
        case error_code::FILE_BAD_DRIVE:            return "file bad drive";
        case error_code::FILE_BAD_PATH:             return "file bad path";
        case error_code::FILE_NO_PERMISSION:        return "file no permission";
        case error_code::FILE_IN_USE:               return "file in use";
        case error_code::FILE_OPEN_FAILED:          return "file open failed";
        case error_code::FILE_READ_FAILED:          return "file read failed";
        case error_code::FILE_WRITE_FAILED:         return "file write failed";
        case error_code::FILE_CORRUPT:              return "file corrupt";
        case error_code::FILE_ERROR:                return "file error";

        case error_code::PLATFORM_ERROR:            return "platform error";
        default:                                    return "";
    }
}

static thread_local error_info s_error;

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

error_info get_error()
{
    return s_error;
}

void set_error(error_code code, const std::string& msg)
{
    s_error.code = code;
    s_error.message = msg;
}

void set_error(error_code code)
{
    set_error(code, error_code_to_string(code));
}

void clear_error()
{
    set_error(error_code::NONE, "");
}

}
}