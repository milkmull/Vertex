#pragma once

#include <cassert>

///////////////////////////////////////////////////////////////////////////////
// Assert
///////////////////////////////////////////////////////////////////////////////

#if defined(VX_DEBUG)
#   define VX_ENABLE_ASSERTS
#endif

#if defined(VX_ENABLE_ASSERTS)
#   define VX_ASSERT(x, msg) if (!(x)) std::cerr << "[ASSERT] [" << VX_FILE << ":" << VX_LINE << "] " << msg << std::endl
#else
#   define VX_ASSERT(x, msg)
#endif