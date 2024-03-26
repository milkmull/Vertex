#pragma once

#if defined(VX_APPLICATION)

namespace vx {
namespace app {

enum class error_code : int
{
    NONE,
    INTERNAL,
    NOT_INITIALIZED,
    INVALID_VALUE,
    OUT_OF_MEMORY,
    VERSION_UNAVAILABLE,
    PLATFORM_ERROR,
    UNKNOWN
};

}
}

#endif