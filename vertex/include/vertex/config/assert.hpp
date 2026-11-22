#pragma once

#include <cassert>

//=============================================================================
// Assert
//=============================================================================

#if defined(VX_DEBUG) && defined(assert)
#   define VX_ASSERT(x) assert(x)
#else
#   define VX_ASSERT(x)
#endif
