#pragma once

#include "vertex/system/platform.hpp"
#include "vertex/system/compiler.hpp"
#include "vertex/system/platform_defines.hpp"

///////////////////////////////////////////////////////////////////////////////
// Config
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex/system/config/windows.hpp"
#else
#   include "vertex/system/config/dummy.hpp"
#endif