#pragma once

#include "vertex/detail/setup.h"

#define VX_ENABLE_MATH_CHECKS 0

// Clip control, define VX_FORCE_DEPTH_ZERO_TO_ONE before including vertex
// to use a clip space between 0 to 1.
// Coordinate system, define VX_FORCE_LEFT_HANDED before including vertex
// to use left handed coordinate system by default.

#define VX_CLIP_CONTROL_ZO_BIT (1 << 0) // ZERO_TO_ONE
#define VX_CLIP_CONTROL_NO_BIT (1 << 1) // NEGATIVE_ONE_TO_ONE
#define VX_CLIP_CONTROL_LH_BIT (1 << 2) // LEFT_HANDED, For DirectX, Metal, Vulkan (+z is into the screen)
#define VX_CLIP_CONTROL_RH_BIT (1 << 3) // RIGHT_HANDED, For OpenGL, default in VX (+z is out of the screen)

#define VX_CLIP_CONTROL_LH_ZO (VX_CLIP_CONTROL_LH_BIT | VX_CLIP_CONTROL_ZO_BIT)
#define VX_CLIP_CONTROL_LH_NO (VX_CLIP_CONTROL_LH_BIT | VX_CLIP_CONTROL_NO_BIT)
#define VX_CLIP_CONTROL_RH_ZO (VX_CLIP_CONTROL_RH_BIT | VX_CLIP_CONTROL_ZO_BIT)
#define VX_CLIP_CONTROL_RH_NO (VX_CLIP_CONTROL_RH_BIT | VX_CLIP_CONTROL_NO_BIT)

#ifdef VX_FORCE_DEPTH_ZERO_TO_ONE
#   ifdef VX_FORCE_LEFT_HANDED
#       define VX_CONFIG_CLIP_CONTROL VX_CLIP_CONTROL_LH_ZO
#   else
#       define VX_CONFIG_CLIP_CONTROL VX_CLIP_CONTROL_RH_ZO
#   endif
#else
#   ifdef VX_FORCE_LEFT_HANDED
#       define VX_CONFIG_CLIP_CONTROL VX_CLIP_CONTROL_LH_NO
#   else
#       define VX_CONFIG_CLIP_CONTROL VX_CLIP_CONTROL_RH_NO
#	endif
#endif