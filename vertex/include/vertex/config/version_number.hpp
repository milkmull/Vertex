#pragma once

#include "vertex/config/util.hpp"

#define VX_VERSION_NUMBER(major, minor, patch) ((((major) % 100) * 10000000) + (((minor) % 100) * 100000) + ((patch) % 100000))

#define VX_VERSION_NUMBER_ZERO VX_VERSION_NUMBER(0, 0, 0)
#define VX_VERSION_NUMBER_MIN VX_VERSION_NUMBER(0, 0, 1)
#define VX_VERSION_NUMBER_AVAILABLE VX_VERSION_NUMBER_MIN
#define VX_VERSION_NUMBER_NOT_AVAILABLE VX_VERSION_NUMBER_ZERO

#define VX_VERSION_NUMBER_MAJOR(V) (((V) / 10000000) % 100)
#define VX_VERSION_NUMBER_MINOR(V) (((V) / 100000) % 100)
#define VX_VERSION_NUMBER_PATCH(V) ((V) % 100000)

#define VX_VERSION_STRING(major, minor, patch) (VX_STRINGIFY(major) "." VX_STRINGIFY(minor) "." VX_STRINGIFY(patch))

///////////////////////////////////////////////////////////////////////////////
// decomposition
///////////////////////////////////////////////////////////////////////////////

// https://github.com/boostorg/predef/blob/a2a5010e2824b7740890a3bf463b8c4b8927aaa7/include/boost/predef/make.h

#define _VX_MAKE_VERSION_NUMBER_10_VRR000(V) VX_VERSION_NUMBER(((V) / 100000) % 10, ((V) / 1000) % 100, 0)

#define _VX_MAKE_VERSION_NUMBER_10_VV00(V) VX_VERSION_NUMBER(((V) / 100) % 100, 0, 0)

#define _VX_MAKE_VERSION_NUMBER_10_VVRR(V) VX_VERSION_NUMBER(((V) / 100) % 100, (V) % 100, 0)

#define _VX_MAKE_VERSION_NUMBER_10_VRP000(V) VX_VERSION_NUMBER(((V) / 100000) % 10, ((V) / 10000) % 10, ((V) / 1000) % 10)

#define _VX_MAKE_VERSION_NUMBER_10_VRPPPP(V) VX_VERSION_NUMBER(((V) / 100000) % 10, ((V) / 10000) % 10, (V) % 10000)

#define _VX_MAKE_VERSION_NUMBER_10_VVRRPPP(V) VX_VERSION_NUMBER(((V) / 100000) % 100, ((V) / 1000) % 100, (V) % 1000)