#pragma once

#include <cstdint>

//=============================================================================
// String and concatenation utilities
//=============================================================================

#define _VX_STRINGIFY(x) #x
#define VX_STRINGIFY(x)  _VX_STRINGIFY(x)

#define _VX_CONCAT(a, b) a##b
#define VX_CONCAT(a, b)  _VX_CONCAT(a, b)

//=============================================================================
// Bit and index utilities
//=============================================================================

#define VX_BIT(i)          (1u << (i))
#define VX_INVALID_INDEX   SIZE_MAX
