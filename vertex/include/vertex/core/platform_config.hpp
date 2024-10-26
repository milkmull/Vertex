#pragma once

#include "vertex/core/platform.hpp"
#include "vertex/core/compiler.hpp"
#include "vertex/core/platform_defines.hpp"

///////////////////////////////////////////////////////////////////////////////
// Config
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_PLATFORM_WINDOWS)
#   include "vertex/core/config/windows.hpp"
#else
#   include "vertex/core/config/dummy.hpp"
#endif