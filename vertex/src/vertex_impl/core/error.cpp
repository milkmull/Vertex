#include "vertex/core/error.hpp"

#if (VX_DEBUG)
#   include <iostream>
#endif

namespace vx {
namespace err {

VX_API const char* code_to_string(code err)
{
    switch (err)
    {
        case code::NONE:                        return "none";
        case code::ERROR:                       return "error";

        case code::RUNTIME_ERROR:               return "runtime error";
        case code::NOT_CONFIGURED:              return "not configured";

        case code::OUT_OF_RANGE:                return "out of range";
        case code::OUT_OF_MEMORY:               return "out of memory";
        case code::SIZE_ERROR:                  return "size error";

        case code::INVALID_DATA:                return "invalid data";
        case code::INVALID_ARGUMENT:            return "invalid argument";
        case code::UNSUPPORTED_FORMAT:          return "unsupported format";
        case code::UNSUPPORTED_CONVERSION:      return "unsupported conversion";
        case code::UNSUPPORTED_OPERATION:       return "unsupported operation";

        case code::RESOURCE_NOT_FOUND:          return "resource not found";
        case code::RESOURCE_ALREADY_EXISTS:     return "resource already exists";

        case code::FILE_OPERATION_FAILED:       return "file operation failed";
        case code::FILE_OPEN_FAILED:            return "file open failed";
        case code::FILE_READ_FAILED:            return "file read failed";
        case code::FILE_WRITE_FAILED:           return "file write failed";

        case code::FILE_NO_PERMISSION:          return "file no permission";
        case code::FILE_IN_USE:                 return "file in use";
        case code::FILE_CORRUPT:                return "file corrupt";

        case code::PLATFORM_ERROR:              return "platform error";
        default:                                return "";
    }
}

static thread_local info s_err;

///////////////////////////////////////////////////////////////////////////////
// error accessors and manipulators
///////////////////////////////////////////////////////////////////////////////

VX_API info get()
{
    return s_err;
}

VX_API void set(code err, const std::string& msg)
{
#if (VX_DEBUG)
    std::cerr << static_cast<int>(err) << ": " << msg;
#endif

    s_err.err = err;
    s_err.message = msg;
}

VX_API void set(code err)
{
    set(err, code_to_string(err));
}

VX_API void clear()
{
    set(code::NONE, std::string());
}

} // namespace err
} // namespace vx