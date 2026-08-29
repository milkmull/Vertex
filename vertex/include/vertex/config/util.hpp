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
// Branch Prediction
//=============================================================================

#define VX_UNLIKELY_COLD_PATH(cond, action) \
    do \
    { \
        if VX_UNLIKELY (cond) \
        { \
            action; \
        } \
    } while (0)

//=============================================================================
// Return If
//=============================================================================

#define VX_RET_IF(cond, ...) \
    do \
    { \
        if ((cond)) \
        { \
            return __VA_ARGS__; \
        } \
    } while (VX_NULL_WHILE_LOOP_CONDITION)

#define VX_RET_IF_UL(cond, ...) \
    VX_UNLIKELY_COLD_PATH((cond), return __VA_ARGS__)
