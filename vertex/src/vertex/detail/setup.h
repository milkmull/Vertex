#pragma once

#include <cassert>
#include <cstddef>

// =============== version ===============

#define VX_VERSION_MAJOR 0
#define VX_VERSION_MINOR 1
#define VX_VERSION_PATCH 0

// =============== compilers ===============

#ifdef _MSC_VER                 // Microsoft Visual C++
#   define VX_COMPILER_MSVC
#elif defined(__GNUC__)         // GNU Compiler Collection
#   define VX_COMPILER_GNU
#elif defined(__clang__)        // Clang
#   define VX_COMPILER_CLANG
#else
#   define VX_COMPILER_UNKNOWN
#endif

// =============== meta ===============