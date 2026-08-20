#pragma once

#include <cstdint>

#include "vertex/config/language_config.hpp"

//=============================================================================
// String and concatenation utilities
//=============================================================================

#define VX_PRIV_STRINGIFY(x) #x
#define VX_STRINGIFY(x)      VX_PRIV_STRINGIFY(x)

#define VX_PRIV_CONCAT(a, b) a##b
#define VX_CONCAT(a, b)      VX_PRIV_CONCAT(a, b)

//=============================================================================
// Bit and index utilities
//=============================================================================

#define VX_BIT(i)        (1u << (i))
#define VX_INVALID_INDEX SIZE_MAX

//=============================================================================
// Return If
//=============================================================================

#define VX_RETURN_IF(cond, ret) \
    do \
    { \
        if ((cond)) \
        { \
            return (ret); \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)
