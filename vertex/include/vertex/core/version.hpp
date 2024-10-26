#pragma once

#define VX_VERSION_MAJOR 0
#define VX_VERSION_MINOR 1
#define VX_VERSION_PATCH 0

#define VX_VERSION_STRING "0.1.0"

#define VX_VERSION (VX_VERSION_MAJOR * 10000 + VX_VERSION_MINOR * 100 + VX_VERSION_PATCH)

#define VX_VERSION_AT_LEAST(major, minor, patch) (VX_VERSION >= ((major) * 10000 + (minor) * 100 + (patch)))