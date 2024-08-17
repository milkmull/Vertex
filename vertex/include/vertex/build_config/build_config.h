#pragma once

#include "vertex/system/platform.h"

#if defined(VX_PLATFORM_WINDOWS)
#   include "windows_build_config.h"
#else
#   include "default_build_config.h"
#endif